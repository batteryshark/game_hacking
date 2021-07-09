'''
	BFPKTool - A 'packed' format utility for Castlevania: Lords of Shadow 1/2
									2014 - rFx
'''

PROJECT_VERSION = 0.75
V3_AES_KEY = "\x50\x43\x56\x80\x72\x73\xEE\x6F\xF1\x44\xF3\x6E\xEA\xDF\x79\x43\x6C\x69\x6D\x61\x78\x53\x74\x75\x64\x69\x6F\x73\x32\x30\x31\x33"
import os,sys,struct,zlib
from Crypto.Cipher import AES

def usage():
	#TODO - denote version number for create and page size.
	print("Usage: %s d/c target.packed" % sys.argv[0])
	exit(1)

def fatal(msg):
	print(msg)
	exit(1)

def warning(msg):
	print(msg)

BFPK_VERSION = {
	0:"RAW",
	1:"ZLIB",
	2:"MULTI_ZLIB",
	3:"ENCRYPTED"

}


def gen_file_table(f,num_files,version):
	curr_file = 0
	file_table = []
	while curr_file < num_files:
		#Read size of file path.
		
		fname_sz = struct.unpack("<I",f.read(4))[0]
		
		fname = f.read(fname_sz)
		
		file_size = struct.unpack("<I",f.read(4))[0]
		file_offset = struct.unpack("<I",f.read(4))[0]
		if(version != "ENCRYPTED" and version != "ENCRYPTED_RAW"):
			deleted = struct.unpack("<I",f.read(4))[0]
		else:
			deleted = 0
		curr_file +=1
		file_table.append({"path":fname,"offset":file_offset,"size":file_size,"deleted":deleted})
	return file_table

def recover(file_table,f,start_offset):
	total_sz = os.path.getsize(sys.argv[2])
	f.seek(start_offset,0)
	file_index = 0
	while (f.tell() < total_sz):
		curr_file = ""
		for cf in file_table:
			if(cf["offset"] == f.tell()):
				curr_file = cf["path"]
				#print(curr_file)
				break
		if(curr_file == ""):
			fsz = struct.unpack("<I",f.read(4))[0]
			if(fsz == 0):
				continue
			print("RECOVERABLE FILE FOUND!")
			
			file_index += 1
			g = open("%d.dat" % file_index,"wb")
			try:
				g.write(zlib.decompress(f.read(fsz)))
			except:
				pass
			g.close()
			continue
		fsz = struct.unpack("<I",f.read(4))[0]
		f.seek(fsz,1)
		#print(f.tell())
			


def dump_packed_file(inpath):
	print("Dumping %s..." % inpath)
	f = open(inpath,"rb")
	magic = f.read(4)
	
	if(magic != "BFPK"):
		fatal("%s is not a valid BPFK file." % inpath)
	
	version = struct.unpack("B",f.read(1))[0]
	
	if(not version in BFPK_VERSION.keys()):
		fatal("%s has an unsupported archive version." % inpath)
	
	version = BFPK_VERSION[version]
	version_alt = struct.unpack("B",f.read(1))[0]
	if(version == "MULTI_ZLIB" and version_alt == 0):
		version = "ENCRYPTED_RAW"
	padding = f.read(2)
	if(version == "ENCRYPTED" or version == "ENCRYPTED_RAW"):
		#Decrypt File Table
		ftable_sz = struct.unpack("<I",f.read(4))[0]
		ftable_iv = f.read(16)
		file_table_bin = f.read(ftable_sz)
		aes = AES.new(V3_AES_KEY, AES.MODE_CBC, ftable_iv)
		file_table_bin = aes.decrypt(file_table_bin)
		num_files = struct.unpack("<I",file_table_bin[:4])[0]

		#For Now, make a tmp file that we can use to construct the ftable.
		g = open("ft.tmp","wb")
		g.write(file_table_bin[4:])
		g.close()
		g = open("ft.tmp","rb")
		file_table = gen_file_table(g,num_files,version)
		g.close()
		
	else:
		if(version == "MULTI_ZLIB"):
			max_page_size = struct.unpack("<I",f.read(4))[0]
		num_files = struct.unpack("<I",f.read(4))[0]
		file_table = gen_file_table(f,num_files,version)
	#Make Root Directory
	root_dir,root_ext = os.path.splitext(inpath)
	if(not os.path.exists(root_dir)):
		os.makedirs(root_dir)
	#Recovery Test	
	'''
	recover(file_table,f,file_table[0]["offset"])
	exit(0)
	'''
	
	#Process File table
	for cf in file_table:
		print("%s" % cf["path"])
		#This makes the entry all fucked up and not point to data...
		if(cf["deleted"] == 1):
			
			continue
		parent_dir,fname = os.path.split(cf["path"])
		if(parent_dir != ""):
			if(not os.path.exists(os.path.join(root_dir,parent_dir))):
				os.makedirs(os.path.join(root_dir,parent_dir))
		g = open(os.path.join(root_dir,cf["path"]),"wb")
		f.seek(cf["offset"],0)
		if(version == "RAW" or version == "ENCRYPTED_RAW"):
			g.write(f.read(cf["size"]))
		if(version == "ZLIB" or version == "ENCRYPTED"):
			comp_sz = struct.unpack("<I",f.read(4))[0]
			g.write(zlib.decompress(f.read(comp_sz)))				
		if(version == "MULTI_ZLIB"):
			total_chunks_sz = struct.unpack("<I",f.read(4))[0]
			curr_offset = 0
			while(curr_offset < total_chunks_sz):
				comp_sz = struct.unpack("<I",f.read(4))[0]
				g.write(zlib.decompress(f.read(comp_sz)))
				#We can't count the compressed size value.
				curr_offset += (comp_sz + 4)


		g.close()

	f.close()        

if(__name__=="__main__"):
	print("BFPKTool v %s" % PROJECT_VERSION)
	if(len(sys.argv) != 3):
		usage()
	if(not os.path.exists(sys.argv[2])):
		usage()
	opcode = sys.argv[1]
	if(opcode != "d" and opcode != "c"):
		usage()

	inpath = sys.argv[2]
	if(opcode == "d"):
		dump_packed_file(inpath)
	if(opcode == "c"):
		pass
