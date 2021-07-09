'''
	ClockTower ADO/ADT Format
			2014 	rFx
'''
import os,sys,struct,binascii
from translate import Translator
ADO_OPCODES = {
#Note: A data_len of -1 denotes special parsing logic required.
0xFF00:0, #Scene Prologue
0xFF01:-1, # --HANDLED
0xFF02:3,
0xFF03:3,
0xFF04:3,
0xFF05:3,
0xFF06:3,
0xFF07:3,
0xFF0A:4, #Looks like an offset to another link in the list.
0xFF0C:4,
0xFF0D:4,
0xFF10:4,
0xFF11:4,
0xFF12:4,
0xFF13:4,
0xFF14:4,
0xFF15:4,
0xFF16:4,
0xFF1F:0, #No Data
0xFF20:0, #No Data -- Only at the end scene (twice) of the ADO
0xFF21:2,
0xFF22:2,
0xFF23:6,
0xFF28:0, #No Data
0xFF29:4,
0xFF2A:4,
0xFF2B:0, #No Data
0xFF2D:2,
0xFF2E:2,
0xFF2F:2,
0xFF30:10,
0xFF32:16, 
0xFF33:-1, #Ingame Text String ;) -- HANDLED
0xFF34:-1, #IDFK -- HANDLED
0xFF36:0,
0xFF37:4,
0xFF39:-1,# -- HANDLED
0xFF3A:-1,# -- HANDLED 
0xFF3B:-1, #can be 0x16 or 0x18 -- Ends with two nulls -- HANDLED
0xFF3C:2,
0xFF3F:0, #No Data
0xFF40:0, #No Data
0xFF41:0, #No Data
0xFF42:0, #No Data
0xFF43:0, #No Data
0xFF46:12,
0xFF47:14,
0xFF48:10,
0xFF49:10,
0xFF4A:0, #No Data
0xFF4B:4,
0xFF4D:14,
0xFF4F:6,
0xFF51:2,
0xFF52:12,
0xFF53:-1, #Two Paths... Sometimes NULL NULL -- HANDLED
0xFF55:16,
0xFF56:2,
0xFF57:0, #No Data
0xFF58:2,
0xFF59:2,
0xFF5A:6,
0xFF5B:8,
0xFF5C:8,
0xFF5D:10,
0xFF5E:0, #No Data
0xFF5F:0, #No Data
0xFF60:0, #No Data
0xFF61:6,
0xFF62:6,
0xFF63:10,
0xFF66:2,
0xFF67:-1, # -- HANDLED
0xFF68:0, #No Data
0xFF69:-1, # -- HANDLED
0xFF6A:0, #No Data
0xFF6B:0, #No Data
0xFF6C:6,
0xFF6D:2,
0xFF6E:0, #No Data
0xFF6F:-1, # -- HANDLED
0xFF71:-1, # -- HANDLED
0xFF72:-1, # -- HANDLED
0xFF73:0, #No Data
0xFF74:12,
0xFF77:8,
0xFF78:0, #No Data
0xFF7A:-1, # -- HANDLED
0xFF7B:-1, # -- HANDLED
0xFF7C:-1, # -- HANDLED
0xFF7D:14,
0xFF80:4,
0xFF81:0x12,
0xFF83:2,
0xFF84:0, #No Data
0xFF85:4,
0xFF86:12,
0xFF87:-1, #This one is used only once - to load the intro AVI file.  -- HANDLED
0xFF88:0, #No Data
0xFFA0:12,
0xFFB0:12,
0xFFDF:2,
0xFFE0:0, #No Data
0xFFEA:0, #No Data
0xFFEF:12
}
class ADOFile:

	def __init__(self,ado_path,adt_path):
		self.ado_path = ado_path
		self.adt_path = adt_path
		self.adt_list = []
		self.adt_adjustments = []
		self.scenes = self.read_scenes()
		
	def read_scenes(self):
		#Get ADT Values
		adt_list = []
		scene_list = []
		scenes = []
		f = open(self.adt_path,"rb")
		data = f.read()
		f.close()
		for i in range(0,len(data),4):
			adt_list.append(self.get_real_offset(struct.unpack("<I",data[i:i+4])[0]))
		data =""
		#Use ADT Values to Read Scenes from ADO.
		f = open(self.ado_path,"rb")
		data = bytearray(f.read())
		f.close()
		for i in range(0,len(adt_list)):

			start_range = adt_list[i]

			#If we're at the last value, fixitfixit.
			if(i == len(adt_list)-1):
				end_range = len(data)
			else:
				end_range = adt_list[i+1]
			#Chunk out each scene into memory.
			scene_list.append(data[start_range:end_range])
		print("%d Scenes Read" % len(scene_list))

		#A Scene is built up of events... each event has an opcode and some data.
		#We need to parse all scenes and process all events in the scene.
		for scene in scene_list:
			scene_parsed = self.disassemble_scene(scene)
			scenes.append(scene_parsed)

		self.adt_list = adt_list

		print("Also, Proc all Opcodes")
		return scenes
	def disassemble_scene(self,data):
		#Parse out every opcode and all data.
		data = str(data)
		scene_parsed = []
		print("Disassembling ADO Data...")
		#print("Disassembling: %s" % binascii.hexlify(data))
		i = 0
		
		opcode = struct.unpack("<H",data[i:i+2])[0]
		if(not opcode in ADO_OPCODES.keys()):
			scene_parsed.append({"opcode":None,"data":data})
			return scene_parsed
		while i < len(data):
			try:
				adt_stub = self.adt_list.index(i+256)
			except:
				adt_stub = -1
			opcode = struct.unpack("<H",data[i:i+2])[0]
			#print("0x%02X" % opcode)
			data_sz = ADO_OPCODES[opcode]
			if(data_sz == -1):
				if(opcode == 0xFF3B or opcode == 0xFF7B or opcode == 0xFF7C):
					data_sz = 2
					#Initial value
					outdata = data[i+2:i+4]


					#Get the ASCII string size.
					a_str_sz = data[i+4:].find('\x00')
					outdata += data[i+4:i+4+a_str_sz+1]
					tail_sz = a_str_sz+4
					
					try:
						if(data[i+tail_sz+2] == '\x00'):
							outdata += data[i+tail_sz:i+tail_sz+2]
						else:
							outdata += data[i+tail_sz:i+tail_sz+1]
					except IndexError:
						outdata += data[i+tail_sz:i+tail_sz+1]
					#print("Size: %d bytes" % len(outdata))
					#print("Data: %s" % binascii.hexlify(outdata))
					scene_parsed.append({"opcode":opcode,"data":outdata,"adt_stub":adt_stub,"offset":i})
					i+=2
					i+=len(outdata)

					continue


				elif(opcode == 0xFF87):
					data_sz = 10
					#Initial value
					outdata = data[i+2:i+12]


					#Get the ASCII string size.
					a_str_sz = data[i+12:].find('\x00')
					outdata += data[i+12:i+12+a_str_sz+1]
					tail_sz = a_str_sz+12
					
					
					if(data[i+tail_sz+2] == '\x00'):
						outdata += data[i+tail_sz:i+tail_sz+2]
					else:
						outdata += data[i+tail_sz:i+tail_sz+1]

					#print("Size: %d bytes" % len(outdata))
					#print("Data: %s" % binascii.hexlify(outdata))
					scene_parsed.append({"opcode":opcode,"data":outdata,"adt_stub":adt_stub,"offset":i})
					i+=2
					i+=len(outdata)
					continue
				elif(opcode == 0xFF33):
					#First 4-byte value.
					outdata = data[i+2:i+6]
					sjis_str_sz = data[i+6:].find('\xff')
					sjis_str_sz -= 1 #Back up 1 byte
					#Put S-JIS String into value.
					outdata += data[i+6:i+6+sjis_str_sz]
					#print("Size: %d bytes" % len(outdata))
					#print("Data: %s" % binascii.hexlify(outdata))
					scene_parsed.append({"opcode":opcode,"data":outdata,"adt_stub":adt_stub,"offset":i})
					i+=2
					i+=len(outdata)
					continue

				elif(opcode == 0xFF7A or opcode == 0xFF34 or opcode == 0xFF6F or opcode == 0xFF01 or opcode == 0xFF71 or opcode == 0xFF72 or opcode == 0xFF53 or opcode == 0xFF67 or opcode == 0xFF39 or opcode == 0xFF69 or opcode == 0xFF3A):
					sjis_str_sz = data[i+2:].find('\xff')
					if(sjis_str_sz == -1):
						sjis_str_sz = len(data[i+2:])
					else:
						sjis_str_sz -= 1 #Back up 1 byte
					#Put Values
					outdata = data[i+2:i+2+sjis_str_sz]
					#print("Size: %d bytes" % len(outdata))
					#print("Data: %s" % binascii.hexlify(outdata))
					scene_parsed.append({"opcode":opcode,"data":outdata,"adt_stub":adt_stub,"offset":i})
					i+=2
					i+=len(outdata)


					continue					
				else:
					continue
			#print("Size: %d bytes" % data_sz)
			#print("Data: %s" % binascii.hexlify(data[i+2:i+2+data_sz])) 
			scene_parsed.append({"opcode":opcode,"data":data[i+2:i+2+data_sz],"adt_stub":adt_stub,"offset":i})
			i+=2
			i+=data_sz 
			

		#print(scene_parsed)
		return scene_parsed
	def translate(self,lang_code):
		#Short-Circut (skip translation)
		#return
		'''
		h = open("xlate_en.txt","rb")
		xlate_data = h.readlines()
		replsdata = {}
		for line in xlate_data:
			offset,newstr = line.split("\t")
			offset = int(offset,0)
			newstr = newstr.replace("[NEWLINE]","\x0a\x00")
			newstr = newstr.replace("[ NEWLINE ]","\x0a\x00")
			replsdata[offset] = newstr
		

		for i in range(0,len(self.scenes)):
			if(self.scenes[i]["opcode"] == 0xFF33):
			
				if(self.scenes[i]["offset"] in replsdata.keys()):
					orig_len = len(self.scenes[i]["data"])
					self.scenes[i]["data"] = self.scenes[i]["data"][:4] + replsdata[self.scenes[i]["offset"]]
					#HACK for LENGTH
					self.scenes[i]["data"] = self.scenes[i]["data"][:orig_len]
					new_len = len(self.scenes[i]["data"])
					str_diff = new_len - orig_len
					#self.adt_adjustments.append({"offset":self.scenes[i]["offset"],"diff_val":str_diff})

		return
		'''
		translator = Translator(to_lang=lang_code)
		for k in range(0,len(self.scenes)):

			for i in range(0,len(self.scenes[k])):

				if(self.scenes[k][i]["opcode"] == 0xFF33):
					strlen = len(self.scenes[k][i]["data"][4:])

					#print("Translating String...")
					t_str = self.scenes[k][i]["data"][4:]
					'''
					#t_str = t_str.replace("\n","|")
					#t_str = t_str.replace("\x00","NULL")
					t_str = t_str.replace("\x0a\x00","[NEWLINE]")
					t_str = t_str.replace("\x00","")
					g.write("%#x\t%s\n" % (self.scenes[i]["offset"],t_str))
					'''
					
					t_str = t_str.decode("SHIFT_JIS").encode("UTF-8")
					#Deal with newlines.
					replacement_str = ""
					#t_str = t_str.replace("\x0a\x00"," NEWLINE ")
					#t_str = t_str.replace("\x00"," NULL ")

					strlist = t_str.split("\x0a\x00")

					for j in range(0,len(strlist)):
						if(strlist[j] != "\x0a\x00" and strlist[j] != ""):
							
						
							ts = translator.translate(strlist[j])
							replacement_str += ts
						
						if(len(strlist) > 1 and j < len(strlist)-1):
							replacement_str+="\x0a\x00"
					'''
					t_str = translator.translate(t_str)
					t_str = t_str.replace(" NULL ","\x00")
					t_str = t_str.replace(" NEWLINE ","\x0a\x00")
					t_str = t_str.replace("NULL","\x00")
					t_str = t_str.replace("NEWLINE","\x0a\x00")				
					#print(replacement_str)
					'''
					#Decode from UTF-8 back to SHIFT-JIS and store.
				
				
					replacement_str = replacement_str.encode("SHIFT_JIS",'replace')

					
					#Test hack - cut the string down so it's not longer than the original (offset hack).
					#if(len(replacement_str) < strlen):
					#	replacement_str+=bytearray(" " * (strlen-len(replacement_str))) 
					#replacement_str = replacement_str[:strlen]
					
					self.scenes[k][i]["data"] = self.scenes[k][i]["data"][:4]+replacement_str
					
				
					#newdata = scene["data"][:4] + t_str.encode("SHIFT-JIS")
					
			#g.close()
	def get_real_offset(self,offset):
		#Get High Value
		high_val = offset & 0xFFFF0000
		high_val /= 2
		low_val = offset & 0xFFFF
		return high_val+low_val
	def get_fake_offset(self,offset):
		#Get High Value
		mult = int(offset / 0x8000)
		shft_val = 0x8000 * mult

		low_val = offset & 0xFFFF

		return offset + shft_val

	def save(self):
		print("Create new ADO/ADT Files and save.")
		adt_data = ""
		#Make ADO Header.
		ado_data = "ADC Object File"+"\x00"+("\xff" * 0xF0)
		adt_offsets = []
		for scene in self.scenes:
			adt_offsets.append(len(ado_data))
			for event in scene:

				ado_data+= struct.pack("<H",event["opcode"])
				ado_data+= event["data"]


		f = open("CT.ADO","wb")
		f.write(ado_data)
		f.close()
		f = open("CT.ADT","wb")
		for offset in adt_offsets:
		
			final_adjustment = 0
			offset = self.get_fake_offset(offset)
				
			f.write(struct.pack("<I",offset))
		f.close()
#Basically, what we need to do is, for every string that we modify, we need to add the event index with how much we inc/dec it by.
#Then, when we build the ado file, we need to say that every opcode after a certain amount has its offset logged to another list with the total
#amount offset. Then, when we build the ADT file, we say that each value is checked in the range of that list and the Values
#get adjusted accordingly. We can eliminate the current method of earmarking indexes that way as well.

#All offsets after X need to be +/- Y. THATS THE QUESTION!

#The offsets at the end are really -0x20000 ... I'm not sure how. for over 0x4 ... anything in the 3k range is -0x18000
#anything in the 2k range is -0x10000 and the 1k range is -0x8000   basically what block it's in.