#include "DzFbxUtils.h"
#include "filesystem.hpp"

template<class C, class T>
auto contains(const C& v, const T& x)
-> decltype(end(v), true)
{
    return end(v) != std::find(begin(v), end(v), x);
}

template<class C, class T>
void remove(const C& v, const T& item)
{
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
}


void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode)
{
  std::map<std::string, int> ExistingBones;
  RenameDuplicateBones(RootNode, ExistingBones);
}

void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode, std::map<std::string, int> &ExistingBones)
{
  if (RootNode == nullptr)
    return;

  FbxNodeAttribute *Attr = RootNode->GetNodeAttribute();
  if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
  {
    QString BoneName = RootNode->GetName();
    if (ExistingBones.find(BoneName.toStdString()) != ExistingBones.end())
    {
      ExistingBones[BoneName.toStdString()] += 1;
      BoneName = QString("%s_RENAMED_%d").arg(BoneName).arg(ExistingBones[BoneName.toStdString()]);

      RootNode->SetName(BoneName.toStdString().c_str());
    }
    else
    {
      ExistingBones.insert(make_pair(BoneName.toStdString(), 1));
    }
  }

  for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
  {
    FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
    RenameDuplicateBones(ChildNode, ExistingBones);
  }
}

void DzFbxUtils::FixClusterTranformLinks(FbxScene *Scene, FbxNode *RootNode)
{
  FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(RootNode->GetMesh());

  // Create missing weights
  if (NodeGeometry)
  {

    for (int DeformerIndex = 0; DeformerIndex < NodeGeometry->GetDeformerCount(); ++DeformerIndex)
    {
      FbxSkin *Skin = static_cast<FbxSkin *>(NodeGeometry->GetDeformer(DeformerIndex));
      if (Skin)
      {
        for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
        {
          // Get the current tranform
          FbxAMatrix Matrix;
          FbxCluster *Cluster = Skin->GetCluster(ClusterIndex);
          Cluster->GetTransformLinkMatrix(Matrix);

          // Update the rotation
          FbxDouble3 Rotation = Cluster->GetLink()->PostRotation.Get();
          Matrix.SetR(Rotation);
          Cluster->SetTransformLinkMatrix(Matrix);
        }
      }
    }
  }

  for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
  {
    FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
    FixClusterTranformLinks(Scene, ChildNode);
  }
}
void DzFbxUtils::PostExport(std::string FBXPath)
{
	 std::string asset_name = "";

  // AssetType = DazAssetType::SkeletalMesh;
	  // Set up the folder paths

	//  std::string FBXFile = FBXPath;

	//  std::string DAZImportFolder = CachedSettings->ImportDirectory.Path;
	//  std::string DAZAnimationImportFolder = CachedSettings->AnimationImportDirectory.Path;
	//  std::string CharacterFolder = DAZImportFolder / AssetName;
	//  std::string CharacterTexturesFolder = CharacterFolder / TEXT("Textures");
	//  std::string CharacterMaterialFolder = CharacterFolder / TEXT("Materials");

	//  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	//  std::string ContentDirectory = FPaths::ProjectContentDir();

	//  std::string LocalDAZImportFolder = DAZImportFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  std::string LocalDAZAnimationImportFolder = DAZAnimationImportFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  std::string LocalCharacterFolder = CharacterFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  std::string LocalCharacterTexturesFolder = CharacterTexturesFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  std::string LocalCharacterMaterialFolder = CharacterMaterialFolder.Replace(TEXT("/Game/"), *ContentDirectory);

	//  // Make any needed folders.  If any of these fail, don't continue
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(ImportDirectory)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(ImportCharacterFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(ImportCharacterTexturesFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(LocalDAZImportFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(LocalDAZAnimationImportFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(LocalCharacterFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(LocalCharacterTexturesFolder)) return nullptr;
	//  if (!FDazToUnrealUtils::MakeDirectoryAndCheck(LocalCharacterMaterialFolder)) return nullptr;

	//  if (AssetType == DazAssetType::Environment)
	//  {
	// 	 FDazToUnrealEnvironment::ImportEnvironment(JsonObject);
	// 	 return nullptr;
	//  }

	 // If there isn't an FBX file, stop

	 if (!ghc::filesystem::exists(FBXPath))
	 {
		  return;
	 }

	 // Load subdivision info
   std::map<std::string,int> SubdivisionLevels;

	//  std::vector<TSharedPtr<FJsonValue>> sdList = JsonObject->Gestd::vectorField(TEXT("Subdivisions"));
	//  for (int32_t i = 0; i < sdList.Num(); i++)
	//  {
	// 	  TSharedPtr<FJsonObject> subdivision = sdList[i]->AsObject();
	// 	  SubdivisionLevels.push_back(subdivision->GetStringField(TEXT("Asset Name")), subdivision->GetIntegerField(TEXT("Value")));
	//  }

	 // Use the maps file to find the textures to load
   std::map<std::string,std::string> TextureFileSourceToTarget;
	 std::vector<std::string> IntermediateMaterials;

  #pragma region ue4 load
	//  std::vector<TSharedPtr<FJsonValue>> matList = JsonObject->Gestd::vectorField(TEXT("Materials"));
	//  for (int32_t i = 0; i < matList.Num(); i++)
	//  {
	// 	  TSharedPtr<FJsonObject> material = matList[i]->AsObject();
	// 	  int32_t Version = material->GetIntegerField(TEXT("Version"));

	// 	  // Version 1 "Version, Material, Type, Color, Opacity, File"
	// 	  if (Version == 1)
	// 	  {
	// 			std::string MaterialName = AssetName + TEXT("_") + material->GetStringField(TEXT("Material Name"));
	// 			MaterialName = FDazToUnrealUtils::SanitizeName(MaterialName);
	// 			std::string TexturePath = material->GetStringField(TEXT("Texture"));
	// 			std::string TextureName = FDazToUnrealUtils::SanitizeName(FPaths::GetBaseFilename(TexturePath));

	// 			if (!MaterialProperties.Contains(MaterialName))
	// 			{
	// 				 MaterialProperties.push_back(MaterialName, std::vector<FDUFTextureProperty>());
	// 			}
	// 			FDUFTextureProperty Property;
	// 			Property.Name = material->GetStringField(TEXT("Name"));
	// 			Property.Type = material->GetStringField(TEXT("Data Type"));
	// 			Property.Value = material->GetStringField(TEXT("Value"));
	// 			if (Property.Type == TEXT("Texture"))
	// 			{
	// 				 Property.Type = TEXT("Color");
	// 			}

	// 			MaterialProperties[MaterialName].push_back(Property);
	// 			if (!TextureName.IsEmpty())
	// 			{
	// 				 // If a texture is attached add a texture property
	// 				 FDUFTextureProperty TextureProperty;
	// 				 TextureProperty.Name = material->GetStringField(TEXT("Name")) + TEXT(" Texture");
	// 				 TextureProperty.Type = TEXT("Texture");

	// 				 if (!TextureFileSourceToTarget.Contains(TexturePath))
	// 				 {
	// 					  int32_t TextureCount = 0;
	// 					  std::string NewTextureName = std::string::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  while (TextureFileSourceToTarget.FindKey(NewTextureName) != nullptr)
	// 					  {
	// 							TextureCount++;
	// 							NewTextureName = std::string::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  }
	// 					  TextureFileSourceToTarget.push_back(TexturePath, NewTextureName);
	// 				 }

	// 				 TextureProperty.Value = TextureFileSourceToTarget[TexturePath];
	// 				 MaterialProperties[MaterialName].push_back(TextureProperty);
	// 				 //TextureFiles.mamaAddUnique(TexturePath);

	// 				 // and a switch property for things like Specular that could come from different channels
	// 				 FDUFTextureProperty SwitchProperty;
	// 				 SwitchProperty.Name = material->GetStringField(TEXT("Name")) + TEXT(" Texture Active");
	// 				 SwitchProperty.Type = TEXT("Switch");
	// 				 SwitchProperty.Value = TEXT("true");
	// 				 MaterialProperties[MaterialName].push_back(SwitchProperty);
	// 			}
	// 	  }

	// 	  // Version 2 "Version, ObjectName, Material, Type, Color, Opacity, File"
	// 	  if (Version == 2)
	// 	  {
	// 			std::string ObjectName = material->GetStringField(TEXT("Asset Name"));
	// 			ObjectName = FDazToUnrealUtils::SanitizeName(ObjectName);
	// 			IntermediateMaterials.mamaAddUnique(ObjectName + TEXT("_BaseMat"));
	// 			std::string ShaderName = material->GetStringField(TEXT("Material Type"));
	// 			std::string MaterialName = AssetName + TEXT("_") + material->GetStringField(TEXT("Material Name"));
	// 			MaterialName = FDazToUnrealUtils::SanitizeName(MaterialName);
	// 			std::string TexturePath = material->GetStringField(TEXT("Texture"));
	// 			std::string TextureName = FDazToUnrealUtils::SanitizeName(FPaths::GetBaseFilename(TexturePath));

	// 			if (!MaterialProperties.Contains(MaterialName))
	// 			{
	// 				 MaterialProperties.push_back(MaterialName, std::vector<FDUFTextureProperty>());
	// 			}
	// 			FDUFTextureProperty Property;
	// 			Property.Name = material->GetStringField(TEXT("Name"));
	// 			Property.Type = material->GetStringField(TEXT("Data Type"));
	// 			Property.Value = material->GetStringField(TEXT("Value"));
	// 			Property.ObjectName = ObjectName;
	// 			Property.ShaderName = ShaderName;
	// 			if (Property.Type == TEXT("Texture"))
	// 			{
	// 				 Property.Type = TEXT("Color");
	// 			}

	// 			// Properties that end with Enabled are switches for functionality
	// 			if (Property.Name.EndsWith(TEXT(" Enable")))
	// 			{
	// 				Property.Type = TEXT("Switch");
	// 				if (Property.Value == TEXT("0"))
	// 				{
	// 					Property.Value = TEXT("false");
	// 				}
	// 				else
	// 				{
	// 					Property.Value = TEXT("true");
	// 				}
	// 			}

	// 			MaterialProperties[MaterialName].push_back(Property);
	// 			if (!TextureName.IsEmpty())
	// 			{
	// 				 // If a texture is attached add a texture property
	// 				 FDUFTextureProperty TextureProperty;
	// 				 TextureProperty.Name = material->GetStringField(TEXT("Name")) + TEXT(" Texture");
	// 				 TextureProperty.Type = TEXT("Texture");
	// 				 TextureProperty.ObjectName = ObjectName;
	// 				 TextureProperty.ShaderName = ShaderName;

	// 				 if (!TextureFileSourceToTarget.Contains(TexturePath))
	// 				 {
	// 					  int32_t TextureCount = 0;
	// 					  std::string NewTextureName = std::string::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  while (TextureFileSourceToTarget.FindKey(NewTextureName) != nullptr)
	// 					  {
	// 							TextureCount++;
	// 							NewTextureName = std::string::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  }
	// 					  TextureFileSourceToTarget.push_back(TexturePath, NewTextureName);
	// 				 }

	// 				 TextureProperty.Value = TextureFileSourceToTarget[TexturePath];
	// 				 MaterialProperties[MaterialName].push_back(TextureProperty);
	// 				 //TextureFiles.mamaAddUnique(TexturePath);

	// 				 // and a switch property for things like Specular that could come from different channels
	// 				 FDUFTextureProperty SwitchProperty;
	// 				 SwitchProperty.Name = material->GetStringField(TEXT("Name")) + TEXT(" Texture Active");
	// 				 SwitchProperty.Type = TEXT("Switch");
	// 				 SwitchProperty.Value = TEXT("true");
	// 				 SwitchProperty.ObjectName = ObjectName;
	// 				 SwitchProperty.ShaderName = ShaderName;
	// 				 MaterialProperties[MaterialName].push_back(SwitchProperty);
	// 			}
	// 	  }
	//  }
#pragma endregion

   // Load the FBX file
	 FbxManager* SdkManager = FbxManager::Create();

	 // create an IOSettings object
	 FbxIOSettings* ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	 SdkManager->SetIOSettings(ios);

	 // Create the geometry converter
	 FbxGeometryConverter* GeometryConverter = new FbxGeometryConverter(SdkManager);

	 FbxImporter* Importer = FbxImporter::Create(SdkManager, "");

	//  const bool bImportStatus = Importer->Initialize(FBXFile);
	 FbxScene* Scene = FbxScene::Create(SdkManager, "");
	 Importer->Import(Scene);

	 FbxNode* RootNode = Scene->GetRootNode();

	 // Find the root bone.  There should only be one bone off the scene root
	 FbxNode* RootBone = nullptr;
	 std::string RootBoneName = "";
	 for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
	 {
		  FbxNode* ChildNode = RootNode->GetChild(ChildIndex);
		  FbxNodeAttribute* Attr = ChildNode->GetNodeAttribute();
		  if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		  {
				RootBone = ChildNode;
				RootBoneName = RootBone->GetName();
				RootBone->SetName("root");
				Attr->SetName("root");
				break;
		  }
	 }

	 // Daz Studio puts the base bone rotations in a different place than Unreal expects them.
		FixClusterTranformLinks(Scene, RootBone);

	 // If this is a skeleton mesh, but a root bone wasn't found, it may be a scene under a group node or something similar
	 // So create a root node.
	 if (AssetType == DazAssetType::SkeletalMesh && RootBone == nullptr)
	 {
		  RootBoneName = AssetName;

		  FbxSkeleton* NewRootNodeAttribute = FbxSkeleton::Create(Scene, "root");
		  NewRootNodeAttribute->SetSkeletonType(FbxSkeleton::eRoot);
		  NewRootNodeAttribute->Size.Set(1.0);
		  RootBone = FbxNode::Create(Scene, "root");
		  RootBone->SetNodeAttribute(NewRootNodeAttribute);
		  RootBone->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));


		  for (int ChildIndex = RootNode->GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
		  {
				FbxNode* ChildNode = RootNode->GetChild(ChildIndex);
				RootBone->AddChild(ChildNode);
				if (FbxSkeleton* ChildSkeleton = ChildNode->GetSkeleton())
				{
					 if (ChildSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
					 {
						  ChildSkeleton->SetSkeletonType(FbxSkeleton::eLimb);
					 }
				}
		  }

		  RootNode->AddChild(RootBone);
	 }

	 RenameDuplicateBones(RootBone);

	 struct local
	 {
		  static void GetWeights(FbxNode* SceneNode, std::map<int, std::vector<int>>& VertexPolygons, std::map<int, double>& ClusterWeights, FbxMatrix TargetMatrix, int SearchFromVertex, std::vector<int>& TouchedPolygons, std::vector<int>& TouchedVertices, double& WeightsOut, double& DistancesOut, int32_t Depth)
		  {
				FbxVector4 TargetPosition;
				FbxQuaternion TargetNormal;
				FbxVector4 TargetShearing;
				FbxVector4 TargetScale;
				double Sign;
				TargetMatrix.GetElements(TargetPosition, TargetNormal, TargetShearing, TargetScale, Sign);

				FbxVector4* VertexLocations = SceneNode->GetMesh()->GetControlPoints();
				for (int PolygonIndex : VertexPolygons[SearchFromVertex])
				{
					 if (contains(TouchedPolygons, PolygonIndex)) continue;
					 TouchedPolygons.push_back(PolygonIndex);
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
					 for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
					 {
						  int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
						  if (contains(TouchedVertices, Vertex)) continue;
						  TouchedVertices.push_back(Vertex);
						  if (contains(ClusterWeights,Vertex))
						  {
								FbxVector4 CompareVertexNormal;
								SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, CompareVertexNormal);
								//double DotProduct = CompareVertexNormal.DotProduct(NeedWeightVertexNormal);
								FbxVector4 CompareLocation = VertexLocations[Vertex];
								FbxVector4 CompareScale = FbxVector4(1.0f, 1.0f, 1.0f);
								FbxMatrix CompareMatrix = FbxMatrix(CompareLocation, CompareVertexNormal, CompareScale);
								//CompareLocation.
								FbxMatrix AdjustedMatrix = CompareMatrix * TargetMatrix.Inverse();

								FbxVector4 AdjustedComparePosition;
								FbxQuaternion AdjustedCompareNormal;
								FbxVector4 AdjustedCompareShearing;
								FbxVector4 AdjustedCompareScale;
								double AdjustedCompareSign;
								AdjustedMatrix.GetElements(AdjustedComparePosition, AdjustedCompareNormal, AdjustedCompareShearing, AdjustedCompareScale, AdjustedCompareSign);

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
								GetWeights(SceneNode, VertexPolygons, ClusterWeights, TargetMatrix, Vertex, TouchedPolygons, TouchedVertices, WeightsOut, DistancesOut, Depth - 1);
						  }
					 }
				}
		  }

		  static void FindVertexNeedingWeight(int SearchFromVertex, FbxNode* SceneNode, std::vector<int>& NeedWeights, std::map<int, std::vector<int>>& VertexPolygons, std::vector<int>& NoWeights, int32_t Depth)
		  {
				for (int PolygonIndex : VertexPolygons[SearchFromVertex])
				{
					 for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
					 {
						  int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
						  if (contains(NoWeights,Vertex))
						  {
								if (!contains(NeedWeights,Vertex)){
									NeedWeights.push_back(Vertex);
								}
						  }

						  if (Depth > 1)
						  {
								FindVertexNeedingWeight(Vertex, SceneNode, NeedWeights, VertexPolygons, NoWeights, Depth - 1);
						  }
					 }
				}
		  }
	 };

	 // Detach geometry from the skeleton
	 for (int NodeIndex = 0; NodeIndex < Scene->GetNodeCount(); ++NodeIndex)
	 {
		  FbxNode* SceneNode = Scene->GetNode(NodeIndex);
		  if (SceneNode == nullptr)
		  {
				continue;
		  }
		  FbxGeometry* NodeGeometry = static_cast<FbxGeometry*>(SceneNode->GetMesh());
		  if (NodeGeometry)
		  {
				if (SceneNode->GetParent() &&
					 SceneNode->GetParent()->GetNodeAttribute() &&
					 SceneNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
				{
					 SceneNode->GetParent()->RemoveChild(SceneNode);
					 RootNode->AddChild(SceneNode);
				}
		  }
#if 1
		  if (SubdivisionLevels.size() > 0)
		  {
				// Create missing weights
				if (NodeGeometry)
				{
					 std::string GeometryName = SceneNode->GetName();
					 if (!contains(SubdivisionLevels,GeometryName)) continue;
					 if (SubdivisionLevels[GeometryName] == 0) continue;

					 for (int DeformerIndex = 0; DeformerIndex < NodeGeometry->GetDeformerCount(); ++DeformerIndex)
					 {
						  FbxSkin* Skin = static_cast<FbxSkin*>(NodeGeometry->GetDeformer(DeformerIndex));
						  if (Skin)
						  {

								FbxVector4* VertexLocations = SceneNode->GetMesh()->GetControlPoints();
								std::vector<int> NoWeights;
								std::vector<int> HasWeights;
								std::map<int, std::vector<int>> VertexPolygons;
								std::map<int, FbxVector4> VertexNormals;
								// iterate the polygons
								for (int PolygonIndex = 0; PolygonIndex < SceneNode->GetMesh()->GetPolygonCount(); ++PolygonIndex)
								{

									 for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
									 {
										  int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
										  if (!contains(VertexPolygons,Vertex))
										  {
												std::vector<int> PolygonList;
												VertexPolygons.insert({Vertex, PolygonList});
										  }
										  VertexPolygons[Vertex].push_back(PolygonIndex);
										  if (contains(HasWeights, Vertex)) continue;
										  if (contains(NoWeights, Vertex)) continue;

										  FbxVector4 VertexNormal;
										  Vertex, SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, VertexNormal);
										  VertexNormals.insert({Vertex, VertexNormal});
											if (!contains(NoWeights,Vertex)){
										  NoWeights.push_back(Vertex);
											}
									 }
								}

								for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
								{
									 FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);

									 for (int ClusterVertexIndex = 0; ClusterVertexIndex < Cluster->GetControlPointIndicesCount(); ++ClusterVertexIndex)
									 {
										  int Vertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
										  //if (Cluster->GetControlPointIndices()[ClusterVertexIndex] == Vertex)
										  {
												remove(NoWeights,Vertex);
												if (!contains(HasWeights,Vertex)){
													HasWeights.push_back(Vertex);
												}
										  }
									 }

								}

								if (HasWeights.size() > 0 && NoWeights.size() > 0)
								{
									 //for (int NoWeightVertex : NoWeights)
									 {
										  // FScopedSlowTask SubdivisionTask(Skin->GetClusterCount(), LOCTEXT("DazToUnrealSudAutoWeightTask", "Creating Subdivision Weights for Cluster:"));

										  for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
										  {
												// SubdivisionTask.EnterProgressFrame();
												FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);
												int ClusterVertexCount = Cluster->GetControlPointIndicesCount();

												//Make a map of all the weights for the cluster
												std::map<int, double> ClusterWeights;
												//std::map<int, double> ClusterVertex;
												for (int ClusterVertexIndex = 0; ClusterVertexIndex < ClusterVertexCount; ++ClusterVertexIndex)
												{
													 int WeightVertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
													 double Weight = Cluster->GetControlPointWeights()[ClusterVertexIndex];
													 ClusterWeights.insert({WeightVertex, Weight});
													 //ClusterVertex.insert({ClusterVertexIndex, WeightVertex});
												}

												std::map<int, double> WeightsToAdd;
												for (int ClusterVertexIndex = 0; ClusterVertexIndex < ClusterVertexCount; ++ClusterVertexIndex)
												{
													 int WeightVertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
													 //if(NoWeights.Contains(NoWeightVertex))
													 {
														  std::vector<int> NeedWeights;
														  local::FindVertexNeedingWeight(WeightVertex, SceneNode, NeedWeights, VertexPolygons, NoWeights, 1);
														  /*for (int PolygonIndex : VertexPolygons[WeightVertex])
														  {
															  for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
															  {
																  int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
																  if (NoWeights.Contains(Vertex))
																  {
																	  NeedWeights.mamaAddUnique(Vertex);
																  }
															  }
														  }*/

														  for (int NeedWeightVertex : NeedWeights)
														  {
																double WeightCount = 0.0f;
																double Weight = 0.0f;

																FbxVector4 NeedWeightVertexLocation = VertexLocations[NeedWeightVertex];
																std::vector<int> TouchedPolygons;
																std::vector<int> TouchedVertices;
																FbxVector4 ScaleVector = FbxVector4(1.0, 1.0, 1.0);
																FbxMatrix VertexMatrix = FbxMatrix(NeedWeightVertexLocation, VertexNormals[NeedWeightVertex], ScaleVector);
																local::GetWeights(SceneNode, VertexPolygons, ClusterWeights, VertexMatrix, NeedWeightVertex, TouchedPolygons, TouchedVertices, Weight, WeightCount, 1);



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
																	 WeightsToAdd.insert({NeedWeightVertex, Weight / (double)WeightCount});
																	 //Cluster->AddControlPointIndex(NeedWeightVertex, Weight / (double)WeightCount);
																}
														  }
													 }
												}

												for (auto WeightToAdd : WeightsToAdd)
												{
													 Cluster->AddControlPointIndex(WeightToAdd.first, WeightToAdd.second);
												}
										  }
									 }
								}
						  }
					 }
				}
		  }
#endif
	 }

	 // Add IK bones
	 if (RootBone)
	 {
		  // ik_foot_root
		  FbxNode* IKRootNode = Scene->FindNodeByName("ik_foot_root");
		  if (!IKRootNode)
		  {
				// Create IK Root
				FbxSkeleton* IKRootNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_root");
				IKRootNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKRootNodeAttribute->Size.Set(1.0);
				IKRootNode = FbxNode::Create(Scene, "ik_foot_root");
				IKRootNode->SetNodeAttribute(IKRootNodeAttribute);
				IKRootNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
				RootBone->AddChild(IKRootNode);
		  }

		  // ik_foot_l
		  FbxNode* IKFootLNode = Scene->FindNodeByName("ik_foot_l");
		  FbxNode* FootLNode = Scene->FindNodeByName("lFoot");
		  if (!IKFootLNode && FootLNode)
		  {
				// Create IK Root
				FbxSkeleton* IKFootLNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_l");
				IKFootLNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKFootLNodeAttribute->Size.Set(1.0);
				IKFootLNode = FbxNode::Create(Scene, "ik_foot_l");
				IKFootLNode->SetNodeAttribute(IKFootLNodeAttribute);
				FbxVector4 FootLocation = FootLNode->EvaluateGlobalTransform().GetT();
				IKFootLNode->LclTranslation.Set(FootLocation);
				IKRootNode->AddChild(IKFootLNode);
		  }

		  // ik_foot_r
		  FbxNode* IKFootRNode = Scene->FindNodeByName("ik_foot_r");
		  FbxNode* FootRNode = Scene->FindNodeByName("rFoot");
		  if (!IKFootRNode && FootRNode)
		  {
				// Create IK Root
				FbxSkeleton* IKFootRNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_r");
				IKFootRNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKFootRNodeAttribute->Size.Set(1.0);
				IKFootRNode = FbxNode::Create(Scene, "ik_foot_r");
				IKFootRNode->SetNodeAttribute(IKFootRNodeAttribute);
				FbxVector4 FootLocation = FootRNode->EvaluateGlobalTransform().GetT();
				IKFootRNode->LclTranslation.Set(FootLocation);
				IKRootNode->AddChild(IKFootRNode);
		  }

		  // ik_hand_root
		  FbxNode* IKHandRootNode = Scene->FindNodeByName("ik_hand_root");
		  if (!IKHandRootNode)
		  {
				// Create IK Root
				FbxSkeleton* IKHandRootNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_root");
				IKHandRootNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKHandRootNodeAttribute->Size.Set(1.0);
				IKHandRootNode = FbxNode::Create(Scene, "ik_hand_root");
				IKHandRootNode->SetNodeAttribute(IKHandRootNodeAttribute);
				IKHandRootNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
				RootBone->AddChild(IKHandRootNode);
		  }

		  // ik_hand_gun
		  FbxNode* IKHandGunNode = Scene->FindNodeByName("ik_hand_gun");
		  FbxNode* HandRNode = Scene->FindNodeByName("rHand");
		  if (!IKHandGunNode && HandRNode)
		  {
				// Create IK Root
				FbxSkeleton* IKHandGunNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_gun");
				IKHandGunNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKHandGunNodeAttribute->Size.Set(1.0);
				IKHandGunNode = FbxNode::Create(Scene, "ik_hand_gun");
				IKHandGunNode->SetNodeAttribute(IKHandGunNodeAttribute);
				FbxVector4 HandLocation = HandRNode->EvaluateGlobalTransform().GetT();
				IKHandGunNode->LclTranslation.Set(HandLocation);
				IKHandRootNode->AddChild(IKHandGunNode);
		  }

		  // ik_hand_r
		  FbxNode* IKHandRNode = Scene->FindNodeByName("ik_hand_r");
		  //FbxNode* HandRNode = Scene->FindNodeByName("rHand")));
		  //FbxNode* IKHandGunNode = Scene->FindNodeByName("ik_hand_gun")));
		  if (!IKHandRNode && HandRNode && IKHandGunNode)
		  {
				// Create IK Root
				FbxSkeleton* IKHandRNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_r");
				IKHandRNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKHandRNodeAttribute->Size.Set(1.0);
				IKHandRNode = FbxNode::Create(Scene, "ik_hand_r");
				IKHandRNode->SetNodeAttribute(IKHandRNodeAttribute);
				IKHandRNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
				IKHandGunNode->AddChild(IKHandRNode);
		  }

		  // ik_hand_l
		  FbxNode* IKHandLNode = Scene->FindNodeByName("ik_hand_l");
		  FbxNode* HandLNode = Scene->FindNodeByName("lHand");
		  //FbxNode* IKHandGunNode = Scene->FindNodeByName("ik_hand_gun")));
		  if (!IKHandLNode && HandLNode && IKHandGunNode)
		  {
				// Create IK Root
				FbxSkeleton* IKHandRNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_l");
				IKHandRNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				IKHandRNodeAttribute->Size.Set(1.0);
				IKHandLNode = FbxNode::Create(Scene, "ik_hand_l");
				IKHandLNode->SetNodeAttribute(IKHandRNodeAttribute);
				FbxVector4 HandLocation = HandLNode->EvaluateGlobalTransform().GetT();
				FbxVector4 ParentLocation = IKHandGunNode->EvaluateGlobalTransform().GetT();
				IKHandLNode->LclTranslation.Set(HandLocation - ParentLocation);
				IKHandGunNode->AddChild(IKHandLNode);
		  }
	 }

	 // Get a list of morph name mappings
	 std::map<std::string, std::string> MorphMappings;
	 std::vector<TSharedPtr<FJsonValue>> morphList = JsonObject->Gestd::vectorField("Morphs");
	 for (int32_t i = 0; i < morphList.Num(); i++)
	 {
		  TSharedPtr<FJsonObject> morph = morphList[i]->AsObject();
		  std::string MorphName = morph->GetStringField(TEXT("Name"));
		  std::string MorphLabel = morph->GetStringField(TEXT("Label"));

		  // Daz Studio seems to strip the part of the name before a period when exporting the morph to FBX
		  if (MorphName.Contains(TEXT(".")))
		  {
			  std::string Left;
			  MorphName.Split(TEXT("."), &Left, &MorphName);
		  }

		  if (CachedSettings->UseInternalMorphName)
		  {
			  MorphMappings.push_back(MorphName, MorphName);
		  }
		  else
		  {
			  MorphMappings.push_back(MorphName, MorphLabel);
		  }
	 }

	 // Get a list of morph name mappings
	 std::vector<std::string> PoseNameList;
	 const std::vector<TSharedPtr<FJsonValue>>* PoseList;
	 if (JsonObject->TryGestd::vectorField(TEXT("Poses"), PoseList))
	 {
		 PoseNameList.push_back(TEXT("ReferencePose"));
		 for (int32_t i = 0; i < PoseList->Num(); i++)
		 {
			 TSharedPtr<FJsonObject> Pose = (*PoseList)[i]->AsObject();
			 std::string PoseName = Pose->GetStringField(TEXT("Name"));
			 std::string PoseLabel = Pose->GetStringField(TEXT("Label"));

			 PoseNameList.push_back(PoseLabel);
		 }
	 }

	 // Combine clothing and body morphs
	 for (int NodeIndex = 0; NodeIndex < Scene->GetNodeCount(); ++NodeIndex)
	 {
		  FbxNode* SceneNode = Scene->GetNode(NodeIndex);
		  if (SceneNode == nullptr)
		  {
				continue;
		  }
		  FbxGeometry* NodeGeometry = static_cast<FbxGeometry*>(SceneNode->GetMesh());
		  if (NodeGeometry)
		  {

				const int32_t BlendShapeDeformerCount = NodeGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
				for (int32_t BlendShapeIndex = 0; BlendShapeIndex < BlendShapeDeformerCount; ++BlendShapeIndex)
				{
					 FbxBlendShape* BlendShape = (FbxBlendShape*)NodeGeometry->GetDeformer(BlendShapeIndex, FbxDeformer::eBlendShape);
					 const int32_t BlendShapeChannelCount = BlendShape->GetBlendShapeChannelCount();

					 std::vector<FbxBlendShapeChannel*> ChannelsToRemove;
					 for (int32_t ChannelIndex = 0; ChannelIndex < BlendShapeChannelCount; ++ChannelIndex)
					 {
						  FbxBlendShapeChannel* Channel = BlendShape->GetBlendShapeChannel(ChannelIndex);
						  if (Channel)
						  {
								std::string ChannelName = Channel->GetNameOnly();
								std::string NewChannelName, Extra;
								ChannelName.Split("__", &Extra, &NewChannelName);
								if (MorphMappings.Contains(NewChannelName))
								{
									 NewChannelName = MorphMappings[NewChannelName];
									 Channel->SetName(*NewChannelName);
								}
								else
								{
									 ChannelsToRemove.mamaAddUnique(Channel);
								}
						  }
					 }
					 for (FbxBlendShapeChannel* Channel : ChannelsToRemove)
					 {
						  BlendShape->RemoveBlendShapeChannel(Channel);
					 }
				}
		  }
	 }

	 // Get a list of Materials with name collisions
	 /*std::vector<std::string> UniqueMaterialNames;
	 std::vector<std::string> DuplicateMaterialNames;
	 for (int32_t MaterialIndex = Scene->GetMaterialCount() - 1; MaterialIndex >= 0; --MaterialIndex)
	 {
		 //MaterialProperties.push_back(MaterialName
		 FbxSurfaceMaterial *Material = Scene->GetMaterial(MaterialIndex);
		 std::string OriginalMaterialName = UTF8_TO_TCHAR(Material->GetName());
		 if (UniqueMaterialNames.Contains(OriginalMaterialName))
		 {
			 DuplciateMaterialNames.push_back(OriginalMaterialName);
		 }
		 UniqueMaterialNames.mamaAddUnique(OriginalMaterialName);
	 }

	 // Rename any duplicates adding their shape name
	 for (int32_t MeshIndex = Scene->GetGeometryCount() - 1; MeshIndex >= 0; --MeshIndex)
	 {
		 FbxGeometry* Geometry = Scene->GetGeometry(MeshIndex);
		 FbxNode* GeometryNode = Geometry->GetNode();
		 for (int32_t MaterialIndex = GeometryNode->GetMaterialCount() - 1; MaterialIndex >= 0; --MaterialIndex)
		 {
			 FbxSurfaceMaterial *Material = GeometryNode->GetMaterial(MaterialIndex);
			 std::string OriginalMaterialName = UTF8_TO_TCHAR(Material->GetName());
			 if (DuplciateMaterialNames.Contains(OriginalMaterialName))
			 {
				 std::string NewMaterialName = GeometryNode
			 }
		 }
	 }*/

	 // Rename Materials
	 FbxArray<FbxSurfaceMaterial*> MaterialArray;
	 Scene->FillMaterialArray(MaterialArray);
	 std::vector<std::string>> MaterialNames;
	 for (int32_t MaterialIndex = MaterialArray.Size() - 1; MaterialIndex >= 0; --MaterialIndex)
	 {
		  //MaterialProperties.push_back(MaterialName
		  FbxSurfaceMaterial* Material = MaterialArray[MaterialIndex];
		  std::string OriginalMaterialName = UTF8_TO_TCHAR(Material->GetName());
		  std::string NewMaterialName = AssetName + TEXT("_") + OriginalMaterialName;
		  NewMaterialName = FDazToUnrealUtils::SanitizeName(NewMaterialName);
		  Material->SetName(TCHAR_TO_UTF8(*NewMaterialName));
		  if (MaterialProperties.Contains(NewMaterialName))
		  {
				MaterialNames.push_back(NewMaterialName);
		  }
		  else
		  {
				for (int32_t MeshIndex = Scene->GetGeometryCount() - 1; MeshIndex >= 0; --MeshIndex)
				{
					 FbxGeometry* Geometry = Scene->GetGeometry(MeshIndex);
					 FbxNode* GeometryNode = Geometry->GetNode();
					 if (GeometryNode->GetMaterialIndex(TCHAR_TO_UTF8(*NewMaterialName)) != -1)
					 {
						  Scene->RemoveGeometry(Geometry);
					 }
				}
				Scene->RemoveMaterial(Material);
		  }

	 }

	 // Create an exporter.
	 FbxExporter* Exporter = FbxExporter::Create(SdkManager, "");
	 int32_t FileFormat = -1;

	 // set file format
	 FileFormat = SdkManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");

	 // Make folders for saving the updated FBX file
	 std::string UpdatedFBXFolder = FPaths::GetPath(FBXFile) / TEXT("UpdatedFBX");
	 std::string UpdatedFBXFile = FPaths::GetPath(FBXFile) / TEXT("UpdatedFBX") / FPaths::GetCleanFilename(FBXPath);
	 if (!MakeDirectoryAndCheck(UpdatedFBXFolder)) return nullptr;

	 // Initialize the exporter by providing a filename.
	 if (!Exporter->Initialize(TCHAR_TO_UTF8(*UpdatedFBXFile), FileFormat, SdkManager->GetIOSettings()))
	 {
		  return nullptr;
	 }

	 // Export the scene.
	 bool Status = Exporter->Export(Scene);

	 // Destroy the exporter.
	 Exporter->Destroy();

}

