import os,sys,struct

class DatFile(object):
	def __init__(self, file_path):
		self.dat_path = file_path
		self.dat_name = os.path.splitext(file_path)[0]
		self.file_db = self.get_file_db()
		
		
	def get_file_db(self):
		file_db = {}
		with open(self.dat_path,'rb') as f:
			# Get Number of files.
			num_files = struct.unpack("<I",f.read(4))[0]
			f.seek(16,0) # skip 12 byte padding (for alignment)
			cur_entry = 0
			
			while cur_entry < num_files:
				cf_name = f.read(16).split("\x00")[0]
				file_db[cf_name] = {
				"offset":struct.unpack("<I",f.read(4))[0],
				"size":struct.unpack("<I",f.read(4))[0],
				"padding1":struct.unpack("<I",f.read(4))[0],
				"padding2":struct.unpack("<I",f.read(4))[0]
				}
				cur_entry +=1
		return file_db
	def extract(self,path="."):
		out_path = os.path.join(path,self.dat_name)
		if(not os.path.exists(out_path)):
			os.makedirs(out_path)
		with open(self.dat_path,'rb') as f:
			for cf in self.file_db.keys():
				f.seek(self.file_db[cf]['offset'],0)
				data = f.read(self.file_db[cf]['size'])
				g = open(os.path.join(out_path,cf),'wb')
				g.write(data)
				g.close()
		
f = DatFile("1stload.dat")
f.extract()
		