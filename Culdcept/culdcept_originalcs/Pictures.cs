using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Drawing;
using theli;
using System.Runtime.InteropServices;
using FreeImageAPI;
using System.Linq;
using System.Collections;

namespace CuldceptDSEditor
{
   class CuldTile : CuldPicture{
        public override void WriteBitmap(BinaryWriter bw)
        {
            
            for (int i = 0; i < bitdata.Length; i++)
            {
                bitdata[i] = (byte)(((bitdata[i] & 0x0F) << 4) | ((bitdata[i] & 0xF0) >> 4));
            }
            
            byte[] data;
            data = new byte[bitmap.Height * bitmap.Width / 2];
            int s = 0;
            Rectangle rectangle = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            System.Drawing.Imaging.BitmapData bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);
            Marshal.Copy(bitdata,0,bmpData.Scan0 ,bitdata.Length);
            bitmap.UnlockBits(bmpData);
            for (int y = 0; y < (bitmap.Height / 8); y++)
            {
                for (int x = 0; x < (bitmap.Width / 8); x++)
                {
                    /*Rectangle*/ rectangle = new Rectangle(x * 8, y * 8, 8, 8);
                    /*System.Drawing.Imaging.BitmapData*/ bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);
                    for (int i = 0; i < 8; i++)
                    {
                        //Marshal.Copy(data, s + i * 4, theli.Utils.PtrOffset(bmpData.Scan0, (i * bmpData.Stride)), 4);
                        Marshal.Copy(theli.Utils.PtrOffset(bmpData.Scan0, (i * bmpData.Stride)), data, s + i * 4, 4);
                    }
                    bitmap.UnlockBits(bmpData);
                    s += (8 * 8 / 2);
                }
            }
            //Marshal.Copy(bmpData.Scan0, data, 0, data.Length);
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = (byte)(((data[i] & 0x0F) << 4) | ((data[i] & 0xF0) >> 4));
            }
            bw.Write(data);
        }
        public bool Read(String src, String name,int PaletteSize, int width)
        {
            int tilesize = 8;
            depth = 4;
            PaletteSize *= 16;
            fname = src;

            this.name = name;
            IsRaw = false;
            byte[] data = new byte[0];
            try
            {
                data = CuldceptCommon.CuldDeLHA(fname);
            }
            catch {
                //MessageBox.Show("Error unpacking");
                return false;
            }
            header = new byte[0];

            width *= tilesize;
            int height = 2*(data.Length - (PaletteSize * 2)) / width;
            if (2*(data.Length - PaletteSize * 2) != (width * height))
                return false;

            origpalette = new byte[PaletteSize*2];
            System.Array.Copy(data, 0, origpalette, 0, origpalette.Length);
            bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
            System.Drawing.Imaging.ColorPalette pal = bitmap.Palette;

            for (int i = 0; i < PaletteSize; i++)
            {
                ushort num = BitConverter.ToUInt16(data, i*2);
                int red = (num & 0x1f) << 3;
                int green = (num & (0x1f << 5)) >> 2;
                int blue = (num & (0x1f << 10)) >> 7;
                if (i < pal.Entries.Length)
                    pal.Entries[i] = Color.FromArgb(red, green, blue);
            }


            for (int i = PaletteSize * 2; i < data.Length; i++)
            {
                data[i] = (byte)(((data[i] & 0x0F) << 4) | ((data[i] & 0xF0) >> 4));
            }

            
            bitmap.Palette = pal;
            int s = PaletteSize*2;
            
            for (int y = 0; y < (height / tilesize); y++)
            {
                for (int x = 0; x < (width / tilesize); x++)
                {
                    Rectangle rectangle = new Rectangle(x * tilesize, y * tilesize, tilesize, tilesize);
                    System.Drawing.Imaging.BitmapData bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);
                    for (int i = 0; i < tilesize; i++)
                    {
                        Marshal.Copy(data, s + i * 4, theli.Utils.PtrOffset(bmpData.Scan0, (i * bmpData.Stride)), 4);
                    }
                    bitmap.UnlockBits(bmpData);
                    s += (tilesize * tilesize / 2);
                }
            }
            

            return true;
        }

    }
    class CuldPicture
    {
        public String fname;
        internal byte[] header;
        public Bitmap bitmap;
        public String name;
        public bool IsRaw;
        public int PaletteSize;
        internal byte[] origpalette;
        internal byte[] bitdata;
        internal bool tiled;
        internal byte[] map;
        internal int depth;
        public bool Read(String src,String name)
        {
            fname = src;

            this.name = name;
            BinaryReader br;
            byte[] data = CuldceptCommon.CuldDeLHA(fname); ;
            tiled = false;
			if (!IsRaw)
            {
                
                int headersize;
                if (data[0] == 8)
                    headersize = data[BitConverter.ToUInt16(data, 4)];
                else
                    headersize = data[0];

                if (headersize == 0)
                    return false;

                if (data[0] == 8)
                {
                    header = new byte[BitConverter.ToUInt16(data, 4) + headersize];
                }
                else
                    header = new byte[headersize];
                
                System.Array.Copy(data, header, header.Length);

                int width, height;

                if (headersize == 0xC)
                {
                    depth = 4;
                    PaletteSize = (BitConverter.ToInt32(header, 4) - headersize) / 2;
                    width = 8 * data[BitConverter.ToInt32(header, 8)];
                    height = 8 * data[BitConverter.ToInt32(header, 8) + 1];
                }
                else
                {
                    depth = header[header.Length - (headersize - 0x8)];
                    width = BitConverter.ToUInt16(header, header.Length - (headersize - 0x4));
                    height = BitConverter.ToUInt16(header, header.Length - (headersize - 0x6));
                    PaletteSize = BitConverter.ToUInt16(header, header.Length - (headersize - 0xA));
                }

                if (depth == 0x8)
                    bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);
                else if ((headersize != 0xC) && (depth == 0x4) && ((headersize != 0x10) || (height != 0xC0)))
                    bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
                else /*if ( (depth == 0x4) &&  (headersize == 0x10) && (height == 0xC0) )*/
                    bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

                Rectangle rectangle = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
                System.Drawing.Imaging.BitmapData bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);

                if (depth == 4 && headersize != 0xC)
                {
                    for (int i = header.Length + PaletteSize * 2; i < header.Length + PaletteSize * 2 + ((bitmap.Height * bitmap.Width) / 2); i++)
                    {
                       data[i] = (byte)(((data[i] & 0x0F) << 4) | ((data[i] & 0xF0) >> 4));
                    }
                }

                if (depth == 0x8)
                {
                    bitdata = new byte[bitmap.Height * bitmap.Width];
                    Array.Copy(data, header.Length + PaletteSize * 2, bitdata, 0, bitdata.Length);
                    Marshal.Copy(data, header.Length + PaletteSize * 2, bmpData.Scan0, bitmap.Height * bitmap.Width);
                    bitmap.UnlockBits(bmpData);
                }
                else if (headersize == 0xC)
                {
                    bitmap.UnlockBits(bmpData);

                    br = new BinaryReader(new MemoryStream(data));
                    br.BaseStream.Position = headersize;
                    Color[] colors = new Color[PaletteSize];
                    for (int p = 0; p < PaletteSize; p++)
                    {
                        ushort num = br.ReadUInt16();
                        int red = (num & 0x1f) << 3;
                        int green = (num & (0x1f << 5)) >> 2;
                        int blue = (num & (0x1f << 10)) >> 7;
                        colors[p] = Color.FromArgb(red, green, blue);
                    }
                    bitdata = br.ReadBytes(BitConverter.ToInt32(header, 8) - BitConverter.ToInt32(header, 4));

                    for (int i = 0; i < bitdata.Length; i++)
                    {
                        bitdata[i] = (byte)(((bitdata[i] & 0x0F) << 4) | ((bitdata[i] & 0xF0) >> 4));
                    }
                    map = br.ReadBytes((int)(br.BaseStream.Length - br.BaseStream.Position));
                    Graphics g = Graphics.FromImage(bitmap);
                    for (int y = 0; y < (height / 8); y++)
                    {
                        for (int x = 0; x < (width / 8); x++)
                        {
                            Int32 m = BitConverter.ToUInt16(map,2+ (x + y * (width / 8)) * 2);
                            UInt16 tilenum = (UInt16)(m & 0x3FF);
                            UInt16 palnum = (UInt16)(m >> 12);
                            Bitmap newbmp = new Bitmap(8, 8, System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
                            rectangle = new Rectangle(0, 0, 8, 8);
                            System.Drawing.Imaging.BitmapData newbmpData = newbmp.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, newbmp.PixelFormat);

                            Marshal.Copy(bitdata, tilenum * 0x20, newbmpData.Scan0, 8 * 8 / 2);
                            newbmp.UnlockBits(newbmpData);

                            if ((m & 0x400) != 0)
                            {
                                newbmp.RotateFlip(System.Drawing.RotateFlipType.RotateNoneFlipX);
                            }
                            if ((m & 0x800) != 0)
                            {
                                newbmp.RotateFlip(System.Drawing.RotateFlipType.RotateNoneFlipY);
                            }

                            palnum = 0;
                            System.Drawing.Imaging.ColorPalette pal = newbmp.Palette;
                            for (int i = 0; i < 16; i++ )
                            {
                                pal.Entries[i] = colors[palnum * 16 + i];
                            }
                            br = new BinaryReader(new MemoryStream(data));
                            newbmp.Palette = pal;


                            g.DrawImage(newbmp, x * 8, y * 8);

                        }
                    }
                    g.Dispose();

                }
                else if ((depth == 0x4) && ((height != 0xC0) || (headersize != 0x10)))
                {
                    Marshal.Copy(data, header.Length + PaletteSize * 2, bmpData.Scan0, (bitmap.Height * bitmap.Width) / 2);
                    bitmap.UnlockBits(bmpData);
                }
                else if ((depth == 0x4) && (headersize == 0x10) && (height == 0xC0))
                {
                    tiled = true;
                    origpalette = new byte[PaletteSize * 2];
                    Array.Copy(data, header.Length, origpalette, 0,origpalette.Length);
                    bitmap.UnlockBits(bmpData);
                    map = new byte[0x300];
                    Array.Copy(data, data.Length - 0x300, map, 0, map.Length);
                    int s = header.Length + PaletteSize * 2;
                    Graphics g = Graphics.FromImage(bitmap);
                    for (int y = 0; y < (height / 8); y++)
                    {
                        for (int x = 0; x < (width / 8); x++)
                        {
                            Bitmap newbmp = new Bitmap(8, 8, System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
                            rectangle = new Rectangle(0, 0, 8, 8);
                            System.Drawing.Imaging.BitmapData newbmpData = newbmp.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, newbmp.PixelFormat);
                            Marshal.Copy(data, s, newbmpData.Scan0, 8*8/2);
                            newbmp.UnlockBits(newbmpData);

                            System.Drawing.Imaging.ColorPalette pal = newbmp.Palette;
                            br = new BinaryReader(new MemoryStream(data));
                            br.BaseStream.Position = header.Length +(0x20 * map[x + y * (width / 8)]);
                            for (int p = 0; p < 0x10; p++)
                            {
                                ushort num = br.ReadUInt16();
                                int red = (num & 0x1f) << 3;
                                int green = (num & (0x1f << 5)) >> 2;
                                int blue = (num & (0x1f << 10)) >> 7;
                                pal.Entries[p] = Color.FromArgb(red, green, blue);
                            }
                            newbmp.Palette = pal;

                            g.DrawImage(newbmp, x*8, y*8);


                            //rectangle = new Rectangle(x * 8, y * 8, 8, 8);
                            //bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);
                            //for (int i = 0; i < 8; i++)
                            //{
                            //    Marshal.Copy(data, s + i * 4, theli.Utils.PtrOffset(bmpData.Scan0, (i * bmpData.Stride)), 4);
                            //}
                            
                            s += 8 * 8 / 2;
                        }
                    }
                    g.Dispose();
                }


                if ((depth != 0x4) || (headersize != 0x10) || (height != 0xC0))
                {
                    System.Drawing.Imaging.ColorPalette pal = bitmap.Palette;
                    br = new BinaryReader(new MemoryStream(data));
                    br.BaseStream.Position = header.Length;
                    origpalette = new byte[PaletteSize * 2];
                    System.Array.Copy(data, header.Length, origpalette, 0, origpalette.Length);
                    for (int i = 0; i < PaletteSize; i++)
                    {
                        ushort num = br.ReadUInt16();
                        int red = (num & 0x1f) << 3;
                        int green = (num & (0x1f << 5)) >> 2;
                        int blue = (num & (0x1f << 10)) >> 7;
                        if (i < pal.Entries.Length)
                            pal.Entries[i] = Color.FromArgb(red, green, blue);
                            //pal.Entries[i] = Color.FromArgb(blue,green,red);
                    }
                    bitmap.Palette = pal;
                }
                //if transparent - impossible save as 8bpp indexed
                //bitmap.MakeTransparent();
            }
            else
            {
                bitmap = new Bitmap(256, 192, System.Drawing.Imaging.PixelFormat.Format16bppRgb555);
                Rectangle rectangle = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
                System.Drawing.Imaging.BitmapData bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadWrite, bitmap.PixelFormat);
                Marshal.Copy(data, 0, bmpData.Scan0, data.Length);
                bitmap.UnlockBits(bmpData);
            }
            return true;
         }
        public bool Import(String src)
        {
            return Paste(new Bitmap(src),src);
        }
        public bool Paste (Bitmap newbmp,String src)
        {
            if ((newbmp.Height != bitmap.Height) || (newbmp.Width != bitmap.Width)/* || ((bitmap.PixelFormat != newbmp.PixelFormat) && !IsRaw)*/)
            {
                return false;
            }

            if (IsRaw)
            {
                bitmap = newbmp;
                Graphics g = Graphics.FromImage(bitmap);
                g.DrawImage(newbmp, new Point(0, 0));
                g.Dispose();
            }
            else if (tiled /*(depth == 0x4) && (header.Length == 0x10) && (bitmap.Height == 0xC0)*/){
                QuitherImport(newbmp);
                //QuantizeImport(newbmp);
            }
            else
            {
                if (src == "")
                {
                    src = "culdtemp" + System.IO.Path.DirectorySeparatorChar + "grit.png";
                    newbmp.Save(src);
                }
                GritImport(src);
                #region deprecated
                /*
                Graphics g = Graphics.FromImage(bitmap);
                g.DrawImage(newbmp, new Point(0, 0));
                g.Dispose();
                */
                #endregion
            }
            return true;
        }
