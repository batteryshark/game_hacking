"""
	Puyo Quest Arcade DB Extractor
"""

"""
FORMAT:
uint32 num_entries

Everything after this depends on the db - some have strlen+string and 4 entries,5,6,7 some just have datasets, no real way to delimit data... the game might have some kind of table to interpret the data.

But it generally goes.
uint32 index value (row index)
data ...

PqTelopMsgTableRecord (8 items per row (including size headers)
Index,
strlen_1
str_1
strlen_2
str_2
strlen_3
str_3
someval
"""

import os,sys,struct


def process_pqtelopmsgtablerecord(in_path):
	f = open(in_path,"r")
	lines = f.readlines()
	line_index = 0
	data = ""
	# Cut off header.
	lines = lines[1:]
	for line in lines:
		if(line == "\r" or line == "\r\n" or line == "\n"):
			continue
		
		line = line.replace("\r","").strip().rstrip().replace("\n","")
		title,desc,text,num_args = line.split("\t")

		num_args = int(num_args)
		data += struct.pack("<I",line_index)
		
		data += struct.pack("<I",len(title))
		data += title

		data += struct.pack("<I",len(desc))
		data += desc
		
		data += struct.pack("<I",len(text))
		data += text
		
		data += struct.pack("<I",num_args)
				
		line_index+=1
	
	data = struct.pack("<I",line_index)+data
	return data

def usage():
	print("Usage: %s in_db_file.csv" % sys.argv[0])
	exit(-1)
	
if(__name__ == "__main__"):
	if(len(sys.argv) < 2):
		usage()
	
	if(not os.path.exists(sys.argv[1])):
		usage()
	
	in_path = sys.argv[1]
	if("PqTelopMsgTableRecord" in in_path):
		data = process_pqtelopmsgtablerecord(in_path)
		
	with open(in_path.replace(".csv",".bin"),"wb") as g:
		g.write(data)
		