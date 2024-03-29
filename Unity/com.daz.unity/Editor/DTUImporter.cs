﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Unity.EditorCoroutines.Editor;
using UnityEditor;
using UnityEditor.AssetImporters;
using UnityEngine;
using UnityEngine.Rendering;
using Object = UnityEngine.Object;

namespace Daz3D
{
    public enum DazFigurePlatform
    {
        [InspectorName("Genesis 8.1")] Genesis81,
        [InspectorName("Genesis 8")] Genesis8,
        [InspectorName("Genesis 3")] Genesis3,
        [InspectorName("Genesis 2")] Genesis2,
        [InspectorName("Victori a")] Victoria,
        [InspectorName("Genesis")] Genesis,
        [InspectorName("Michael")] Michael,
        [InspectorName("TheFreak")] TheFreak,
        [InspectorName("Victoria 4")] Victoria4,
        [InspectorName("Victoria4Elite")] Victoria4Elite,
        [InspectorName("Michael 4")] Michael4,
        [InspectorName("Michael4Elite")] Michael4Elite,
        [InspectorName("Stephanie 4")] Stephanie4,
        [InspectorName("Aiko 4")] Aiko4
    }

    public static class DazFigurePlatformExtensions
    {
        public static DazFigurePlatform ToFigurePlatform(this string src)
        {
             var token = src.ToLower();

            // Debug.Log($"Token {token}");

            if (token.ToLower().StartsWith("genesis8_1"))
            {
                return DazFigurePlatform.Genesis81;
            }

            foreach (DazFigurePlatform dfp in Enum.GetValues(typeof(DazFigurePlatform)))
            {
                if (token.Contains(dfp.ToString().ToLower()))
                    return dfp;
            }

            return DazFigurePlatform.Genesis8; //default
        }
    }


    [ScriptedImporter(1, "dtu", 0x7FFFFFFF)]
    public class DTUImporter : ScriptedImporter
    {
        [SerializeField] bool ApplySubdivisions = true;
        [SerializeField] bool AutoImportDTUChanges = true;
        [SerializeField] bool GenerateUnityPrefab = true;
        [SerializeField] bool ReplaceSceneInstances = true;
        [SerializeField] bool AutomateMecanimAvatarMappings = true;
        [SerializeField] bool ReplaceMaterials = true;
        [SerializeField] bool UseHighQualityTextures = true;
        [SerializeField] bool RegenerateMaterials = false;
        [SerializeField] public DTUFile dtuFile;

        private EditorCoroutine m_ImportCoroutine;

        // public string path = "";

        private static Dictionary<string, Material> s_StandardMaterialCollection = new Dictionary<string, Material>();
        private static MaterialMap _map;

        #region Types

        public static Queue<ImportEventRecord> EventQueue = new Queue<ImportEventRecord>();
        private const bool ENDLINE = true;

        public static void EmptyEventQueue()
        {
            EventQueue = new Queue<ImportEventRecord>();
        }

        public override void OnImportAsset(AssetImportContext ctx)
        {
            if (!AutoImportDTUChanges) return;
            
            var dtuPath = ctx.assetPath;
            var fbxPath = dtuPath.Replace(".dtu", ".fbx");

            dtuFile = DTUFile.Load(dtuPath);

            ctx.AddObjectToAsset("DTUFile", dtuFile);
            ctx.SetMainObject(dtuFile);

            Import(dtuFile, fbxPath, () => { Utilities.Log($"Importer Done for {dtuFile.AssetName}"); });
        }

        // [MenuItem("Daz3D/Extract materials from selected DTU", true)]
        // [MenuItem("Assets/Daz3D/Create Unity Prefab", true)]
        // [MenuItem("Daz3D/Create Unity Prefab from selected DTU", true)]
        // [MenuItem("Assets/Daz3D/Extract materials", true)]
        public static bool ValidateDTUSelected()
        {
            var obj = Selection.activeObject;

            // Return false if no transform is selected.
            if (obj == null)
                return false;

            return (AssetDatabase.GetAssetPath(obj).ToLower().EndsWith(".dtu"));
        }

        public void Import(DTUFile dtuFile, string fbxPath, Action done)
        {
            //path = dtuPath;
            m_ImportCoroutine = EditorCoroutineUtility.StartCoroutineOwnerless(ImportRoutine(dtuFile, fbxPath, done));
            //DazCoroutine.StartCoroutine(ImportRoutine(dtuFile, fbxPath, done));
        }

