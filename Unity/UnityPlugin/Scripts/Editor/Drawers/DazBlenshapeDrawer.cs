using System;
using UnityEditor;
using UnityEngine;

namespace Daz3D
{
    [CustomPropertyDrawer(typeof(DazBlendshape))]
    public class DazBlendshapeDrawer : PropertyDrawer
    {
        // Draw the property inside the given rect
        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            // Using BeginProperty / EndProperty on the parent property means that
            // prefab override logic works on the entire property.
            EditorGUI.BeginProperty(position, label, property);

            // Draw label
            //position = EditorGUI.PrefixLabel(position, GUIUtility.GetControlID(FocusType.Passive), label);

            // Don't make child fields be indented
            var indent = EditorGUI.indentLevel;
            EditorGUI.indentLevel = 0;

            // Calculate rects
            position = new Rect(position.x, position.y, position.width - 70, position.height);

            var weight = property.FindPropertyRelative("weight");
            
            EditorGUI.Slider(position, weight, 0, 100, label);
            
            position = new Rect(position.width + 30, position.y, 30, position.height);
            
            // Quick access buttons
            if (GUI.Button(position, new GUIContent("C", "[Clear] Sets the weight to 0.")))
            {
                weight.floatValue = 0f;
                GUI.changed = true;
            }

            position = new Rect(position.x + 30, position.y, 30, position.height);
            if (GUI.Button(position, new GUIContent("M", "[Middle|Max] Sets the weight to 100 or 50 (toggles)")))
            {
                weight.floatValue = Math.Abs(weight.floatValue - 100) < 0.01 ? 50 : 100;
                GUI.changed = true;
            }

            // Set indent back to what it was
            EditorGUI.indentLevel = indent;
            EditorGUI.EndProperty();

        }
    }
}