#if false
        /* doesn't work good */
        public virtual bool QuantizeImport(Bitmap newbmp){
            List<List<Color>> palettes = new List<List<Color>>();

            newbmp = FreeImageHelper.BitmapTo8bppIndexed(newbmp);
            for (int y = 0; y < (newbmp.Height / 8); y++)
            {
                for (int x = 0; x < (newbmp.Width / 8); x++)
                {
                    List<Color> xxx = FreeImageHelper.GetUniqueColors(newbmp, new Rectangle(x * 8, y * 8, 8, 8)).OrderBy(c => c.ToArgb()).ToList();
                    palettes.Add(xxx);
                }
            }
            ListComparer<Color> comparer = new ListComparer<Color>();
            ColorComparer colorcomp = new ColorComparer();
            //remove duplicates
            palettes = palettes.Distinct(comparer).ToList();
            //remove palettes which are subsets of other
            palettes.RemoveAll(pal => palettes.Exists(x => !x.SequenceEqual(pal) && pal.All(y => x.Contains(y,colorcomp))));

            //select 10 combinations .. pretty random but simple
            while (palettes.Count > 0x10)
            {
                var combination = from x in palettes
                                  from y in palettes
                                  where y != x /* && x.Union(y).Count() <= 0x10 */
                                  orderby x.Union(y).Count() ascending /* descending */
                                  select x.Union(y);
                palettes.Add(combination.First().ToList());
                palettes.RemoveAll(pal => palettes.Exists(x => !x.SequenceEqual(pal) && pal.All(y => x.Contains(y, colorcomp))));
            }

            //remove pretty unique colors
            foreach (List<Color> palette in palettes.Where(x => x.Count > 0x10))
            {
                while (palette.Count > 0x10 && !palette.Exists(x => palettes.Exists(p => p != palette && p.Contains(x))))
                {
                    var c = from p in palette
                            where palette.Exists(x => palettes.Exists(z => z != palette && z.Contains(x)))
                            select p;
                    palette.Remove(c.First());
                }
            }

            //reduce palettes to 0x10 colors
            foreach (List<Color> palette in palettes.Where(x => x.Count > 0x10))
            {
                while (palette.Count > 0x10)
                {
                    var c = from p in palette
                            from p2 in palette
                            where p2 != p
                            orderby ((p2.R - p.R) * (p2.R - p.R) + (p2.G - p.G) * (p2.G - p.G) + (p2.B - p.B) * (p2.B - p.B)) ascending
                            select p2;
                    palette.Remove(c.First());
                }
            }

            return true;
        }