        #endregion

        private IEnumerator ImportRoutine(DTUFile dtuFile, string fbxPath, Action done = null)
        {
            Bridge.CurrentToolbarMode = Bridge.ToolbarMode.History; //force into history mode during import

            Bridge.Progress = .03f;
            yield return null;

            _map = new MaterialMap(dtuFile.AssetPath);

            while (!IrayShadersReady())
                yield return null;

            yield return ImportDTURoutine(dtuFile, .8f);
            // var dtu = new DTUFile();
            // var routine = ImportDTURoutine(dtuFile, .8f);
            // while (routine.MoveNext())
            //     yield return null;
            //     //yield return new WaitForEndOfFrame();

            Bridge.Progress = .9f;
            yield return null;
            //yield return new WaitForEndOfFrame();

            if (GenerateUnityPrefab)
            {
                
               /* if (ApplySubdivisions)
                {
                    Utilities.Log("Applying Subdivisions");
                    var routineSubdivisions = DazFBXUtils.ApplySubdivisions(dtuFile.FBXFile, dtuFile);
                    while (routineSubdivisions.MoveNext())
                        yield return null;
                        //yield return new WaitForEndOfFrame();
                }
                */
                Utilities.Log("Generating Prefab");
                GeneratePrefabFromFBX(fbxPath, dtuFile.FigureType, done);
            }

            Bridge.Progress = 1f;
            yield return null;
            //yield return new WaitForEndOfFrame();

            _map = null;

            Bridge.Progress = 0;
        }

        private static bool IrayShadersReady()
        {
            if (
                Shader.Find(Constants.shaderNameMetal) == null ||
                Shader.Find(Constants.shaderNameSpecular) == null ||
                Shader.Find(Constants.shaderNameIraySkin) == null ||
                Shader.Find(Constants.shaderNameHair) == null ||
                Shader.Find(Constants.shaderNameWet) == null ||
                Shader.Find(Constants.shaderNameInvisible) == null
            )
            {
                return false;
            }

            return true;
        }

        public IEnumerator ImportDTURoutine(DTUFile dtuFile, float progressLimit)
        {
            Utilities.Log("ImportDTU for " + dtuFile.AssetPath);

            EventQueue.FoldAll();

            ImportEventRecord record = new ImportEventRecord();
            EventQueue.Enqueue(record);


            var dtuObject = AssetDatabase.LoadAssetAtPath<Object>(dtuFile.AssetPath);

            record.AddToken("Imported DTU file: " + dtuFile.AssetPath);
            record.AddToken(dtuObject.name, dtuObject, ENDLINE);

            //UnityEngine.Debug.Log("DTU: " + dtu.AssetName + " contains: " + dtu.Materials.Count + " materials");

            record.AddToken("Generated materials: ");
            float progressIncrement = (progressLimit - Bridge.Progress) / dtuFile.Materials.Count;

            foreach (var material in dtuFile.Materials.Select(dtuMat => dtuMat.ConvertToUnity(dtuFile,RegenerateMaterials)))
            {
                _map.AddMaterial(material);

                record.AddToken(material.name, material);

                Bridge.Progress = Mathf.MoveTowards(Bridge.Progress, progressLimit, progressIncrement);

                yield return null;
                //yield return new WaitForEndOfFrame();
            }

            record.AddToken(" based on DTU file.", null, ENDLINE);


            Bridge bridge = EditorWindow.GetWindow(typeof(Bridge)) as Bridge;
            if (bridge == null)
            {
                var consoleType = Type.GetType("ConsoleWindow,UnityEditor.dll");
                bridge = EditorWindow.CreateWindow<Bridge>(consoleType);
            }

            bridge.Focus();

            //just a safeguard to keep the history data at a managable size (100 records)
            while (EventQueue.Count > 100)
            {
                EventQueue.Dequeue();
            }
        }

