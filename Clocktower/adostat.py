'''
opcode stats for ADO
'''

import os,sys,struct


f = open("CT.ADO","rb")
data = f.read()
f.close()

for opcode in range(0xFF00,0xFFFF):
	opstr = struct.pack("<H",opcode)
	count = data.count(opstr)
	if(count > 0):
		print("""{"opcode":%#4X,"data_len":0},""" % opcode)
	else:
		#print("""{"opcode":%#4X,"data_len":0}, #Not Used""" % opcode)		
		pass