void DzFbxUtils::AddWeightsToAllNodes(FbxNode *Parent)
{
  //for (int ChildIndex = Parent->GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
  {
    FbxNode *ChildNode = Parent; //Parent->GetChild(ChildIndex);
    //RootBone->AddChild(ChildNode);
    const char* ChildName = ChildNode->GetName();
    //UE_LOG(LogTemp, Warning, TEXT("ChildNode %s Checking Weights"), *ChildName);

    if (FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(ChildNode->GetMesh()))
    {
      //UE_LOG(LogTemp, Warning, TEXT("  No Deformers"), *ChildName);
      if (NodeGeometry->GetDeformerCount() == 0)
      {
        FbxCluster *Cluster = FbxCluster::Create(Parent->GetScene(), "");
        Cluster->SetLink(Parent);
        Cluster->SetLinkMode(FbxCluster::eTotalOne);

        FbxSkin *Skin = FbxSkin::Create(Parent->GetScene(), "");
        Skin->AddCluster(Cluster);
        NodeGeometry->AddDeformer(Skin);

        for (int PolygonIndex = 0; PolygonIndex < ChildNode->GetMesh()->GetPolygonCount(); ++PolygonIndex)
        {
          for (int VertexIndex = 0; VertexIndex < ChildNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
          {
            int Vertex = ChildNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
            Cluster->AddControlPointIndex(Vertex, 1.0f);
          }
        }
      }
    }

    //AddWeightsToAllNodes(ChildNode);
  }
}
