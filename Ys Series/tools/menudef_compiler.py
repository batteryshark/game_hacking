import os,sys,struct

f = open('menudef.txt','rb')


lines = f.readlines()
out_lines = []
strdata = ""
strtab_offsets = []
trap_offset_list = []
for line in lines:
	if("[[[TRAP_BYTE]]]" in line):
		#log the offset and come back to it after to write the data.
		strtab_offsets.append(0xDEADBEEF)
		continue

	if(line.endswith("\n")):
		line = line[:-1]
	strtab_offsets.append(len(strdata))
	strdata+=line+"\x00"
		
#fucking 1a
strdata+="\x00"	
trap_len = len(strdata) - 1

	
tbb_header = struct.pack("<I",0x1A)
strtab = ""


for offset in strtab_offsets:
		if(offset == 0xDEADBEEF):
			offset = trap_len
		strtab+=struct.pack("<I",offset)
#Fix the rounding shit for the strtab size.
num_of_padding = 16 - (len(strtab) % 16)
padding = "\x00" * num_of_padding
strtab += padding

with open("menudef2.tbb",'wb') as g:
	g.write(tbb_header+struct.pack("<I",(len(strtab)) >> 4) + struct.pack("<I",len(strdata))+strdata+strtab)