using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;

namespace CuldceptDSEditor
{
    class CuldDat
    {
        public struct datEntry
        {
            public Int32 offset;
            public Int32 size;
        }
        public static bool packDAT(string file, string dir)
        {
            const Int32 round = 4;

            Int32 nFiles, size, offset = 0;
            string[] fileArray = Directory.GetFiles(dir);
            nFiles = fileArray.Length;
            Array.Sort(fileArray);

            offset = 8 * nFiles;
            BinaryWriter bw = new BinaryWriter(new FileStream(file, FileMode.Create));

            for (int i = 0; i < nFiles ; i++)
            {
                String fname = fileArray[i];
                FileInfo finfo = new FileInfo(fname);
                size = (int)finfo.Length;
                bw.BaseStream.Position = i * 8;
                bw.Write(offset);
                bw.Write(size);
                bw.BaseStream.Position = offset;
                Int32 writtensize = size;
                BinaryReader br = new BinaryReader(new FileStream(fname, FileMode.Open));
                bw.Write(br.ReadBytes(writtensize));
                br.Close();
                if (writtensize != size)
                {
                    bw.Close();
                    return false;
                }
                offset += Utils.RoundUp(size, round);
            }
            bw.Close();
            return true;

        }
        public static void unpackDAT(string file, string dirname)
        {
            Int32 nFiles;

            BinaryReader br = new BinaryReader(new FileStream(file, FileMode.Open/*,FileAccess.Read*/));

            nFiles = 0;


            if (Directory.Exists(dirname))
            {
                //Console.Write("directory \"" + dirname + "\" already exists, overwrite? [y/n]");
                //if ((Console.Read() == 'y') || (Console.Read() == 'Y'))
                {
                    // Console.WriteLine("deleting \"" + dirname + "\"");
                    Directory.Delete(dirname, true);
                }
                //else
                //    return;
            }
            Directory.CreateDirectory(dirname);

            int i = 0;

            datEntry[] Files = new datEntry[5636];

            Files[0] = new datEntry();
            Files[0].offset = br.ReadInt32();
            Files[0].size = br.ReadInt32();
            while (br.BaseStream.Position < Files[0].offset)
            {
                nFiles++;
                Files[nFiles] = new datEntry();
                Files[nFiles].offset = br.ReadInt32();
                Files[nFiles].size = br.ReadInt32();
            }
            byte[] buff;
            BinaryWriter writer;

            for (i = 0; i <= nFiles; i++)
            {
                datEntry entry = Files[i];
                buff = new byte[entry.size];
                br.BaseStream.Position = entry.offset;
                br.Read(buff, 0, entry.size);
                writer = new BinaryWriter(File.Open(dirname + System.IO.Path.DirectorySeparatorChar + String.Format("{0:00000}", i), FileMode.Create));
                writer.Write(buff, 0, entry.size);
                writer.Close();
            }
            br.Close();
            //Console.WriteLine("done unpacking to \"" + dirname + "\"");
        }

    }
}
