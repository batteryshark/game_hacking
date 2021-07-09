# Falcom LZ Library
import os,struct
import flzss
class FLZ(object):
	def __init__(self,data):
		self.uncompressed_data = self.decompress_groups(data)
		
	def decompress_groups(self,data):
		offset = 0
		uncompressed_data = ""
		data_sz = len(data)
		
		while len(data):

			group_size = struct.unpack("<I",data[0:4])[0]
			#HACK - will fix later.
			if(group_size < 3):
				return uncompressed_data
			data = data[4:]
			tmp = FLZG(data[:group_size])
			uncompressed_data += tmp.uncompressed_data

			data = data[group_size:]
			offset+= group_size
		return uncompressed_data
		
		
class FLZG(object):
	def __init__(self,data):
		self.group_size = len(data)
		print("Compressed group size: %x bytes" % self.group_size)
		self.uncompressed_size = struct.unpack("<I",data[:4])[0]
		print("uncompressed size of group: %d bytes" % self.uncompressed_size)
		self.comp_level = struct.unpack("<I",data[4:8])[0]
		data = data[8:]
		self.uncompressed_data = self.uncompress_data(data)

		
		
		
	def uncompress_data(self,indata):
		outdata = ""
		print("Compressed chunk size : %x bytes" % len(indata))
		while len(outdata) < self.uncompressed_size:
			
			chunk_sz = struct.unpack("<H",indata[:2])[0]
			#NOM NOM NOM 
			indata = indata[3:]
			chunk_sz -= 2

						
			z_chunk = indata[:chunk_sz]
			z_chunk = bytearray(z_chunk)
			print("LENGTH %x" % len(z_chunk))
			indata = indata[chunk_sz:]
			outdata += unlz(z_chunk)
			
		return outdata
		
def unlz(data):
	outdata = flzss.decompress_block(data)
	return outdata
	
	
if(__name__=="__main__"):
	f = open('logo.itp','rb')
	indata = f.read()
	f.close()
	indata = indata[16:]
	flzo = FLZ(indata)
	with open('out.bin','wb') as f:
		f.write(flzo.uncompressed_data)
	
		
	
	
