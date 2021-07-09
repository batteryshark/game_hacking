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
	entries = []
	f = open(in_path,"rb")
	header = "Title\tDescription\tText\tNum_Args"
	num_entries = struct.unpack("<I",f.read(4))[0]
	print("Number of Entries: %d" % num_entries)
	for i in range(0,num_entries):
		row_id = struct.unpack("<I",f.read(4))[0]
		strlen_1 = struct.unpack("<I",f.read(4))[0]
		str_1 = f.read(strlen_1)
		strlen_2 = struct.unpack("<I",f.read(4))[0]
		str_2 = f.read(strlen_2)
		strlen_3 = struct.unpack("<I",f.read(4))[0]
		str_3 = f.read(strlen_3)
		someval = struct.unpack("<I",f.read(4))[0]
		entries.append("%s\t%s\t%s\t%d" % (str_1,str_2,str_3,someval))
	return entries,header

def usage():
	print("Usage: %s in_db_file.bin" % sys.argv[0])
	exit(-1)
	
if(__name__ == "__main__"):
	if(len(sys.argv) < 2):
		usage()
	
	if(not os.path.exists(sys.argv[1])):
		usage()
	
	in_path = sys.argv[1]
	if("PqTelopMsgTableRecord" in in_path):
		data,header = process_pqtelopmsgtablerecord(in_path)
		
	with open(in_path.replace(".bin",".csv"),"wb") as g:
		g.write(header+"\n")
		for line in data:
			g.write(line+"\n")
		