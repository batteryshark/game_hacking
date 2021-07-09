using System;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Collections;
//using Google.API;
using theli;


namespace CuldceptDSEditor
{
    public partial class MainForm : Form
    {
		private String tmpdir = "culdtemp";
        private Arm9Data arm9data = new Arm9Data();
		private Int32 FindStart = 0;
        private Maps m = new Maps();
        private CardsNCrests cards = new CardsNCrests();
        private Script script = new Script();
        private Script help_script = new Script();
        private Script taunts = new Script();
        private CuldPicture[] pictures;
        private CuldTile tile;
        public MainForm()
        {

            InitializeComponent();
			if (File.Exists(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked"))
				this.RescanbuttonClick(null,null);

            //this is so that it work for chinese
            foreach (Control ctrl in this.Controls)
            {
                ctrl.ImeMode = ImeMode.Inherit;
            }
            
        }
        public void LogMSG(String msg) { LogMSG(msg, true); }
        public void LogMSG(String msg , bool newline)
        {
            this.LogBox.AppendText(msg);
            if (newline)
            {
                this.LogBox.AppendText(System.Environment.NewLine);
            }
        }
        public void Working(bool state)
        {
            this.openbutton.Enabled = state;
            //this.CompresscheckBox.Enabled = state;
            this.savebutton.Enabled = state;
            //this.treeView1.Enabled = state;
        }
        private void openbutton_Click(object sender, EventArgs e)
        {
            Working(false);
            
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            dlg.CheckPathExists = true;
            dlg.DefaultExt = "nds";
            dlg.RestoreDirectory = true;
            dlg.Filter = "nds files (*.nds)|*.nds|All files (*.*)|*.*";
            if (dlg.ShowDialog() != DialogResult.OK){ 
                LogMSG("file selection canceled");
                Working(true);                
            }
            else
            {
                LogMSG("Unpacking \"" + dlg.FileName + "\" into \"" + tmpdir + "\"");
                if (UnpackROM(dlg.FileName,tmpdir))
                {
                    LogMSG("Unpacking succesfully finished");
                }
                else
                {
                    LogMSG("Unpacking failed");
                }
            }
            Working(true);
        }
        public bool UnpackROM(String file, String where)
        {
            if (Directory.Exists(where))
            {
                LogMSG("deleting " + where);
                Directory.Delete(where, true);
            }

            Directory.CreateDirectory(where);
            Directory.CreateDirectory(where + System.IO.Path.DirectorySeparatorChar + "outer");
            Directory.CreateDirectory(where + System.IO.Path.DirectorySeparatorChar + "inner");


            LogMSG("Unpacking ROM with ndstool....",false);
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
            CuldDat.unpackDAT(where + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" , where + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT");
            LogMSG("OK");

			RescanbuttonClick(null,null);
			LogMSG("DONE");

            return res;
        }
        void SavebuttonClick(object sender, EventArgs e)
        {
			Working(false);
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.CheckPathExists = false;
            dlg.DefaultExt = "nds";
            dlg.RestoreDirectory = true;
            dlg.Filter = "nds files (*.nds)|*.nds|All files (*.*)|*.*";
            if (dlg.ShowDialog() != DialogResult.OK){ 
                LogMSG("file selection canceled");
            }
            else
            {
                //SaveArm9Button_Click(null, null);
                //MapsToFileButton_Click(null,null);
                //CardsToFilebutton_Click(null, null);
                //ScriptSaveButton_Click(null, null);
				LogMSG("Repacking CULDCEPT.DAT...",false);
				CuldDat.packDAT(tmpdir + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" , tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT");
				LogMSG("OK");

				LogMSG("Repacking ROM with ndstool....",false);
				PackROM(dlg.FileName,"culdtemp");
				LogMSG("OK");
			}
			Working(true);
        }
		void PackROM(String Filename , String where)
		{
			
			if (System.Environment.OSVersion.Platform == PlatformID.Unix)
                Utils.SystemCall("ndstool -c \\\"" + Filename + "\\\" -9 " + where  + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked -7 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
            else
                Utils.SystemCall("ndstool -c \"" + Filename + "\" -9 " + where  + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked -7 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "arm7.bin -y9 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y9.bin -y7 " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "y7.bin -d " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "data -y " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "overlay -t " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "banner.bin -h " + where  + System.IO.Path.DirectorySeparatorChar + "outer" + System.IO.Path.DirectorySeparatorChar + "header.bin");
		}
     
        void OnSaveTextButtonClick(object sender, EventArgs e)
        {
			if (TransBox.Text.Length == 0)
				return;
            arm9data.entries[TextList.SelectedIndex].text = TransBox.Text;
            textBox.Text = TransBox.Text;
            TransBox.Text = "";
            this.TextList.Items[TextList.SelectedIndex] = textBox.Text;
        }
        
        void TextListClick(object sender, EventArgs e)
        {
            if (TextList.SelectedIndex == -1)
                return;
			textBox.Text = arm9data.entries[TextList.SelectedIndex].text;
			MaxLengthBox.Text = arm9data.entries[TextList.SelectedIndex].maxsize.ToString();
            offsetBox.Text = arm9data.entries[TextList.SelectedIndex].offset.ToString();
			TransBox.Text = "";
        }
        
        void RescanbuttonClick(object sender, EventArgs e)
        {
        	LogMSG("Trying to read strings from unpacked ARM9.BIN...", false);
			arm9data.SimpleTextRead(0x108e7c,0x110044,tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
			LogMSG(arm9data.entries.Length.ToString() + " entries");
			TextList.BeginUpdate();
			TextList.Items.Clear();
			foreach (textEntry entry in arm9data.entries)
			{
				TextList.Items.Add(entry.text);
			}
			TextList.EndUpdate();
			this.TextList.SelectedIndex = 0;

            LogMSG("Reading maps...", false);
            m.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00164");
            MapBox.BeginUpdate();
            MapBox.Items.Clear();
            for (int x = 0; x < m.Count; x++)
            {
                MapBox.Items.Add(m.Name[x]);
            }
            MapBox.EndUpdate();
            this.MapBox.SelectedIndex = 0;
            LogMSG(m.Count + " entries");
            ////
            //cards
            CardBox.BeginUpdate();
            CardBox.Items.Clear();
            LogMSG("Reading cards...", false);
            cards.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar +/*00316*/ "00316");
            for (int x = 0; x < cards.Count; x++)
            {
                CardBox.Items.Add(cards.entries[x].title);
            }
            LogMSG(cards.Count + " entries");
            CardBox.SelectedIndex = 0;
            CardBox.EndUpdate();

            //crests
            CrestBox.BeginUpdate();
            CrestBox.Items.Clear();
            LogMSG("Reading crests...", false);
            for (int x = 0; x < cards.CrestCount; x++)
            {
                CrestBox.Items.Add(cards.crestentries[x].title);
            }
            CrestBox.SelectedIndex = 0;
            LogMSG(cards.CrestCount + " entries");
            CrestBox.EndUpdate();

            //script
            LogMSG("Reading script...", false);
            ScriptBox.BeginUpdate();
            ScriptBox.Items.Clear();
            script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00756",0x5B70);
            //script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00757");
            for (int x = 0; x < script.entries.Length; x++)
            {
                ScriptBox.Items.Add(script.entries[x].str);
            }
            ScriptBox.SelectedIndex = 0;
            ScriptBox.EndUpdate();
            LogMSG(script.entries.Length + " entries");

            //help
            LogMSG("Reading help system...", false);
            HelpBox.BeginUpdate();
            HelpBox.Items.Clear();
            //script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00756", 0x5B70);
            help_script.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00757",0x102c);
            for (int x = 0; x < help_script.entries.Length; x++)
            {
                HelpBox.Items.Add(help_script.entries[x].str);
            }
            HelpBox.SelectedIndex = 0;
            HelpBox.EndUpdate();
            LogMSG(help_script.entries.Length + " entries");


            //taunts
            LogMSG("Reading taunts...", false);
            TauntBox.BeginUpdate();
            TauntBox.Items.Clear();
            taunts.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00761", 0x11fc);
            for (int x = 0; x < taunts.entries.Length; x++)
            {
                TauntBox.Items.Add(taunts.entries[x].str);
            }
            TauntBox.SelectedIndex = 0;
            TauntBox.EndUpdate();
            LogMSG(taunts.entries.Length + " entries");


            //manual
            PictureList.BeginUpdate();
            PictureList.Items.Clear();
            pictures = Pictures.getPictures();
            for (int x = 0; x < pictures.Length; x++)
            {
                PictureList.Items.Add(pictures[x].name);
            }
            //pictureBox.Image = pictures[0].bitmap;
            PictureList.EndUpdate();
			PictureList.SelectedIndex = 0;

            //////////////////////////////////////////////////////////////////////////
            //create wikitable for script
            #region taunts
            /*
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
                "00771",//tutorial
                "00757",//help
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
                0xFac,//tutorial
                0x102c,//help
            };

            for (int i = 0; i < tauntnames.Length; i++)
            {
                StreamWriter tw = new StreamWriter(new FileStream("taunts"+i.ToString()+".wiki", FileMode.Create));
                int end;
                taunts.Read(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + tauntnames[i], tauntoffsets[i]);
                for (int x = 0; x < taunts.entries.Length; )
                {
                    end = x + 9;
                    if (end >= taunts.entries.Length)
                        end = (int)taunts.entries.Length - 1;
                    tw.WriteLine("==Entries {0}-{1}==", x, end);
                    tw.WriteLine("{|border=\"1\"");
                    tw.WriteLine("! ID !! Japanese !! Translation");

                    for (; x <= end; x++)
                    {
                        tw.WriteLine("|-");
                        tw.WriteLine("!{0}", x);
                        tw.WriteLine("|{0}", taunts.entries[x].str);
                        tw.WriteLine("|");
                    }
                    tw.WriteLine("|}");
                }
                tw.Close();
            }
            */
            #endregion
            //////////////////////////////////////////////////////////////////////////
            //create wikitable
//             StreamWriter tw = new StreamWriter(new FileStream("cards.wiki", FileMode.Create));
//             int end;
//             for (int x = 0; x < cards.Count;)
//             {
//                 end = x + 10;
//                 if (end >= cards.Count)
//                     end = (int)cards.Count - 1;
//                 tw.WriteLine("==Cards {0}-{1}==",x,end);
//                 tw.WriteLine("{|border=\"1\"");
//                 tw.WriteLine("! ID !! Name !! Ability 1 !! Ability 2 !! Ability 3 !! Description");
// 
//                 for (; x <= end; x++ )
//                 {
//                     tw.WriteLine("|-");
//                     tw.WriteLine("![[Tooltip:Card/{0}|{0}]]", cards.entries[x]._ID);
//                     tw.WriteLine("|{0}", cards.entries[x].name);
//                     tw.WriteLine("|{0}", cards.entries[x].ability);
//                     tw.WriteLine("|{0}", cards.entries[x].ability2);
//                     tw.WriteLine("|{0}", cards.entries[x].ability3);
//                     tw.WriteLine("|{0}", cards.entries[x].description);
//                 }
//                 tw.WriteLine("|}");
//             }
//             tw.Close();


                //create wikitable for crests
            
//              StreamWriter tw = new StreamWriter(new FileStream("crests.wiki", FileMode.Create));
//              int end;
//              for (int x = 0; x < cards.CrestCount;)
//              {
//                  end = x + 10;
//                  if (end >= cards.Count)
//                      end = (int)cards.Count - 1;
//                  tw.WriteLine("==Crests {0}-{1}==",x,end);
//                  tw.WriteLine("{|border=\"1\"");
//                  tw.WriteLine("! ¹ !! Title !! Name !! String 1 !! String 2 !! String 3");
// 
//                  for (; x <= end && x < cards.CrestCount; x++)
//                  {
//                      tw.WriteLine("|-");
//                      tw.WriteLine("|{0}",x);
//                      tw.WriteLine("|{0}", cards.crestentries[x].title);
//                      tw.WriteLine("|{0}", cards.crestentries[x].name);
//                      tw.WriteLine("|{0}", cards.crestentries[x].str3);
//                      tw.WriteLine("|{0}", cards.crestentries[x].str4);
//                      tw.WriteLine("|{0}", cards.crestentries[x].str5);
//                  }
//                  tw.WriteLine("|}");
//              }
//              tw.Close();

            
                //create wikitable for maps
            /*
             StreamWriter tw = new StreamWriter(new FileStream("maps.wiki", FileMode.Create));
             int end;
             for (int x = 0; x < m.Count;)
             {
                 end = x + 10;
                 if (end >= m.Count)
                     end = (int)m.Count - 1;
                 tw.WriteLine("==Maps {0}-{1}==",x,end);
                 tw.WriteLine("{|border=\"1\"");
                 tw.WriteLine("! ¹ !! Title !! Description");

                 for (; x <= end && x < cards.CrestCount; x++)
                 {
                     tw.WriteLine("|-");
                     tw.WriteLine("|{0}",x);
                     tw.WriteLine("|{0}", m.Name[x]);
                     tw.WriteLine("|{0}", m.Desc[x]);
                 }
                 tw.WriteLine("|}");
             }
             tw.Close();
            */
  //////////////////////////////////////////////////////////////////////////

//             //create wikitable
//             StreamWriter tw = new StreamWriter(new FileStream("menus.wiki", FileMode.Create));
//             int end;
//             for (int x = 0; x < arm9data.entries.Length; )
//             {
//                 end = x + 9;
//                 if (end >= arm9data.entries.Length)
//                     end = (int)arm9data.entries.Length - 1;
//                 tw.WriteLine("==Entries {0}-{1}==", x, end);
//                 tw.WriteLine("{|border=\"1\"");
//                 tw.WriteLine("! offset !! Maximum length in bytes !! Japanese !! Translation");
// 
//                 for (; x <= end; x++)
//                 {
//                     tw.WriteLine("|-");
//                     tw.WriteLine("|{0}", arm9data.entries[x].offset);
//                     tw.WriteLine("|{0}", arm9data.entries[x].maxsize);
//                     tw.WriteLine("|{0}", arm9data.entries[x].text);
//                     tw.WriteLine("|{0}", arm9data.entries[x].text);
//                 }
//                 tw.WriteLine("|}");
//             }
//             tw.Close();
            //////////////////////////////////////////////////////////////////////////


            //////////////////////////////////////////////////////////////////////////
            
            /*
            byte[] buff;
            BinaryReader br;
            String file;
            Int32 Offset = 4;
            for (int x = 0; x <= 1045; x++)
            {
                file = String.Format("{0:00000}", x);
                String path = tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar
                   + file;
                br = new BinaryReader(new FileStream(path, FileMode.Open));
                //this is not right/complete but i'm lazy
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

                buff = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));
                try
                {
                    if (algo == 0xC)
                        buff = Lha.LHADecode(Lha.LzhCompressMethodType.LH7, buff, uncompsize);
                    else if (algo == 0x8)
                        buff = Lha.LHADecode(Lha.LzhCompressMethodType.LH5, buff, uncompsize);
                    //else 
                    //if (algo == 0x0)
                    //    buff = Lha.LHADecode(Lha.LzhCompressMethodType.LH4, buff, uncompsize);
                    else
                    {
                        LogMSG(file);
                        continue;
                    }
                       
                    FileStream fs = new FileStream("b"+System.IO.Path.DirectorySeparatorChar + file + "", FileMode.Create);
                    fs.Write(buff, 0, buff.Length);
                    fs.Close();
                }
                catch {
                    LogMSG(file);
                };

                br.Close();
            }
            */
            //////////////////////////////////////////////////////////////////////////

        }

		void OnTransTextChanged(object sender, EventArgs e)
        {
			Int32 count = 0;
            
            count = SJSSpecEncoder.Encode(TransBox.Lines).Length;

			if( count > arm9data.entries[TextList.SelectedIndex].maxsize)
				this.savetextbutton.Enabled = false;
			else
				this.savetextbutton.Enabled = true;
			this.curlength.Text = count.ToString();
        }
        void SearchbuttonClick(object sender, EventArgs e)
        {
			int i;
            if (tabControl.SelectedIndex == 0)
            {
                for (i = this.FindStart; i < arm9data.entries.Length; i++)
                {
                    if (arm9data.entries[i].text.Contains(this.FindBox.Text))
                    {
                        this.TextList.SelectedIndex = i;
                        this.FindStart = i + 1;
                        TextListClick(null, null);
                        return;
                    }
                }
            }
            else if (tabControl.SelectedIndex == 2)
            {
                for (i = this.FindStart; i < cards.Count; i++)
                {
                    if (cards.entries[i].description.Contains(this.FindBox.Text) | cards.entries[i].ability.Contains(this.FindBox.Text) | cards.entries[i].name.Contains(this.FindBox.Text) | cards.entries[i].title.Contains(this.FindBox.Text))
                    {
                        this.CardBox.SelectedIndex = i;
                        this.FindStart = i + 1;
                        return;
                    }
                }
            }
            else if (tabControl.SelectedIndex == 1)
            {
                for (i = this.FindStart; i < m.Count; i++)
                {
                    if (m.Name[i].Contains(this.FindBox.Text) | m.Desc[i].Contains(this.FindBox.Text))
                    {
                        this.MapBox.SelectedIndex = i;
                        this.FindStart = i + 1;
                        return;
                    }
                }
            }
            else if (tabControl.SelectedIndex == 3)
            {
                for (i = this.FindStart; i < cards.CrestCount; i++)
                {
                    if (cards.crestentries[i].str3.Contains(this.FindBox.Text) | cards.crestentries[i].str4.Contains(this.FindBox.Text) | cards.crestentries[i].name.Contains(this.FindBox.Text) | cards.crestentries[i].title.Contains(this.FindBox.Text) | cards.crestentries[i].str5.Contains(this.FindBox.Text))
                    {
                        this.CrestBox.SelectedIndex = i;
                        this.FindStart = i + 1;
                        return;
                    }
                }
            }

			LogMSG("not found");
        }
		void OnTransButtonClick(object sender, EventArgs e)
        {   try
			{
			//	TransBox.Text = Google.API.Translate.Translator.Translate(textBox.Text, Language.Japanese, Language.English,Google.API.Translate.TranslateFormat.text);
			}
			catch
			{
				LogMSG("error getting translation, try again");
			}
        }
        
        void DownbuttonClick(object sender, EventArgs e)
        {
        	TransBox.Text = textBox.Text;
        }
        
        void onFindChanged(object sender, EventArgs e)
        {
        	this.FindStart = 0;
        }

        private void OnMapIndexChange(object sender, EventArgs e)
        {
            if (MapBox.SelectedIndex == -1)
                return;
            MapDescBox.Text = m.Desc[MapBox.SelectedIndex];
            MapNameBox.Text = m.Name[MapBox.SelectedIndex];
        }

        private void OnSaveMap(object sender, EventArgs e)
        {
            m.Desc[MapBox.SelectedIndex] = MapDescBox.Text;
            m.Name[MapBox.SelectedIndex] = MapNameBox.Text;
            MapBox.Items[MapBox.SelectedIndex] = MapNameBox.Text;
        }

        private void label10_Click(object sender, EventArgs e)
        {

        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void OnCardBoxSelectionChanged(object sender, EventArgs e)
        {
            if (CardBox.SelectedIndex == -1)
                return;
            CardTitle.Text = cards.entries[CardBox.SelectedIndex].title;
            CardName.Text = cards.entries[CardBox.SelectedIndex].name;
            CardEffect.Text = cards.entries[CardBox.SelectedIndex].ability;
            CardDescr.Text = cards.entries[CardBox.SelectedIndex].description;
            CardID.Text = cards.entries[CardBox.SelectedIndex]._ID.ToString();
            CardHP.Text = cards.entries[CardBox.SelectedIndex]._HP.ToString();
            CardST.Text = cards.entries[CardBox.SelectedIndex]._ST.ToString();
            CardG.Text = cards.entries[CardBox.SelectedIndex]._G.ToString();
            CardAbility2.Text = cards.entries[CardBox.SelectedIndex].ability2;
            CardAbility3.Text = cards.entries[CardBox.SelectedIndex].ability3;
        }

        private void SaveCardsbutton_Click(object sender, EventArgs e)
        {
            cards.entries[CardBox.SelectedIndex].title = CardTitle.Text;
            cards.entries[CardBox.SelectedIndex].name = CardName.Text;
            cards.entries[CardBox.SelectedIndex].ability = CardEffect.Text;
            cards.entries[CardBox.SelectedIndex].ability2 = CardAbility2.Text;
            cards.entries[CardBox.SelectedIndex].ability3 = CardAbility3.Text;
            cards.entries[CardBox.SelectedIndex].description = CardDescr.Text;

            CardBox.Items[CardBox.SelectedIndex] = CardTitle.Text;
        }

        private void MapsToFileButton_Click(object sender, EventArgs e)
        {
            LogMSG("Saving map descriptions...", false);
            m.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00164");
            LogMSG("OK");
        }

        private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.FindStart = 0;
        }

        private void CardsToFilebutton_Click(object sender, EventArgs e)
        {
            LogMSG("Saving cards and crests...", false);
            cards.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00316",
                tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
            LogMSG("OK");

        }

        private void SaveCrestbutton_Click(object sender, EventArgs e)
        {
            cards.crestentries[CrestBox.SelectedIndex].name = CrestName.Text;
            cards.crestentries[CrestBox.SelectedIndex].title = CrestTitle.Text;
            cards.crestentries[CrestBox.SelectedIndex].str3 = Crest3.Text;
            cards.crestentries[CrestBox.SelectedIndex].str4 = Crest4.Text;
            cards.crestentries[CrestBox.SelectedIndex].str5 = Crest5.Text;
            CrestBox.Items[CrestBox.SelectedIndex] = CrestTitle.Text;
        }

        private void CrestBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (CrestBox.SelectedIndex == -1)
                return;
            CrestName.Text = cards.crestentries[CrestBox.SelectedIndex].name;
            CrestTitle.Text = cards.crestentries[CrestBox.SelectedIndex].title;
            Crest3.Text = cards.crestentries[CrestBox.SelectedIndex].str3;
            Crest4.Text = cards.crestentries[CrestBox.SelectedIndex].str4;
            Crest5.Text = cards.crestentries[CrestBox.SelectedIndex].str5;
        }

        private void CardWikibutton_Click(object sender, EventArgs e)
        {
            LogMSG("Updating cards from wiki...", false);
            CardBox.BeginUpdate();
            CardBox.Items.Clear();
            cards.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Card_List&action=raw");
            for (int x = 0; x < cards.Count; x++)
            {
                CardBox.Items.Add(cards.entries[x].title);
            }
            CardBox.SelectedIndex = 0;
            CardBox.EndUpdate();
            LogMSG("OK");

        }

        private void SaveArm9Button_Click(object sender, EventArgs e)
        {
            arm9data.save(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "arm9.unpacked");
        }

        private void TextFromWikiButton_Click(object sender, EventArgs e)
        {
            LogMSG("Updating from wiki...", false);
            arm9data.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Menus&action=raw");
            TextList.BeginUpdate();
            TextList.Items.Clear();
            for (int x = 0; x < arm9data.entries.Length; x++ )
            {
                TextList.Items.Add(arm9data.entries[x].text);
            }
            TextList.SelectedIndex = 0;
            TextList.EndUpdate();
            LogMSG("OK");
        }

        private void ScriptBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ScriptText.Text = script.entries[ScriptBox.SelectedIndex].str;
        }

        private void ScriptApplybutton_Click(object sender, EventArgs e)
        {
            script.entries[ScriptBox.SelectedIndex].str = ScriptText.Text;
        }

        private void ScriptSaveButton_Click(object sender, EventArgs e)
        {
            LogMSG("saving script...", false);
            script.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00756");
            LogMSG("OK");
        }

        private void ScriptWikiButton_Click(object sender, EventArgs e)
        {
            LogMSG("Updating Script from wiki...", false);
            script.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Story_Script&action=raw");
            ScriptBox.BeginUpdate();
            ScriptBox.Items.Clear();
            for (int x = 0; x < script.entries.Length; x++)
            {
                ScriptBox.Items.Add(script.entries[x].str);
            }
            ScriptBox.SelectedIndex = 0;
            ScriptBox.EndUpdate();
            LogMSG("OK");
        }

        private void PictureList_SelectedIndexChanged(object sender, EventArgs e)
        {
            pictureBox.Image = pictures[PictureList.SelectedIndex].bitmap;
        }

        private void ImgSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.CheckPathExists = false;
            dlg.RestoreDirectory = true;
            
            //if ( (pictures[PictureList.SelectedIndex].bitmap.PixelFormat & System.Drawing.Imaging.PixelFormat.Indexed) == 0)
            //{
            dlg.FileName = pictures[PictureList.SelectedIndex].fname + ".png";
                dlg.DefaultExt = "png";
                dlg.Filter = "png files (*.png)|*.png";
            //}
            //else
            //{
            //    dlg.DefaultExt = "bmp";
            //    dlg.Filter = "bmp files (*.bmp)|*.bmp";
            //}

            if (dlg.ShowDialog() != DialogResult.OK) return;
            //if ((pictures[PictureList.SelectedIndex].bitmap.PixelFormat & System.Drawing.Imaging.PixelFormat.Indexed) == 0)
                this.pictureBox.Image.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Png);
            //else
            //    this.pictureBox.Image.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Bmp);
        }

