using System;
using System.Collections.Generic;
using System.Text;
using theli;
using System.IO;
using System.Collections;


namespace CuldceptDSEditor
{
    public struct textEntry
    {
        public Int32 offset;
        public Int32 maxsize;
        public String text;
    }
    class Arm9Data
    {
        public textEntry[] entries;
        public static bool UnpackARM9(String src, String dst)
        {
            if (!File.Exists(src))
                return false;
            Int32 start_size, d, s;
            Int32 endoffset = 0xC;
            BinaryReader br = new BinaryReader(new FileStream(src, FileMode.Open));
            br.BaseStream.Position = 0xbb4;
            if (br.ReadInt32() == 0)
            {
                br.Close();
                File.Copy(src, dst);
                return true;
            }
            br.BaseStream.Position = br.BaseStream.Length - endoffset - 8;
            start_size = br.ReadInt32();
            d = br.ReadInt32();
            d += (int)br.BaseStream.Length - endoffset;
            s = (int)br.BaseStream.Length - endoffset - (start_size >> 24);
            start_size &= 0x00FFFFFF;
            start_size = (int)br.BaseStream.Length - endoffset - start_size;

            byte[] data = new byte[d];
            br.BaseStream.Position = 0;
            br.Read(data, 0, (int)br.BaseStream.Length);
            br.Close();

            while (s > start_size)
            {
                byte b = data[--s];
                for (int i = 0; (i < 8) && (s > start_size); i++,b <<= 1)
                {
                    if ((b & 0x80) == 0)
                        data[--d] = data[--s];
                    else
                    {
                        Int32 len = data[--s];
                        Int32 link = ((data[--s] | (len << 8)) & 0x0FFF) + 2;
                        len += 0x20;
                        while (len >= 0)
                        {
                            data[--d] = data[d + 1 + link];
                            len -= 0x10;
                        }
                    }
                }
            }
            data[0xbb4] = data[0xbb5] = data[0xbb6] = data[0xbb7] = 0;
            BinaryWriter bw = new BinaryWriter(new FileStream(dst, FileMode.Create));
            bw.Write(data, 0, data.Length);
            bw.Close();
            return true;
        }
        #region From ASM
        /*
        public static bool UnpackARM9(String src, String dst)
        {
            // just a rip from ASM for now :D
            // skip 79010 steps from beginning in desmume


            Int32 offset = -0x2000000;
            if (!File.Exists(src))
            {
                return false;
            }

            Int32 r0, r1, r2, r3, r5, r6, r7, r12;
            r0 = 0x20C8828;

            BinaryReader br = new BinaryReader(new FileStream(src, FileMode.Open));
            br.BaseStream.Position = 0xbb4;
            if (br.ReadInt32() == 0)
            {
                br.Close();
                File.Copy(src, dst);
                return true;
            }
            br.BaseStream.Position = offset + r0 - 4 - 4;
            r1 = br.ReadInt32();
            r2 = br.ReadInt32();
            r2 += r0;
            r3 = r0 - (r1 >> 24);
            r1 &= 0x00FFFFFF;
            r1 = r0 - r1;
            //r4 = r2;

            byte[] data = new byte[offset + r2];
            br.BaseStream.Position = 0;
            br.Read(data, 0, (int)br.BaseStream.Length);
            br.Close();

        loc_2000994:

            if (r3 <= r1)
                goto loc_20009F4;
            r5 = data[--r3 + offset];
            r6 = 8;

        loc_20009A4:
            r6--;
            if (r6 < 0)
                goto loc_2000994;
            if ((r5 & 0x80) != 0)
                goto loc_20009C0;
            r0 = data[--r3 + offset];
            data[--r2 + offset] = (byte)r0;
            goto loc_20009E8;

        loc_20009C0:
            r12 = data[--r3 + offset];
            r7 = data[--r3 + offset];
            r7 |= r12 << 8;
            r7 &= 0x0fff;
            r7 += 2;
            r12 += 0x20;

        loc_20009D8:
            r0 = data[r2 + r7 + offset];
            data[--r2 + offset] = (byte)r0;
            r12 -= 0x10;
            if (r12 >= 0)
                goto loc_20009D8;

        loc_20009E8:
            r5 <<= 1;
            if (r3 > r1)
                goto loc_20009A4;

        loc_20009F4:
            //loc_20009FC:

            data[0xbb4] = 0;
            data[0xbb5] = 0;
            data[0xbb6] = 0;
            data[0xbb7] = 0;

            BinaryWriter bw = new BinaryWriter(new FileStream(dst, FileMode.Create));
            bw.Write(data, 0, data.Length);
            bw.Close();

            return true;
        }
        */
        #endregion
        public void SimpleTextRead(UInt32 offset, UInt32 endoffset, String file)
        {
            BinaryReader br = new BinaryReader(new FileStream(file, FileMode.Open));
            br.BaseStream.Position = offset;
            System.Collections.ArrayList result = new ArrayList();
            while (br.BaseStream.Position < endoffset)
            {
                textEntry entry = new textEntry();
                entry.offset = (Int32)br.BaseStream.Position;
                entry.text = Utils.ReadStringZ(br);
                //Int32 size = (Int32)br.BaseStream.Position - entry.offset;
                while ((br.ReadByte() == 0) && (br.BaseStream.Position < endoffset)) ;
                br.BaseStream.Position--;
                entry.maxsize = (Int32)br.BaseStream.Position - entry.offset - 1;
                //if (entry.maxsize < size)
                //    entry.maxsize = size;
                result.Add((textEntry)entry);
            }
            br.Close();
            //return (textEntry[])result.ToArray(typeof(textEntry));
            entries = (textEntry[])result.ToArray(typeof(textEntry));
        }
        public void save (String file){
            //BinaryWriter bw = new BinaryWriter(new FileStream(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked",FileMode.Open));
            BinaryWriter bw = new BinaryWriter(new FileStream(file,FileMode.Open));

            for (int x = 0; x < entries.Length; x++)
            {
                bw.BaseStream.Position = entries[x].offset;
                byte[] data = new byte[entries[x].maxsize];
                byte[] newdata = SJSSpecEncoder.EncodeString(entries[x].text);
                if (newdata.Length <= data.Length)
                    newdata.CopyTo(data, 0);
                else
                    Array.Copy(newdata, data, entries[x].maxsize);
                

                bw.Write(data, 0, data.Length);
            }
            bw.Close();
        }
        public void UpdateFromWikiUrl(String url)
        {
            string strResult = Utils.GetUrlContents(url);

            int index, index2;
            
            for (int x = 0; x < entries.Length - 1; x++)
            {
                index = strResult.IndexOf(String.Format("|{0}", entries[x].offset));
                index = strResult.IndexOf("\n|", index) + 2;
                index = strResult.IndexOf("\n|", index) + 2;
                index = strResult.IndexOf("\n|", index) + 2;
				if ((strResult[index] != '\n') || (strResult[index+1] != '|'))
                {
					index2 = strResult.IndexOf("\n|", index);
				    entries[x].text = strResult.Substring(index, index2 - index).Replace("\n", System.Environment.NewLine);
				}
            }
        }
    }

}
