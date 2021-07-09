import os,sys,struct
'''
247 0xF7
198 0xC6
123 0x7B

rgb = (r << 11) | (g << 5) | b;

505856 | 6336 | 123
'''
def rgb888torgb565(r,g,b):
	tmp = r
	r = b
	b = tmp
	return ( (((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3) )
	


print("%02X" % rgb888torgb565(0xF7,0xC6,0x7B))