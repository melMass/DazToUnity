using System;
using System.Collections.Generic;
using System.Linq;
using Autodesk.Fbx;
using MTB.Assets;
using UnityEngine;

namespace Daz3D
{
    public class DazFBXUtils
    {
        public static void ApplySubdivisions(string fbxPath, DTUSubdivision[] subdivisions)
        {
            Log($"Logging information from {fbxPath}");
            //var fbxFile = AssetDatabase.LoadAssetAtPath<GameObject>(fbxPath);

            // Initialize the SDK manager. This object handles memory management.
            var manager = FbxManager.Create();
            // Create the IO settings object.
            var ioSettings = FbxIOSettings.Create(manager, "Manager");
            manager.SetIOSettings(ioSettings);

            // Create an importer using the SDK manager.
            FbxImporter lImporter = FbxImporter.Create(manager, "");

            if (!lImporter.Initialize(fbxPath, -1, manager.GetIOSettings()))
            {
                Log("Call to FbxImporter::Initialize() failed.\n");
                Log($"Error returned: {lImporter.GetStatus().GetErrorString()}\n\n");
                return;
            }

            // Create a new scene so that it can be populated by the imported file.
            FbxScene lScene = FbxScene.Create(manager, "Root");
            // Import the contents of the file into the scene.
            lImporter.Import(lScene);

            // File format version numbers to be populated.
            lImporter.GetFileVersion(out int lFileMajor, out int lFileMinor, out int lFileRevision);


            // The file is imported, so get rid of the importer.
            lImporter.Destroy();

            // Print the nodes of the scene and their attributes recursively.
            // Note that we are not printing the root node because it should
            // not contain any attributes.
            FbxNode lRootNode = lScene.GetRootNode();

            var childs = GetChilds(lRootNode);

            Log($"Found {childs.Length} nodes.");

            // foreach (var node in childs.Where(n =>
            //     subdivisions.Select(s => s.AssetName).Where(sn => sn == n.GetName()).ToArray().Length > 0))
            // {

            Log($"Subdivisions Array Size: {subdivisions.Length}");

            foreach (var subd in subdivisions)
            {
                var name = subd.AssetName;
                var currentNode = childs.First(m => m.GetName() == name);

                Log($"Node {currentNode.GetName()} has {subd.Value} subdivisions");
            }

            // Destroy the SDK manager and all the other objects it was handling.
            manager.Destroy();
        }

        private static FbxNode[] GetChilds(FbxNode node)
        {
            var all = new List<FbxNode>();
            for (int i = 0; i < node.GetChildCount(); i++)
            {
                all.Add(node.GetChild(i));
                all.AddRange(GetChilds(node.GetChild(i)));
            }

            return all.ToArray();
        }

        private static void Log(object message)
        {
            Debug.Log($"<color=cyan>DAZ FBX</color>: {message}");
        }
    }
}