using System;
using System.IO;

namespace theli
{
    public class Lha
    {
        public enum LzhCompressMethodType
        {
            LH0,
            LH1,
            LH2,
            LH3,
            LH4,
            LH5,
            LH6,
            LH7,
        }
        public static byte[] LHADecode (LzhCompressMethodType type,byte[] input, int OriginalDataSize){
            return (new Lha(type)).DecodeLzHuf(input, OriginalDataSize);
        }
        public Lha(LzhCompressMethodType CompressMethod)
        {
            switch (CompressMethod)
            {
                case LzhCompressMethodType.LH0:
                    Dicbit = LZHUFF0_DICBIT;
                    break;
                case LzhCompressMethodType.LH1:
                    Dicbit = LZHUFF1_DICBIT;
                    break;
                case LzhCompressMethodType.LH2:
                    Dicbit = LZHUFF2_DICBIT;
                    break;
                case LzhCompressMethodType.LH3:
                    Dicbit = LZHUFF3_DICBIT;
                    break;
                case LzhCompressMethodType.LH4:
                    Dicbit = LZHUFF4_DICBIT;
                    break;
                case LzhCompressMethodType.LH5:
                    Dicbit = LZHUFF5_DICBIT;
                    break;
                case LzhCompressMethodType.LH6:
                    Dicbit = LZHUFF6_DICBIT;
                    break;
                case LzhCompressMethodType.LH7:
                    Dicbit = LZHUFF7_DICBIT;
                    break;
            }
            Dicsize = (1 << Dicbit);
            method = CompressMethod;
        }
        #region Copyright Notice
        // Following code is based on LHa for UNIX Ver. 1.14i.
        // Original copyright text is as follows.
        /* ------------------------------------------------------------------------ */
        /* LHa for UNIX                                                             */
        /*              lharc.c -- append to archive                                */
        /*                                                                          */
        /*      Copyright (C) MCMLXXXIX Yooichi.Tagawa                              */
        /*      Modified                Nobutaka Watazaki                           */
        /*                          Thanks to H.Yoshizaki. (MS-DOS LHarc)           */
        /*                                                                          */
        /*  Ver. 0.00  Original                         1988.05.23  Y.Tagawa        */
        /*  Ver. 0.01  Alpha Version (for 4.2BSD)       1989.05.28  Y.Tagawa        */
        /*  Ver. 0.02  Alpha Version Rel.2              1989.05.29  Y.Tagawa        */
        /*  Ver. 0.03  Release #3  Beta Version         1989.07.02  Y.Tagawa        */
        /*  Ver. 0.03a Debug                            1989.07.03  Y.Tagawa        */
        /*  Ver. 0.03b Modified                         1989.07.13  Y.Tagawa        */
        /*  Ver. 0.03c Debug (Thanks to void@rena.dit.junet)                        */
        /*                                              1989.08.09  Y.Tagawa        */
        /*  Ver. 0.03d Modified (quiet and verbose)     1989.09.14  Y.Tagawa        */
        /*  V1.00  Fixed                                1989.09.22  Y.Tagawa        */
        /*  V1.01  Bug Fixed                            1989.12.25  Y.Tagawa        */
        /*                                                                          */
        /*  DOS-Version Original LHx V C2.01        (C) H.Yohizaki                  */
        /*                                                                          */
        /*  V2.00  UNIX Lharc + DOS LHx -> OSK LHx      1990.11.01  Momozou         */
        /*  V2.01  Minor Modified                       1990.11.24  Momozou         */
        /*                                                                          */
        /*  Ver. 0.02  LHx for UNIX                     1991.11.18  M.Oki           */
        /*  Ver. 0.03  LHa for UNIX                     1991.12.17  M.Oki           */
        /*  Ver. 0.04  LHa for UNIX beta version        1992.01.20  M.Oki           */
        /*  Ver. 1.00  LHa for UNIX Fixed               1992.03.19  M.Oki           */
        /*                                                                          */
        /*  Ver. 1.10  for Symbolic Link                1993.06.25  N.Watazaki      */
        /*  Ver. 1.11  for Symbolic Link Bug Fixed      1993.08.18  N.Watazaki      */
        /*  Ver. 1.12  for File Date Check              1993.10.28  N.Watazaki      */
        /*  Ver. 1.13  Bug Fixed (Idicator calcurate)   1994.02.21  N.Watazaki      */
        /*  Ver. 1.13a Bug Fixed (Sym. Link delete)     1994.03.11  N.Watazaki      */
        /*  Ver. 1.13b Bug Fixed (Sym. Link delete)     1994.07.29  N.Watazaki      */
        /*  Ver. 1.14  Source All chagned               1995.01.14  N.Watazaki      */
        /*  Ver. 1.14b,c  Bug Fixed                     1996.03.07  t.okamoto       */
        /*  Ver. 1.14d Version up                       1997.01.12  t.okamoto       */
        /*  Ver. 1.14g Bug Fixed                        2000.05.06  t.okamoto       */
        /*  Ver. 1.14i Modified                         2000.10.06  t.okamoto       */
        /* ------------------------------------------------------------------------ */
        #endregion
        #region decoding
        private const int THRESHOLD = 3;
        private byte[] packedData;
        private int packedDataPos;
        private byte[] decodedData;
        private int Dicbit;
        private int Dicsize;
        public byte[] DecodeLzHuf(byte[] input, int OriginalDataSize)
        {

            packedData = input;
            packedDataPos = 0;
            int PackedDataSize = input.Length;

            byte[] dtext = new byte[Dicsize];
            for (int i = 0; i < Dicsize; ++i)
            {
                dtext[i] = (byte)(' ');
            }
            DecodeStart();
            int dicsizeMinus1 = Dicsize - 1;
            int adjust = 256 - THRESHOLD;
            decodedData = new byte[OriginalDataSize];
            int decodedDataWritePos = 0;
            int decodedCount = 0;
            int dtextDataSize = 0;
            while (decodedCount < OriginalDataSize)
            {
                int c = DecodeC();
                if (c < 256)
                {
                    dtext[dtextDataSize++] = (byte)c;
                    if (dtextDataSize == Dicsize)
                    {
                        System.Array.Copy(dtext, 0, decodedData, decodedDataWritePos, dtextDataSize);
                        decodedDataWritePos += dtextDataSize;
                        dtextDataSize = 0;
                    }
                    ++decodedCount;
                }
                else
                {
                    int matchlen = c - adjust;
                    int matchoff = DecodeP() + 1;
                    int matchpos = (dtextDataSize - matchoff) & dicsizeMinus1;
                    decodedCount += matchlen;
                    for (uint i = 0; i < matchlen; i++)
                    {
                        c = dtext[(matchpos + i) & dicsizeMinus1];
                        dtext[dtextDataSize++] = (byte)c;
                        if (dtextDataSize == Dicsize)
                        {
                            System.Array.Copy(dtext, 0, decodedData, decodedDataWritePos, dtextDataSize);
                            decodedDataWritePos += dtextDataSize;
                            dtextDataSize = 0;
                        }
                    }
                }
            }
            if (dtextDataSize != 0)
            {
                System.Array.Copy(dtext, 0, decodedData, decodedDataWritePos, dtextDataSize);
                decodedDataWritePos += dtextDataSize;
                dtextDataSize = 0;
            }
            dtext = null;
            packedData = null;
            return decodedData;
        }

