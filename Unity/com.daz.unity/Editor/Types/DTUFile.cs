using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using SimpleJSON;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Daz3D
{
    [Serializable]
    public class DTUSubdivision

    {
        public float Version;
        public string AssetName;
        public int Value;
    }

    [Serializable]
    public class DTUMorph

    {
        public string Name;
        public string Label;
    }

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

        //public object Value;
        public int AsInteger; // => (int)Value;

        public float AsFloat; // => (float)Value;
        public double AsDouble; // => (float)Value;

        public Color AsColor; //
        // {
        //     get
        //     {
        //         Color color;
        //         var tmpStr = (string) Value; 
        //         if (!ColorUtility.TryParseHtmlString(tmpStr, out color))
        //         {
        //             Debug.LogError("Failed to parse color hex code: " + tmpStr);
        //             throw new Exception("Invalid color hex code");
        //         }
        //
        //         return color;
        //     }
        // }

        public string AsString; // => (string) Value;

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
            var propDataType = prop["DataType"].Value;
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
        [SerializeField] public List<DTUMorph> Morphs;

        [SerializeField] public DazFigurePlatform FigureType = DazFigurePlatform.Genesis8;
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
                _directory = Utilities.RelativePath(_directory);
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

            // dtuFile = JsonUtility.FromJson<DTUFile>(text);
            // var settings = new JsonSerializerSettings();
            // settings.Error = (serializer, err) => { err.ErrorContext.Handled = true; };
            //
            // JsonConvert.PopulateObject(text, dtuFile, settings);


            // return dtuFile;
            // 
            //
            var root = JSON.Parse(text);

            dtuFile.AssetID = root["AssetId"].Value;
            dtuFile.AssetName = root["AssetName"].Value;
            dtuFile.AssetType = root["AssetType"].Value;
            dtuFile.FBXFile = Utilities.RelativePath(root["FBXFile"].Value);
            dtuFile.ImportFolder = root["ImportFolder"].Value;

            dtuFile.Materials = new List<DTUMaterial>();
            
            dtuFile.FigureType = dtuFile.AssetID.ToFigurePlatform();
            dtuFile.AssetPath = path;

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
                dtuSub.AssetName = subdivision["AssetName"].Value;
                dtuSub.Value = (int) subdivision["Value"].AsFloat;

                dtuFile.Subdivisions.Add(dtuSub);
            }

            dtuFile.Morphs = new List<DTUMorph>();
            var morphs = root["Morphs"].AsArray;
            foreach (var morphKVP in morphs)
            {
                var morph = morphKVP.Value;
                var dtuMorph = new DTUMorph();

                dtuMorph.Name = morph["Name"].Value;
                dtuMorph.Label = morph["Label"].Value;

                dtuFile.Morphs.Add(dtuMorph);
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
                dtuMat.ConvertToUnity(this, true);
            }
        }
    }
}