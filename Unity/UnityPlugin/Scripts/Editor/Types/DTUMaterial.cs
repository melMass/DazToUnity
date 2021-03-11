using System;
using System.Collections.Generic;
using SimpleJSON;
using UnityEngine;

namespace Daz3D
{
        public class MaterialMap
        {
            public MaterialMap(string path)
            {
                Path = path;
            }

            public void AddMaterial(Material material)
            {
                if (material && !Map.ContainsKey(material.name))
                    Map.Add(material.name, material);
            }

            public string Path { get; set; }
            public Dictionary<string, Material> Map = new Dictionary<string, Material>();
        }
    #region Material Properties

    public struct DTUMaterialProperty
    {
        public string Name;
        public DTUValue Value;
        public string Texture;

        public bool TextureExists()
        {
            return !string.IsNullOrEmpty(Texture);
        }

        /// <summary>
        /// True if this property was found in the DTU
        /// </summary>
        public bool Exists;

        public Color Color => Value.AsColor;

        public float ColorStrength => Utilities.GetStrengthFromColor(Color);

        public float Float => (float) Value.AsDouble;

        public bool Boolean => Value.AsDouble > 0.5;


        public static DTUMaterialProperty FromJSON(JSONNode prop)
        {
            var dtuMatProp = new DTUMaterialProperty();
            
            //since this property was found, mark it
            dtuMatProp.Exists = true;
            
            dtuMatProp.Name = prop["Name"].Value;
            dtuMatProp.Texture = prop["Texture"].Value;

            var v = DTUValue.FromJSON(prop);

            dtuMatProp.Value = v;

            return dtuMatProp;
        }
    }


    /// <summary>
    /// These are analagous to the shaders in Daz3D, if your shader is not in this list
    ///  the material will fail or be skipped
    /// </summary>
    public enum DTUMaterialType
    {
        Unknown,
        IrayUber,
        PBRSP,
        DazStudioDefault,
        OmUberSurface,
        OOTHairblendingHair,
        BlendedDualLobeHair, //used in some dforce hairs
        PBRSkin
    }

    /// <summary>
    /// Used with the Daz Studio Default shader
    /// </summary>
    public enum DTULightingModel
    {
        Unknown = -1,
        Plastic = 0,
        Metallic = 1,
        Skin = 2,
        GlossyPlastic = 3,
        Matte = 4,
        GlossyMetallic = 5,
    }

    /// <summary>
    /// Used when the shader type is Iray Uber, this defines the flow and changes which properties are read
    /// </summary>
    public enum DTUBaseMixing
    {
        Unknown = -1,
        PBRMetalRoughness = 0,
        PBRSpecularGlossiness = 1,
        Weighted = 2,
    }

    #endregion

    [Serializable]
    public struct DTUMaterial
    {
        public float Version;
        public string AssetName;
        public string MaterialName;
        public string MaterialType;
        public string Value;

        public List<DTUMaterialProperty> Properties;

        private Dictionary<string, DTUMaterialProperty> _map;

        public static DTUMaterial FromJSON(JSONNode matValue)
        {
            var dtuMat = new DTUMaterial();
            dtuMat.Version = matValue["Version"].AsFloat;
            dtuMat.AssetName = matValue["Asset Name"].Value;
            dtuMat.MaterialName = matValue["Material Name"].Value;
            dtuMat.MaterialType = matValue["Material Type"].Value;
            dtuMat.Value = matValue["Value"].Value;
            dtuMat.Properties = new List<DTUMaterialProperty>();

            var properties = matValue["Properties"];

            foreach (var propKVP in properties)
            {
                var dtuMatProp = DTUMaterialProperty.FromJSON(propKVP.Value);
                dtuMat.Properties.Add(dtuMatProp);
            }

            return dtuMat;
        }

        public Dictionary<string, DTUMaterialProperty> Map
        {
            get
            {
                if (_map == null || _map.Count == 0)
                {
                    _map = new Dictionary<string, DTUMaterialProperty>();
                    foreach (var prop in Properties)
                    {
                        _map[prop.Name] = prop;
                    }
                }

                return _map;
            }
        }

        public DTUMaterialProperty Get(string key)
        {
            if (Map.ContainsKey(key))
            {
                return Map[key];
            }

            return new DTUMaterialProperty();
        }
    }
}