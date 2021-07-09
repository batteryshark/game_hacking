
import os,sys

num = 0xFFBE676F

print("%04X" % ((((num>>16)&0x8000 | (num>>9)&0x7C00 | (num>>6)&0x03E0 | (num>>3)&0x1F))))