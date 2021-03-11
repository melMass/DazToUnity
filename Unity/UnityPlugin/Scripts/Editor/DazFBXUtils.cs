using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Autodesk.Fbx;
using UnityEngine;

namespace Daz3D
{
    public class DazFBXUtils
    {
        static void FindVertexNeedingWeight(int SearchFromVertex, FbxNode SceneNode, ref HashSet<int> NeedWeights,
            ref Dictionary<int, List<int>> VertexPolygons, ref HashSet<int> NoWeights, int Depth)
        {
            foreach (var PolygonIndex in VertexPolygons[SearchFromVertex])
            {
                for (int VertexIndex = 0; VertexIndex < SceneNode.GetMesh().GetPolygonSize(PolygonIndex); ++VertexIndex)
                {
                    int Vertex = SceneNode.GetMesh().GetPolygonVertex(PolygonIndex, VertexIndex);
                    if (NoWeights.Contains(Vertex))
                    {
                        NeedWeights.Add(Vertex);
                    }

                    if (Depth > 1)
                    {
                        FindVertexNeedingWeight(Vertex, SceneNode, ref NeedWeights, ref VertexPolygons, ref NoWeights,
                            Depth - 1);
                    }
                }
            }
        }

        static void
            GetWeights(FbxNode SceneNode, ref Dictionary<int, List<int>> VertexPolygons,
                ref Dictionary<int, double> ClusterWeights, FbxMatrix TargetMatrix, int
                    SearchFromVertex, ref HashSet<int> TouchedPolygons, ref HashSet<int> TouchedVertices,
                ref double WeightsOut, ref double DistancesOut, int Depth)
        {
            FbxVector4 TargetPosition;
            FbxVector4 TargetNormal;
            FbxVector4 TargetShearing;
            FbxVector4 TargetScale;
            double Sign = default;

            TargetMatrix.GetElements(out TargetPosition, out TargetNormal, out TargetShearing, out TargetScale,
                out Sign);

            //FbxVector4 VertexLocations = SceneNode.GetMesh().GetControlPoints();
            foreach (int PolygonIndex in VertexPolygons[SearchFromVertex])
            {
                if (TouchedPolygons.Contains(PolygonIndex)) continue;
                TouchedPolygons.Add(PolygonIndex);
                FbxVector4 NeedWeightVertexNormal;
                /*for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                {
                    int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                    if (Vertex == SearchFromVertex)
                    {
                        SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, NeedWeightVertexNormal);
                    }
                }*/
                // Set the vertices with no weight, to be the average of the ones with weight.
                for (int VertexIndex = 0;
                    VertexIndex < SceneNode.GetMesh().GetPolygonSize(PolygonIndex);
                    ++VertexIndex)
                {
                    int Vertex = SceneNode.GetMesh().GetPolygonVertex(PolygonIndex, VertexIndex);
                    if (TouchedVertices.Contains(Vertex)) continue;
                    TouchedVertices.Add(Vertex);
                    if (ClusterWeights.Keys.Contains(Vertex))
                    {
                        FbxVector4 CompareVertexNormal;
                        SceneNode.GetMesh().GetPolygonVertexNormal(PolygonIndex, VertexIndex, out CompareVertexNormal);
                        //double DotProduct = CompareVertexNormal.DotProduct(NeedWeightVertexNormal);
                        FbxVector4 CompareLocation = SceneNode.GetMesh().GetControlPointAt(Vertex);
                        FbxVector4 CompareScale = new FbxVector4(1.0f, 1.0f, 1.0f);
                        FbxMatrix CompareMatrix = new FbxMatrix(CompareLocation, CompareVertexNormal, CompareScale);
                        //CompareLocation.
                        FbxMatrix AdjustedMatrix = CompareMatrix * TargetMatrix.Inverse();

                        AdjustedMatrix.GetElements(out var AdjustedComparePosition, out var AdjustedCompareNormal,
                            out var AdjustedCompareShearing, out var AdjustedCompareScale, out var AdjustedCompareSign);

                        double InverseDotProduct = 1 - AdjustedCompareNormal.DotProduct(TargetNormal);
                        double AngleAdjustment = InverseDotProduct * 3.14;
                        //AdjustedComparePosition[2] = 0.0;
                        //FbxVector4 Test = TargetPosition.;
                        //double VertexDistance = AdjustedComparePosition.Length();//AdjustedComparePosition.Distance(TargetPosition);
                        double VertexDistance = CompareLocation.Distance(TargetPosition) * AngleAdjustment;
                        //VertexDistance = VertexDistance * VertexDistance* VertexDistance* VertexDistance* VertexDistance* VertexDistance* VertexDistance* VertexDistance;
                        /*if (DotProduct > 0.0)
                        {
                            VertexDistance = VertexDistance * DotProduct;
                        }*/

                        //double AdditionalWeightCount = FurthestDistance / VertexDistance;
                        //double AdditionalWeight = AdditionalWeightCount * ClusterWeights[Vertex];
                        double AdditionalWeightCount = 1 / VertexDistance;
                        double AdditionalWeight = ClusterWeights[Vertex] / VertexDistance;

                        WeightsOut += AdditionalWeight;
                        DistancesOut += AdditionalWeightCount;
                    }

                    if (Depth > 1)
                    {
                        GetWeights(SceneNode, ref VertexPolygons, ref ClusterWeights, TargetMatrix, Vertex,
                            ref TouchedPolygons,
                            ref TouchedVertices, ref WeightsOut, ref DistancesOut, Depth - 1);
                    }
                }
            }
        }


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
                if (subd.Value <= 0) return;