        public void GeneratePrefabFromFBX(string fbxPath, DazFigurePlatform platform, Action done = null)
        {
            GameObject fbxPrefab;
            
            fbxPath = Utilities.RelativePath(fbxPath);

            if (ApplySubdivisions)
                fbxPath = Path.Combine(Path.GetDirectoryName(fbxPath)!,
                    dtuFile.AssetName + "_Updated.fbx");

            fbxPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(fbxPath);

            var fbxName = Path.GetFileNameWithoutExtension(dtuFile.FBXFile);
            
            if (fbxPrefab == null)
            {
                Debug.LogWarning("no FBX model prefab found at " + fbxPath);
                return;
            }

            if (PrefabUtility.GetPrefabAssetType(fbxPrefab) != PrefabAssetType.Model)
            {
                Debug.LogWarning(fbxPath + " is not a model prefab ");
                return;
            }


            MethodInfo resetPose = null;
            MethodInfo xferPose = null;

            var avatarInstance = Instantiate(fbxPrefab);
            avatarInstance.name = "AvatarInstance";

            if (AutomateMecanimAvatarMappings)
            {
                Utilities.Log("Setting up Mecanim");
                var record = new ImportEventRecord();

                ModelImporter importer = GetAtPath(fbxPath) as ModelImporter;
                if (importer)
                {
                    importer.animationType = ModelImporterAnimationType.Human;
                    importer.avatarSetup = ModelImporterAvatarSetup.CreateFromThisModel;

                    var description = importer.humanDescription;
                    // Utilities.Log("TODO??: I have disabled the human mapping of the initial plugin");
                    DescribeHumanJointsForFigure(ref description, platform);
                    
                    importer.humanDescription = description;
                    
                    importer.importBlendShapeNormals = ModelImporterNormals.None;
                    importer.optimizeMeshPolygons = false;
                    importer.optimizeMeshVertices = false;
                    importer.weldVertices = false;
                    importer.meshCompression = ModelImporterMeshCompression.Medium;

                     // Utilities.Log("TODO??: I have disabled the avatar setup of the initial plugin");
                    // Genesis 8 is modeled in A-pose, so we correct to T-pose before configuring avatar joints
                    // using Unity's internal MakePoseValid method, which does a perfect job
                    if (platform == DazFigurePlatform.Genesis8 || platform == DazFigurePlatform.Genesis81)
                    {
                        //use reflection to access AvatarSetupTool;
                        var setupToolType = Type.GetType("UnityEditor.AvatarSetupTool,UnityEditor.dll");
                        var boneWrapperType = Type.GetType("UnityEditor.AvatarSetupTool+BoneWrapper,UnityEditor.dll");
                    
                        if (boneWrapperType != null && setupToolType != null)
                        {
                            var existingMappings = new Dictionary<string, string>();
                            var human = description.human;
                    
                            for (var i = 0; i < human.Length; ++i)
                            {
                                if (!string.IsNullOrEmpty(human[i].boneName))
                                {
                                    Debug.Log($"Human Name: {human[i].humanName} -> Bone Name: {human[i].boneName}");
                                    existingMappings.Add(human[i].humanName, human[i].boneName);
                                }
                            }
                    
                            var getModelBones = setupToolType.GetMethod("GetModelBones");
                            var getHumanBones = setupToolType.GetMethod("GetHumanBones",
                                new[] {typeof(Dictionary<string, string>), typeof(Dictionary<Transform, bool>)});
                            var makePoseValid = setupToolType.GetMethod("MakePoseValid");
                            resetPose = setupToolType.GetMethod("CopyPose");
                            xferPose = setupToolType.GetMethod("TransferPoseToDescription");
                    
                            if (getModelBones != null && getHumanBones != null && makePoseValid != null)
                            {
                                record.AddToken("Corrected Avatar Setup T-pose for Genesis8 figure: ");
                                record.AddToken(fbxPrefab.name, fbxPrefab, ENDLINE);
                    
                                var modelBones = (Dictionary<Transform, bool>) getModelBones.Invoke(null,
                                    new object[] {avatarInstance.transform, false, null});
                                var humanBones = (ICollection<object>) getHumanBones.Invoke(null,
                                    new object[] {existingMappings, modelBones});
                    
                                // a little dance to populate array of Unity's internal BoneWrapper type 
                                var humanBonesArray = new object[humanBones.Count];
                                humanBones.CopyTo(humanBonesArray, 0);
                                Array destinationArray = Array.CreateInstance(boneWrapperType, humanBones.Count);
                                Array.Copy(humanBonesArray, destinationArray, humanBones.Count);
                    
                                //This mutates the transforms (modelBones) via Bonewrapper class
                                makePoseValid.Invoke(null, new[] {destinationArray});
                            }
                        }
                        else
                        {
                            Utilities.Log("Relfection failed. AvataSetup tool not found");
                        }
                    }

 

                    AssetDatabase.WriteImportSettingsIfDirty(fbxPath);
                    AssetDatabase.ImportAsset(fbxPath, ImportAssetOptions.ForceUpdate);


                    // i think this might unT-pose the gen8 skeleton instance
                    if (resetPose != null && xferPose != null)
                    {
                        SerializedObject modelImporterObj = new SerializedObject(importer);
                        var skeleton = modelImporterObj?.FindProperty("m_HumanDescription.m_Skeleton");
                    
                        if (skeleton != null)
                        {
                            resetPose.Invoke(null, new object[] {avatarInstance, fbxPrefab});
                            //xferPose.Invoke(null, new object[] { skeleton, avatarInstance.transform });
                        }
                    }

                    //Utilities.Log("DEBUG: Not removing AvatarInstance");
                    DestroyImmediate(avatarInstance);
                    
                    record.AddToken("Automated Mecanim avatar setup for " + fbxName + ": ");

                    //a little dance to get the avatar just reimported
                    // var allAvatars = Resources.FindObjectsOfTypeAll(typeof(Avatar));
                    // var avatar = Array.Find(allAvatars, element => element.name.StartsWith(fbxPrefab.name));
                    // if (avatar)
                    //     record.AddToken(avatar.name, avatar, ENDLINE);
                }
                else
                {
                    Debug.LogWarning("Could not acquire importer for " + fbxPath +
                                     " ...could not automatically configure humanoid avatar.");
                    record.AddToken(
                        "Could not acquire importer for " + fbxPath +
                        " ...could not automatically configure humanoid avatar.", null, ENDLINE);
                }

                EventQueue.Enqueue(record);
            }


            //remap the materials
            var workingInstance = Instantiate(fbxPrefab);
            workingInstance.name = "Daz3d_" + fbxName;

            var renderers = workingInstance.GetComponentsInChildren<Renderer>();
            if (renderers?.Length == 0)
            {
                Debug.LogWarning("no renderers found for material remapping");
                return;
            }

            var modelPath = AssetDatabase.GetAssetPath(fbxPrefab);

            if (ReplaceMaterials)
            {
                foreach (var renderer in renderers)
                {
                    var dict = new Dictionary<Material, Material>();

                    if (renderer.name.ToLower().Contains("eyelashes"))
                        renderer.shadowCastingMode = ShadowCastingMode.Off;
                    
                    foreach (var keyMat in renderer.sharedMaterials)
                    {
                        var key = keyMat.name;

                        key = Utilities.ScrubKey(key);

                        Material nuMat = null;

                        if (_map != null && _map.Map.ContainsKey(key))
                        {
                            nuMat = _map
                                .Map[key]; // the preferred uber/iRay based material generated by the DTUConverter
                        }
                        else if (s_StandardMaterialCollection.ContainsKey(key))
                        {
                            nuMat = new Material(s_StandardMaterialCollection[key]);
                            //FixupStandardBasedMaterial(ref nuMat, fbxPrefab, keyMat.name, data);
                        }
                        else
                        {
                            var shader = Shader.Find("HDRP/Lit");

                            if (shader == null)
                            {
                                Debug.LogWarning("couldn't find HDRP/Lit shader");
                                continue;
                            }

                            nuMat = new Material(shader);
                            nuMat.CopyPropertiesFromMaterial(keyMat);

                            // just copy the textures, colors and scalars that are appropriate given the base material type
                            //DazMaterialPropertiesInfo info = new DazMaterialPropertiesInfo();
                            //CustomizeMaterial(ref nuMat, info);

                            var matPath = Path.GetDirectoryName(modelPath);
                            matPath = Path.Combine(matPath, fbxName + "Daz3D_Materials");
                            matPath = AssetDatabase.GenerateUniqueAssetPath(matPath);

                            if (!Directory.Exists(matPath))
                                Directory.CreateDirectory(matPath);

                            //Debug.Log("obj path " + path);
                            AssetDatabase.CreateAsset(nuMat, matPath + "/Daz3D_" + keyMat.name + ".mat");
                        }

                        dict.Add(keyMat, nuMat);
                    }

                    //remap the meshes in the fbx prefab to the value materials in dict
                    var count = renderer.sharedMaterials.Length;
                    var copy = new Material[count]; //makes a copy
                    for (int i = 0; i < count; i++)
                    {
                        var key = renderer.sharedMaterials[i];
                        // Utilities.Log("remapping: " + renderer.sharedMaterials[i].name + " to " + dict[key].name);
                        copy[i] = dict[key]; //fill copy
                    }

                    renderer.sharedMaterials = copy; //overwrite sharedMaterials, because set indexer assigns to a copy
                }
            }

            //write the prefab to the asset database
            // WARNING: I have disabled uniqueness so prefabs ARE overwritten.
            // Make sure the file name is unique, in case an existing Prefab has the same name
            var nuPrefabPathPath = Path.GetDirectoryName(modelPath);
            nuPrefabPathPath = Path.Combine(nuPrefabPathPath, fbxName + "_Prefab");
            //nuPrefabPathPath = AssetDatabase.GenerateUniqueAssetPath(nuPrefabPathPath);
            if (!Directory.Exists(nuPrefabPathPath))
                Directory.CreateDirectory(nuPrefabPathPath);

            nuPrefabPathPath += "/Daz3D_" + fbxName + ".prefab";

            // For future refreshment
            var component = workingInstance.AddComponent<Daz3DInstance>();
            component.SourceFBX = fbxPrefab;

            workingInstance.AddComponent<BlendshapesSyncedController>();


            // Create the new Prefab.

            var prefab = PrefabUtility.SaveAsPrefabAssetAndConnect(workingInstance, nuPrefabPathPath,
                InteractionMode.AutomatedAction);


            var Prefab = AssetDatabase.LoadAssetAtPath<GameObject>(nuPrefabPathPath);

            dtuFile.Prefab = Prefab;

            Selection.activeGameObject = prefab;

            //now, seek other instance(s) in the scene having been sourced from this fbx asset
            var otherInstances = FindObjectsOfType<Daz3DInstance>();
            int foundCount = 0;

            var resultingInstance = workingInstance;

            if (ReplaceSceneInstances)
            {
                foreach (var otherInstance in otherInstances)
                {
                    if (otherInstance == component) //ignore this working instance
                        continue;
                    if (otherInstance.SourceFBX != fbxPrefab) //ignore instances of other assets
                        continue;


                    //for any found that flag ReplaceOnImport, delete that instance and replace with a copy of 
                    //this one, at their respective transforms
                    if (otherInstance.ReplaceOnImport)
                    {
                        foundCount++;
                        var xform = otherInstance.transform;
                        var replacementInstance = PrefabUtility.InstantiatePrefab(prefab, xform.parent) as GameObject;
                        replacementInstance.transform.position = xform.position;
                        replacementInstance.transform.rotation = xform.rotation;
                        //var replacementInstance = Instantiate(prefab, xform.position, xform.rotation, xform.parent);
                        //PrefabUtility.RevertPrefabInstance(replacementInstance, InteractionMode.AutomatedAction);
                        DestroyImmediate(otherInstance.gameObject);
                        resultingInstance = replacementInstance;
                    }
                }
            }

            //if no prior instances found, then don't destroy this instance
            //since it appears to be the first one to arrive
            if (foundCount > 0)
                DestroyImmediate(workingInstance);


            ImportEventRecord pfbRecord = new ImportEventRecord();
            pfbRecord.AddToken("Created Unity Prefab: ");
            pfbRecord.AddToken(prefab.name, Prefab);
            pfbRecord.AddToken(" and an instance in the scene: ");
            pfbRecord.AddToken(resultingInstance.name, resultingInstance, ENDLINE);
            EventQueue.Enqueue(pfbRecord);

            done?.Invoke();
            
            //highlight/select the object in the scene view
            Selection.activeGameObject = resultingInstance;
        }

