using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using theli;

namespace CuldceptDSEditor
{
    class Script
    {
        public struct ScriptEntry 
        {
            public List<Int32> refs;
            public String str;
        }
        public ScriptEntry[] entries;
        public byte[] data;
        public bool Read(string path,int textoffset)
        {
            
            byte[] buff;
            BinaryReader br;

            #region deprecated
            /*
            br = new BinaryReader(new FileStream(path, FileMode.Open));
            br.ReadByte();
            Int32 uncompsize = br.ReadByte() << 8 | br.ReadByte();
            br.ReadByte();
            buff = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));
            buff = Lha.LHADecode(Lha.LzhCompressMethodType.LH7, buff, uncompsize);
            */
            #endregion

            buff = CuldceptCommon.CuldDeLHA(path);
            br = new BinaryReader(new MemoryStream(buff));

            
            data = br.ReadBytes(textoffset);
            

            List<ScriptEntry> se = new List<ScriptEntry>();

            //br.BaseStream.Position = (long)0x1C0;
            br.BaseStream.Position = BitConverter.ToUInt16(data, 0);
            Dictionary<Int32, Int32> uniqueStore = new Dictionary<Int32, Int32>();
            Int32 x = 0;
            //while (br.BaseStream.Position < 0x5B70)
            while (br.BaseStream.Position < textoffset)
            {
                Int32 a = br.ReadInt32();
                if (((a & 0xFF000000) == 0) && ((a & 0x000000FF) == 0x30))
                {
                    Int32 offset = (Int32)(br.BaseStream.Position + ((a & 0x00FFFF00) >> 8));
                    if (!uniqueStore.ContainsKey(offset))
                    {
                        uniqueStore.Add(offset, x);
                        x++;
                        ScriptEntry entry = new ScriptEntry();
                        entry.refs = new List<Int32>();
                        entry.refs.Add((Int32)br.BaseStream.Position - 3);
                        long pos = br.BaseStream.Position;
                        br.BaseStream.Position = offset;
                        entry.str = Utils.ReadStringZ(br);
                        se.Add(entry);
                        br.BaseStream.Position = pos;
                    }
                    else
                    {
                        se[uniqueStore[offset]].refs.Add((Int32)br.BaseStream.Position - 3);
                    }
                    
                }
                
            }
            entries = se.ToArray();
            br.Close();
            return true;
        }
        public bool Write(String path)
        {
            BinaryWriter bw = new BinaryWriter(new MemoryStream());

            bw.Write(data);
            
            foreach (ScriptEntry entry in entries)
            {
                Int16 offset = (Int16)bw.BaseStream.Position;
                bw.Write(SJSSpecEncoder.EncodeString(entry.str));
                bw.Write((byte)0);
                long pos = bw.BaseStream.Position;
                foreach (Int32 a in entry.refs)
                {
                    bw.BaseStream.Position = a;
                    bw.Write((Int16)(offset - (a + 3)));
                }
                bw.BaseStream.Position = pos;
            }

            Int32 origlength = (Int32) bw.BaseStream.Position;
            byte[] buff = new byte[origlength];
            bw.BaseStream.Position = 0;
            bw.BaseStream.Read(buff, 0, origlength);
            bw.Close();
            //////////
            //(new FileStream("00756.u", FileMode.Create)).Write(buff, 0, buff.Length);
            //for testing
            /////////

            #region deprecated
            /*
            buff = Lha.LHAEncode(Lha.LzhCompressMethodType.LH7, buff);
            bw = new BinaryWriter(new FileStream(path, FileMode.Create));
            bw.Write((byte)0x0C);
            bw.Write((byte)((origlength >> 8) & 0xFF));
            bw.Write((byte)((origlength) & 0xFF));
            bw.Write((byte)0x01);
            bw.Write(buff);
            bw.Close();
            */
            #endregion
            CuldceptCommon.CuldLHA(buff, path);

            return true;
        }

        public void UpdateFromWikiUrl(String url)
        {
            string strResult = Utils.GetUrlContents(url);

            int index, index2;

            //real update

            for (int x = 0; x < entries.Length /*- 1*/; x++)
            {
                index = strResult.IndexOf(String.Format("!{0}", x));
                index = strResult.IndexOf("\n|", index) + 2;
                index = strResult.IndexOf("\n|", index) + 2;
                if ((strResult[index] != '\n') || (strResult[index+1] != '|') )
                {
                    index2 = strResult.IndexOf("\n|", index);
                    entries[x].str = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                }
            }
        }
    }
}
