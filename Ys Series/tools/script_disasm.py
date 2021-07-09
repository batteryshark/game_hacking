# Falcom Script Disassembler for YSC
# Brought to you by Professor BatteryShark

import os,sys,struct
import csv
import opcodes
import dpostproc

OPCODE_SZ = 2

branch_lst = []
# Todo - Add offset table to support negative Jump values.
offset_lst = []
def dis_function(f,fsz):
	
	instruction_lst = []
	while f.tell() < fsz:
		
		if(f.tell() in branch_lst):
			instruction_lst.append("LABEL_%d" % f.tell())
		offset_lst.append({'offset':f.tell(),'instr_index':len(instruction_lst)})
		copcode = struct.unpack("<H",f.read(OPCODE_SZ))[0]
		"""
		print("----------------")
		for inst in instruction_lst:
			print(inst)
		print("----------------")
		"""
		if(copcode not in opcodes.OPCODE_DB):
			for inst in instruction_lst:
				print(inst)
			print("UNKNOWN OPCODE: %#x" % copcode)
			exit(1)
		opcode_data = opcodes.OPCODE_DB[copcode]
		operands = opcode_data['handler'](f,copcode)
		if(opcodes.OPCODE_DB[copcode]['name'] in opcodes.BRANCH_INSTRUCTIONS):
			jmp_offset = int(operands[-1]) + f.tell()

			operands = operands[:-1]
			original_offset = jmp_offset
			if(original_offset < f.tell()):
				etosplice = -1
				for oe in offset_lst:
					if(oe['offset'] == original_offset):
						etosplice = oe['instr_index']
				if(etosplice == -1):
					print("Fail! Couldn't find the jmpback offset")
					exit(1)
				else:
					#Add the instruction in.
					instruction_lst.insert(etosplice,"LABEL_%d" % original_offset)

			operands.append("GOTO_%d" % jmp_offset)
			branch_lst.append(jmp_offset)
			
			
		
		instruction_lst.append("%s\t%s" % (opcode_data['name'],'\t'.join(operands)))
	return instruction_lst

ASM_HDR_LST = ["Instruction",'Param1','Param2','Param3','Param4','Param5','Param6','Param7','Param8','Param9','Param10','Param11','Param12','Param13','Param14','Param15','Param16']
ASM_HDR = '\t'.join(ASM_HDR_LST)
	
def write_csv(instruction_lst,out_path):
	myfile = open(out_path, 'wb')
	myfile.write(ASM_HDR+"\n")
	for inst in instruction_lst:
		myfile.write(inst+"\n")


if(__name__=="__main__"):
	try:
		infile = sys.argv[1]
	except:
		print("Usage: %s infile" % sys.argv[0])
		exit(1)
	#Test Harness - decompile one function.
	f = open(infile,'rb')
	fname = os.path.split(infile)[1]
	fpath = os.path.split(infile)[0]
	fnx = os.path.splitext(fname)[0]
	out_path = os.path.join(fpath,fnx+'.csv')
	
	func_sz = os.path.getsize(infile)
	instruction_lst = dis_function(f,func_sz)
	for i in range(0,len(instruction_lst)):
		pass # TODO - PostProcessing.
	write_csv(instruction_lst,out_path)
	