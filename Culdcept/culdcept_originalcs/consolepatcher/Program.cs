using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using theli;
using CuldceptDSEditor;

namespace consolepatcher
{
    class Program
    {
        private static String tmpdir = "culdtemp";
        private static Arm9Data arm9data = new Arm9Data();
        private static Maps m = new Maps();
        private static CardsNCrests cards = new CardsNCrests();
        private static Script script = new Script();
        private static Script help_script = new Script();
        private static Script tutorial = new Script();


        public static void LogMSG(String msg) { LogMSG(msg, true); }
        public static void LogMSG(String msg, bool newline)
        {
            if (newline)
                Console.WriteLine(msg);
            else
                Console.Write(msg);
        }

        static void Main(string[] args)
        {
            /////
            //for taunts an
            string[] tauntnames = new string[]{
                "00759",
                "00760",
                "00761",
                "00762",
                "00763",
                "00764",
                "00765",
                "00766",
                "00767",
                "00768",
                "00769",
                "00770",
            };
            int[] tauntoffsets = new int[]{
                0x12b8,
                0x12e0,
                0x11fc,
                0x1180,
                0x111c,
                0x109c,
                0x117c,
                0x117c,
                0x1180,
                0x1180,
                0xfdc,
                0x10b0,
            };

            Script[] taunts = new Script[tauntnames.Length];



            String file = args[0];
            //////////////////////////////////////////////////////////////////////////
            //unpack
            //////////////////////////////////////////////////////////////////////////
            String where = tmpdir;
            if (Directory.Exists(where))
            {
                LogMSG("deleting " + where);
                Directory.Delete(where, true);
            }

            Directory.CreateDirectory(where);
            Directory.CreateDirectory(where + System.IO.Path.DirectorySeparatorChar + "outer");
            Directory.CreateDirectory(where + System.IO.Path.DirectorySeparatorChar + "inner");


            LogMSG("Unpacking ROM with ndstool....", false);
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
                Utils.SystemCall("ndstool -v -x \\\"" + file + "\\\" -9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm9.bin -7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
            else
                Utils.SystemCall("ndstool -v -x \"" + file + "\" -9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm9.bin -7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
            LogMSG("DONE", true);

            LogMSG("Unpacking ARM9.BIN...", false);
            bool res = Arm9Data.UnpackARM9(where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm9.bin", where + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
            if (res)
            {
                LogMSG("OK");
            }
            else
            {
                LogMSG("FAILED");
            }
            LogMSG("Unpacking CULDCEPT.DAT...", false);
            CuldDat.unpackDAT(where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT", where + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT");
            LogMSG("OK");

            LogMSG("Trying to read strings from unpacked ARM9.BIN...", false);
            arm9data.SimpleTextRead(0x108e7c, 0x110044, tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
            LogMSG(arm9data.entries.Length.ToString() + " entries");

            LogMSG("Reading maps...", false);
            m.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00164");
            LogMSG(m.Count + " entries");

            LogMSG("Reading cards...", false);
            cards.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00316");
            LogMSG(cards.Count + " entries");

            LogMSG("Reading crests...", false);
            LogMSG(cards.CrestCount + " entries");

            LogMSG("Reading script...", false);
            script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00756",0x5b70);
            LogMSG(script.entries.Length + " entries");

            LogMSG("Reading help_script...", false);
            help_script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00757", 0x102C);
            LogMSG(help_script.entries.Length + " entries");

            LogMSG("Reading tutorial...", false);
            tutorial.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00771", 0xfaC);
            LogMSG(tutorial.entries.Length + " entries");

            for (int i = 0; i < taunts.Length; i ++  )
            {
                LogMSG("Reading taunts "+ i.ToString()+" script...", false);
                taunts[i] = new Script();
                taunts[i].Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + tauntnames[i], tauntoffsets[i]);
                LogMSG(taunts[i].entries.Length + " entries");
            }


            LogMSG("Updating menus from wiki...", false);
            arm9data.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Menus&action=raw");
            LogMSG("OK");

            LogMSG("Updating cards from wiki...", false);
            cards.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Card_List&action=raw");
            LogMSG("OK");

            LogMSG("Updating crests from wiki...", false);
            cards.UpdateCrestsFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Crests_List&action=raw");
            LogMSG("OK");

            LogMSG("Updating maps from wiki...", false);
            m.UpdateFromUrl(@"http://culdcept.referata.com/w/index.php?title=Maps&action=raw");
            LogMSG("OK");

            LogMSG("Updating Script from wiki...", false);
            script.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Story_Script&action=raw");
            LogMSG("OK");

            LogMSG("Updating help Script from wiki...", false);
            help_script.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Help_Script&action=raw");
            LogMSG("OK");

            LogMSG("Updating tutorial from wiki...", false);
            tutorial.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Tutorial&action=raw");
            LogMSG("OK");

            for (int i = 0; i < taunts.Length; i++)
            {
                LogMSG("Updating taunts " + i.ToString() + " Script from wiki...", false);
                taunts[i].UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Taunts" + i.ToString() + "_Script&action=raw");
                LogMSG("OK");
            }

            //////////////////////////////////////////////////////////////////////////
            //saving
            //////////////////////////////////////////////////////////////////////////
            arm9data.save(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
            LogMSG("Saving cards and crests...", false);
            cards.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00316",
                tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
            LogMSG("OK");

            LogMSG("Saving map descriptions...", false);
            m.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00164");
            LogMSG("OK");

            LogMSG("saving script...", false);
            script.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00756");
            LogMSG("OK");

            LogMSG("saving help script...", false);
            help_script.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00757");
            LogMSG("OK");

            LogMSG("saving tutorial script...", false);
            tutorial.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00771");
            LogMSG("OK");

            for (int i = 0; i < taunts.Length; i++)
            {
                LogMSG("saving taunts " + i.ToString() + " script...", false);
                taunts[i].Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + tauntnames[i]);
                LogMSG("OK");
            }



            LogMSG("Repacking CULDCEPT.DAT...", false);
            CuldDat.packDAT(tmpdir + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT", tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT");
            LogMSG("OK");

            LogMSG("Repacking ROM with ndstool....", false);
            String Filename = args[1];
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
                Utils.SystemCall("ndstool -c \\\"" + Filename + "\\\" -9 " + where + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked -7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
            else
                Utils.SystemCall("ndstool -c \"" + Filename + "\" -9 " + where + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked -7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
            LogMSG("OK");

        }
    }
}
