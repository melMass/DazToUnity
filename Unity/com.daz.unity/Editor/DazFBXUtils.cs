using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
//using FbxSdk;
using UnityEditor;
using UnityEngine;

namespace Daz3D
{
    public class DazFBXUtils
    {
        /*static void RenameDuplicateBones(FbxNode RootNode)
        {
            Dictionary<string, int> ExistingBones = new Dictionary<string, int>();
            RenameDuplicateBones(RootNode, ref ExistingBones);
        }

        static void RenameDuplicateBones(FbxNode RootNode, ref Dictionary<string, int> ExistingBones)
        {
            if (RootNode == null) return;

            FbxNodeAttribute Attr = RootNode.GetNodeAttribute();
            if (Attr != null && Attr.GetAttributeType() == FbxNodeAttribute.EType.eSkeleton)
            {
                var BoneName = RootNode.GetName();
                if (ExistingBones.Keys.Contains(BoneName))
                {
                    ExistingBones[BoneName] += 1;
                    BoneName = $"{BoneName}_RENAMED_{ExistingBones[BoneName]}";

                    RootNode.SetName(BoneName);
                }
                else
                {
                    ExistingBones.Add(BoneName, 1);
                }
            }

            for (int ChildIndex = 0; ChildIndex < RootNode.GetChildCount(); ++ChildIndex)
            {
                FbxNode ChildNode = RootNode.GetChild(ChildIndex);
                RenameDuplicateBones(ChildNode, ref ExistingBones);
            }
        }*/


        /*static void FindVertexNeedingWeight(int SearchFromVertex, FbxNode SceneNode, ref HashSet<int> NeedWeights,
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
        }*/

