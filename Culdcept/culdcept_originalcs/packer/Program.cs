using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;

namespace packer
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
            if ((args.Length < 3) || ((args[0] != "d") && (args[0]) != "c"))
            {
                Console.WriteLine("usage:");
                Console.WriteLine("\tdecompress:\t packer.exe d source destination");
                Console.WriteLine("\tcompress:\t packer.exe c source destination");
            }
            if (args[0] == "c")
            {
                BinaryReader br = new BinaryReader(new FileStream(args[1], FileMode.Open));
                CuldceptCommon.CuldLHA(br.ReadBytes((int)br.BaseStream.Length), args[2]);
            }
            else
            {
                byte[] data = CuldceptCommon.CuldDeLHA(args[1]);
                (new FileStream(args[2], FileMode.Create)).Write(data, 0, data.Length);
            }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

        }
    }
}
