using System;
using System.Collections.Generic;
using System.IO;
using SimpleJSON;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Daz3D
{
    public struct DTUValue
    {
        public enum DataType
        {
            Integer,
            Float,
            Double,
            Color,
            String,
            Texture,
        }

        public DataType Type;

        public int AsInteger;
        public float AsFloat;
        public double AsDouble;
        public Color AsColor;
        public string AsString;

        public override string ToString()
        {
            switch (Type)
            {
                case DataType.Integer:
                    return "int:" + AsInteger;
                case DataType.Float:
                    return "float:" + AsFloat;
                case DataType.Double:
                    return "double:" + AsDouble;
                case DataType.Color:
                    return "color:" + AsColor;
                case DataType.String:
                    return AsString;
                default:
                    throw new Exception("Unsupported type");
            }
        }

        public static DTUValue FromJSON(JSONNode prop)
        {
            var v = new DTUValue();
            var propDataType = prop["Data Type"].Value;
            if (propDataType == "Double")
            {
                v.Type = DTUValue.DataType.Double;
                v.AsDouble = prop["Value"].AsDouble;
            }
            else if (propDataType == "Integer")
            {
                v.Type = DTUValue.DataType.Integer;
                v.AsInteger = prop["Value"].AsInt;
            }
            else if (propDataType == "Float")
            {
                v.Type = DTUValue.DataType.Float;
                v.AsDouble = prop["Value"].AsFloat;
            }
            else if (propDataType == "String")
            {
                v.Type = DTUValue.DataType.String;
                v.AsString = prop["Value"].Value;
            }
            else if (propDataType == "Color")
            {
                v.Type = DTUValue.DataType.Color;
                var tmpStr = prop["Value"].Value;
                Color color;
                if (!ColorUtility.TryParseHtmlString(tmpStr, out color))
                {
                    Debug.LogError("Failed to parse color hex code: " + tmpStr);
                    throw new Exception("Invalid color hex code");
                }

                v.AsColor = color;
            }
            else if (propDataType == "Texture")
            {
                v.Type = DTUValue.DataType.Texture;

                //these values will be hex colors 
                var tmpStr = prop["Value"].Value;
                Color color;
                if (!ColorUtility.TryParseHtmlString(tmpStr, out color))
                {
                    Debug.LogError("Failed to parse color hex code: " + tmpStr);
                    throw new Exception("Invalid color hex code");
                }

                v.AsColor = color;
            }

            else
            {
                Debug.LogError("Type: " + propDataType + " is not supported");
                throw new Exception("Unsupported type");
            }

            return v;
        }
    }

    [Serializable]
    public class DTUFile : ScriptableObject
    {
        [SerializeField] public string AssetID;
        [SerializeField] public string AssetName;
        [SerializeField] public string AssetType;
        [SerializeField] public string FBXFile;
        [SerializeField] public string ImportFolder;
        [SerializeField] public List<DTUMaterial> Materials;
        [SerializeField] public List<DTUSubdivision> Subdivisions;
        [SerializeField] public DazFigurePlatform FigureType;
        [SerializeField] public GameObject Prefab;


        /// <summary>
        /// Where is the source DTU, this is important as this is in the root folder for the asset, textures and materials will exist at this location
        /// </summary>
        public string AssetPath;

        private string _directory;

        public string Directory
        {
            get
            {
                if (!string.IsNullOrEmpty(_directory)) return _directory;

                _directory = Path.GetDirectoryName(AssetPath);
                if (_directory == null) return null;

                //if we have regular backslashes on windows, convert to forward slashes
                _directory = _directory.Replace(@"\", @"/");
                //remove everything up to Assets from the path so it's relative to the project folder
                _directory = _directory.Replace(Application.dataPath, "Assets");
                return _directory;
            }
        }

        public static DTUFile Load(string path = default)
        {
            var dtuFile = CreateInstance<DTUFile>();
            if (path == default)
                return dtuFile;
            if (!File.Exists(path))
            {
                Debug.LogError("DTU File: " + path + " does not exist");
                return dtuFile;
            }

            var text = File.ReadAllText(path);

            if (text.Length <= 0)
            {
                Debug.LogError("DTU File: " + path + " is empty");
                return dtuFile;
            }

            dtuFile.AssetPath = path;

            var root = JSON.Parse(text);

            dtuFile.AssetID = root["Asset Id"].Value;
            dtuFile.AssetName = root["Asset Name"].Value;
            dtuFile.AssetType = root["Asset Type"].Value;
            dtuFile.FBXFile = root["FBX File"].Value;
            dtuFile.ImportFolder = root["Import Folder"].Value;

            dtuFile.FigureType = dtuFile.DiscoverFigurePlatform();


            dtuFile.Materials = new List<DTUMaterial>();

            var materials = root["Materials"].AsArray;

            foreach (var matKVP in materials)
            {
                dtuFile.Materials.Add(DTUMaterial.FromJSON(matKVP.Value));
            }

            dtuFile.Subdivisions = new List<DTUSubdivision>();
            var subdivisions = root["Subdivisions"].AsArray;
            foreach (var subdivisionKVP in subdivisions)
            {
                var subdivision = subdivisionKVP.Value;
                var dtuSub = new DTUSubdivision();

                dtuSub.Version = subdivision["Version"].AsFloat;
                dtuSub.AssetName = subdivision["Asset Name"].Value;
                dtuSub.Value = (int) subdivision["Value"].AsFloat;

                dtuFile.Subdivisions.Add(dtuSub);
            }

            return dtuFile;
        }


        public void ExtractMaterials()
        {
            // var activeObject = Selection.activeObject;
            // var path = AssetDatabase.GetAssetPath(activeObject);

            // var dtu = new DTUFile(path);

            Debug.Log("DTU: " + AssetName + " contains: " + Materials.Count + " materials");

            foreach (var dtuMat in Materials)
            {
                dtuMat.ConvertToUnity(this);
            }
        }

        private DazFigurePlatform DiscoverFigurePlatform()
        {
            var token = AssetID.ToLower();

            foreach (DazFigurePlatform dfp in Enum.GetValues(typeof(DazFigurePlatform)))
            {
                if (token.Contains(dfp.ToString().ToLower()))
                    return dfp;
            }

            return DazFigurePlatform.Genesis8; //default
        }
    }
}