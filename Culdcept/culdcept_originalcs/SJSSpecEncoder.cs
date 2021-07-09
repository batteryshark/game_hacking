using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Text;

namespace theli
{
    public sealed class SJSSpecEncoder
    {
        private static Encoding encode = Encoding.GetEncoding(932);
        private static SJSSpecEncoder instance = new SJSSpecEncoder();


        public static SJSSpecEncoder Instance { get { return instance; } }



        public static string Decode(byte[] bytes){
            int index = 0;
            string result="";
            while (index < bytes.Length)
            {
                if (((bytes[index] & 0x80) > 0) && (index < (bytes.Length - 1)))
                {
                    if (bytes[index] == 0xEF)
                    {
                        result += "{" + bytes[index].ToString("X2") + "}";
                        result += "{" + bytes[++index].ToString("X2") + "}";
                    }
                    else if (bytes[index] >= 0xF0)
                    {
                        result += "{" + bytes[index].ToString("X2") + "}";
                    }
                    else
                    {
                        if ( (bytes[index] >= 161) && (bytes[index] <= 223) )
                            result += encode.GetString(bytes, index, 1);
                        else
                            result += encode.GetString(bytes, index++, 2);
                    }
                }
                else
                {
                    if (bytes[index] < 0x20)
                    {
                        if ( bytes[index] == 0x0A)
                        {
                            result += System.Environment.NewLine;
                        } 
                        else
                        {
                            result += "{" + bytes[index].ToString("X2") + "}";
                        }
                    } 
                    else
                    {
                        result += encode.GetString(bytes, index, 1);
                    }
                }
                index++;
            }
            return result;
        }
        
        public static byte[] Encode(string[] input){
            List<byte> list = new List<byte>();
            foreach (string str in input)
            {
                list.AddRange(EncodeString(str));
                list.Add(0x0A);
            }
            if (input.Length > 0)
                list.RemoveAt(list.Count - 1);
            return list.ToArray();
        }
        public static byte[] EncodeString(string input)
        {
            input = input.Replace(System.Environment.NewLine, "{0A}");
            //fix buggy "CP932" in mono on linux
            input = input.Replace("\uFF02", "{EE}{FC}");
            input = input.Replace("\uFF07", "{FA}{56}");
            input = input.Replace("\uFF0D", "{81}{7C}");
            input = input.Replace("\uFF3C", "{81}{5F}");
            input = input.Replace("\uFF5E", "{81}{60}");
            input = input.Replace("\uFFE0", "{81}{91}");
            input = input.Replace("\uFFE1", "{81}{92}");
            input = input.Replace("\uFFE2", "{81}{CA}");
            input = input.Replace("\uFFE3", "{81}{50}");
            input = input.Replace("\uFFE4", "{EE}{FA}");
            input = input.Replace("\uFFE5", "{81}{8F}");
            //fix end

            List<byte> list = new List<byte>();
            if (input.Length == 0)
            {
                return list.ToArray();
            }
            Regex re = new Regex(@"({[A-F0-9][A-F0-9]})", RegexOptions.Compiled);
            string[] strs = re.Split(input);
            foreach (string str in strs)
            {
                if (str.Length > 0)
                {
                    if (re.IsMatch(str))
                    {
                        list.Add(Convert.ToByte(str.Substring(1, 2), 16));
                    } 
                    else
                    {
                        list.AddRange(encode.GetBytes(str));
                    }
                }
            }
            return list.ToArray();
        }

    }
}
