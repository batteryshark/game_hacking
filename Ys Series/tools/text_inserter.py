# Ys Celceta Text Inserter

import os,sys

tl = open('Translation_cvrt.csv','rb')
lines = tl.readlines()
tl.close()

# Eat header
lines = lines[1:]
ctr = 0
total = len(lines)
for line in lines:
	ctr +=1
	
	line = line[:-2] # Get rid of 0x0d0a
	ele = line.split("\t")
	file_path = ele[0]
	
	lno = int(ele[1]) - 1
	print("(%d/%d)Patching %s Line:%d" % (ctr,total,file_path,lno))
	rtxt_start = line.find(ele[2])
	rtxt = line[rtxt_start:]
	# Moment of Truth
	f = open(file_path,'rb')
	olines = f.readlines()
	f.close()
	olines[lno] = rtxt+"\n"
	if(not olines[lno].startswith(ele[2])):
		print("FUCK!")
		exit(1)
	f = open(file_path,'wb')
	for ol in olines:
		f.write(ol)
	f.close()