        private const int LZHUFF0_DICBIT = 0;
        private const int LZHUFF1_DICBIT = 12;
        private const int LZHUFF2_DICBIT = 13;
        private const int LZHUFF3_DICBIT = 13;
        private const int LZHUFF4_DICBIT = 12;
        private const int LZHUFF5_DICBIT = 13;
        private const int LZHUFF6_DICBIT = 15;
        private const int LZHUFF7_DICBIT = 16;
        private int pbit, np;
        private ushort blocksize;
        private LzhCompressMethodType method;

        private void DecodeStart()
        {
            switch (Dicbit)
            {
                case LZHUFF2_DICBIT:
                    pbit = 4;
                    np = LZHUFF5_DICBIT + 1;
                    break;
                case LZHUFF6_DICBIT:
                    pbit = 5;
                    np = LZHUFF6_DICBIT + 1;
                    break;
                case LZHUFF7_DICBIT:
                    pbit = 5;
                    np = LZHUFF7_DICBIT + 1;
                    break;
                default:
                    System.Diagnostics.Debug.Assert(true);
                    break;
            }
            InitGetBits();
            blocksize = 0;
        }


        private ushort DecodeC()
        {
            if (blocksize == 0)
            {
                blocksize = GetBits(16);
                ReadPtLen(NT, TBIT, 3);
                ReadCLen();
                ReadPtLen(np, pbit, -1);
            }
            blocksize--;
            ushort j = cTable[PeekBits(12)];
            if (j < NC)
            {
                FillBuf(cLen[j]);
                return j;
            }
            FillBuf(12);
            int mask = 1 << (16 - 1);
            do
            {
                if (0 != (GetBitbuf() & mask))
                {
                    j = right[j];
                }
                else
                {
                    j = left[j];
                }
                mask >>= 1;
            } while (j >= NC && ((mask != 0) || (j != left[j])));
            FillBuf(cLen[j] - 12);
            return j;
        }

