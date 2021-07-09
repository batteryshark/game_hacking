using System;
using System.IO;


namespace theli
{
    class CuldceptCommon
    {
        public static void Unpack00001 (String src, String dst){
            if (Directory.Exists(dst))
                Directory.Delete(dst, true);
            Directory.CreateDirectory(dst);

            long curr, next, first,pos;
            BinaryReader br = new BinaryReader(new FileStream(src, FileMode.Open));
            first = (br.ReadByte() << 16 )| (br.ReadByte() << 8) | br.ReadByte() ;
            next = first;
            byte[] buff;
            for (int i = 0; br.BaseStream.Position < first  - 3; i++ )
            {
                curr = next;
                next = (br.ReadByte() << 16) | (br.ReadByte() << 8) | br.ReadByte();
                pos = br.BaseStream.Position;
                br.BaseStream.Position = curr;
                buff = br.ReadBytes((int)(next - curr));
                (new BinaryWriter(new FileStream(dst + System.IO.Path.DirectorySeparatorChar + String.Format("{0:00000}", i),FileMode.Create))).Write(CuldDeLHA(buff));
                br.BaseStream.Position = pos;
            }
            br.Close();
            
        }
        public static void Pack00001(String src, String dst)
        {

        }
        public static byte[] CuldDeLHA(String src){
            BinaryReader br = new BinaryReader(new FileStream(src, FileMode.Open));
            return CuldDeLHA(br);
        }
        public static byte[] CuldDeLHA(byte[] src)
        {
            BinaryReader br = new BinaryReader(new MemoryStream(src));
            return CuldDeLHA(br);
        }
        private static byte[] CuldDeLHA(BinaryReader br)
        {
            byte algo = br.ReadByte();
            Int32 uncompsize;
            Int32 b = br.ReadSByte();
            uncompsize = br.ReadByte();
            uncompsize |= (b << 8);
            if ((uncompsize & 0x80000000) != 0)
            {
                uncompsize &= 0x7FFF;
                b = br.ReadSByte();
                uncompsize |= (b << 0xF);
                if ((uncompsize & 0x80000000) != 0)
                {
                    uncompsize &= 0x3FFFFF;
                    b = br.ReadByte();
                    uncompsize |= (b << 0x16);
                }
            }

            byte[] data = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));
            br.Close();
            Console.WriteLine("algo: {0}, uncompsize: {1}", algo, uncompsize);
            if (algo == 0xC)
                data = Lha.LHADecode(Lha.LzhCompressMethodType.LH7, data, uncompsize);
            //else if (algo == 0x5)
            //    data = Lha.LHADecode(Lha.LzhCompressMethodType.LH7, data, uncompsize);
            else
                data = Lha.LHADecode(Lha.LzhCompressMethodType.LH5, data, uncompsize);
            return data;
        }
        public static void CuldLHA(byte[]buff ,String dest)
        {
            Int32 origlength;
            origlength = buff.Length;
            buff = Lha.LHAEncode(Lha.LzhCompressMethodType.LH7, buff);

            BinaryWriter bw = new BinaryWriter(new FileStream(dest, FileMode.Create));
            bw.Write((byte)0x0C);
            bw.Write((byte)((origlength >> 8) & 0xFF));
            bw.Write((byte)((origlength) & 0xFF));
            if (origlength >= 0x8000)
            {

                bw.Write((byte)((origlength >> 0xF)));
                if (origlength >= 0x800000)
                {
                    throw new SystemException("TODO!");
                }
            }

            bw.Write(buff);
            bw.Close();
        }
    }
}
