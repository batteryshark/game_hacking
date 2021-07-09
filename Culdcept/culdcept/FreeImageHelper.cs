using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using FreeImageAPI;
using System.Runtime.InteropServices;
using System.Linq;

namespace CuldceptDSEditor
{
    class FreeImageHelper
    {
        public static Bitmap BitmapTo8bppIndexed(Bitmap bmp){
            FIBITMAP dib = new FIBITMAP();
            dib = FreeImage.CreateFromBitmap(bmp);
            FIBITMAP dib2 = FreeImage.ConvertTo24Bits(dib);
            FreeImage.Unload(dib);
            //dib = FreeImage.ColorQuantize(dib2, FREE_IMAGE_QUANTIZE.FIQ_WUQUANT);
            dib = FreeImage.ColorQuantize(dib2, FREE_IMAGE_QUANTIZE.FIQ_NNQUANT);
            
            Bitmap newbmp = FreeImage.GetBitmap(dib);
            if (!dib.IsNull)
                FreeImage.Unload(dib);
            if (!dib2.IsNull)
                FreeImage.Unload(dib2);
            return newbmp;
            
        }
        public static Bitmap ApplyPaletteFromImage(Bitmap palbmp, Bitmap newbmp){
            //Bitmap newbmpIndexed = BitmapTo8bppIndexed(newbmp);
            Bitmap palbmpIndexed = BitmapTo8bppIndexed(palbmp);
            FIBITMAP dib = new FIBITMAP();
            dib = FreeImage.CreateFromBitmap(palbmpIndexed);
            FIBITMAP dib2 = new FIBITMAP();
            dib2 = FreeImage.CreateFromBitmap(newbmp);
            RGBQUAD[] rgbq = FreeImage.GetPaletteEx(dib).AsArray;
            FreeImage.Unload(dib);
            dib2 = FreeImage.ConvertTo24Bits(dib2);
            dib = FreeImage.ColorQuantizeEx(dib2, FREE_IMAGE_QUANTIZE.FIQ_NNQUANT, rgbq.Length,rgbq.Length, rgbq);
            Bitmap result = FreeImage.GetBitmap(dib);

            if (!dib.IsNull)
                FreeImage.Unload(dib2);
            if (!dib2.IsNull)
                FreeImage.Unload(dib2);
            return result;
        }
        public static int FindBestPalette(Bitmap inbmp, Rectangle section,List<List<Color>> PaletteList)
        {
            List<Color> imgpalette = GetUniqueColors(inbmp, section).ToList();
            int respalnum = -1;
            int distance = 0x10000000;
            int palnum = 0;
            for (; palnum < PaletteList.Count && distance > 0; palnum++)
            {
                int curdistance = 0;
                List<Color> curpalette = PaletteList[palnum];
                foreach (Color curcolor in imgpalette)
                    if (!curpalette.Contains(curcolor))
                        curdistance += curpalette.Min( x => ColorDistance(curcolor,x));
                if (curdistance < distance)
                {
                    distance = curdistance;
                    respalnum = palnum;
                }
            }
            return respalnum;
        }
        public static Color[] GetUniqueColors(Bitmap inbmp, Rectangle section)
        {
            Bitmap bmp = new Bitmap(section.Width, section.Height);
            Graphics g = Graphics.FromImage(bmp);
            g.DrawImage(inbmp, 0, 0, section, GraphicsUnit.Pixel);
            g.Dispose();
            var colors = new List<Color>();
            for (int x = 0; x < bmp.Width; x++ )
            {
                for (int y = 0; y < bmp.Height; y ++ )
                {
                    Color color = bmp.GetPixel(x,y);
                    if (!colors.Contains(color))
                        colors.Add(color);
                }
            }

            return colors.ToArray();
        }
        public static int ColorDistance(Color p , Color p2)
        {
            return ((p2.R - p.R) * (p2.R - p.R) + (p2.G - p.G) * (p2.G - p.G) + (p2.B - p.B) * (p2.B - p.B));
        }
        public static byte[] Get4BppRectWithPalette(Bitmap inbmp, Rectangle section, byte[] pal)
        {
            byte[] data;

            //copy region
            Bitmap bmp = new Bitmap(section.Width, section.Height);
            Graphics g = Graphics.FromImage(bmp);
            g.DrawImage(inbmp, 0, 0, section, GraphicsUnit.Pixel);
            g.Dispose();

            //convert to 24Bpp
            FIBITMAP temp = new FIBITMAP();
            temp = FreeImage.CreateFromBitmap(bmp);
            FIBITMAP dib = FreeImage.ConvertTo24Bits(temp);
            FreeImage.Unload(temp);

            //create needed palette 
            RGBQUAD[] rgbq = new RGBQUAD[pal.Length / 2];
            for (int i = 0; i < rgbq.Length; i++)
            {
                UInt16 num = BitConverter.ToUInt16(pal, i * 2);
                int red = (num & 0x1f) << 3;
                int green = (num & (0x1f << 5)) >> 2;
                int blue = (num & (0x1f << 10)) >> 7;
                rgbq[i] = Color.FromArgb(red, green, blue);
            }

            //apply palette
            temp = FreeImage.ColorQuantizeEx(dib, FREE_IMAGE_QUANTIZE.FIQ_NNQUANT, rgbq.Length, rgbq, 4);
            //somehow it is needed o_O
            FreeImage.FlipVertical(temp);

            //copy data
            data = new byte[section.Width * section.Height / 2];
            Marshal.Copy(FreeImage.GetBits(temp),data,0,data.Length);           


            return data;
        }
        public static byte[] QuantizeToPalette(Bitmap bmp, byte[] pal,int depth)
        {
            byte[] data;

            //convert to 24Bpp
            FIBITMAP temp = new FIBITMAP();
            temp = FreeImage.CreateFromBitmap(bmp);
            FIBITMAP dib = FreeImage.ConvertTo24Bits(temp);
            FreeImage.Unload(temp);

            //create needed palette 
            RGBQUAD[] rgbq = new RGBQUAD[pal.Length / 2];
            for (int i = 0; i < rgbq.Length; i++)
            {
                UInt16 num = BitConverter.ToUInt16(pal, i * 2);
                int red = (num & 0x1f) << 3;
                int green = (num & (0x1f << 5)) >> 2;
                int blue = (num & (0x1f << 10)) >> 7;
                rgbq[i] = Color.FromArgb(red, green, blue);
            }

            //apply palette
            temp = FreeImage.ColorQuantizeEx(dib, FREE_IMAGE_QUANTIZE.FIQ_WUQUANT, rgbq.Length, rgbq, depth);
            //somehow it is needed o_O
            FreeImage.FlipVertical(temp);

            //copy data
            data = new byte[bmp.Width * bmp.Height / (8/depth)];
            Marshal.Copy(FreeImage.GetBits(temp), data, 0, data.Length);


            return data;
        }

    }
}
