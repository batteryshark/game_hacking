using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;
using theli;

namespace CuldceptDSEditor
{
    public class cardEntry
    {
        private Int16 ID;
        private Int16 StringsOffset;
        private Int16 HP;
        private Int16 ST;
        private Int16 G;
        private byte[] unk1;// = new byte[0x26];
        private Int16 Unkoffset;
        public String title;
        private byte[] unk3;// = new byte[StringsOffset - Unk3Offset]
        public String name;
        public String ability;
        public String ability2;
        public String ability3;
        public String description;
        public Int16 _HP { get { return HP; } }
        public Int16 _ID { get { return ID; } }
        public Int16 _ST { get { return ST; } }
        public Int16 _G { get { return G; } }
        public void Read(BinaryReader br){
            Int32 pos = (int)br.BaseStream.Position;
            ID = br.ReadInt16();
            StringsOffset = br.ReadInt16();
            HP = br.ReadInt16();
            ST = br.ReadInt16();
            G = br.ReadInt16();
            unk1 = br.ReadBytes(0x26);

            title = Utils.ReadStringZ(br);
            br.BaseStream.Position = Utils.RoundUp((int)br.BaseStream.Position, 4);
            Unkoffset = (Int16)(br.BaseStream.Position - pos);
            unk3 = br.ReadBytes(pos + StringsOffset - (int)br.BaseStream.Position);

            br.BaseStream.Position = pos + StringsOffset;
            ability = Utils.ReadStringZ(br);
            ability2 = Utils.ReadStringZ(br);
            if (this.GetType().ToString() == "CuldceptDSEditor.cardEntry")
            {
                name = Utils.ReadStringZ(br);
                description = Utils.ReadStringZ(br);
                ability3 = Utils.ReadStringZ(br);
            }
        }
        public void Write(BinaryWriter bw){
            Int32 pos = (int)bw.BaseStream.Position;
            Int32 round = 4;
            bw.Write(ID);
            bw.Write(StringsOffset);
            bw.Write(HP);
            bw.Write(ST);
            bw.Write(G);
            bw.Write(unk1);
            bw.Write(SJSSpecEncoder.EncodeString(title));
            bw.Write((byte)0);
            do 
                bw.Write((byte)0);
            while (bw.BaseStream.Position % round != 0);

            Int16 diff = (Int16)(Unkoffset - (bw.BaseStream.Position - pos));
            bw.Write(unk3);

            StringsOffset = (Int16)(bw.BaseStream.Position - pos);
            bw.Write(SJSSpecEncoder.EncodeString(ability));
            bw.Write((byte)0);
            bw.Write(SJSSpecEncoder.EncodeString(ability2));
            bw.Write((byte)0);

            if (this.GetType().ToString() == "CuldceptDSEditor.cardEntry")
            {
                bw.Write(SJSSpecEncoder.EncodeString(name));
                bw.Write((byte)0);
                bw.Write(SJSSpecEncoder.EncodeString(description));
                bw.Write((byte)0);
                bw.Write(SJSSpecEncoder.EncodeString(ability3));
            }
            do
                bw.Write((byte)0);
            while (bw.BaseStream.Position % round != 0);

            Int32 endpos = (int)bw.BaseStream.Position;

            for (int x = 0; x < 4; x++ )
            {
                bw.BaseStream.Position = pos + 0x22 + x * 4;
                Int16 offset = (new BinaryReader(bw.BaseStream)).ReadInt16();
                if (offset != 0)
                {
                    offset -= diff;
                    bw.BaseStream.Position = pos + 0x22 + x * 4;
                    bw.Write(offset);
                }
            }

            bw.BaseStream.Position = pos + 2;
            bw.Write(StringsOffset);
            bw.BaseStream.Position = endpos;
        }
        public Int32 GetLength(){
            return 0;
        }
    }
    public class ShrineEntry : cardEntry { };
    public class crestEntry 
    {
        public String title;
        public String name;
        public String str3;
        public String str4;
        public String str5;
        public void Read(BinaryReader br)
        {
            title = Utils.ReadStringZ(br);
            name = Utils.ReadStringZ(br);
            str3 = Utils.ReadStringZ(br);
            str4 = Utils.ReadStringZ(br);
            str5 = Utils.ReadStringZ(br);
        }
        public void Write(BinaryWriter bw){
            throw new SystemException();
        }
    }
    public class block{
        //no need for Block 0 since offset is ZERO 8-)
        public const Int32 Block1Offset = 0x286FC;
        public const Int32 Block2Offset = 0x2B990;
        public const Int32 Block3Offset = 0x2C704;
        public const Int32 Block4Offset = 0x28B20;
        public const Int32 Block5Offset = 0x370F0;
        public block(Int32 _arm9offset)
        {
            arm9offset = _arm9offset;
        }

