using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace Daz3D
{
    // [Serializable]
    // public struct DazBlendshape
    // {
    //     public string name;
    //     public float weight;
    //
    //     public List<Action<float>> onValueChangeActions;
    //     public HashSet<string> prefixes ;
    //
    //     public DazBlendshape(string name, float weight)
    //     {
    //         this.name = name;
    //         this.weight = weight;
    //         onValueChangeActions = new List<Action<float>>();
    //         prefixes = new HashSet<string>();
    //     }
    //
    //     public void Apply()
    //     {
    //         foreach (var action in onValueChangeActions)
    //         {
    //             action.Invoke(weight);
    //         }
    //     }
    // }
    
     
    [ExecuteInEditMode]
    public class BlendshapesSyncedController : MonoBehaviour
    {
        [SerializeField] 
        private DazBlendshape[] blendshapes;

        [SerializeField] private bool dirty=false;

        public DazBlendshape singleShape;
        private void OnEnable()
        {
            GetBlendshapes();
        }

        /// <summary>
        /// Get the Blend Shapes of all the skinned mesh children.
        /// By Daz's convention, those are split by '__' and Morphs who share the same name will be tied together.
        /// </summary>
        private void GetBlendshapes()
        {
            var blendshapesList = new List<DazBlendshape>();
            var sm = GetComponentsInChildren<SkinnedMeshRenderer>();
            foreach (var s in sm)
            {
                for (int x = 0; x < s.sharedMesh.blendShapeCount; x++)
                {
                    var bsName = s.sharedMesh.GetBlendShapeName(x);
                    var prefix = bsName.Split('_')[0];
                    DazBlendshape current;
                    if (bsName.Contains("__"))
                    {
                        bsName = bsName.Split(new string[] {"__"}, StringSplitOptions.None)[1];
                    }

                    current = blendshapesList.Find(m => m.name == bsName);
                    // if (current == null)
                    if (string.IsNullOrEmpty(current.name))
                    {
                        current = DazBlendshape.Init(bsName, s.GetBlendShapeWeight(x));
                        blendshapesList.Add(current);
                    }

                    int x1 = x;
                    current.onValueChangeActions.Add(
                        (v) =>
                        {
                            #if UNITY_EDITOR
                            Undo.RecordObject(s,"Daz Blendshape - Set");
                            #endif
                            
                            s.SetBlendShapeWeight(x1, v);
                        });
                    current.prefixes.Add(prefix);
                }
            }

            blendshapes = blendshapesList.ToArray();
        }

        private void Update()
        {
                if (!dirty) return;
            
            Array.ForEach(blendshapes,m => m.Apply());
            
            dirty = false;
        }

    }
}