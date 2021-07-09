using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;

namespace _00001.packer
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("00001  files repacker");
                Console.WriteLine("\tunpack: 00001.packer.exe file directory");
                Console.WriteLine("\tpack: 00001.packer.exe directory file");
            }
            else
            {
                if (File.Exists(args[0]) || Directory.Exists(args[0]))
                {
                    FileAttributes attr = File.GetAttributes(@args[0]);
                    if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                    {
                        Console.WriteLine(args[0] + " is a directory, packing");
                        //CuldceptCommon.Unpack00001(args)

                    }
                    else
                    {
                        Console.WriteLine(args[0] + " is a file,unpacking");
                        CuldceptCommon.Unpack00001(args[0],args[1]);
                    }
                }
                else
                {
                    Console.WriteLine("path doesn't exist");
                }
            }

        }

    }
}
