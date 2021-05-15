#include "DzFbxUtils.h"
#include <QDir>
#include <QFile>


// template <class C, class T>
// auto const.contains(C &v, const T &x)
// 		-> decltype(end(v), true)
// {
// 	return end(v) != std::find(begin(v), end(v), x);
// }

// template <class C, class T>
// auto contains(const C &v, const T &x)
// {
// 	return end(v) != std::find(begin(v), end(v), x);
// }

// bool contains(QVector<int> target, int elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }
// bool contains(QVector<QString> target, QString elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }

// bool contains(QVector<FbxBlendShapeChannel*> target, FbxBlendShapeChannel* elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }
// bool contains(QMap<int,QVector<int> > target, int elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }
// bool contains(QMap<int,double > target, int elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }
// bool contains(QMap<QString,int > target, QString elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }
// bool contains(QMap<QString,QString > target, QString elem){
// 	return end(target) != std::find(begin(target),end(target),elem);
// }

void remove(QVector<int> from, int elem){
	from.erase(std::remove(from.begin(),from.end(),elem),from.end());
}


// template <class C, class T>
// void remove(const C &v, const T &item)
// {
// 	v.erase(std::remove(v.begin(), v.end(), item), v.end());
// }

std::vector<std::string> split_string(const std::string &s, char seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
	return output;
}
std::vector<std::string> split_string(const std::string &s, std::string seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
	return output;
}

void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode)
{
	QMap<QString, int> ExistingBones;
	RenameDuplicateBones(RootNode, ExistingBones);
}