                var currentNode = childs.First(m => m.GetName() == name);
                
                Utilities.Log($"Found node with subdivisions: {currentNode.GetName()}");
                var mesh = currentNode.GetMesh();
                var deformerCount = mesh.GetDeformerCount();
                Utilities.Log($"with mesh {mesh.GetName()} containing {deformerCount} deformer");
                for (int DeformerIndex = 0; DeformerIndex < deformerCount; ++DeformerIndex)
                {
                    var deformer = mesh.GetDeformer(DeformerIndex);
                    
                    Utilities.Log($"Deformer {DeformerIndex}: {deformer.GetName()} - {deformer.GetDeformerType()} ");
                    
                    if (deformer.GetDeformerType() != FbxDeformer.EDeformerType.eSkin) continue;
                    var skin = deformer.AsSkin();
                    
                    Utilities.Log("Found a skin deformer");
                    HashSet<int> NoWeights = new HashSet<int>();
                    HashSet<int> HasWeights = new HashSet<int>();
                    Dictionary<int, List<int>> VertexPolygons = new Dictionary<int, List<int>>();
                    Dictionary<int, FbxVector4> VertexNormals = new Dictionary<int, FbxVector4>();
                    // FbxVector4[] VertexLocations = new FbxVector4[mesh.GetControlPointsCount()];
                    FbxVector4[] VertexLocations = new FbxVector4[mesh.GetControlPointsCount()];
                    for (int c = 0; c < mesh.GetControlPointsCount(); c++)
                    {
                        VertexLocations[c] = mesh.GetControlPointAt(c);
                    }

                    // iterate the polygons
                    for (int PolygonIndex = 0;
                        PolygonIndex < mesh.GetPolygonCount();
                        ++PolygonIndex)
                    {
                        for (int VertexIndex = 0; VertexIndex < mesh.GetPolygonSize(PolygonIndex); ++VertexIndex)
                        {
                            int Vertex = mesh.GetPolygonVertex(PolygonIndex, VertexIndex);
                            if (!VertexPolygons.Keys.Contains(Vertex))
                            {
                                List<int> PolygonList = new List<int>();
                                VertexPolygons.Add(Vertex, PolygonList);
                            }

                            VertexPolygons[Vertex].Add(PolygonIndex);
                            if (HasWeights.Contains(Vertex)) continue;
                            if (NoWeights.Contains(Vertex)) continue;

                            FbxVector4 VertexNormal;
                            // Vertex, 
                            mesh.GetPolygonVertexNormal(PolygonIndex, VertexIndex, out VertexNormal);
                            VertexNormals.Add(Vertex, VertexNormal);

                            NoWeights.Add(Vertex);
                        }
                    }

                    for (int ClusterIndex = 0; ClusterIndex < skin.GetClusterCount(); ++ClusterIndex)
                    {
                        FbxCluster Cluster = skin.GetCluster(ClusterIndex);

                        for (int ClusterVertexIndex = 0;
                            ClusterVertexIndex < Cluster.GetControlPointIndicesCount();
                            ++ClusterVertexIndex)
                        {
                            int Vertex = Cluster.GetControlPointIndexAt(ClusterVertexIndex);
                            //if (Cluster->GetControlPointIndices()[ClusterVertexIndex] == Vertex)
                            {
                                NoWeights.Remove(Vertex);
                                HasWeights.Add(Vertex);
                            }
                        }
                    }

                    if (HasWeights.Count > 0 && NoWeights.Count > 0)
                    {
                        //for (int NoWeightVertex : NoWeights)
                        {
                            // FScopedSlowTask SubdivisionTask(Skin->
                            // GetClusterCount(), LOCTEXT("DazToUnrealSudAutoWeightTask",
                            // "Creating Subdivision Weights for Cluster:"));

                            for (int ClusterIndex = 0; ClusterIndex < skin.GetClusterCount(); ++ClusterIndex)
                            {
                                // SubdivisionTask.EnterProgressFrame();
                                FbxCluster Cluster = skin.GetCluster(ClusterIndex);
                                int ClusterVertexCount = Cluster.GetControlPointIndicesCount();

                                //Make a map of all the weights for the cluster
                                Dictionary<int, double> ClusterWeights = new Dictionary<int, double>();
                                //TMap<int, double> ClusterVertex;
                                for (int ClusterVertexIndex = 0;
                                    ClusterVertexIndex < ClusterVertexCount;
                                    ++ClusterVertexIndex)
                                {
                                    int WeightVertex = Cluster.GetControlPointIndexAt(ClusterVertexIndex);
                                    double Weight = Cluster.GetControlPointWeightAt(ClusterVertexIndex);
                                    ClusterWeights.Add(WeightVertex, Weight);
                                    //ClusterVertex.Add(ClusterVertexIndex, WeightVertex);
                                }

                                Dictionary<int, double> WeightsToAdd = new Dictionary<int, double>();
                                // TMap<int, double> 
                                for (int ClusterVertexIndex = 0;
                                    ClusterVertexIndex < ClusterVertexCount;
                                    ++ClusterVertexIndex)
                                {
                                    int WeightVertex = Cluster.GetControlPointIndexAt(ClusterVertexIndex);
                                    //if(NoWeights.Contains(NoWeightVertex))
                                    {
                                        //TArray<int> NeedWeights;
                                        HashSet<int> NeedWeights = new HashSet<int>();
                                        FindVertexNeedingWeight(WeightVertex, currentNode, ref NeedWeights,
                                            ref VertexPolygons, ref NoWeights, 1);
                                        /*for (int PolygonIndex : VertexPolygons[WeightVertex])
                                            {
                                                for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                                                {
                                                    int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                                                    if (NoWeights.Contains(Vertex))
                                                    {
                                                        NeedWeights.AddUnique(Vertex);
                                                    }
                                                }
                                            }*/

                                        foreach (int NeedWeightVertex in NeedWeights)
                                        {
                                            double WeightCount = 0.0f;
                                            double Weight = 0.0f;

                                            FbxVector4 NeedWeightVertexLocation = VertexLocations[NeedWeightVertex];
                                            HashSet<int> TouchedPolygons = new HashSet<int>();
                                            HashSet<int> TouchedVertices = new HashSet<int>();
                                            FbxVector4 ScaleVector = new FbxVector4(1.0, 1.0, 1.0);
                                            FbxMatrix VertexMatrix = new FbxMatrix(NeedWeightVertexLocation,
                                                VertexNormals[NeedWeightVertex], ScaleVector);
                                            GetWeights(currentNode, ref VertexPolygons, ref ClusterWeights,
                                                VertexMatrix, NeedWeightVertex, ref TouchedPolygons,
                                                ref TouchedVertices,
                                                ref Weight, ref WeightCount, 1);


                                            /*for (int PolygonIndex : VertexPolygons[NeedWeightVertex])
                                                {
                                                    FbxVector4 NeedWeightVertexNormal;
                                                    for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                                                    {
                                                        int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                                                        if (Vertex == NeedWeightVertex)
                                                        {
                                                            SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, NeedWeightVertexNormal);
                                                        }
                                                    }
                                                    // Set the vertices with no weight, to be the average of the ones with weight.
                                                    for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                                                    {
                                                        int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                                                        if (ClusterWeights.Contains(Vertex))
                                                        {
                                                            FbxVector4 CompareVertexNormal;
                                                            SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, CompareVertexNormal);
                                                            double DotProduct = CompareVertexNormal.DotProduct(NeedWeightVertexNormal);
                                                            FbxVector4 CompareLocation = VertexLocations[Vertex];
                                                            double VertexDistance = NeedWeightVertexLocation.Distance(CompareLocation);
                                                            if (DotProduct > 0.0)
                                                            {
                                                                VertexDistance = VertexDistance * DotProduct;
                                                            }
                                                            //double AdditionalWeightCount = FurthestDistance / VertexDistance;
                                                            //double AdditionalWeight = AdditionalWeightCount * ClusterWeights[Vertex];
                                                            double AdditionalWeightCount = 1 / VertexDistance;
                                                            double AdditionalWeight = ClusterWeights[Vertex] /  VertexDistance;
                                                            Weight += AdditionalWeight;
                                                            WeightCount += AdditionalWeightCount;
                                                        }
                                                    }
                                                }*/
                                            if (WeightCount > 0)
                                            {
                                                if (!WeightsToAdd.Keys.Contains(NeedWeightVertex))
                                                    WeightsToAdd.Add(NeedWeightVertex, Weight / (double) WeightCount);
                                                    //Cluster->AddControlPointIndex(NeedWeightVertex, Weight / (double)WeightCount);
                                            }
                                        }
                                    }
                                }

                                foreach (var WeightToAdd in WeightsToAdd)
                                {
                                    Cluster.AddControlPointIndex(WeightToAdd.Key, WeightToAdd.Value);
                                }
                            }
                        }
                    }
                    else
                    {
                         Utilities.Log("Something went wrong");
                    }
                }

                // Create an exporter.
                FbxExporter Exporter = FbxExporter.Create(manager, "");
                // Initialize the exporter by providing a filename.
                if (!Exporter.Initialize(Path.Combine(Path.GetDirectoryName(fbxPath), name + "_Updated.fbx"), -1,
                    manager.GetIOSettings()))
                {
                    return;
                }

                // Set compatibility to 2014
                Exporter.SetFileExportVersion("FBX201400");
                // Export the scene.
                bool Status = Exporter.Export(lScene);
                 // Destroy the exporter.
	            Exporter.Destroy();
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