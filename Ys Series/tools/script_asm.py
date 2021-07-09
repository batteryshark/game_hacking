# Falcom Script Assembler for YSC
# Brought to you by Professor BatteryShark
import os,sys,csv,struct
import opcodes

JMP_LST = []
LABEL_DB = {}
def asm_function(instruction_lst):
	
	function_data = ""
	#Construct Inverse Key Lookup for Opcode DB
	rodb = {}
	for ok in opcodes.OPCODE_DB.keys():
		rodb[opcodes.OPCODE_DB[ok]['name']] = {'bin':struct.pack("<H",ok),'params':opcodes.OPCODE_DB[ok]['params']}
	lbl_ctr = 0
	for entry in instruction_lst:
		if(entry.startswith("#")):
			continue # Skip Comments
		#print(entry)
		elements = entry.split("\t")
		if("LABEL_" in elements[0]):
			LABEL_DB[elements[0]] = len(function_data)
			continue
		elif(elements[0] not in rodb.keys()):
			print("UNKNOWN Instruction: %s" % elements[0])
			exit(1)
		function_data+=rodb[elements[0]]['bin']
		params = elements[1:rodb[elements[0]]['params']+1]
		function_data += opcodes.get_binargs(params)
		if(elements[-1].startswith("GOTO_")):
			gta = elements[-1].replace("GOTO_","")
			JMP_LST.append({'offset':len(function_data),'label':"LABEL_%s" % gta})
	
			
				
	
	return function_data


if(__name__=="__main__"):
	try:
		infile = sys.argv[1]
	except:
		print("Usage: %s infile" % sys.argv[0])
		exit(1)
	out_path,fname = os.path.split(infile)
	fname = fname.replace(".csv",".bin")
	#Test Harness - compile one function.
	f = open(infile,'rb')
	with open(infile, 'rb') as f:
		
		instruction_lst = f.readlines()[1:]
		
	for i in range(0,len(instruction_lst)):
		instruction_lst[i] = instruction_lst[i].rstrip('\n')
		pass # TODO - PreProcessing.
	
	function_data = asm_function(instruction_lst)
	# Restructure branch instructions.
	function_data = bytearray(function_data)
	for jmp in JMP_LST:
		label_entry = LABEL_DB[jmp['label']]
		rel_offset =  label_entry - jmp['offset']
		rel_offset_bytes = struct.pack("<i",rel_offset)
		function_data[jmp['offset']-4:jmp['offset']] = rel_offset_bytes

		
	outpath = os.path.join(out_path,fname)
	with open(outpath,'wb') as g:
		g.write(function_data)
	
	