void DzFbxUtils::RenameDuplicateBones(FbxNode *RootNode, QMap<QString, int> &ExistingBones)
{
	if (RootNode == nullptr)
		return;

	FbxNodeAttribute *Attr = RootNode->GetNodeAttribute();
	if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		QString BoneName = RootNode->GetName();
		if (ExistingBones.find(BoneName) != ExistingBones.end())
		{
			ExistingBones[BoneName] += 1;
			BoneName = QString("%s_RENAMED_%d").arg(BoneName).arg(ExistingBones[BoneName]);

			RootNode->SetName(BoneName.toStdString().c_str());
		}
		else
		{
			//MAP-ExistingBones.insert(make_pair(BoneName.toStdString(), 1));
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
	DEBUG("Static Cast RootNode to FBX Geometry");
	if (RootNode == nullptr){
		WARN("Cannot find root bone, aborting cluster fix");
		return;
	}
	FbxMesh* RootMesh = RootNode->GetMesh();

	if (RootMesh){
		SUCCESS("Got mesh from root.");
	}

	FbxGeometry *NodeGeometry = RootNode->GetGeometry();

	// Create missing weights
	if (NodeGeometry)
	{
		DEBUG("Found Geometry, creating missing weights");

		for (int DeformerIndex = 0; DeformerIndex < NodeGeometry->GetDeformerCount(); ++DeformerIndex)
		{
			FbxSkin *Skin = static_cast<FbxSkin *>(NodeGeometry->GetDeformer(DeformerIndex));
			if (Skin)
			{
				DEBUG("Found Skin modifier");
				for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
				{
					// Get the current tranform
					FbxAMatrix Matrix;
					FbxCluster *Cluster = Skin->GetCluster(ClusterIndex);
					Cluster->GetTransformLinkMatrix(Matrix);

					// Update the rotation
					DEBUG("Updating the c");
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
void DzFbxUtils::PostExport(const DTUConfig config)
{

	// AssetType = DazAssetType::SkeletalMesh;
	// Set up the folder paths

	//  QString FBXFile = FBXPath;

	//  QString DAZImportFolder = CachedSettings->ImportDirectory.Path;
	//  QString DAZAnimationImportFolder = CachedSettings->AnimationImportDirectory.Path;
	//  QString CharacterFolder = DAZImportFolder / AssetName;
	//  QString CharacterTexturesFolder = CharacterFolder / TEXT("Textures");
	//  QString CharacterMaterialFolder = CharacterFolder / TEXT("Materials");

	//  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	//  QString ContentDirectory = FPaths::ProjectContentDir();

	//  QString LocalDAZImportFolder = DAZImportFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  QString LocalDAZAnimationImportFolder = DAZAnimationImportFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  QString LocalCharacterFolder = CharacterFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  QString LocalCharacterTexturesFolder = CharacterTexturesFolder.Replace(TEXT("/Game/"), *ContentDirectory);
	//  QString LocalCharacterMaterialFolder = CharacterMaterialFolder.Replace(TEXT("/Game/"), *ContentDirectory);

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
	DEBUG("Starting Custom FBX Modifications");
	// If there isn't an FBX file, stop
	QFile fbx_file = QFile(config.FBXFile);

	if (!fbx_file.exists())
	{
		DEBUG("Fbx File not found?!");
		return;
	}

	// Load subdivision info

	//  QVector<TSharedPtr<FJsonValue>> sdList = JsonObject->GeQVectorField(TEXT("Subdivisions"));
	//  for (int32_t i = 0; i < sdList.Num(); i++)
	//  {
	// 	  TSharedPtr<FJsonObject> subdivision = sdList[i]->AsObject();
	// 	  config.subdivisionLevels.push_back(subdivision->GetStringField(TEXT("Asset Name")), subdivision->GetIntegerField(TEXT("Value")));
	//  }

	// Use the maps file to find the textures to load
	QMap<QString, QString > TextureFileSourceToTarget;
	QVector<QString> IntermediateMaterials;

#pragma region ue4 load
	//  QVector<TSharedPtr<FJsonValue>> matList = JsonObject->GeQVectorField(TEXT("Materials"));
	//  for (int32_t i = 0; i < matList.Num(); i++)
	//  {
	// 	  TSharedPtr<FJsonObject> material = matList[i]->AsObject();
	// 	  int32_t Version = material->GetIntegerField(TEXT("Version"));

	// 	  // Version 1 "Version, Material, Type, Color, Opacity, File"
	// 	  if (Version == 1)
	// 	  {
	// 			QString MaterialName = AssetName + TEXT("_") + material->GetStringField(TEXT("Material Name"));
	// 			MaterialName = FDazToUnrealUtils::SanitizeName(MaterialName);
	// 			QString TexturePath = material->GetStringField(TEXT("Texture"));
	// 			QString TextureName = FDazToUnrealUtils::SanitizeName(FPaths::GetBaseFilename(TexturePath));

	// 			if (!MaterialProperties.Contains(MaterialName))
	// 			{
	// 				 MaterialProperties.push_back(MaterialName, QVector<FDUFTextureProperty>());
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
	// 					  QString NewTextureName = QString::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  while (TextureFileSourceToTarget.FindKey(NewTextureName) != nullptr)
	// 					  {
	// 							TextureCount++;
	// 							NewTextureName = QString::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
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
	// 			QString ObjectName = material->GetStringField(TEXT("Asset Name"));
	// 			ObjectName = FDazToUnrealUtils::SanitizeName(ObjectName);
	// 			IntermediateMaterials.mamaAddUnique(ObjectName + TEXT("_BaseMat"));
	// 			QString ShaderName = material->GetStringField(TEXT("Material Type"));
	// 			QString MaterialName = AssetName + TEXT("_") + material->GetStringField(TEXT("Material Name"));
	// 			MaterialName = FDazToUnrealUtils::SanitizeName(MaterialName);
	// 			QString TexturePath = material->GetStringField(TEXT("Texture"));
	// 			QString TextureName = FDazToUnrealUtils::SanitizeName(FPaths::GetBaseFilename(TexturePath));

	// 			if (!MaterialProperties.Contains(MaterialName))
	// 			{
	// 				 MaterialProperties.push_back(MaterialName, QVector<FDUFTextureProperty>());
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
	// 					  QString NewTextureName = QString::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
	// 					  while (TextureFileSourceToTarget.FindKey(NewTextureName) != nullptr)
	// 					  {
	// 							TextureCount++;
	// 							NewTextureName = QString::Printf(TEXT("%s_%02d_%s"), *TextureName, TextureCount, *AssetName);
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
	DEBUG("Creating an FBX Manager");
	FbxManager *SdkManager = FbxManager::Create();

	DEBUG("creating I/O Settings");
	// create an IOSettings object
	FbxIOSettings *ios = FbxIOSettings::Create(SdkManager, IOSROOT);

	SdkManager->SetIOSettings(ios);

	DEBUG("creating Geometry Converter");
	// Create the geometry converter
	FbxGeometryConverter *GeometryConverter = new FbxGeometryConverter(SdkManager);

	DEBUG("creating importer");
	FbxImporter *Importer = FbxImporter::Create(SdkManager, "");

	DEBUG("creating scene");
	//  const bool bImportStatus = Importer->Initialize(FBXFile);
	FbxScene *Scene = FbxScene::Create(SdkManager, "");
	DEBUG("import into scene");
	Importer->Import(Scene);

	DEBUG("looking for scene root");
	FbxNode *RootNode = Scene->GetRootNode();

	DEBUG("looking for root bone");
	// Find the root bone.  There should only be one bone off the scene root
	FbxNode *RootBone = nullptr;
	QString RootBoneName = "";
	for (int ChildIndex = 0; ChildIndex < RootNode->GetChildCount(); ++ChildIndex)
	{
		FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
		FbxNodeAttribute *Attr = ChildNode->GetNodeAttribute();
		if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			SUCCESS("found the root bone!");
			RootBone = ChildNode;
			RootBoneName = RootBone->GetName();
			RootBone->SetName("root");
			Attr->SetName("root");
			break;
		}
	}

	DEBUG("moving base bone rotation");
	// Daz Studio puts the base bone rotations in a different place than Unreal expects them.
	FixClusterTranformLinks(Scene, RootBone);

	// If this is a skeleton mesh, but a root bone wasn't found, it may be a scene under a group node or something similar
	// So create a root node.
	if (config.assetType == SkeletalMesh && RootBone == nullptr)
	{
		DEBUG("scene might be under a group...");
		RootBoneName = config.assetName;

		FbxSkeleton *NewRootNodeAttribute = FbxSkeleton::Create(Scene, "root");
		NewRootNodeAttribute->SetSkeletonType(FbxSkeleton::eRoot);
		NewRootNodeAttribute->Size.Set(1.0);

		DEBUG("creating a custom root.");
		RootBone = FbxNode::Create(Scene, "root");
		RootBone->SetNodeAttribute(NewRootNodeAttribute);
		RootBone->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));

		DEBUG("Add everything as a child of it");
		for (int ChildIndex = RootNode->GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
		{
			FbxNode *ChildNode = RootNode->GetChild(ChildIndex);
			RootBone->AddChild(ChildNode);
			if (FbxSkeleton *ChildSkeleton = ChildNode->GetSkeleton())
			{
				if (ChildSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
				{
					ChildSkeleton->SetSkeletonType(FbxSkeleton::eLimb);
				}
			}
		}

		RootNode->AddChild(RootBone);
	}

	DEBUG("Renaming duplicate materials");
	RenameDuplicateBones(RootBone);

	struct local
	{
		static void GetWeights(FbxNode *SceneNode, QMap<int, QVector<int> > &VertexPolygons, QMap<int, double > &ClusterWeights, FbxMatrix TargetMatrix, int SearchFromVertex, QVector<int> &TouchedPolygons, QVector<int> &TouchedVertices, double &WeightsOut, double &DistancesOut, int32_t Depth)
		{
			FbxVector4 TargetPosition;
			FbxQuaternion TargetNormal;
			FbxVector4 TargetShearing;
			FbxVector4 TargetScale;
			double Sign;
			TargetMatrix.GetElements(TargetPosition, TargetNormal, TargetShearing, TargetScale, Sign);

			FbxVector4 *VertexLocations = SceneNode->GetMesh()->GetControlPoints();
			for (int PolygonIndex : VertexPolygons[SearchFromVertex])
			{
				if (TouchedPolygons.contains(PolygonIndex))
					continue;
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
					if (TouchedVertices.contains( Vertex))
						continue;
					TouchedVertices.push_back(Vertex);
					if (ClusterWeights.contains( Vertex))
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

		static void FindVertexNeedingWeight(int SearchFromVertex, FbxNode *SceneNode, QVector<int> &NeedWeights, QMap<int, QVector<int> > &VertexPolygons, QVector<int> &NoWeights, int32_t Depth)
		{
			for (int PolygonIndex : VertexPolygons[SearchFromVertex])
			{
				for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
				{
					int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
					if (NoWeights.contains( Vertex))
					{
						if (!NeedWeights.contains( Vertex))
						{
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

	DEBUG("Dettaching geometry from the skeleton");
	// Detach geometry from the skeleton
	for (int NodeIndex = 0; NodeIndex < Scene->GetNodeCount(); ++NodeIndex)
	{
		FbxNode *SceneNode = Scene->GetNode(NodeIndex);
		if (SceneNode == nullptr)
		{
			continue;
		}
		FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(SceneNode->GetMesh());
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
		DEBUG("Checking if the asset contain subdivisions");

		if (config.subdivisionLevels.size() > 0)
		{
			DEBUG("Yep, found subdivisions!");
			// Create missing weights
			if (NodeGeometry)
			{
				QString GeometryName = SceneNode->GetName();
				if (!config.subdivisionLevels.contains( GeometryName))
					continue;
				if (config.subdivisionLevels[GeometryName] == 0)
					continue;

				for (int DeformerIndex = 0; DeformerIndex < NodeGeometry->GetDeformerCount(); ++DeformerIndex)
				{
					FbxSkin *Skin = static_cast<FbxSkin *>(NodeGeometry->GetDeformer(DeformerIndex));
					if (Skin)
					{

						FbxVector4 *VertexLocations = SceneNode->GetMesh()->GetControlPoints();
						QVector<int> NoWeights;
						QVector<int> HasWeights;
						QMap<int, QVector<int> > VertexPolygons;
						QMap<int, FbxVector4 > VertexNormals;
						// iterate the polygons
						for (int PolygonIndex = 0; PolygonIndex < SceneNode->GetMesh()->GetPolygonCount(); ++PolygonIndex)
						{

							for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
							{
								int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
								if (!VertexPolygons.contains( Vertex))
								{
									QVector<int> PolygonList;
									//MAP-VertexPolygons.insert({Vertex, PolygonList});
								}
								VertexPolygons[Vertex].push_back(PolygonIndex);
								if (HasWeights.contains( Vertex))
									continue;
								if (NoWeights.contains( Vertex))
									continue;

								FbxVector4 VertexNormal;
								Vertex, SceneNode->GetMesh()->GetPolygonVertexNormal(PolygonIndex, VertexIndex, VertexNormal);
								//MAP-VertexNormals.insert({Vertex, VertexNormal});
								if (!NoWeights.contains( Vertex))
								{
									NoWeights.push_back(Vertex);
								}
							}
						}

						for (int ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ++ClusterIndex)
						{
							FbxCluster *Cluster = Skin->GetCluster(ClusterIndex);

							for (int ClusterVertexIndex = 0; ClusterVertexIndex < Cluster->GetControlPointIndicesCount(); ++ClusterVertexIndex)
							{
								int Vertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
								//if (Cluster->GetControlPointIndices()[ClusterVertexIndex] == Vertex)
								{
									remove(NoWeights, Vertex);
									if (!HasWeights.contains( Vertex))
									{
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
									FbxCluster *Cluster = Skin->GetCluster(ClusterIndex);
									int ClusterVertexCount = Cluster->GetControlPointIndicesCount();

									//Make a map of all the weights for the cluster
									QMap<int, double > ClusterWeights;
									//QMap<int, double> ClusterVertex;
									for (int ClusterVertexIndex = 0; ClusterVertexIndex < ClusterVertexCount; ++ClusterVertexIndex)
									{
										int WeightVertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
										double Weight = Cluster->GetControlPointWeights()[ClusterVertexIndex];
										//MAP-ClusterWeights.insert({WeightVertex, Weight});
										//ClusterVertex.Add({ClusterVertexIndex, WeightVertex});
									}

									QMap<int, double > WeightsToAdd;
									for (int ClusterVertexIndex = 0; ClusterVertexIndex < ClusterVertexCount; ++ClusterVertexIndex)
									{
										int WeightVertex = Cluster->GetControlPointIndices()[ClusterVertexIndex];
										//if(NoWeights.NoWeightVertex.Contains()
										{
											QVector<int> NeedWeights;
											local::FindVertexNeedingWeight(WeightVertex, SceneNode, NeedWeights, VertexPolygons, NoWeights, 1);
											/*for (int PolygonIndex : VertexPolygons[WeightVertex])
														  {
															  for (int VertexIndex = 0; VertexIndex < SceneNode->GetMesh()->GetPolygonSize(PolygonIndex); ++VertexIndex)
															  {
																  int Vertex = SceneNode->GetMesh()->GetPolygonVertex(PolygonIndex, VertexIndex);
																  if (NoWeights.Vertex.Contains()
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
												QVector<int> TouchedPolygons;
												QVector<int> TouchedVertices;
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
																		if (ClusterWeights.Vertex.Contains()
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
													//MAP-WeightsToAdd.insert({NeedWeightVertex, Weight / (double)WeightCount});
													//Cluster->AddControlPointIndex(NeedWeightVertex, Weight / (double)WeightCount);
												}
											}
										}
									}

									QMapIterator<int, double> wi(WeightsToAdd);
									while (wi.hasNext()) {
										wi.next();
										Cluster->AddControlPointIndex(wi.key(), wi.value());
									}
								}
							}
						}
					}
				}
			}
			else{
				DEBUG("Can't get the geometry, aborting...!!");
			}
		}
#endif
	}

	DEBUG("Adding IK Bones");
	// Add IK bones
	if (RootBone)
	{
		// ik_foot_root
		FbxNode *IKRootNode = Scene->FindNodeByName("ik_foot_root");
		if (!IKRootNode)
		{
			// Create IK Root
			FbxSkeleton *IKRootNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_root");
			IKRootNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKRootNodeAttribute->Size.Set(1.0);
			IKRootNode = FbxNode::Create(Scene, "ik_foot_root");
			IKRootNode->SetNodeAttribute(IKRootNodeAttribute);
			IKRootNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
			RootBone->AddChild(IKRootNode);
		}

		// ik_foot_l
		FbxNode *IKFootLNode = Scene->FindNodeByName("ik_foot_l");
		FbxNode *FootLNode = Scene->FindNodeByName("lFoot");
		if (!IKFootLNode && FootLNode)
		{
			// Create IK Root
			FbxSkeleton *IKFootLNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_l");
			IKFootLNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKFootLNodeAttribute->Size.Set(1.0);
			IKFootLNode = FbxNode::Create(Scene, "ik_foot_l");
			IKFootLNode->SetNodeAttribute(IKFootLNodeAttribute);
			FbxVector4 FootLocation = FootLNode->EvaluateGlobalTransform().GetT();
			IKFootLNode->LclTranslation.Set(FootLocation);
			IKRootNode->AddChild(IKFootLNode);
		}

		// ik_foot_r
		FbxNode *IKFootRNode = Scene->FindNodeByName("ik_foot_r");
		FbxNode *FootRNode = Scene->FindNodeByName("rFoot");
		if (!IKFootRNode && FootRNode)
		{
			// Create IK Root
			FbxSkeleton *IKFootRNodeAttribute = FbxSkeleton::Create(Scene, "ik_foot_r");
			IKFootRNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKFootRNodeAttribute->Size.Set(1.0);
			IKFootRNode = FbxNode::Create(Scene, "ik_foot_r");
			IKFootRNode->SetNodeAttribute(IKFootRNodeAttribute);
			FbxVector4 FootLocation = FootRNode->EvaluateGlobalTransform().GetT();
			IKFootRNode->LclTranslation.Set(FootLocation);
			IKRootNode->AddChild(IKFootRNode);
		}

		// ik_hand_root
		FbxNode *IKHandRootNode = Scene->FindNodeByName("ik_hand_root");
		if (!IKHandRootNode)
		{
			// Create IK Root
			FbxSkeleton *IKHandRootNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_root");
			IKHandRootNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKHandRootNodeAttribute->Size.Set(1.0);
			IKHandRootNode = FbxNode::Create(Scene, "ik_hand_root");
			IKHandRootNode->SetNodeAttribute(IKHandRootNodeAttribute);
			IKHandRootNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
			RootBone->AddChild(IKHandRootNode);
		}

		// ik_hand_gun
		FbxNode *IKHandGunNode = Scene->FindNodeByName("ik_hand_gun");
		FbxNode *HandRNode = Scene->FindNodeByName("rHand");
		if (!IKHandGunNode && HandRNode)
		{
			// Create IK Root
			FbxSkeleton *IKHandGunNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_gun");
			IKHandGunNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKHandGunNodeAttribute->Size.Set(1.0);
			IKHandGunNode = FbxNode::Create(Scene, "ik_hand_gun");
			IKHandGunNode->SetNodeAttribute(IKHandGunNodeAttribute);
			FbxVector4 HandLocation = HandRNode->EvaluateGlobalTransform().GetT();
			IKHandGunNode->LclTranslation.Set(HandLocation);
			IKHandRootNode->AddChild(IKHandGunNode);
		}

		// ik_hand_r
		FbxNode *IKHandRNode = Scene->FindNodeByName("ik_hand_r");
		//FbxNode* HandRNode = Scene->FindNodeByName("rHand")));
		//FbxNode* IKHandGunNode = Scene->FindNodeByName("ik_hand_gun")));
		if (!IKHandRNode && HandRNode && IKHandGunNode)
		{
			// Create IK Root
			FbxSkeleton *IKHandRNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_r");
			IKHandRNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
			IKHandRNodeAttribute->Size.Set(1.0);
			IKHandRNode = FbxNode::Create(Scene, "ik_hand_r");
			IKHandRNode->SetNodeAttribute(IKHandRNodeAttribute);
			IKHandRNode->LclTranslation.Set(FbxVector4(0.0, 00.0, 0.0));
			IKHandGunNode->AddChild(IKHandRNode);
		}

		// ik_hand_l
		FbxNode *IKHandLNode = Scene->FindNodeByName("ik_hand_l");
		FbxNode *HandLNode = Scene->FindNodeByName("lHand");
		//FbxNode* IKHandGunNode = Scene->FindNodeByName("ik_hand_gun")));
		if (!IKHandLNode && HandLNode && IKHandGunNode)
		{
			// Create IK Root
			FbxSkeleton *IKHandRNodeAttribute = FbxSkeleton::Create(Scene, "ik_hand_l");
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

	DEBUG("Combining clothing and body morphs");
	// Combine clothing and body morphs
	for (int NodeIndex = 0; NodeIndex < Scene->GetNodeCount(); ++NodeIndex)
	{
		FbxNode *SceneNode = Scene->GetNode(NodeIndex);
		if (SceneNode == nullptr)
		{
			continue;
		}
		FbxGeometry *NodeGeometry = static_cast<FbxGeometry *>(SceneNode->GetMesh());
		if (NodeGeometry)
		{

			const int32_t BlendShapeDeformerCount = NodeGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int32_t BlendShapeIndex = 0; BlendShapeIndex < BlendShapeDeformerCount; ++BlendShapeIndex)
			{
				FbxBlendShape *BlendShape = (FbxBlendShape *)NodeGeometry->GetDeformer(BlendShapeIndex, FbxDeformer::eBlendShape);
				const int32_t BlendShapeChannelCount = BlendShape->GetBlendShapeChannelCount();

				QVector<FbxBlendShapeChannel *> ChannelsToRemove;
				for (int32_t ChannelIndex = 0; ChannelIndex < BlendShapeChannelCount; ++ChannelIndex)
				{
					FbxBlendShapeChannel *Channel = BlendShape->GetBlendShapeChannel(ChannelIndex);
					if (Channel)
					{
						FbxString ChannelName = Channel->GetNameOnly();
						// QString NewChannelName, Extra;

						QVector<QString> splitName = QString(ChannelName).split("__").toVector();
						QString NewChannelName = splitName[splitName.size() - 1];
						//ChannelName.Split("__", &Extra, &NewChannelName);
						if (config.morphMappings.contains( NewChannelName))
						{
							NewChannelName = config.morphMappings[NewChannelName];
							Channel->SetName(NewChannelName.toStdString().c_str());
						}
						else
						{
							if (!ChannelsToRemove.contains( Channel))
							{
								ChannelsToRemove.push_back(Channel);
							}
						}
					}
				}
				for (FbxBlendShapeChannel *Channel : ChannelsToRemove)
				{
					BlendShape->RemoveBlendShapeChannel(Channel);
				}
			}
		}
	}
	DEBUG("Skipping Material related task (for now)");
	// Get a list of Materials with name collisions
	/*QVector<QString> UniqueMaterialNames;
	 QVector<QString> DuplicateMaterialNames;
	 for (int32_t MaterialIndex = Scene->GetMaterialCount() - 1; MaterialIndex >= 0; --MaterialIndex)
	 {
		 //MaterialProperties.push_back(MaterialName
		 FbxSurfaceMaterial *Material = Scene->GetMaterial(MaterialIndex);
		 QString OriginalMaterialName = UTF8_TO_TCHAR(Material->GetName());
		 if (UniqueMaterialNames.OriginalMaterialName.Contains()
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
			 QString OriginalMaterialName = UTF8_TO_TCHAR(Material->GetName());
			 if (DuplciateMaterialNames.OriginalMaterialName.Contains()
			 {
				 QString NewMaterialName = GeometryNode
			 }
		 }
	 }*/

	// Rename Materials
	// FbxArray<FbxSurfaceMaterial *> MaterialArray;
	// Scene->FillMaterialArray(MaterialArray);
	// QVector<QString> MaterialNames;
	// for (int32_t MaterialIndex = MaterialArray.Size() - 1; MaterialIndex >= 0; --MaterialIndex)
	// {
	// 	//MaterialProperties.push_back(MaterialName
	// 	FbxSurfaceMaterial *Material = MaterialArray[MaterialIndex];
	// 	QString OriginalMaterialName = Material->GetName();
	// 	QString NewMaterialName = config.assetName + "_" + OriginalMaterialName;
	// 	NewMaterialName = DzUtils::SanitizeName(NewMaterialName);
	// 	Material->SetName(NewMaterialName);
	// 	if (MaterialProperties.NewMaterialName.Contains()
	// 	{
	// 		MaterialNames.push_back(NewMaterialName);
	// 	}
	// 	else
	// 	{
	// 		for (int32_t MeshIndex = Scene->GetGeometryCount() - 1; MeshIndex >= 0; --MeshIndex)
	// 		{
	// 			FbxGeometry *Geometry = Scene->GetGeometry(MeshIndex);
	// 			FbxNode *GeometryNode = Geometry->GetNode();
	// 			if (GeometryNode->GetMaterialIndex(TCHAR_TO_UTF8(*NewMaterialName)) != -1)
	// 			{
	// 				Scene->RemoveGeometry(Geometry);
	// 			}
	// 		}
	// 		Scene->RemoveMaterial(Material);
	// 	}
	// }

	DEBUG("Creating an FBX Exporter");
	// Create an exporter.
	FbxExporter *Exporter = FbxExporter::Create(SdkManager, "");
	int32_t FileFormat = -1;

	// set file format
	FileFormat = SdkManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");

	DEBUG("Make sure output directory exist");
	// Make folders for saving the updated FBX file
	QFileInfo fbx_file_info = QFileInfo(fbx_file);
	QString UpdatedFBXFolder = fbx_file_info.absoluteDir().filePath("UpdatedFBX");



	QString UpdatedFBXFile = QDir(UpdatedFBXFolder).filePath(QFile(config.FBXFile).fileName());

	if (!QDir(UpdatedFBXFolder).exists())
	{
		DEBUG("Directory not found, creating it.");
		if (!QDir().mkpath(UpdatedFBXFolder))
		{
			DEBUG(QString("Something went wrong while trying to make dir %s").arg(UpdatedFBXFolder));
			return;
		}
	}
	DEBUG("Initializing the exporter");
	// Initialize the exporter by providing a filename.
	if (!Exporter->Initialize(UpdatedFBXFile.toStdString().c_str(), FileFormat, SdkManager->GetIOSettings()))
	{
		DEBUG("Something went wrong while trying to init the exporter");
		return;
	}

	DEBUG("Exporting custom FBX");

	// Export the scene.
	bool Status = Exporter->Export(Scene);

	if (Status){
		SUCCESS("Exported custom FBX");
	}

	// Destroy the exporter.
	Exporter->Destroy();
}

void DzFbxUtils::AddWeightsToAllNodes(FbxNode *Parent)
{
	//for (int ChildIndex = Parent->GetChildCount() - 1; ChildIndex >= 0; --ChildIndex)
	{
		FbxNode *ChildNode = Parent; //Parent->GetChild(ChildIndex);
		//RootBone->AddChild(ChildNode);
		const char *ChildName = ChildNode->GetName();
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