        private static int Min(int l, int r)
        {
            return (l < r) ? l : r;
        }

        private const int NT = 19;
        private const int TBIT = 5;
        private const int MAXMATCH = 256;
        private const int NC = 255 + MAXMATCH + 2 - THRESHOLD;
        private ushort[] cTable = new ushort[4096];
        private byte[] cLen = new byte[NC];
        private ushort[] left = new ushort[2 * NC - 1];
        private ushort[] right = new ushort[2 * NC - 1];
        private const int NPT = 128;
        private byte[] ptLen = new byte[NPT];
        private ushort[] ptTable = new ushort[256];
        private const int CBIT = 9;

        private void ReadPtLen(int nn, int nbit, int iSpecial)
        {
            int n = GetBits(nbit);
            if (n == 0)
            {
                int c = GetBits(nbit);
                for (int i = 0; i < nn; i++)
                {
                    ptLen[i] = 0;
                }
                for (int i = 0; i < 256; i++)
                {
                    ptTable[i] = (ushort)c;
                }
                return;
            }
            {
                int i = 0;
                while (i < Min(n, NPT))
                {
                    int c = PeekBits(3);
                    if (c != 7)
                    {
                        FillBuf(3);
                    }
                    else
                    {
                        ushort mask = 1 << (16 - 4);
                        while (0 != (GetBitbuf() & mask))
                        {
                            mask >>= 1;
                            c++;
                        }
                        FillBuf(c - 3);
                    }
                    ptLen[i++] = (byte)c;
                    if (i == iSpecial)
                    {
                        c = GetBits(2);
                        while (--c >= 0 && i < NPT)
                        {
                            ptLen[i++] = 0;
                        }
                    }
                }
                while (i < nn)
                {
                    ptLen[i++] = 0;
                }
                MakeTable(nn, ptLen, 8, ptTable);
            }
        }

        

        private void ReadCLen()
        {
            int n = GetBits(CBIT);
            if (n == 0)
            {
                int c = GetBits(CBIT);
                for (int i = 0; i < NC; i++)
                {
                    cLen[i] = 0;
                }
                for (int i = 0; i < 4096; i++)
                {
                    cTable[i] = (ushort)c;
                }
                return;
            }
            {
                int i = 0;
                while (i < Min(n, NC))
                {
                    int c = ptTable[PeekBits(8)];
                    if (c >= NT)
                    {
                        ushort mask = 1 << (16 - 9);
                        do
                        {
                            if (0 != (GetBitbuf() & mask))
                            {
                                c = right[c];
                            }
                            else
                            {
                                c = left[c];
                            }
                            mask >>= 1;
                        } while (c >= NT && ((0 != mask) || c != left[c]));
                    }
                    FillBuf(ptLen[c]);
                    if (c <= 2)
                    {
                        if (c == 0)
                        {
                            c = 1;
                        }
                        else if (c == 1)
                        {
                            c = GetBits(4) + 3;
                        }
                        else
                        {
                            c = GetBits(CBIT) + 20;
                        }
                        while (--c >= 0)
                        {
                            cLen[i++] = 0;
                        }
                    }
                    else
                    {
                        cLen[i++] = (byte)(c - 2);
                    }
                }
                while (i < NC)
                {
                    cLen[i++] = 0;
                }
                MakeTable(NC, cLen, 12, cTable);
            }
        }

