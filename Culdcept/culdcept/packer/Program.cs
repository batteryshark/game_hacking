
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;


namespace packer
{
    class Program
    {
        static int Main(string[] args)
        {
            using (BinaryReader b = new BinaryReader(File.Open("fulldata.pak", FileMode.Open)))
            {
                int pos = 0;
                int fcount = 1;
                int length = (int)b.BaseStream.Length;
                
                 while (pos < length)
	            {
                    b.BaseStream.Seek(pos, SeekOrigin.Begin);
                    byte buff = b.ReadByte();
                    b.BaseStream.Seek(pos, SeekOrigin.Begin);

                    if (buff == 0x08 || buff == 0x09)
                    {
                        byte[] data;
                        if (length - pos <= 2048000)
                        {
                            byte[] indata = b.ReadBytes(2048000);
                              try
                            {
                            data = CuldceptCommon.CuldDeLHA(indata);
                            }
                              catch (System.Exception ex)
                              {
                                 // Console.WriteLine(ex.Message);
                                  return 1;
                              }
                        }
                        else
                        {
                            byte[] indata = b.ReadBytes(length - pos);
                            try
                            {
                            data = CuldceptCommon.CuldDeLHA(indata);
                                                        }
                              catch (System.Exception ex)
                              {
                             //     Console.WriteLine(ex.Message);
                                  pos++;
                                  continue;
                              }
                        }
                       
                        if (data.Length != 0)
                        {
                            for (int i = 0; i < data.Length; i++)
                            {
                                if (data[i] != 0x00)
                                {  
                                    (new FileStream("f_"+fcount.ToString("D5")+".bin", FileMode.Create)).Write(data, 0, data.Length);
                                    data = new Byte[5242880];
                                    System.Console.WriteLine("f_" + fcount.ToString("D5") + ".bin from offset 0x" + pos.ToString("X4") + " written.");
                                    fcount++;
                                }
                            }

                        }
                    }

                    pos++;
                   
                }
                 
            }
        

            return 0;
        }
    }
}
