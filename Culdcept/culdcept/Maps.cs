using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;

namespace CuldceptDSEditor
{
    public struct mapEntry
    {
        public Int32 unk1;
        public Int32 unk2;
        public Int32 dataoffset;
        public byte[] unk3;// = new byte[0x30];
        public String name;
        public String description;
        //public Int16 datasig;
        //public Int16 datalength;
        public byte[] data;
    }
    class Maps
    {
        private mapEntry[] entries;
        public long Count { get { return entries.Length; } }
        public DescHelper Desc{
            get { return new DescHelper(this); }
        }
        public NameHelper Name
        {
            get { return new NameHelper(this); }
        }
        public LengthHelper Length
        {
            get { return new LengthHelper(this); }
        }
        public DataOffsetHelper DataOffset
        {
            get { return new DataOffsetHelper(this); }
        }        

        #region Description named indexer helper class
        public class DescHelper
        {
            private Maps parent;
            public DescHelper(Maps parent)
            {this.parent = parent;}
            public String this[int index]
            {
                get{return parent.entries[index].description;}
                set{parent.entries[index].description = value;}
            }
        }
        #endregion
        #region Name named indexer helper class
        public class NameHelper
        {
            private Maps parent;
            public NameHelper(Maps parent)
            { this.parent = parent; }
            public String this[int index]
            {
                get { return parent.entries[index].name; }
                set { parent.entries[index].name = value; }
            }
        }
        #endregion
        #region DataOffsetHelper named indexer helper class
        public class DataOffsetHelper
        {
            private Maps parent;
            public DataOffsetHelper(Maps parent)
            { this.parent = parent; }
            public Int32 this[int index]
            {
                get
                {
                    Int32 length = 0;
                    length += 4; //unk1
                    length += 4; //unk2
                    length += 4; //dataoffset
                    length += 0x30; //unk3
                    length += SJSSpecEncoder.Encode(parent.entries[index].name.Split(System.Environment.NewLine.ToCharArray())).Length + 1; //Name + 0x00
                    length += SJSSpecEncoder.Encode(parent.entries[index].description.Split(System.Environment.NewLine.ToCharArray())).Length + 1; //Desc + 0x00

                    return length;
                }
            }
        }
        #endregion
        #region Length named indexer helper class
        public class LengthHelper
        {
            private Maps parent;
            public LengthHelper(Maps parent)
            { this.parent = parent; }
            public Int32 this[int index]
            {
                get {
                    Int32 length = parent.DataOffset[index];
                    //////////////////////////////////////////////////////////////////////////
                    //length += 2;//datasig
                    //length += 2;//datalength
                    //length += parent.entries[index].datalength;//datalength
                    //////////////////////////////////////////////////////////////////////////
                    length += parent.entries[index].data.Length;
                    return length;
                }
            }
        }
        #endregion

        public bool Read(string path){
            BinaryReader br = new BinaryReader(new FileStream(path, FileMode.Open));
            br.BaseStream.Position = 0;
            List<int> offsets = new List<int>();
            offsets.Add(br.ReadInt32());
            
            while (br.BaseStream.Position < offsets[0])
                offsets.Add(br.ReadInt32());
            entries = new mapEntry[offsets.Count];
            for (int i = 0 ; i < offsets.Count ; i ++)
            {
                Int32 offset = offsets[i];
                br.BaseStream.Position = offset;
                mapEntry entry = new mapEntry();
                entry.unk1 = br.ReadInt32();
                entry.unk2 = br.ReadInt32();
                entry.dataoffset = br.ReadInt32();
                entry.unk3 = new byte[0x30];
                br.Read(entry.unk3, 0, 0x30);
                entry.name = Utils.ReadStringZ(br);
                entry.description = Utils.ReadStringZ(br);
                br.BaseStream.Position = offset + entry.dataoffset;
                //entry.datasig = br.ReadInt16();
                //entry.datalength = br.ReadInt16();
                /////FIXXXXXXX
                ////STUPID!
                int datalength = 0;
                if (i == offsets.Count - 1)
                {
                    datalength = (int)(br.BaseStream.Length - br.BaseStream.Position);
                } 
                else
                {
                    datalength = (int)(offsets[i+1] - br.BaseStream.Position);
                }
                //////////////////////////////////////////////////////////////////////////
                entry.data = new byte[datalength];
                br.Read(entry.data, 0, datalength);
                entries[i] = entry;
            }
            br.Close();
            return true;
        }
        public void UpdateFromUrl(string url)
        {
            string strResult = Utils.GetUrlContents(url);

            int index, index2;
            for (int x = 0; x < Count; x++)
            {
                index = strResult.IndexOf(String.Format("|{0}", x));
                index = strResult.IndexOf("\n|", index) + 2;
                index2 = strResult.IndexOf("\n|", index);
                Name[x] = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                Desc[x] = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
            }

        }
        public bool Write(string path){
            Int32 mainoffset = entries.Length * 4;
            long round = 4;
            List<Int32> offsets = new List<Int32>();

            byte b = 0x00;
            byte[] data;
            BinaryWriter bw = new BinaryWriter(new FileStream(path, FileMode.Create));
            bw.BaseStream.Position = mainoffset;
            for (int i = 0; i < entries.Length; i ++ )
            {
                mapEntry entry = entries[i];
                offsets.Add((int)bw.BaseStream.Position);
                entry.dataoffset = this.DataOffset[i];
                bw.Write(entry.unk1);
                bw.Write(entry.unk2);
                bw.Write(entry.dataoffset);
                bw.Write(entry.unk3);
                data = SJSSpecEncoder.Encode(entry.name.Split(System.Environment.NewLine.ToCharArray()));
                bw.Write(data);
                bw.Write(b);
                data = SJSSpecEncoder.Encode(entry.description.Split(System.Environment.NewLine.ToCharArray()));
                bw.Write(data);
                bw.Write(b);
                //////////////////////////////////////////////////////////////////////////
                //FIXXXX
                //bw.Write(entry.datasig);
                //bw.Write(entry.datalength);
                //////////////////////////////////////////////////////////////////////////
                bw.Write(entry.data);
                while (bw.BaseStream.Position % round != 0)
                {
                    bw.Write(b);
                }
            }
            bw.BaseStream.Position = 0;
            foreach (Int32 offset in offsets)
            {
                bw.Write(offset);
            }
            bw.Close();

            return true;
        }
    }
}
