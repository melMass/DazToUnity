using System;
using System.Collections.Generic;
using UnityEngine;

namespace Daz3D
{
    [Serializable]
    public struct DazBlendshape
    {
        public new string name;

        [SerializeField] public float weight;


        public List<Action<float>> onValueChangeActions;
        public HashSet<string> prefixes;

        // public DazBlendshape(string name, float weight)
        // {
        //     this.name = name;
        //     this.weight = weight;
        //     onValueChangeActions = new List<Action<float>>();
        //     prefixes = new HashSet<string>();
        // }
        public void Apply()
        {
            foreach (var action in onValueChangeActions)
            {
                action.Invoke(weight);
            }
        }

        public static DazBlendshape Init(string initName, float initWeight)
        {
            //var instance = CreateInstance<DazBlendshape>();
            var instance = new DazBlendshape();
            instance.name = initName;
            instance.weight = initWeight;
            instance.onValueChangeActions = new List<Action<float>>();
            instance.prefixes = new HashSet<string>();
            return instance;
        }
    }
}