        private static void DescribeHumanJointsForFigure(ref HumanDescription description, DazFigurePlatform figure)
        {
            var map = GetJointMapForFigure(figure);

            HumanBone[] humanBones = new HumanBone[HumanTrait.BoneName.Length];
            int mapIdx = 0;

            foreach (var humanBoneName in HumanTrait.BoneName)
            {
                if (map.ContainsKey(humanBoneName))
                {
                    HumanBone humanBone = new HumanBone();
                    humanBone.humanName = humanBoneName;
                    humanBone.boneName = map[humanBoneName];
                    humanBone.limit.useDefaultValues = true; //todo get limits from dtu?
                    humanBones[mapIdx++] = humanBone;
                }
            }

            description.human = humanBones;
        }

        private static Dictionary<string, string> GetJointMapForFigure(DazFigurePlatform figure)
        {
            Dictionary<string, string> map = new Dictionary<string, string>();

            switch (figure)
            {
                case DazFigurePlatform.Genesis8:
                case DazFigurePlatform.Genesis3:
                    ConfigureGenesisMapStandard(map);
                    break;

                case DazFigurePlatform.Genesis2:
                    ConfigureGenesisMapStandard(map); //todo account for Gen2 variances
                    break;

                default:
                    //do nothing, let unity's excellent guesser handle it
                    break;
            }

            return map;
        }