#endif
        public virtual bool QuitherImport(Bitmap newbmp)
        {
            #region deprecated
            /*
            //convert to 8bpp BMP
            Bitmap bmp = FreeImageHelper.BitmapTo8bppIndexed(newbmp);

            //process with quither
            String tmp = "culdtemp" + System.IO.Path.DirectorySeparatorChar;
            bmp.Save(tmp + "quither.bmp",System.Drawing.Imaging.ImageFormat.Bmp);
            
            String cmd = "quither -r 16 0 " + tmp + "quither.bmp " + tmp + "quither";

            Utils.SystemCall(cmd);
            BinaryReader br;
            br = new BinaryReader(new FileStream(tmp + "quither.gfx", FileMode.Open));
            byte[] newbitdata = br.ReadBytes((int)br.BaseStream.Length);
            br.Close();
            br = new BinaryReader(new FileStream(tmp + "quither.pal", FileMode.Open));
            br.Read(origpalette, 0, (int)br.BaseStream.Length);
            br.Close();

            br = new BinaryReader(new FileStream(tmp + "quither.map", FileMode.Open));
            byte[] newmap = br.ReadBytes((int)br.BaseStream.Length);
            br.Close();

            bitdata = new byte[bitmap.Width * bitmap.Height / 2];

            for (int i = 0; i < map.Length; i++)
            {
                UInt16 m = BitConverter.ToUInt16(newmap, i * 2);
                UInt16 tilenum = (UInt16)(m & 0x3FF);
                UInt16 palnum = (UInt16)(m >> 12);
                Array.Copy(newbitdata, tilenum * 0x20, bitdata, i * 0x20, 0x20);
                map[i] = (byte)(palnum);
            }
            return true;
            */
        #endregion
            Bitmap bmp = newbmp;
            bitdata = new byte[bmp.Height * bmp.Width / 2];
            BinaryWriter bw = new BinaryWriter(new MemoryStream(bitdata));

            var PaletteList = new List<List<Color>>();

            for (int i = 0; i < origpalette.Length; i += 0x20)
            {
                List<Color> pall = new List<Color>();
                for (int colornum = 0; colornum < 0x10; colornum++ )
                {
                    UInt16 num = BitConverter.ToUInt16(origpalette, i + colornum * 2);
                    int red = (num & 0x1f) << 3;
                    int green = (num & (0x1f << 5)) >> 2;
                    int blue = (num & (0x1f << 10)) >> 7;
                    pall.Add(Color.FromArgb(red, green, blue));
                }
                PaletteList.Add(pall);
            }
            byte[] pal = new byte[0x20];
            for (int y = 0; y < (bmp.Height / 8); y++)
            {
                for (int x = 0; x < (bmp.Width / 8); x++)
                {
                    /*
                    int palnum = map[x + y * (bmp.Width / 8)];
                    Array.Copy(origpalette, 0x20 * palnum, pal, 0, pal.Length);
                    bool foundpal = foundpal = CheckPal(bmp, new Rectangle(x * 8, y * 8, 8, 8), pal); ;
                    for (int z = 0; (z < origpalette.Length / pal.Length) && (!foundpal); z++)
                    {
                        Array.Copy(origpalette, 0x20 * z, pal, 0, pal.Length);
                        foundpal = CheckPal(bmp, new Rectangle(x * 8, y * 8, 8, 8), pal);
                        if (foundpal)
                            palnum = z;
                    }
                    */
                    int palnum = FreeImageHelper.FindBestPalette(bmp, new Rectangle(x * 8, y * 8, 8, 8), PaletteList);
                    Array.Copy(origpalette, 0x20 * palnum, pal, 0, pal.Length);
                    bw.Write(FreeImageHelper.Get4BppRectWithPalette(bmp, new Rectangle(x*8,y*8,8,8), pal));
                    map[x + y * (bmp.Width / 8)] = (byte)palnum;
                }
            }
            bw.Close();
            for (int i = 0; i < bitdata.Length; i++)
            {
                bitdata[i] = (byte)(((bitdata[i] & 0x0F) << 4) | ((bitdata[i] & 0xF0) >> 4));
            }
            return true;
       }

        public virtual bool GritImport(String src){
            String tmp = "culdtemp" + System.IO.Path.DirectorySeparatorChar;
            String cmd = "grit " + src + " -ftb -fh! ";
            //cmd += String.Format("-gT {0:X2}{1:X2}{2:X2} ", bitmap.Palette.Entries[0].R, bitmap.Palette.Entries[0].G, bitmap.Palette.Entries[0].B);
            cmd += "-gTFF00FF ";
            //cmd += "-g407898 ";
            if (!tiled)
            {
#if false   
                cmd += "-gb ";
                cmd += String.Format("-gB{0} ",depth);
                cmd += "-pn " + origpalette.Length / 2;
                cmd += " -o" + tmp + "grit";
                Utils.SystemCall(cmd);
                BinaryReader br;
                br = new BinaryReader(new FileStream(tmp + "grit.img.bin", FileMode.Open));
                bitdata = br.ReadBytes((int)br.BaseStream.Length);
                br.Close();
                br = new BinaryReader(new FileStream(tmp + "grit.pal.bin", FileMode.Open));
                br.Read(origpalette, 0, origpalette.Length);
                br.Close();
#else

                Bitmap bmp = new Bitmap(src);
                //bitdata = FreeImageHelper.QuantizeToPalette(bmp, origpalette, depth);
                bitdata = FreeImageHelper.Get4BppRectWithPalette(bmp, new Rectangle(0, 0, bmp.Width, bmp.Height), origpalette);
                /*
                for (int i = 0; i < bitdata.Length; i++)
                {
                    
                    if ((bitdata[i] & 0x0F) == 0x0E)
                    {
                        bitdata[i] &= 0xF0;
                    }
                    if ((bitdata[i] & 0xF0) == 0xE0)
                    {
                        bitdata[i] &= 0x0F;
                    }
                    
                    bitdata[i] = (byte)(((bitdata[i] & 0x0F) << 4) | ((bitdata[i] & 0xF0) >> 4));
                }
                */
#endif
            }
            #region deprecated
            else
            {
                cmd += " -gt -gB4 -mLf -mR! -m -mRp ";
                //cmd += "-pn " + origpalette.Length / 2;
                cmd += " -o" + tmp + "grit";

                //cmd = @"grit culdtemp\grit.png -Ww -ftb -fh!  -gt -gB4 -mLf -pn 256 -mR! -m -mRp -gT0 -oculdtemp\grit";
                Utils.SystemCall(cmd);
                BinaryReader br;
                br = new BinaryReader(new FileStream(tmp + "grit.img.bin", FileMode.Open));
                byte[] newbitdata = br.ReadBytes((int)br.BaseStream.Length);
                br.Close();
                br = new BinaryReader(new FileStream(tmp + "grit.pal.bin", FileMode.Open));
                br.Read(origpalette, 0, (int)br.BaseStream.Length);
                br.Close();

                br = new BinaryReader(new FileStream(tmp + "grit.map.bin", FileMode.Open));
                byte[] newmap = br.ReadBytes((int)br.BaseStream.Length);
                br.Close();
              
                bitdata = new byte[bitmap.Width * bitmap.Height / 2];
                
                for (int i = 0; i < map.Length; i++)
                {
                    UInt16 m = BitConverter.ToUInt16(newmap, i * 2);
                    UInt16 tilenum = (UInt16)(m & 0x3FF);
                    UInt16 palnum = (UInt16)(m >> 12);
                    Array.Copy(newbitdata, tilenum * 0x20, bitdata, i * 0x20, 0x20);
                    map[i] = (byte)(palnum);
                }

            }
            #endregion
            return true;
        }
        private void WritePalette(BinaryWriter bw)
        {
            #region deprecated
            /*
            for (int i = 0; i < PaletteSize; i ++ )
            {
                Color color = bitmap.Palette.Entries[i];
                bw.Write((ushort)(((color.B << 7) | (color.G << 2)) | (color.R >> 3)));
            }
            */
            #endregion
            bw.Write(origpalette);
        }
        public virtual void WriteBitmap(BinaryWriter bw)
        {
            bw.Write(bitdata);
            #region deprecated
            /*
            Rectangle rectangle = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            System.Drawing.Imaging.BitmapData bmpData = bitmap.LockBits(rectangle, System.Drawing.Imaging.ImageLockMode.ReadOnly, bitmap.PixelFormat);
            byte[] data;
            if (!IsRaw)
                if (this.bitmap.PixelFormat == System.Drawing.Imaging.PixelFormat.Format4bppIndexed)
                    data = new byte[bmpData.Height * bmpData.Width / 2];
                else
                    data = new byte[bmpData.Height * bmpData.Width];
            else
                data = new byte[bmpData.Height * bmpData.Width * 2];

            Marshal.Copy(bmpData.Scan0, data, 0, data.Length);
            if (bitmap.PixelFormat == System.Drawing.Imaging.PixelFormat.Format4bppIndexed)
            {
                for (int i = 0; i < data.Length; i++)
                {
                    data[i] = (byte)(((data[i] & 0x0F) << 4) | ((data[i] & 0xF0) >> 4));
                }
            }

            bitmap.UnlockBits(bmpData);
            bw.Write(data);
            */
            #endregion
        }
        public bool Save()
        {
            //BinaryWriter bw = new BinaryWriter(new FileStream("pic.tmp",FileMode.Create));
            BinaryWriter bw = new BinaryWriter(new MemoryStream());
            if (!IsRaw)
            {
                bw.Write(header);
                WritePalette(bw);
            }
            WriteBitmap(bw); 
            if (tiled)
            {
                bw.Write(map);
            }
            Int32 origlength = (int)bw.BaseStream.Position;
            byte[] buff = new byte[origlength];
            bw.BaseStream.Position = 0;
            bw.BaseStream.Read(buff, 0, origlength);
            bw.Close();
            CuldceptCommon.CuldLHA(buff, fname);
            return true;
        }
    }
    class Pictures
    {
        #region filenames
        public static String[] fnames = new String[]{
            "00002",
            "00003",
            "00004",
            "00005",
            "00006",
            "00007",
            "00008",
            "00027",
            "00028",
            "00029",
            "00030",
            "00031",
            "00032",
            "00033",
            "00034",
            "00086",
            "00087",
            "00090",
            "00099",
            "00106",
            "00117",
            "00118",
            "00120",
            "00122",
            "00128",
            "00129",
            "00165",
            "00166",
            "00167",
            "00168",
            "00169",
            "00170",
            "00171",
            "00172",
            "00173",
            "00174",
            "00175",
            "00176",
            "00177",
            "00178",
            "00179",
            "00180",
            "00181",
            "00182",
            "00183",
            "00184",
            "00187",
            "00229",
            "00230",
            "00231",
            "00232",
            "00233",
            "00234",
            "00235",
            "00236",
            "00237",
            "00238",
            "00239",
            "00240",
            "00241",
            "00242",
            "00243",
            "00244",
            "00245",
            "00246",
            "00247",
            "00248",
            "00249",
            "00250",
            "00251",
            "00252",
            "00253",
            "00254",
            "00255",
            "00256",
            "00257",
            "00258",
            "00259",
            "00260",
            "00261",
            "00262",
            "00264",
            "00266",
            "00267",
            "00268",
            "00269",
            "00270",
            "00271",
            "00272",
            "00273",
            "00274",
            "00275",
            "00276",
            "00277",
            "00278",
            "00279",
            "00280",
            "00281",
            "00282",
            "00283",
            "00284",
            "00285",
            "00286",
            "00287",
            "00288",
            "00289",
            "00290",
            "00291",
            "00292",
            "00293",
            "00294",
            "00295",
            "00296",
            "00297",
            "00298",
            "00299",
            "00300",
            "00301",
            "00302",
            "00303",
            "00304",
            "00305",
            "00306",
            "00307",
            "00308",
            "00309",
            "00310",
            "00311",
            "00312",
            "00313",
            "00314",
            "00315",
            "00384",
            "00385",
            "00675",
            "00676",
            "00677",
            "00678",
            "00679",
            "00680",
            "00681",
            "00682",
            "00683",
            "00684",
            "00685",
            "00686",
            "00687",
            "00688",
            "00689",
            "00690",
            "00691",
            "00692",
            "00693",
            "00694",
            "00695",
            "00696",
            "00697",
            "00698",
            "00699",
            "00700",
            "00701",
            "00702",
            "00703",
            "00704",
            "00705",
            "00706",
            "00707",
            "00708",
            "00709",
            "00710",
            "00711",
            "00712",
            "00713",
            "00714",
            "00715",
            "00716",
            "00717",
            "00718",
            "00719",
            "00720",
            "00721",
            "00722",
            "00723",
            "00724",
            "00725",
            "00726",
            "00727",
            "00728",
            "00729",
            "00730",
            "00731",
            "00732",
            "00733",
            "00734",
            "00735",
            "00736",
            "00737",
            "00738",
            "00739",
            "00740",
            "00741",
            "00742",
            "00743",
            "00744",
            "00745",
            "00746",
            "00747",
            "00748",
            "00749",
            "00750",
            "00751",
            "00752",
            "00753",
            "00754",
            "00755",
            "00773",
            "00774",
            "00775",
            "00776",
            "00777",
            "00778",
            "00779",
            "00780",
            "00781",
            "00782",
            "00783",
            "00784",
            "00785",
            "00786",
            "00787",
            "00788",
            "00789",
            "00790",
            "00791",
            "00792",
            "00793",
            "00794",
            "00795",
            "00796",
            "00797",
            "00798",
            "00799",
            "00800",
            "00801",
            "00802",
            "00803",
            "00804",
            "00805",
            "00806",
            "00807",
            "00808",
            "00809",
            "00810",
            "00811",
            "00812",
            "00813",
            "00814",
            "00815",
            "00816",
            "00817",
            "00818",
            "00819",
            "00820",
            "00821",
            "00822",
            "00823",
            "00824",
            "00825",
            "00826",
            "00827",
            "00828",
            "00829",
            "00830",
            "00831",
            "00832",
            "00833",
            "00834",
            "00835",
            "00836",
            "00837",
            "00838",
            "00839",
            "00840",
            "00841",
            "00842",
            "00843",
            "00844",
            "00845",
            "00846",
            "00847",
            "00848",
            "00849",
            "00850",
            "00851",
            "00852",
            "00853",
            "00854",
            "00855",
            "00856",
            "00857",
            "00858",
            "00859",
            "00860",
            "00861",
            "00862",
            "00863",
            "00864",
            "00865",
            "00866",
            "00867",
            "00868",
            "00869",
            "00870",
            "00871",
            "00872",
            "00873",
            "00874",
            "00875",
            "00876",
            "00877",
            "00878",
            "00879",
            "00880",
            "00881",
            "00882",
            "00883",
            "00884",
            "00885",
            "00886",
            //"00887", //scen
            "00888",
            "00889",
            "00890",
            "00891",
            "00892",
            "00893",
            "00894",
            "00895",
            "00896",
            "00897",
            "00898",
            "00899",
            "00900",
            "00901",
            "00902",
            "00903",
            "00904",
            "00905",
            "00906",
            "00907",
            "00908",
            "00909",
            "00910",
            "00911",
            "00912",
            "00913",
            "00914",
            "00915",
            //"00916", //scen
            "00917",
            "00918",
            "00919",
            "00920",
            "00921",
            "00922",
            "00923",
            "00924",
            "00925",
            "00926",
            "00927",
            "00928",
            "00929",
            "00930",
            "00931",
            "00932",
            "00933",
            "00934",
            "00935",
            "00936",
            "00937",
            "00938",
            "00939",
            "00940",
            "00941",
            "00942",
            "00943",
            "00944",
            "00945",
            //"00946", //scen
            "00947",
            "00948",
            "00949",
            "00950",
            "00951",
            "00952",
            "00953",
            "00954",
            "00955",
            "00956",
            "00957",
            "00958",
            "00959",
            "00960",
            "00961",
            "00962",
            //"00963", //scen
            "00964",
            "00965",
            "00966",
            "00967",
            "00968",
            "00969",
            "00970",
            "00971",
            "00972",
            "00973",
            "00974",
            "00975",
            "00976",
            "00977",
            "00978",
            "00979",
            //"00980",//scen
            "00981",
            "00982",
            "00983",
            "00984",
            "00985",
            "00986",
            "00987",
            "00988",
            "00989",
            "00990",
            "00991",
            "00992",
            "00993",
            "00994",
            "00995",
            "00996",
            //"00997",//scen
            "00998",
            "00999",
            "01000",
            "01001",
            "01002",
            "01003",
            "01004",
            "01005",
            "01006",
            "01007",
            "01008",
            "01009",
            "01010",
            "01011",
            "01012",
            "01013",
            //"01014",//scen
            "01015",
            "01016",
            "01017",
            "01018",
            "01019",
            "01020",
            "01021",
            "01022",
            "01023",
            "01024",
            "01025",
            "01026",
            "01027",
            "01028",
            "01029",
            "01030",
            "01031",
            "01032",
            "01033",
            "01034",
            "01035",
            "01036",
            "01037",
            "01038",
            "01039",
            "01040",
            "01041",
            "01042",
            "01043",
            "01044",
            "01045",

            //tile based
            //"00092",
            //"00093",
            //"00094",
            //"00096",
            //"00097",
            //"00111",
            //"00112",
            //"00113",



            //need to check
            //do not need translations
            //"00132", 
            //"00133",
            //"00134",
            //"00135",


    };
        #endregion 
        #region rawfnames
        public static String[] rawfnames = new String[]{
            "00188",
            "00189",
            "00190",
            "00191",
            "00192",
            "00193",
            "00194",
            "00195",
            "00196",
            "00197",
            "00198",
            "00199",
            "00200",
            "00201",
            "00202",
            "00203",
            "00204",
            "00205",
            "00206",
            "00207",
            "00208",
            "00209",
            "00210",
            "00211",
            "00212",
            "00213",
            "00214",
            "00215",
            "00216",
            "00217",
            "00218",
            "00219",
            "00220",
            "00221",
            "00222",
            "00223",
            "00224",
            "00225",
            "00226",
            "00227",
            "00228",
        };
        #endregion
        public static CuldPicture[] getPictures()
        {
            CuldPicture[] pages = new CuldPicture[fnames.Length + rawfnames.Length];
            for (int i = 0; i < rawfnames.Length; i++)
            {
                pages[i] = new CuldPicture();
                pages[i].IsRaw = true;
                pages[i].Read("culdtemp" + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar +
                    rawfnames[i], rawfnames[i]);
            }
            for (int i = 0; i < fnames.Length; i++)
            {
                pages[i + rawfnames.Length] = new CuldPicture();
                pages[i + rawfnames.Length].IsRaw = false;
                pages[i + rawfnames.Length].Read("culdtemp" + System.IO.Path.DirectorySeparatorChar + "inner" + System.IO.Path.DirectorySeparatorChar + "CULDCEPT.DAT" + System.IO.Path.DirectorySeparatorChar +
                    fnames[i], fnames[i]);
            }

            return pages;
        }
    }
}