        private Int32 arm9offset;
        public Int32 dataoffset;
        public byte[] data;
        public Int32 ReadPart(BinaryReader br)
        {
            Int32 buff;
            buff = br.ReadInt32();
            return (int)Utils.rotr((uint)(buff & 0xFF), (buff & 0xF00) >> 7);
        }
        public Int32 ReadOffset(BinaryReader br)
        {
            br.BaseStream.Position = arm9offset;
            return ReadPart(br) + ReadPart(br);
        }
        public void Write(BinaryWriter bw){
            bw.BaseStream.Position = arm9offset + 4;
            Int32 part = (dataoffset & (0xFF << 0xA));
            Int32 part2 = dataoffset - part;
            bw.BaseStream.Position = arm9offset;
            Int32 arm = (new BinaryReader(bw.BaseStream)).ReadInt32();
            bw.BaseStream.Position = arm9offset;
            arm &= (Int32)~0xFFF;
            arm |= ((part2 >> 2) & 0xFF) | 0xF00;
            bw.Write((Int32)(arm));

            arm = (new BinaryReader(bw.BaseStream)).ReadInt32();
            bw.BaseStream.Position -= 4;
            arm &= (Int32)~0xFFF;
            arm |= (part >> 0xA) | 0xb00;
            bw.Write((Int32)(arm));


            if (ReadOffset(new BinaryReader(bw.BaseStream)) != this.dataoffset)
            {
                throw new SystemException();
            }
        }
    }
    class CardsNCrests
    {
        public cardEntry[] entries;
        public crestEntry[] crestentries;
        private block[] blocks;
        public long Count { get { return entries.Length; } }
        public long CrestCount { get { return crestentries.Length; } }
        
        public bool Read(string path)
        {
            byte[] buff;
            BinaryReader br;
            blocks = new block[5];

            buff = CuldceptCommon.CuldDeLHA(path);
            //////////
            //(new FileStream("316.orig", FileMode.Create)).Write(buff, 0, buff.Length);
            //for testing
            /////////
            br = new BinaryReader(new MemoryStream(buff));

            List<Int32> offsets = new List<Int32>();
            offsets.Add(br.ReadInt32());
            while (br.BaseStream.Position < offsets[0])
                offsets.Add(br.ReadInt32());
            entries = new cardEntry[offsets.Count];
            for (Int32 i = 0; i < offsets.Count; i++ )
            {
                br.BaseStream.Position = offsets[i];
                (entries[i] = new cardEntry()).Read(br);
            }
            br.BaseStream.Position = Utils.RoundUp((int)br.BaseStream.Position, 4);
            blocks[0] = new block(block.Block1Offset);
            blocks[0].data = br.ReadBytes(0x514);
            blocks[1] = new block(block.Block2Offset);
            blocks[1].data = br.ReadBytes(0x110);
            blocks[2] = new block(block.Block3Offset);
            blocks[2].data = br.ReadBytes(0x320);
            List<crestEntry> crestlist = new List<crestEntry>();
            crestEntry entry;
            while (buff[br.BaseStream.Position] != 0)
            {
               (entry = new crestEntry()).Read(br);
               crestlist.Add(entry); 
            }
            crestentries = crestlist.ToArray();
            br.BaseStream.Position = Utils.RoundUp((int)br.BaseStream.Position, 4);
            //unkData2 = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));
            blocks[3] = new block(block.Block4Offset);
            blocks[3].data = br.ReadBytes(0xBBC);
            blocks[4] = new block(block.Block5Offset);
            blocks[4].data = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));