        private static void ConfigureGenesisMapStandard(Dictionary<string, string> map)
        {
            //note: Genesis 3 finger bones have "Carpal#" parents

            //Body/Body (Gen8)
            map["Hips"] = "hip";
            map["Spine"] = "abdomenUpper";
            map["Chest"] = "chestLower";
            map["UpperChest"] = "chestUpper";

            //Body/Left Arm (Gen8)
            map["LeftShoulder"] = "lCollar";
            map["LeftUpperArm"] = "lShldrBend";
            map["LeftLowerArm"] = "lForearmBend";
            map["LeftHand"] = "lHand";

            //Body/Right Arm (Gen8)
            map["RightShoulder"] = "rCollar";
            map["RightUpperArm"] = "rShldrBend";
            map["RightLowerArm"] = "rForearmBend";
            map["RightHand"] = "rHand";

            //Body/Left Leg (Gen8)
            map["LeftUpperLeg"] = "lThighBend";
            map["LeftLowerLeg"] = "lShin";
            map["LeftFoot"] = "lFoot";
            map["LeftToes"] = "lToe";

            //Body/Right Leg (Gen8)
            map["RightUpperLeg"] = "rThighBend";
            map["RightLowerLeg"] = "rShin";
            map["RightFoot"] = "rFoot";
            map["RightToes"] = "rToe";

            //Head (Gen8)
            map["Neck"] = "neckLower";
            map["Head"] = "head";
            map["LeftEye"] = "lEye";
            map["RightEye"] = "rEye";
            map["Jaw"] = "lowerJaw";

            //Left Hand (Gen8)
            map["Left Thumb Proximal"] = "lThumb1";
            map["Left Thumb Intermediate"] = "lThumb2";
            map["Left Thumb Distal"] = "lThumb3";
            map["Left Index Proximal"] = "lIndex1";
            map["Left Index Intermediate"] = "lIndex2";
            map["Left Index Distal"] = "lIndex3";
            map["Left Middle Proximal"] = "lMid1";
            map["Left Middle Intermediate"] = "lMid2";
            map["Left Middle Distal"] = "lMid3";
            map["Left Ring Proximal"] = "lRing1";
            map["Left Ring Intermediate"] = "lRing2";
            map["Left Ring Distal"] = "lRing3";
            map["Left Little Proximal"] = "lPinky1";
            map["Left Little Intermediate"] = "lPinky2";
            map["Left Little Distal"] = "lPinky3";

            //Right Hand (Gen8)
            map["Right Thumb Proximal"] = "rThumb1";
            map["Right Thumb Intermediate"] = "rThumb2";
            map["Right Thumb Distal"] = "rThumb3";
            map["Right Index Proximal"] = "rIndex1";
            map["Right Index Intermediate"] = "rIndex2";
            map["Right Index Distal"] = "rIndex3";
            map["Right Middle Proximal"] = "rMid1";
            map["Right Middle Intermediate"] = "rMid2";
            map["Right Middle Distal"] = "rMid3";
            map["Right Ring Proximal"] = "rRing1";
            map["Right Ring Intermediate"] = "rRing2";
            map["Right Ring Distal"] = "rRing3";
            map["Right Little Proximal"] = "rPinky1";
            map["Right Little Intermediate"] = "rPinky2";
            map["Right Little Distal"] = "rPinky3";
        }

