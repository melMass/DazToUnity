using UnityEditor;
using UnityEngine;

namespace Daz3D
{
    [CustomEditor(typeof(BlendshapesSyncedController))]
    public class BlendshapesControllerEditor : Editor
    {
        SerializedProperty _blendshapes;
        SerializedProperty _dirty;
        private BlendshapesSyncedController _controller;

        private void OnEnable()
        {
            _controller = target as BlendshapesSyncedController;
            _blendshapes = serializedObject.FindProperty("blendshapes");
            _dirty = serializedObject.FindProperty("dirty");
        }

        public override void OnInspectorGUI()
        {
            serializedObject.Update();

            // if (_dirty.boolValue)
            // {
            //     EditorGUILayout.HelpBox("Dirty!",MessageType.Error);
            // }
            
            // ReSharper disable once ForCanBeConvertedToForeach
            for (int x = 0; x < _blendshapes.arraySize; x++)
            {
                EditorGUI.BeginChangeCheck();
                var blendShape = _blendshapes.GetArrayElementAtIndex(x);
                EditorGUILayout.PropertyField(blendShape);
                if (EditorGUI.EndChangeCheck())
                {
                    _dirty.boolValue = true;
                }
            }

            if (GUILayout.Button("Clear All"))
            {
                for (int x = 0; x < _blendshapes.arraySize; x++)
                {
                    var blendShape = _blendshapes.GetArrayElementAtIndex(x);
                    blendShape.FindPropertyRelative("weight").floatValue = 0;
                }
                _dirty.boolValue = true;
            }

            serializedObject.ApplyModifiedProperties();
        }
    }
}