using System;
using System.Collections.Generic;
using Object = UnityEngine.Object;

namespace Daz3D
{
    [Serializable]
    public class ImportEventRecord
    {
        public DateTime Timestamp = DateTime.Now;

        public struct Token
        {
            public string Text;
            public Object Selectable;
            public bool EndLine;
        }

        public List<Token> Tokens = new List<Token>();

        public bool Unfold = true;

        internal void AddToken(string str, Object obj = null, bool endline = false)
        {
            Tokens.Add(new Token {Text = str, Selectable = obj, EndLine = endline});
        }
    }

    public static class RecordExtensions
    {
        public static void FoldAll(this Queue<ImportEventRecord> records)
        {
            foreach (var record in records)
                record.Unfold = false;
        }
    }
}