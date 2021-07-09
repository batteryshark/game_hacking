'''
Mercurysteam utilty - extract/repack .pack 1.0 (Castlevania: Mirror of Fate)
            2014 rFx
'''
import os,sys,struct

'''
    Packfile notes:
    0x00 0x4 Ftable size
    0x04 0x4 data size after ftable
    0x08 0x04 number of files 

    File Entry -- 1032 bytes (0x408) - basically 1024 bytes for filename and 8 bytes for beg_end
    0x00 0x?? File path cstr()
    0x400 0x4 File begin
    0x404 0x4 File end
    

'''

def usage():
    print("%s d/c target.pack " % sys.argv[0])
    exit(1)


def dump_packfile(inpath):
    file_table = []
    f = open(inpath,"rb")
    ftable_sz = struct.unpack("<I",f.read(4))[0]
    data_sz = struct.unpack("<I",f.read(4))[0]
    num_files = struct.unpack("<I",f.read(4))[0]

    #Populate File Table
    for i in range(0,num_files):
        file_entry={"path":"","offset":0,"size":0}
        file_entry["path"] = f.read(1024).split("\x00")[0]
        file_entry["offset"] = struct.unpack("<I",f.read(4))[0]
        file_entry["size"] = struct.unpack("<I",f.read(4))[0] - file_entry["offset"]
        file_table.append(file_entry)

    #Make Root Directory
    root_dir,root_ext = os.path.splitext(inpath)
    if(not os.path.exists(root_dir)):
        os.makedirs(root_dir)

    #Process File table
    for cf in file_table:
        parent_dir,fname = os.path.split(cf["path"])
        if(parent_dir != ""):
            if(not os.path.exists(os.path.join(root_dir,parent_dir))):
                os.makedirs(os.path.join(root_dir,parent_dir))
        g = open(os.path.join(root_dir,cf["path"]),"wb")
        f.seek(cf["offset"],0)
        g.write(f.read(cf["size"]))
        g.close()

    f.close()

def create_packfile(outpath):
	file_table = []
    #Running total of data size
	data_sz = 0
	for dirpath, dnames, fnames in os.walk(outpath):
		for f in fnames:
			file_entry={"path":"","size":0}
			file_entry["path"] = os.path.join(dirpath, f)
			file_entry["size"] = os.path.getsize(file_entry["path"])
			data_sz += file_entry["size"]
			file_table.append(file_entry)
	#Create new .pack file.
	base,dirname = os.path.split(outpath)
	g = open(dirname+".pack","wb")
	# Write size of file table + 8
	g.write(struct.pack("<I",(len(file_table) * 1032)+8))
	# Write size of data
	g.write(struct.pack("<I",data_sz))
	#Write number of files
	g.write(struct.pack("<I",len(file_table)))




	#Virtual offset for constructing the table
	#This is where the files will eventually be written.
	voffset = (len(file_table) * 1032) + 12
	
	#Add each file to the pack.	We mIGHT need 64-68 bytes of padding, but I'm not sure.
	for cf in file_table:
		f_entry = cf["path"]
		pbs,parent_root = os.path.split(outpath)
		f_entry = f_entry.replace(parent_root+"/","")
		f_entry += bytearray(1024 - len(f_entry))
		f_entry += struct.pack("<I",voffset)
		voffset += cf["size"]
		f_entry += struct.pack("<I",voffset)
		#Write File table to file.
		g.write(f_entry)
	

	for cf in file_table:
		f = open(cf["path"],"rb")
		g.write(f.read())
		f.close()
	g.close()
	print("Done!")

if(__name__=="__main__"):
    print("Mercury Steam .PACK Utility")
    if(len(sys.argv) < 3):
        usage()
    if(sys.argv[1] != "d" and sys.argv[1] != "c"):
            usage()
    if(sys.argv[1] == "d"):
        if(not os.path.exists(sys.argv[2])):
            usage()
        dump_packfile(sys.argv[2])
    if(sys.argv[1] == "c"):
            create_packfile(sys.argv[2])