        #region unused

        // public static void ResetOptions()
        // {
        //     AutoImportDTUChanges = true;
        //     GenerateUnityPrefab = true;
        //     ReplaceSceneInstances = true;
        //     AutomateMecanimAvatarMappings = true;
        //     ReplaceMaterials = true;
        //     UseHighQualityTextures = true;
        //  
        // }

        //[MenuItem("Daz3D/Create Unity Prefab from selected DTU")]
        // public void MenuItemConvert()
        // {
        //     var activeObject = Selection.activeObject;
        //     var dtuPath = AssetDatabase.GetAssetPath(activeObject);
        //     var fbxPath = dtuPath.Replace(".dtu", ".fbx");
        //
        //     Import(dtuPath, fbxPath);
        // }


        // private void FixupStandardBasedMaterial(ref Material nuMat, GameObject fbxPrefab, string key /*, DTUData data*/)
        // {
        //     //todo need fixup missing textures from the json
        //     Debug.LogWarning("dtuData has " + data.Materials.Count + " materials ");
        //
        //     var modelPath = AssetDatabase.GetAssetPath(fbxPrefab);
        //     var nuTexturePath = Path.GetDirectoryName(modelPath);
        //     nuTexturePath = BuildUnityPath(nuTexturePath, fbxPrefab.name + "Textures___");
        //     nuTexturePath = AssetDatabase.GenerateUniqueAssetPath(nuTexturePath);
        //
        //     //walk data until find a material named with key
        //     foreach (var material in data.Materials)
        //     {
        //         if (material.MaterialName == key && false) //TODO hack to bypass unfinished fn
        //         {
        //             //walk properties and work on any with a texture path
        //             foreach (var property in material.Properties)
        //             {
        //                 if (!string.IsNullOrEmpty(property.Texture))
        //                 {
        //                     //and the daz folder has that texture 
        //                     if (File.Exists(property.Texture))
        //                     {
        //                         //copy it into the local textures folder
        //                         if (!Directory.Exists(nuTexturePath))
        //                             Directory.CreateDirectory(nuTexturePath);
        //
        //                         var nuTextureName = BuildUnityPath(nuTexturePath, Path.GetFileName(property.Texture));
        //
        //                         //TODO-----------------------------
        //                         //todo some diffuse maps are jpg with no alpha channel, 
        //                         //instead use the FBX's embedded/collected texture which already has alpha channel, 
        //                         //test whether that material already has a valid diffuse color texture
        //                         //if so, reimport that with the importer options below
        //
        //                         //copy the texture file from the daz folder to nuTexturePath
        //                         File.Copy(property.Texture, nuTextureName);
        //
        //                         TextureImporter importer = (TextureImporter)AssetImporter.GetAtPath(nuTextureName);
        //                         if (importer != null)
        //                         {
        //                             //todo twiddle other switches here, before the reimport happens only once
        //                             importer.alphaIsTransparency = KeyToTransparency(key);
        //                             importer.alphaSource = KeyToAlphaSource(key);
        //                             importer.convertToNormalmap = KeyToNormalMap(key);
        //                             importer.heightmapScale = KeyToHeightmapScale(key);
        //                             importer.normalmapFilter = KeyToNormalMapFilter(key);
        //                             importer.wrapMode = KeyToWrapMode(key);
        //
        //                             importer.SaveAndReimport();
        //                         }
        //                         else
        //                         {
        //                             Debug.LogWarning("texture " + nuTextureName + " is not a project asset.");
        //                         }
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }

