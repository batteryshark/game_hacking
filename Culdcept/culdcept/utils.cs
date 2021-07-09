using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Diagnostics;
using System.Net;
using System.Drawing;

namespace theli
{
    public class ListComparer<T> : IEqualityComparer<List<T>>
    {
        public bool Equals(List<T> x, List<T> y)
        {
            return x.SequenceEqual(y);

        }
        public int GetHashCode(List<T> obj)
        {
            int hash = 17;

            foreach (var value in obj)
                hash = hash * 23 + value.GetHashCode();

            return hash;
        }
    }
    public class ColorComparer : IEqualityComparer<System.Drawing.Color>
    {
        public bool Equals(System.Drawing.Color x, System.Drawing.Color y)
        {
            return x.ToArgb() == y.ToArgb();

        }
        public int GetHashCode(System.Drawing.Color obj)
        {
            return obj.ToArgb();
        }
    }
    public class Utils
    {
        public static IntPtr PtrOffset(IntPtr src, int offset)
        {
            switch (IntPtr.Size)
            {
                case 4:
                    return new IntPtr(src.ToInt32() + offset);
                case 8:
                    return new IntPtr(src.ToInt64() + offset);
                default:
                    throw new NotSupportedException("Surprise!  This is running on a machine where pointers are " + IntPtr.Size + " bytes and arithmetic doesn't work in C# on them.");
            }
        }
        public static string ReadStringZ(BinaryReader reader)
        {
            ArrayList al = new ArrayList();
            byte c;
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                if ((c = (byte)reader.ReadByte()) == 0)
                {
                    break;
                }
                al.Add((byte)c);
            }
            return SJSSpecEncoder.Decode((byte[])al.ToArray(typeof(byte)));
        }
        public static int RoundUp(int num, int multiple)
        {
            if (multiple == 0)
                return 0;
            int add = multiple / Math.Abs(multiple);
            return ((num + multiple - add) / multiple) * multiple;
        }
        public static void SystemCall(string command)
        {
            Process process = new Process();
            ProcessStartInfo info;
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
               info = new ProcessStartInfo("bash", "-c \"" + command + "\"");
            else
               info = new ProcessStartInfo("cmd ", "/D /c " + command);
            info.UseShellExecute = false;
            info.CreateNoWindow = true;
            process.StartInfo = info;
            process.EnableRaisingEvents = false;
            process.Start();
            process.WaitForExit();
        }
        public static UInt32 rotl(UInt32 value, Int32 shift)
        {
            shift &= 31;
            return (value << shift) | (value >> (32 - shift));
        }
        public static UInt32 rotr(UInt32 value, Int32 shift)
        {
            shift &= 31;
            return (value >> shift) | (value << (32 - shift));
        }
        public static String GetUrlContents(String url){
            string strResult = "";

            WebResponse objResponse;
            WebRequest objRequest = System.Net.HttpWebRequest.Create(url);

            objResponse = objRequest.GetResponse();

            using (StreamReader sr = new StreamReader(objResponse.GetResponseStream()))
            {
                strResult = sr.ReadToEnd();
                // Close and clean up the StreamReader
                sr.Close();
            }
            return strResult;
        }
   }
}

