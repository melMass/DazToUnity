#pragma once

#include <iostream>
#include <QMap>
#include <QVector>

// #include <nlohmann/json.hpp>

// using json = nlohmann::json;

enum DazAssetType
{
	SkeletalMesh,
	StaticMesh,
	Animation,
	Environment,
	Pose
};

struct MorphInfo
{
  QString Name;
  QString Label;
  QString Type;
  QString Path;

  inline bool operator==(MorphInfo other)
  {
    if (Name == other.Name)
    {
      return true;
    }
    return false;
  }

  MorphInfo()
  {
    Name = QString();
    Label = QString();
    Type = QString();
    Path = QString();
  }
};

// #include <fmt/core.h>

struct Color
{
  float r;
  float g;
  float b;
};

struct DTUValue
{
  enum DataType
  {
    Integer,
    Float,
    Double,
    Color,
    String,
    Texture,
  };

  DataType Type;
  int AsInteger;
  float AsFloat;
  double AsDouble;
  struct Color AsColor;
  std::string AsString;

  std::string ToString()
  {
    switch (Type)
    {
    case Integer:
      return "int:";// + AsInteger;
    case Float:
      return "float:"; //+ AsFloat;
    case Double:
      return "double:"; //+ AsDouble;
    case Color:
      return "color:"; //+ AsColor;
    case String:
      return AsString;
    default:
      return nullptr; //Exception?
    }
  };
};


enum class DTUBaseMixing
{
  Unknown = -1,
  PBRMetalRoughness = 0,
  PBRSpecularGlossiness = 1,
  Weighted = 2,
};

enum class DTULightingModel
{
  Unknown = -1,
  Plastic = 0,
  Metallic = 1,
  Skin = 2,
  GlossyPlastic = 3,
  Matte = 4,
  GlossyMetallic = 5,
};

enum class DTUMaterialType
{
  Unknown,
  IrayUber,
  PBRSP,
  DazStudioDefault,
  OmUberSurface,
  OOTHairblendingHair,
  BlendedDualLobeHair, //used in some dforce hairs
  PBRSkin
};

struct DTUMaterialProperty
{
  std::string Name;
  DTUValue Value;
  std::string Texture;

  bool TextureExists()
  {
    return true;
    // return !std::string::empty(Texture);
  }
};


struct DTUSubdivision

{
  float Version;
  std::string AssetName;
  int Value;
};

struct DTUMorph
{
  std::string Name;
  std::string Label;
};
struct DTUPose
{
  std::string Name;
  std::string Label;
};

struct DTUConfig
{

  QString assetId;
  QString assetName;
  DazAssetType assetType;
  QString FBXFile;
  QString importFolder;
  QMap<QString, int> subdivisionLevels;
  QMap<QString, QString> morphMappings;
  QVector<QString> poseNameList;
	QVector<QString> materialNames;
  QString FBXVersion;

  QString getAssetType(){
    switch(assetType){
      case SkeletalMesh:
        return "SkeletalMesh";
      case StaticMesh:
        return "StaticMesh";
      case Environment:
        return "Environment";
      case Animation:
        return "Animation";
      case Pose:
        return "Pose";
      default:
        return "ErrorType";
    }
  }
//   DTUConfig from_json(const json& j) {
//     DTUConfig config;

//     // config.assetId = QString::fromStdString(j.at("Asset ID").get<std::string>());
//     // config.assetName = QString::fromStdString(j.at("Asset Name").get<std::string>());
//     // config.FBXFile = QString::fromStdString(j.at("FBX File").get<std::string>());
//     // config.importFolder = QString::fromStdString(j.at("Import Folder").get<std::string>());
//     // config.setType(QString::fromStdString(j.at("Asset Type").get<std::string>()));

//     return config;
// }
  void setType(QString assetTypeString)
  {
    if (assetTypeString == "SkeletalMesh")
    {
      assetType = SkeletalMesh;
    }
    if (assetTypeString == "StaticMesh")
    {
      assetType = SkeletalMesh;
    }
    if (assetTypeString == "Animation")
    {
      assetType = SkeletalMesh;
    }
    if (assetTypeString == "Environment")
    {
      assetType = SkeletalMesh;
    }
    if (assetTypeString == "Pose")
    {
      assetType = SkeletalMesh;
    }
  }
  //std::vector <C
};