        // private TextureImporterAlphaSource KeyToAlphaSource(string key)
        // {
        //     throw new NotImplementedException();
        // }
        //
        // private TextureWrapMode KeyToWrapMode(string key)
        // {
        //     throw new NotImplementedException();
        // }
        //
        // private TextureImporterNormalFilter KeyToNormalMapFilter(string key)
        // {
        //     throw new NotImplementedException();
        // }
        //
        // private float KeyToHeightmapScale(string key)
        // {
        //     throw new NotImplementedException();
        // }
        //
        // private bool KeyToNormalMap(string key)
        // {
        //     throw new NotImplementedException();
        // }
        //
        // private bool KeyToTransparency(string key)
        // {
        //     throw new NotImplementedException();
        // }

        //private void CustomizeMaterial(ref Material material, DazMaterialPropertiesInfo info)
        //{
        //    material.SetColor("_BaseColor", info.BaseColor);
        //    material.SetFloat("_SurfaceType", info.Transparent ? 1 : 0); //0 == opaque, 1 == transparent

        //    Texture mainTexture = material.mainTexture;
        //    CustomizeTexture(ref mainTexture, info.Transparent);

        //    var normalMap = material.GetTexture("_NormalMap");
        //    if (!IsValidNormalMap(normalMap))
        //        material.SetTexture("_NormalMap", null);//nuke the invalid normal map, if its a mistake


        //    material.SetFloat("_Metallic", info.Metallic);
        //    material.SetFloat("_Smoothness", info.Smoothness);
        //    material.SetInt("_MaterialID", (int)info.MaterialType);
        //    material.SetFloat("_DoubleSidedEnable", info.DoubleSided ? 0 : 1);
        //}


