using Newtonsoft.Json;
using Unity.EditorCoroutines.Editor;
using UnityEditor;
using UnityEditor.AssetImporters;
using UnityEngine;

namespace Daz3D
{
    [CustomEditor(typeof(DTUImporter))]
    public class DTUImporterEditor : AssetImporterEditor
    {
        [HideInInspector] [SerializeField] private Texture2D icon;

        // protected override bool useAssetDrawPreview => true;
        private JsonSerializerSettings settings;

        public override void OnInspectorGUI()
        {
            serializedObject.Update();
            var importer = serializedObject.targetObject as DTUImporter;
            // SerializedProperty Prefab = serializedObject.FindProperty("Prefab");

            // EditorGUILayout.PropertyField(Prefab, new GUIContent("Prefab", "The associated prefab."));

            EditorGUILayout.LabelField("Import Settings", EditorStyles.boldLabel);
            {
                EditorGUI.indentLevel++;
                SerializedProperty ApplySubdivisions = serializedObject.FindProperty("ApplySubdivisions");
                SerializedProperty UseHighQualityTextures = serializedObject.FindProperty("UseHighQualityTextures");
                SerializedProperty AutoImportDTUChanges = serializedObject.FindProperty("AutoImportDTUChanges");
                SerializedProperty GenerateUnityPrefab = serializedObject.FindProperty("GenerateUnityPrefab");
                SerializedProperty ReplaceSceneInstances = serializedObject.FindProperty("ReplaceSceneInstances");
                SerializedProperty AutomateMecanimAvatarMappings =
                    serializedObject.FindProperty("AutomateMecanimAvatarMappings");
                SerializedProperty ReplaceMaterials = serializedObject.FindProperty("ReplaceMaterials");

                EditorGUILayout.PropertyField(ApplySubdivisions,
                    new GUIContent("Apply Subdivisions", "If necessary, rebinds missing skin weights."));
                EditorGUILayout.PropertyField(UseHighQualityTextures,
                    new GUIContent("Use High Quality Textures", "Use Highest Quality for Textures"));
                EditorGUILayout.PropertyField(AutoImportDTUChanges,
                    new GUIContent("Auto-import changes", "Automatically import when DTU changes are detected"));
                EditorGUILayout.PropertyField(GenerateUnityPrefab,
                    new GUIContent("Generate Prefab", "Generate a Unity Prefab based on FBX and DTU"));
                EditorGUILayout.PropertyField(ReplaceSceneInstances,
                    new GUIContent("Replace Scene Instances", "Replace instances of Unity Prefab in active scene(s)"));
                EditorGUILayout.PropertyField(ReplaceSceneInstances,
                    new GUIContent("Replace Materials",
                        "Replace FBX materials with high quality Daz-shader materials"));

                EditorGUILayout.PropertyField(AutomateMecanimAvatarMappings,
                    new GUIContent("Mecanim Setup", "Automatically setup the Mecanim Avatar"));

                EditorGUI.indentLevel--;
            }

            // EditorGUILayout.LabelField("Summary");
            // SerializedProperty Figure = serializedObject.FindProperty("FigureType");
            // SerializedProperty Subdivisions = serializedObject.FindProperty("Subdivisions");
            //
            //
            // var summary = "Using: ";
            // summary += Figure.enumDisplayNames[Figure.enumValueIndex] + "\n";
            // var subdivText = "";
            //
            //
            if (importer is { })
            {
                //     foreach (var s in importer.Subdivisions)
                //     {
                //         subdivText += $"{s.AssetName} -> {s.Value}\n";
                //     }
                //
                //
                //     summary += "FBX Path: " + importer.path.Replace(".dtu", ".fbx");
                //
                //
                //     if (!string.IsNullOrEmpty(subdivText))
                //     {
                //         summary += "Subdivisions:\n" + subdivText;
                //     }
                //
                //     EditorGUILayout.HelpBox(summary, MessageType.Info);
                //


                if (GUILayout.Button(new GUIContent("Apply Subdivisions",
                    "This will generate a separate fbx file with the new skin weights")))
                {
                    //DazCoroutine.StartCoroutine(DazFBXUtils.ApplySubdivisions(importer.dtuFile.FBXFile,
                    //    importer.dtuFile));    
                    EditorCoroutineUtility.StartCoroutineOwnerless(DazFBXUtils.ApplySubdivisions(
                        importer.dtuFile.FBXFile,
                        importer.dtuFile));
                }

                if (GUILayout.Button(new GUIContent("Regenerate Prefab",
                    "This sets the right settings on the fbx file and generate a prefab from it.")))
                {
                    importer.GeneratePrefabFromFBX(importer.dtuFile.FBXFile, importer.dtuFile.FigureType);
                }

                if (GUILayout.Button(new GUIContent("Guess Genesis Type",
                    "[DEBUG]")))
                {
                    importer.dtuFile.FigureType = importer.dtuFile.AssetID.ToFigurePlatform();
                }

                if (GUILayout.Button(new GUIContent("Export JSON")))
                {
                    if (settings == null)
                    {
                        settings = new JsonSerializerSettings();
                        settings.Error = (serializer, err) => { err.ErrorContext.Handled = true; };
                    }


                    // Debug.Log(EditorJsonUtility.ToJson(importer.dtuFile));
                    Debug.Log(JsonConvert.SerializeObject(importer.dtuFile, settings));
                }
            }

            serializedObject.ApplyModifiedProperties();
            base.ApplyRevertGUI();
        }

        // public override bool HasPreviewGUI()
        // {
        //     return false;
        // }


        public override Texture2D RenderStaticPreview(string assetPath, Object[] subAssets, int width, int height)
        {
            Debug.Log("Render Static");
            if (icon != null) return icon;

            var logoPath = AssetDatabase.GUIDToAssetPath("dd78f3d9db910448d935cbca82edd9ff");
            var tx = AssetDatabase.LoadAssetAtPath<Texture2D>(logoPath);
            Debug.Log(logoPath);
            icon = new Texture2D(width, height);
            EditorUtility.CopySerialized(tx, icon);
            return icon;
        }
    }
}