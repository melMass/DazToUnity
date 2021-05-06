using UnityEditor;
using UnityEngine;

namespace Daz3D
{
    /// <summary>
    /// A component to attach to the scene instance of the prefab created by the DTUImporter
    /// allows the instance to be "refreshed" with the newer prefab, between successive import sessions
    /// If the scene containing the instance is open and the fbx is fresher than the scene, then 
    /// a instance of the new prefab will replace the old
    /// TODO add a switch in the DTU JSON to override this behavior
    /// </summary>
    public class Daz3DInstance : MonoBehaviour
    {
        /// <summary>
        /// The FBX modelPrefab is used as the unifying key between the import session
        /// and the instance in the scene.
        /// </summary>
        public GameObject SourceFBX;

        [Tooltip("If true, future bridge imports will replace this instance in the scene, including any prefab overrides.")]
        public bool ReplaceOnImport = true;

     
    }
}