        // void CustomizeTexture(ref Texture texture, bool alphaIsTransparent)
        // {
        //     if (texture != null)
        //     {
        //         var texPath = AssetDatabase.GetAssetPath(texture);
        //         TextureImporter importer = (TextureImporter) GetAtPath(texPath);
        //         if (importer != null)
        //         {
        //             if (alphaIsTransparent && importer.DoesSourceTextureHaveAlpha())
        //             {
        //                 importer.alphaIsTransparency = true;
        //             }
        //
        //             //todo twiddle other switches here, before the reimport happens only once
        //             importer.SaveAndReimport();
        //         }
        //         else
        //             Debug.LogWarning("texture " + texture.name + " is not a project asset.");
        //     }
        //     else
        //         Debug.LogWarning("null texture");
        // }


        // bool IsValidNormalMap(Texture normalMap)
        // {
        //     if (normalMap == null)
        //         return false;
        //
        //     var nmPath = AssetDatabase.GetAssetPath(normalMap);
        //     TextureImporter importer = (TextureImporter) GetAtPath(nmPath);
        //     if (importer != null)
        //     {
        //         var settings = new TextureImporterSettings();
        //         importer.ReadTextureSettings(settings);
        //         return settings.textureType == TextureImporterType.NormalMap;
        //     }
        //
        //     Debug.LogWarning("texture " + normalMap.name + " is not a project asset.");
        //
        //     return true;
        // }


        // [MenuItem("Assets/Daz3D/Create Unity Prefab")]
        // static void DoStuffToSelectedDTU()
        // {
        //     CreateDTUPrefab(Selection.activeObject);
        //     Utilities.Log("Selected Transform is on " + Selection.activeTransform.gameObject.name + ".");
        // }

        // private void CreateDTUPrefab(Object activeObject)
        // {
        //     if (activeObject)
        //     {
        //         var dtuPath = AssetDatabase.GetAssetPath(activeObject);
        //         var fbxPath = dtuPath.Replace(".dtu", ".fbx");
        //
        //         Import(dtuPath, fbxPath);
        //     }
        // }

        // public static void ImportDTU(string path)
        // {
        //     Utilities.Log("ImportDTU for " + path);
        //
        //     FoldAll();
        //
        //     ImportEventRecord record = new ImportEventRecord();
        //     EventQueue.Enqueue(record);
        //
        //     var dtu = DTUConverter.ParseDTUFile(path);
        //
        //     var dtuObject = AssetDatabase.LoadAssetAtPath<Object>(path);
        //
        //     record.AddToken("Imported DTU file: " + path);
        //     record.AddToken(dtuObject.name, dtuObject, ENDLINE);
        //
        //     //UnityEngine.Debug.Log("DTU: " + dtu.AssetName + " contains: " + dtu.Materials.Count + " materials");
        //
        //     record.AddToken("Generated materials: ");
        //     foreach (var dtuMat in dtu.Materials)
        //     {
        //         var material = dtu.ConvertToUnity(dtuMat);
        //         _map.AddMaterial(material);
        //
        //         record.AddToken(material.name, material);
        //     }
        //
        //     record.AddToken(" based on DTU file.", null, ENDLINE);
        //
        //
        //     Bridge bridge = EditorWindow.GetWindow(typeof(Bridge)) as Bridge;
        //     if (bridge == null)
        //     {
        //         var consoleType = Type.GetType("ConsoleWindow,UnityEditor.dll");
        //         bridge = EditorWindow.CreateWindow<Bridge>(consoleType);
        //     }
        //
        //     bridge?.Focus();
        //
        //     //just a safeguard to keep the history data at a managable size (100 records)
        //     while (EventQueue.Count > 100)
        //     {
        //         EventQueue.Dequeue();
        //     }
        // }


        // enum MaterialID //these positions map to the bitflags in the compiled HDRP lit shader
        // {
        //     SSS = 0,
        //     Standard = 1,
        //     Anisotropy = 2,
        //     Iridescence = 3,
        //     SpecularColor = 4,
        //     Translucent = 5
        // }
        //
        // private enum StandardMaterialType
        // {
        //     Arms,
        //     Cornea,
        //     Ears,
        //     Eyelashes,
        //     EyeMoisture_1,
        //     EyeMoisture,
        //     EyeSocket,
        //     Face,
        //     Fingernails,
        //     Irises,
        //     Legs,
        //     Lips,
        //     Mouth,
        //     Pupils,
        //     Sclera,
        //     Teeth,
        //     Toenails,
        //     Torso
        // }

        #endregion
    }
}