        private void ImgOpen_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            dlg.CheckPathExists = true;
            dlg.RestoreDirectory = true;
            /*
            if ((pictures[PictureList.SelectedIndex].bitmap.PixelFormat & System.Drawing.Imaging.PixelFormat.Indexed) == 0)
            {
                dlg.DefaultExt = "png";
                dlg.Filter = "png files (*.png)|*.png";
            }
            else{
                dlg.DefaultExt = "bmp";
                dlg.Filter = "bmp files (*.bmp)|*.bmp";
            }
            */
            dlg.Filter = "Image Files(*.BMP;*.JPG;*.GIF;*.PNG)|*.BMP;*.JPG;*.GIF;*.PNG";

            if (dlg.ShowDialog() != DialogResult.OK) return;

           

            if (!pictures[PictureList.SelectedIndex].Import(dlg.FileName))
            {
                MessageBox.Show("Error: importing picture\n picture should be of matching format", "Error");
            }
            else
            {
                pictures[PictureList.SelectedIndex].Save();
                pictures[PictureList.SelectedIndex].Read(pictures[PictureList.SelectedIndex].fname,pictures[PictureList.SelectedIndex].name);
                pictureBox.Image = pictures[PictureList.SelectedIndex].bitmap;
                pictureBox.Refresh();
            }
        }
        private void PastePicture_Click(object sender, EventArgs e)
        {
            pictures[PictureList.SelectedIndex].Paste(new Bitmap(Clipboard.GetImage()),"");
            pictures[PictureList.SelectedIndex].Save();
            pictures[PictureList.SelectedIndex].Read(pictures[PictureList.SelectedIndex].fname, pictures[PictureList.SelectedIndex].name);
            pictureBox.Image = pictures[PictureList.SelectedIndex].bitmap;
            pictureBox.Refresh();
        }
        private void TileGo_Click(object sender, EventArgs e)
        {
            string filename = ((UInt32)tilenum.Value).ToString("D5");
            tile = new CuldTile();
            tile.Read("culdtemp" + 
                System.IO.Path.DirectorySeparatorChar + 
                "inner" + System.IO.Path.DirectorySeparatorChar + 
                "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar +
                filename, filename, (Int32)(Tilepalettesize.Value), (Int32)tilewidth.Value);
            TileBox.Image = tile.bitmap;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            dlg.CheckPathExists = true;
            dlg.RestoreDirectory = true;

            //dlg.DefaultExt = "bmp";
            //dlg.Filter = "bmp files (*.bmp)|*.bmp";
            dlg.DefaultExt = "png";
            dlg.Filter = "png files (*.png)|*.png";
            if (dlg.ShowDialog() != DialogResult.OK) return;

            if (!tile.Import(dlg.FileName))
            {
                MessageBox.Show("Error: importing picture\n picture should be of matching format", "Error");
            }
            else
            {
                tile.Save();
                tile.Read(tile.fname, tile.name, (Int32)(Tilepalettesize.Value), (Int32)tilewidth.Value);
                TileBox.Image = tile.bitmap;
                TileBox.Refresh();
            }

        }

        private void ExportTile_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.CheckPathExists = false;
            dlg.RestoreDirectory = true;
            //dlg.DefaultExt = "bmp";
            //dlg.Filter = "bmp files (*.bmp)|*.bmp";
            dlg.DefaultExt = "png";
            dlg.Filter = "png files (*.png)|*.png";
            
            if (dlg.ShowDialog() != DialogResult.OK) return;
            //TileBox.Image.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Bmp);
            TileBox.Image.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Png);

        }

        private void PictureCopy_Click(object sender, EventArgs e)
        {
            Clipboard.SetImage(pictures[PictureList.SelectedIndex].bitmap);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            help_script.entries[HelpBox.SelectedIndex].str = Helptext.Text;

        }

        private void HelpBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Helptext.Text = help_script.entries[HelpBox.SelectedIndex].str;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            LogMSG("saving help script...", false);
            help_script.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00757");
            LogMSG("OK");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            LogMSG("Updating Help Script from wiki...", false);
            help_script.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Help_Script&action=raw");
            HelpBox.BeginUpdate();
            HelpBox.Items.Clear();
            for (int x = 0; x < help_script.entries.Length; x++)
            {
                HelpBox.Items.Add(help_script.entries[x].str);
            }
            HelpBox.SelectedIndex = 0;
            HelpBox.EndUpdate();
            LogMSG("OK");
        }

        private void TauntBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            TauntText.Text = taunts.entries[TauntBox.SelectedIndex].str;
        }

        private void TauntApply_Click(object sender, EventArgs e)
        {
            taunts.entries[TauntBox.SelectedIndex].str = TauntText.Text;
        }

        private void TauntSave_Click(object sender, EventArgs e)
        {
            LogMSG("saving taunts script...", false);
            taunts.Write(tmpdir + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar + "00759");
            LogMSG("OK");

        }

        private void TauntUpdate_Click(object sender, EventArgs e)
        {
            LogMSG("Updating Taunts Script from wiki...", false);
            taunts.UpdateFromWikiUrl(@"http://culdcept.referata.com/w/index.php?title=Taunts_Script&action=raw");
            TauntBox.BeginUpdate();
            TauntBox.Items.Clear();
            for (int x = 0; x < taunts.entries.Length; x++)
            {
                TauntBox.Items.Add(taunts.entries[x].str);
            }
            TauntBox.SelectedIndex = 0;
            TauntBox.EndUpdate();
            LogMSG("OK");
        }
    }

}