            return true;
        }
        public bool Write(string path,string arm9bin)
        {
            
            Int32 mainoffset = entries.Length * 4;
            List<Int32> offsets = new List<Int32>();
            BinaryWriter bw = new BinaryWriter(new MemoryStream());
            bw.BaseStream.Position = mainoffset;
            for (int i = 0; i < entries.Length; i++)
            {
                offsets.Add((int)bw.BaseStream.Position);
                entries[i].Write(bw);
            }
            blocks[0].dataoffset = (int)bw.BaseStream.Position;
            bw.Write(blocks[0].data);
            blocks[1].dataoffset = (int)bw.BaseStream.Position;
            bw.Write(blocks[1].data);
            blocks[2].dataoffset = (int)bw.BaseStream.Position;
            //////////////////////////////////////////////////////////////////////////
            long crestsoffset = bw.BaseStream.Position;
            bw.Write(blocks[2].data);
            for (int i = 0; i < crestentries.Length; i++)
            {
                BitConverter.GetBytes((UInt16)((bw.BaseStream.Position - crestsoffset) - i * 0x10)).CopyTo(blocks[2].data, i * 0x10);
                bw.Write(SJSSpecEncoder.EncodeString(crestentries[i].title));
                bw.Write((byte)0);

                bw.Write(SJSSpecEncoder.EncodeString(crestentries[i].name));
                bw.Write((byte)0);

                BitConverter.GetBytes((UInt16)((bw.BaseStream.Position - crestsoffset) - i * 0x10)).CopyTo(blocks[2].data, 2 + i * 0x10);
                bw.Write(SJSSpecEncoder.EncodeString(crestentries[i].str3));
                bw.Write((byte)0);
                
                BitConverter.GetBytes((UInt16)((bw.BaseStream.Position - crestsoffset) - i * 0x10)).CopyTo(blocks[2].data, 4 + i * 0x10);
                bw.Write(SJSSpecEncoder.EncodeString(crestentries[i].str4));
                bw.Write((byte)0);
                
                BitConverter.GetBytes((UInt16)((bw.BaseStream.Position - crestsoffset) - i * 0x10)).CopyTo(blocks[2].data, 6 + i * 0x10);
                bw.Write(SJSSpecEncoder.EncodeString(crestentries[i].str5));
                bw.Write((byte)0);
            }
            bw.BaseStream.Position = crestsoffset;
            bw.Write(blocks[2].data);
            bw.BaseStream.Position = bw.BaseStream.Length;
            //////////////////////////////////////////////////////////////////////////
            bw.BaseStream.Position = Utils.RoundUp((int)bw.BaseStream.Position, 4);
            blocks[3].dataoffset = (int)bw.BaseStream.Position;
            bw.Write(blocks[3].data);
            blocks[4].dataoffset = (int)bw.BaseStream.Position;
            bw.Write(blocks[4].data);
            Int32 origlength = (int)bw.BaseStream.Position;
            bw.BaseStream.Position = 0;
            foreach (Int32 offset in offsets)
            {
                bw.Write(offset);
            }
            byte[] buff = new byte[origlength];
            bw.BaseStream.Position = 0;
            bw.BaseStream.Read(buff, 0, origlength);
            //////////
            //FileStream fs;
            //(fs = new FileStream("316.u", FileMode.Create)).Write(buff, 0, buff.Length);
            //fs.Close();
            //for testing
            /////////

            CuldceptCommon.CuldLHA(buff, path);

            //changing offsets in arm9.bin
            BinaryWriter arm9bw = new BinaryWriter(new FileStream(arm9bin, FileMode.Open));
            foreach (block b in blocks)
                b.Write(arm9bw);            
            arm9bw.Close();
            return true;
        }
        public void UpdateFromWikiUrl(String url){
            string strResult = Utils.GetUrlContents(url);

            int index,index2;
            for (int x = 0; x < entries.Length; x++ )
            {
                index = strResult.IndexOf( String.Format("![[Tooltip:Card/{0}|{0}]]",x));
                index = strResult.IndexOf("\n|",index)+2;
                index2 = strResult.IndexOf("\n|", index);
                entries[x].title = strResult.Substring(index, index2 - index).Replace("\n",System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
				entries[x].name = strResult.Substring(index, index2 - index).Replace("\n",System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                entries[x].ability = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                entries[x].ability2 = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);                
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                entries[x].ability3 = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                entries[x].description = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
            }
        }
        public void UpdateCrestsFromWikiUrl(String url)
        {
            string strResult = Utils.GetUrlContents(url);

            int index, index2;
            for (int x = 0; x < crestentries.Length; x++)
            {
                index = strResult.IndexOf(String.Format("|{0}", x));
                index = strResult.IndexOf("\n|", index) + 2;
                index2 = strResult.IndexOf("\n|", index);
                crestentries[x].title = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                crestentries[x].name = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                crestentries[x].str3 = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                crestentries[x].str4 = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
                index = index2 + 2;
                index2 = strResult.IndexOf("\n|", index);
                crestentries[x].str5 = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
            }
        }
    }
}