        /*static void GetWeights(FbxNode SceneNode, ref Dictionary<int, List<int>> VertexPolygons,
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
                *//*for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                {
                    int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                    if (Vertex == SearchFromVertex)
                    {
                        SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, NeedWeightVertexNormal);
                    }
                }*//*
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
                        *//*if (DotProduct > 0.0)
                        {
                            VertexDistance = VertexDistance * DotProduct;
                        }*//*

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

*/
      /*  static void FixClusterTranformLinks(FbxScene Scene, FbxNode RootNode)
        {
            FbxGeometry NodeGeometry = RootNode.GetGeometry();

            // Create missing weights
            if (NodeGeometry != null)
            {
                for (int DeformerIndex = 0; DeformerIndex < NodeGeometry.GetDeformerCount(); ++DeformerIndex)
                {
                    FbxSkin Skin = NodeGeometry.GetDeformer(DeformerIndex).AsSkin();
                    if (Skin != null)
                    {
                        for (int ClusterIndex = 0; ClusterIndex < Skin.GetClusterCount(); ++ClusterIndex)
                        {
                            // Get the current tranform
                            FbxAMatrix Matrix = new FbxAMatrix();
                            ;
                            FbxCluster Cluster = Skin.GetCluster(ClusterIndex);
                            Matrix = Cluster.GetTransformLinkMatrix(Matrix);

                            // Update the rotation
                            var Rotation = Cluster.GetLink().GetPostRotation(FbxNode.EPivotSet.eSourcePivot);
                            Matrix.SetR(Rotation);
                            Cluster.SetTransformLinkMatrix(Matrix);
                        }
                    }
                }
            }

            for (int ChildIndex = 0; ChildIndex < RootNode.GetChildCount(); ++ChildIndex)
            {
                FbxNode ChildNode = RootNode.GetChild(ChildIndex);
                FixClusterTranformLinks(Scene, ChildNode);
            }
        }
*/
       /* public static IEnumerator ApplySubdivisions(string fbxPath, DTUFile dtuFile, Action OnDone = default)
        {
            Log($"Logging information from {fbxPath}");
            //var fbxFile = AssetDatabase.LoadAssetAtPath<GameObject>(fbxPath);
            yield return new WaitForEndOfFrame();

            // Initialize the SDK manager. This object handles memory management.
            var manager = FbxManager.Create();
            // Create the IO settings object.
            var ioSettings = FbxIOSettings.Create(manager, "Manager");
            manager.SetIOSettings(ioSettings);

            // Create the geometry converter
            FbxGeometryConverter GeometryConverter = new FbxGeometryConverter(manager);


            // Create an importer using the SDK manager.
            FbxImporter lImporter = FbxImporter.Create(manager, "");

            yield return new WaitForEndOfFrame();

            if (!lImporter.Initialize(fbxPath, -1, manager.GetIOSettings()))
            {
                Log("Call to FbxImporter::Initialize() failed.\n");
                Log($"Error returned: {lImporter.GetStatus().GetErrorString()}\n\n");
                yield break;
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

            // Find the root bone.  There should only be one bone off the scene root
            FbxNode RootBone = null;
            string RootBoneName = "";
            for (int ChildIndex = 0; ChildIndex < lRootNode.GetChildCount(); ++ChildIndex)
            {
                FbxNode ChildNode = lRootNode.GetChild(ChildIndex);
                FbxNodeAttribute Attr = ChildNode.GetNodeAttribute();
                if (Attr != null && Attr.GetAttributeType() == FbxNodeAttribute.EType.eSkeleton)
                {
                    RootBone = ChildNode;
                    RootBoneName = RootBone.GetName();
                    RootBone.SetName("root");
                    Attr.SetName("root");
                    break;
                }
            }

            var fixBoneRotation = false;
            // Daz Studio puts the base bone rotations in a different place than Unreal expects them.
            if (fixBoneRotation && RootBone != null)
            {
                FixClusterTranformLinks(lScene, RootBone);
            }

            // If this is a skeleton mesh, but a root bone wasn't found, it may be a scene under a group node or something similar
            // So create a root node.
            if (RootBone == null)
            {
                RootBoneName = dtuFile.AssetName;

                FbxSkeleton NewRootNodeAttribute = FbxSkeleton.Create(lScene, "root");
                NewRootNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eRoot);
                NewRootNodeAttribute.Size.Set(1.0);
                RootBone = FbxNode.Create(lScene, "root");
                RootBone.SetNodeAttribute(NewRootNodeAttribute);
                RootBone.LclTranslation.Set(new FbxDouble3(0));


                for (int ChildIndex = lRootNode.GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
                {
                    FbxNode ChildNode = lRootNode.GetChild(ChildIndex);
                    RootBone.AddChild(ChildNode);
                    FbxSkeleton ChildSkeleton = ChildNode.GetSkeleton();
                    if (ChildSkeleton != null)
                    {
                        if (ChildSkeleton.GetSkeletonType() == FbxSkeleton.EType.eRoot)
                        {
                            ChildSkeleton.SetSkeletonType(FbxSkeleton.EType.eLimb);
                        }
                    }
                }

                lRootNode.AddChild(RootBone);
            }

            RenameDuplicateBones(RootBone);

            #region Subdivisions

            for (int NodeIndex = 0; NodeIndex < lScene.GetNodeCount(); ++NodeIndex)
            {
                FbxNode SceneNode = lScene.GetNode(NodeIndex);
                if (SceneNode == null)
                {
                    continue;
                }

                FbxGeometry NodeGeometry = SceneNode.GetGeometry();

                // Detach geometry from the skeleton
                if (NodeGeometry != null)
                {
                    if (SceneNode.GetParent() != null &&
                        SceneNode.GetParent().GetNodeAttribute() != null &&
                        SceneNode.GetParent().GetNodeAttribute().GetAttributeType() == FbxNodeAttribute.EType.eSkeleton)
                    {
                        SceneNode.GetParent().RemoveChild(SceneNode);
                        lRootNode.AddChild(SceneNode);
                    }
                }

                // var childs = GetChilds(lRootNode);
                // Log($"Found {childs.Length} nodes.");

                // foreach (var node in childs.Where(n =>
                //     subdivisions.Select(s => s.AssetName).Where(sn => sn == n.GetName()).ToArray().Length > 0))
                // {

                foreach (string name in from subd in dtuFile.Subdivisions
                    let name = subd.AssetName
                    where subd.Value > 0
                    where SceneNode.GetName() == name
                    select name)
                {
                    var mesh = SceneNode.GetMesh();
                    var deformerCount = mesh.GetDeformerCount();
                    Utilities.Log(
                        $"Found node with subdivisions: {name} with mesh {mesh.GetName()} containing {deformerCount} deformer");


                    for (int DeformerIndex = 0; DeformerIndex < deformerCount; ++DeformerIndex)
                    {
                        var deformer = mesh.GetDeformer(DeformerIndex);

                        Utilities.Log(
                            $"Deformer {DeformerIndex}: {deformer.GetName()} - {deformer.GetDeformerType()} ");

                        if (deformer.GetDeformerType() != FbxDeformer.EDeformerType.eSkin) continue;
                        var skin = deformer.AsSkin();

                        Utilities.Log("Found a skin deformer");

                        HashSet<int> NoWeights = new HashSet<int>();
                        HashSet<int> HasWeights = new HashSet<int>();

                        Dictionary<int, List<int>> VertexPolygons = new Dictionary<int, List<int>>();
                        Dictionary<int, FbxVector4> VertexNormals = new Dictionary<int, FbxVector4>();

                        // FbxVector4[] VertexLocations = new FbxVector4[mesh.GetControlPointsCount()];
                        //
                        // for (int c = 0; c < mesh.GetControlPointsCount(); c++)
                        // {
                        //     VertexLocations[c] = mesh.GetControlPointAt(c);
                        // }

                        Utilities.Log($"Iterating over all the polygons of {mesh.GetName()} to find missing weights");

                        // iterate the polygons to find missing weights
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


                        var clusterCount = skin.GetClusterCount();

                        Utilities.Log($"Iterating over all the found cluster (found {clusterCount} clusters)");

                        for (int ClusterIndex = 0; ClusterIndex < clusterCount; ++ClusterIndex)
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

                            yield return new WaitForEndOfFrame();
                        }


                        Utilities.Log($"Found {NoWeights.Count} vertices with missing weights");


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
                                            FindVertexNeedingWeight(WeightVertex, SceneNode, ref NeedWeights,
                                                ref VertexPolygons, ref NoWeights, 1);
                                            *//*for (int PolygonIndex : VertexPolygons[WeightVertex])
                                                {
                                                    for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
                                                    {
                                                        int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
                                                        if (NoWeights.Contains(Vertex))
                                                        {
                                                            NeedWeights.AddUnique(Vertex);
                                                        }
                                                    }
                                                }*//*


                                            foreach (int NeedWeightVertex in NeedWeights)
                                            {
                                                double WeightCount = 0.0f;
                                                double Weight = 0.0f;

                                                FbxVector4 NeedWeightVertexLocation =
                                                    mesh.GetControlPointAt(NeedWeightVertex);
                                                HashSet<int> TouchedPolygons = new HashSet<int>();
                                                HashSet<int> TouchedVertices = new HashSet<int>();
                                                FbxVector4 ScaleVector = new FbxVector4(1.0, 1.0, 1.0);
                                                FbxMatrix VertexMatrix = new FbxMatrix(NeedWeightVertexLocation,
                                                    VertexNormals[NeedWeightVertex], ScaleVector);
                                                GetWeights(SceneNode, ref VertexPolygons, ref ClusterWeights,
                                                    VertexMatrix, NeedWeightVertex, ref TouchedPolygons,
                                                    ref TouchedVertices,
                                                    ref Weight, ref WeightCount, 1);


                                                *//*for (int PolygonIndex : VertexPolygons[NeedWeightVertex])
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
                                                    }*//*
                                                if (WeightCount > 0)
                                                {
                                                    if (!WeightsToAdd.Keys.Contains(NeedWeightVertex))
                                                        WeightsToAdd.Add(NeedWeightVertex,
                                                            Weight / (double) WeightCount);
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
                }
            }

            #endregion

            #region Add IK bones

            //  Utilities.Log($"Adding IK Bones");
            //
            // if (RootBone != null)
            // {
            //     // ik_foot_root
            //     FbxNode IKRootNode = lScene.FindNodeByName("ik_foot_root");
            //     if (IKRootNode == null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKRootNodeAttribute =
            //             FbxSkeleton.Create(lScene, "ik_foot_root");
            //         IKRootNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKRootNodeAttribute.Size.Set(1.0);
            //         IKRootNode = FbxNode.Create(lScene, "ik_foot_root");
            //         IKRootNode.SetNodeAttribute(IKRootNodeAttribute);
            //         IKRootNode.LclTranslation.Set(new FbxDouble3());
            //         RootBone.AddChild(IKRootNode);
            //     }
            //
            //     // ik_foot_l
            //     FbxNode IKFootLNode = lScene.FindNodeByName("ik_foot_l");
            //     FbxNode FootLNode = lScene.FindNodeByName("lFoot");
            //     if (IKFootLNode == null && FootLNode != null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKFootLNodeAttribute =
            //             FbxSkeleton.Create(lScene, "ik_foot_l");
            //         IKFootLNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKFootLNodeAttribute.Size.Set(1.0);
            //         IKFootLNode = FbxNode.Create(lScene, "ik_foot_l");
            //         IKFootLNode.SetNodeAttribute(IKFootLNodeAttribute);
            //         FbxVector4 FootLocation = FootLNode.EvaluateGlobalTransform().GetT();
            //         IKFootLNode.LclTranslation.Set(new FbxDouble3(FootLocation.X, FootLocation.Y,
            //             FootLocation.Z));
            //         IKRootNode.AddChild(IKFootLNode);
            //     }
            //
            //     // ik_foot_r
            //     FbxNode IKFootRNode = lScene.FindNodeByName("ik_foot_r");
            //     FbxNode FootRNode = lScene.FindNodeByName("rFoot");
            //     if (IKFootRNode == null && FootRNode != null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKFootRNodeAttribute =
            //             FbxSkeleton.Create(lScene, "ik_foot_r");
            //         IKFootRNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKFootRNodeAttribute.Size.Set(1.0);
            //         IKFootRNode = FbxNode.Create(lScene, "ik_foot_r");
            //         IKFootRNode.SetNodeAttribute(IKFootRNodeAttribute);
            //         FbxVector4 FootLocation = FootRNode.EvaluateGlobalTransform().GetT();
            //         IKFootRNode.LclTranslation.Set(new FbxDouble3(FootLocation.X, FootLocation.Y,
            //             FootLocation.Z));
            //         IKRootNode.AddChild(IKFootRNode);
            //     }
            //
            //     // ik_hand_root
            //     FbxNode IKHandRootNode = lScene.FindNodeByName("ik_hand_root");
            //     if (IKHandRootNode == null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKHandRootNodeAttribute = FbxSkeleton.Create(lScene, "ik_hand_root");
            //         IKHandRootNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKHandRootNodeAttribute.Size.Set(1.0);
            //         IKHandRootNode = FbxNode.Create(lScene, "ik_hand_root");
            //         IKHandRootNode.SetNodeAttribute(IKHandRootNodeAttribute);
            //         IKHandRootNode.LclTranslation.Set(new FbxDouble3(0));
            //         RootBone.AddChild(IKHandRootNode);
            //     }
            //
            //     // ik_hand_gun
            //     FbxNode IKHandGunNode = lScene.FindNodeByName("ik_hand_gun");
            //     FbxNode HandRNode = lScene.FindNodeByName("rHand");
            //     if (IKHandGunNode == null && HandRNode != null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKHandGunNodeAttribute = FbxSkeleton.Create(lScene, "ik_hand_gun");
            //         IKHandGunNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKHandGunNodeAttribute.Size.Set(1.0);
            //         IKHandGunNode = FbxNode.Create(lScene, "ik_hand_gun");
            //         IKHandGunNode.SetNodeAttribute(IKHandGunNodeAttribute);
            //         FbxVector4 HandLocation = HandRNode.EvaluateGlobalTransform().GetT();
            //         IKHandGunNode.LclTranslation.Set(new FbxDouble3(HandLocation.X, HandLocation.Y,
            //             HandLocation.Z));
            //         IKHandRootNode.AddChild(IKHandGunNode);
            //     }
            //
            //     // ik_hand_r
            //     FbxNode IKHandRNode = lScene.FindNodeByName("ik_hand_r");
            //     //FbxNode* HandRNode = Scene.FindNodeByName("rHand");
            //     //FbxNode* IKHandGunNode = Scene.FindNodeByName("ik_hand_gun");
            //     if (IKHandRNode == null && HandRNode != null && IKHandGunNode != null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKHandRNodeAttribute = FbxSkeleton.Create(lScene, "ik_hand_r");
            //         IKHandRNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKHandRNodeAttribute.Size.Set(1.0);
            //         IKHandRNode = FbxNode.Create(lScene, "ik_hand_r");
            //         IKHandRNode.SetNodeAttribute(IKHandRNodeAttribute);
            //         IKHandRNode.LclTranslation.Set(new FbxDouble3(0));
            //         IKHandGunNode.AddChild(IKHandRNode);
            //     }
            //
            //     // ik_hand_l
            //     FbxNode IKHandLNode = lScene.FindNodeByName("ik_hand_l");
            //     FbxNode HandLNode = lScene.FindNodeByName("lHand");
            //     //FbxNode* IKHandGunNode = Scene.FindNodeByName(TCHAR_TO_UTF8(TEXT("ik_hand_gun")));
            //     if (IKHandLNode == null && HandLNode != null && IKHandGunNode != null)
            //     {
            //         // Create IK Root 
            //         FbxSkeleton IKHandRNodeAttribute =
            //             FbxSkeleton.Create(lScene, "ik_hand_l");
            //         IKHandRNodeAttribute.SetSkeletonType(FbxSkeleton.EType.eLimbNode);
            //         IKHandRNodeAttribute.Size.Set(1.0);
            //         IKHandLNode = FbxNode.Create(lScene, "ik_hand_l");
            //         IKHandLNode.SetNodeAttribute(IKHandRNodeAttribute);
            //         FbxVector4 HandLocation = HandLNode.EvaluateGlobalTransform().GetT();
            //         FbxVector4 ParentLocation = IKHandGunNode.EvaluateGlobalTransform().GetT();
            //         var tr = HandLocation - ParentLocation;
            //         IKHandLNode.LclTranslation.Set(new FbxDouble3(tr.X, tr.Y, tr.Z));
            //         IKHandGunNode.AddChild(IKHandLNode);
            //     }
            // }

            #endregion

            // Get a list of morph name mappings
            Dictionary<string, string> MorphMappings = new Dictionary<string, string>();

            // TODO: Add to settings
            var UseInternalMorphName = false;
            Utilities.Log($"Not using internal morph names (I need to add it to the dtuFile options)");

            // TArray<TSharedPtr<FJsonValue>> morphList = JsonObject->GetArrayField(TEXT("Morphs"));
            var morphList = dtuFile.Morphs; // TO REPLACE WITH MORPHS FROM DTU
            for (int i = 0; i < morphList.Count; i++)
            {
                var morph = morphList[i];
                var MorphName = morph.Name;
                var MorphLabel = morph.Label;

                // Daz Studio seems to strip the part of the name before a period when exporting the morph to FBX
                if (MorphName.Contains("."))
                {
                    MorphName = MorphName.Split('.')[1];
                }

                if (UseInternalMorphName)
                {
                    MorphMappings.Add(MorphName, MorphName);
                }
                else
                {
                    MorphMappings.Add(MorphName, MorphLabel);
                }
            }

            // ~~Combine clothing and body morphs~~
            Utilities.Log($"Renaming Morphs");

            for (int NodeIndex = 0; NodeIndex < lScene.GetNodeCount(); ++NodeIndex)
            {
                FbxNode SceneNode = lScene.GetNode(NodeIndex);
                if (SceneNode == null)
                {
                    continue;
                }

                FbxGeometry NodeGeometry = SceneNode.GetGeometry();
                if (NodeGeometry != null)
                {
                    int BlendShapeDeformerCount = NodeGeometry.GetDeformerCount(FbxDeformer.EDeformerType.eBlendShape);
                    for (int BlendShapeIndex = 0;
                        BlendShapeIndex < BlendShapeDeformerCount;
                        ++BlendShapeIndex)
                    {
                        FbxBlendShape BlendShape = NodeGeometry.GetBlendShapeDeformer(BlendShapeIndex);
                        int BlendShapeChannelCount = BlendShape.GetBlendShapeChannelCount();

                        HashSet<FbxBlendShapeChannel> ChannelsToRemove = new HashSet<FbxBlendShapeChannel>();
                        for (int ChannelIndex = 0; ChannelIndex < BlendShapeChannelCount; ++ChannelIndex)
                        {
                            FbxBlendShapeChannel Channel = BlendShape.GetBlendShapeChannel(ChannelIndex);
                            if (Channel != null)
                            {
                                //var ChannelName = Channel.GetNameOnly();
                                var ChannelName = Channel.GetNameWithoutNameSpacePrefix();


                                var NewChannelName = ChannelName.Split(new string[] {"__"}, StringSplitOptions.None)[1];

                                if (MorphMappings.Keys.Contains(NewChannelName))
                                {
                                    NewChannelName = MorphMappings[NewChannelName];
                                    Channel.SetName(NewChannelName);
                                }
                                else
                                {
                                    ChannelsToRemove.Add(Channel);
                                }
                            }
                        }

                        foreach (FbxBlendShapeChannel Channel in ChannelsToRemove)
                        {
                            BlendShape.RemoveBlendShapeChannel(Channel);
                        }
                    }
                }
            }

            Utilities.Log($"Initializing the exporter.");

            yield return new WaitForEndOfFrame();


            // Create an exporter.
            FbxExporter Exporter = FbxExporter.Create(manager, "");
            // Initialize the exporter by providing a filename.
            var outPath = Path.Combine(Path.GetDirectoryName(fbxPath), dtuFile.AssetName + "_Updated.fbx");
            if (!Exporter.Initialize(outPath, -1,
                manager.GetIOSettings()))
            {
                yield break;
            }

            Utilities.Log($"Exported {outPath}");

            // Set compatibility to 2014
            Exporter.SetFileExportVersion("FBX201400");
            // Export the scene.
            bool Status = Exporter.Export(lScene);
            // Destroy the exporter.
            Exporter.Destroy();

            yield return new WaitForEndOfFrame();

            // Destroy the SDK manager and all the other objects it was handling.
            manager.Destroy();

            // So that it becomes aware of our new fbx file.
            AssetDatabase.Refresh();
            if (OnDone != null)
                OnDone();
        }

        private static FbxNode[] GetChilds(FbxNode node)
        {
            var all = new List<FbxNode>();
            for (int i = 0;
                i < node.GetChildCount();
                i++)
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
    }*/
}
}

