using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using SimpleJSON;
using UnityEditor;
using UnityEditor.Rendering.HighDefinition;
using UnityEngine;
using UnityEngine.Rendering;

namespace Daz3D
{
    public static class Constants
    {
        public const string shaderNameMetal = "Daz3D/IrayUberMetal";
        public const string shaderNameSpecular = "Daz3D/IrayUberSpec";

        // public const string shaderNameIraySkin = "Daz3D/IrayUberSkin";
        public const string shaderNameIraySkin = "MTB/UberSkin";
        public const string shaderNameHair = "Daz3D/Hair";
        public const string shaderNameWet = "Daz3D/Wet";
        public const string shaderNameInvisible = "Daz3D/Invisible"; //special shader that doesn't render anything
    }

    public class DTUConverter : Editor
    {
        /// <summary>
        /// Parses the DTU file and converts all materials and textures if dirty, will place next to DTU file
        /// </summary>
        // [MenuItem("Assets/Daz3D/Extract materials")]
        // [MenuItem("Daz3D/Extract materials from selected DTU")]
        

        #region unused

        // /// <summary>
        // /// A temporary data structure that reads and interprest the .dtu file the Daz to Unity bridge produces
        // /// </summary>
        // [Serializable]
        // public struct DTU
        // {
        //
        //
        //     //DiffusionProfile is sealed in older versions of HDRP, will need to use reflection if we want access to it
        //     //public UnityEngine.Rendering.HighDefinition.DiffusionProfile diffusionProfile = null;
        //
        //     private static DTUMaterialProperty ExtractDTUMatProperty(ref DTUMaterial dtuMaterial, string key)
        //     {
        //         DTUMaterialProperty result;
        //         if (!dtuMaterial.Map.TryGetValue(key, out result))
        //             Debug.LogWarning("'" + key + "' property not found in Material: " + dtuMaterial.MaterialName);
        //         return result;
        //     }
        // }

        // public static DTU ParseDTUFile(string path)
        // {
        //     var dtu = new DTUFile(path);
        //     return dtu;
        // }


        /// <summary>
        /// Strips spaces from the json text in preparation for the JsonUtility (which doesn't handle spaces in keys)
        /// This won't appropriately handle the special Value/Data Type in the Properties array, but if you don't need that this cleaner may help you
        /// </summary>
        /// <param name="jsonRaw"></param>
        /// <returns></returns>
        protected static string CleanJSON(string jsonText)
        {
            //Converts something like "Asset Name" :  => "AssetName"
            // basically its... find something starting with whitespace, then a " then any space anywhere up to the next quote, but only the first occurance on the line
            // then only replace it with the first capture and third capture group, skipping the 2nd capture group (the space)
            var result = Regex.Replace(jsonText, "^(\\s+\"[^\"]+)([\\s]+)([^\"]+\"\\s*)", "$1$3",
                RegexOptions.Multiline);
            return result;
        }

        #endregion
    }
}