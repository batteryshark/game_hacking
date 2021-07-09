import os,sys,struct

f = open('item.tsv','rb')
lines = f.readlines()
out_lines = []
for line in lines:
	
	items = line.split("\t")
	
	for item in items:
		
		item = item.replace("\n","")
		item = item.replace("\xE2\x96\xA0","\xA1\xF6")
		item = item.replace("\xE2\x96\xA1","\xA1\xF5")
		out_lines.append(item)

tbb_header = struct.pack("<I",0x20)

strtab_offsets = []
strtab = ""
strdata = ""


for line in out_lines:
	strtab_offsets.append(len(strdata))
	strdata+=line+"\x00"

for offset in strtab_offsets:
		strtab+=struct.pack("<I",offset)
#Fix the rounding shit for the strtab size.
num_of_padding = 16 - (len(strtab) % 16)
padding = "\x00" * num_of_padding
strtab += padding

#Because >> 7 makes perfect sense in this scenario.
tbb_header+= struct.pack("<I",(len(strtab)) >> 7) + struct.pack("<I",len(strdata))
with open("item.tbb",'wb') as f:
	f.write(tbb_header+strdata+strtab)
	
	

	