        private ushort DecodeP()
        {
            ushort j = ptTable[PeekBits(8)];
            if (j < np)
            {
                FillBuf(ptLen[j]);
            }
            else
            {
                FillBuf(8);
                ushort mask = 1 << (16 - 1);
                do
                {
                    if (0 != (GetBitbuf() & mask))
                    {
                        j = right[j];
                    }
                    else
                    {
                        j = left[j];
                    }
                    mask >>= 1;
                } while (j >= np && ((0 != mask) || j != left[j]));
                FillBuf(ptLen[j] - 8);
            }
            if (j != 0)
            {
                j = (ushort)((1 << (j - 1)) + GetBits(j - 1));
            }
            return j;
        }

        private void MakeTable(int nchar, byte[] bitLen, int tablebits, ushort[] table)
        {
            ushort[] count = new ushort[17];  // count of bitlen 
            ushort[] weight = new ushort[17]; // 0x10000ul >> bitlen 
            int i;
            int avail = nchar;
            for (i = 1; i <= 16; i++)
            {
                count[i] = 0;
                weight[i] = (ushort)(1 << (16 - i));
            }
            for (i = 0; i < nchar; i++)
            {
                if (bitLen[i] > 16)
                {
                    throw new System.IO.InvalidDataException();
                }
                count[bitLen[i]]++;
            }
            ushort total = 0;
            ushort[] start = new ushort[17];
            for (i = 1; i <= 16; i++)
            {
                start[i] = total;
                total += (ushort)(weight[i] * count[i]);
            }
            if ((total & 0xffff) != 0 || tablebits > 16)
            {
                throw new System.IO.InvalidDataException();
            }
            int m = 16 - tablebits;
            for (i = 1; i <= tablebits; i++)
            {
                start[i] >>= m;
                weight[i] >>= m;
            }
            int j = start[tablebits + 1] >> m;
            int k = Min(1 << tablebits, 4096);
            if (j != 0)
            {
                for (i = j; i < k; i++)
                {
                    table[i] = 0;
                }
            }
            for (j = 0; j < nchar; j++)
            {
                k = bitLen[j];
                if (k == 0)
                {
                    continue;
                }
                int l = start[k] + weight[k];
                if (k <= tablebits)
                {
                    l = Min(l, 4096);
                    for (i = start[k]; i < l; i++)
                    {
                        table[i] = (ushort)j;
                    }
                }
                else
                {
                    i = start[k];
                    if ((i >> m) > 4096)
                    {
                        throw new System.IO.InvalidDataException();
                    }
                    int pos = i >> m;
                    TableOrLeftOrRight tlr = TableOrLeftOrRight.Table;
                    i <<= tablebits;
                    int n = k - tablebits;
                    while (--n >= 0)
                    {
                        if (TableOrLeftOrRightGet(tlr, table, pos) == 0)
                        {
                            right[avail] = left[avail] = 0;
                            TableOrLeftOrRightSet(tlr, table, pos, (ushort)(avail++));
                        }
                        if (0 != (i & 0x8000))
                        {
                            pos = TableOrLeftOrRightGet(tlr, table, pos);
                            tlr = TableOrLeftOrRight.Right;
                        }
                        else
                        {
                            pos = TableOrLeftOrRightGet(tlr, table, pos);
                            tlr = TableOrLeftOrRight.Left;
                        }
                        i <<= 1;
                    }
                    TableOrLeftOrRightSet(tlr, table, pos, (ushort)j);
                }
                start[k] = (ushort)l;
            }
        }

