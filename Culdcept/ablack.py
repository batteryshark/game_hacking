'''
Audio Blackout blacks out ps2 audio files

Finds an Audio file header, reads the size, prints out offset and size
'''

import os,sys,struct,re

aud_files = {}
aud_sz = []
aud_offsets = []
infile = open(sys.argv[1],"rb")
indata = infile.read()
offset = 0
size = 0
infile.close()

aud_offsets = [m.start() for m in re.finditer(b'\x00\x22\x56', indata)]
for offset in aud_offsets:
	aud_sz.append(struct.unpack("<H",indata[offset+3:offset+5])[0])

for i in range(0,len(aud_offsets)):
	aud_files[aud_offsets[i]] = aud_sz[i]



curstart = 0
inv_index = []
for i in range(0,len(aud_offsets)):
	inv_index.append((curstart,aud_offsets[i]))
	curstart = aud_offsets[i] + aud_sz[i]
	
outctr = 0
for invfile in inv_index:
	outctr+=1
	outfile = open("data_%d.bin" % outctr,"wb")
	outfile.write(indata[invfile[0]:invfile[1]])
	outfile.close()


'''
curptr = 0
lastptr = 0
for i in range(0,len(indata)):
	if i in aud_offsets:
		lastptr = curptr
		curptr = i
		print("Last: %04X Cur: %04X" % (lastptr,curptr))
		i = (aud_files[i] - 1)

indata = ""

fcounter = 0
lastoff = 0
f = open(sys.argv[1],'rb')
try:
    byte = f.read(1)
    while byte != "":
        if(f.tell() in aud_offsets):
			fcounter+=1
			curptr = f.tell()
			f.seek(lastoff,0)
			outdata = f.read(curptr-lastoff)
			outfile = open("output."+str(fcounter),"wb")
			outfile.write(outdata)
			outfile.close()
			f.seek(curptr+aud_files[curptr],0)
			lastoff = f.tell()
        byte = f.read(1)
finally:
    f.close()

aud_files[aud_offsets[i]]
'''