        enum TableOrLeftOrRight
        {
            Table,
            Left,
            Right
        }

        private void TableOrLeftOrRightSet(TableOrLeftOrRight tlr, ushort[] table, int pos, ushort v)
        {
            switch (tlr)
            {
                case TableOrLeftOrRight.Table:
                    table[pos] = v;
                    return;
                case TableOrLeftOrRight.Left:
                    left[pos] = v;
                    return;
                case TableOrLeftOrRight.Right:
                    right[pos] = v;
                    return;
            }
        }

        private ushort TableOrLeftOrRightGet(TableOrLeftOrRight tlr, ushort[] table, int pos)
        {
            switch (tlr)
            {
                case TableOrLeftOrRight.Table:
                    return table[pos];
                case TableOrLeftOrRight.Left:
                    return left[pos];
                case TableOrLeftOrRight.Right:
                    return right[pos];
            }
            System.Diagnostics.Debug.Assert(false);
            return 0;
        }

        #region bitio
        private ushort bitbuf;
        private byte subbitbuf, bitcount;

        private ushort GetBitbuf()
        {
            return bitbuf;
        }

        /// <summary>
        /// Shift bitbuf n bits left, read n bits
        /// </summary>
        private void FillBuf(int n)
        {
            while (n > bitcount)
            {
                n -= bitcount;

                bitbuf = (ushort)((bitbuf << bitcount) + (subbitbuf >> (8 - bitcount)));
                ////????
                if (packedDataPos >= packedData.Length)
                {
                    packedDataPos++;
                    subbitbuf = 0;
                }
                else
                    subbitbuf = packedData[packedDataPos++]; // ReadRawOneByte();
                bitcount = 8;
            }
            bitcount -= (byte)n;
            bitbuf = (ushort)((bitbuf << n) + (subbitbuf >> (8 - n)));
            subbitbuf <<= n;
        }

        private ushort GetBits(int n)
        {
            int x = bitbuf >> (2 * 8 - n);
            FillBuf(n);
            return (ushort)x;
        }

        private void InitGetBits()
        {
            bitbuf = 0;
            subbitbuf = 0;
            bitcount = 0;
            FillBuf(2 * 8);
        }

        private ushort PeekBits(int n)
        {
            return (ushort)(bitbuf >> (2 * 8 - (n)));
        }
        #endregion

        #endregion
		#region encoding
		
		private const String LhaExec = "lha";
        private const int round = 4;
		
		public static byte[] LHAEncode (LzhCompressMethodType type,byte[] input){
            /*
			String LhaTmpIn = Path.GetTempFileName();
			String LhaTmpOut = Path.GetTempFileName();
            */
            String LhaTmpIn = "tmpin";
            String LhaTmpOut = "tmpout.lzh";
			String opts = " a0";
            
			switch (type)
            {
                case LzhCompressMethodType.LH4:
                    // not supported
                    break;
                case LzhCompressMethodType.LH5:
                    opts += "o5";
                    break;
                case LzhCompressMethodType.LH6:
                    opts += "o6";
                    break;
                case LzhCompressMethodType.LH7:
                    opts += "o7";
                    break;
            }
            
			FileStream infile = new FileStream(LhaTmpIn,FileMode.Create);
			infile.Write(input,0,input.Length);
			infile.Close();
			Utils.SystemCall(LhaExec + opts + " " + LhaTmpOut + " " + LhaTmpIn);
			BinaryReader br = new BinaryReader(new FileStream(LhaTmpOut,FileMode.Open));
			br.BaseStream.Position = 0;
			byte headersize = br.ReadByte();
			br.BaseStream.Position = 7;
			int complength = br.ReadInt32();
			br.BaseStream.Position = headersize + 2;
            byte[] output;
            output = new byte[Utils.RoundUp(complength,round) + round];
            br.Read(output,0,complength);
			br.Close();
			File.Delete(LhaTmpIn);
			File.Delete(LhaTmpOut);
			return output;
		}
		#endregion

    }
}
