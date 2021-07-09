import os,sys,flz,struct



def extract_bon3(indata):
	if(not indata.startswith("MISCEVENTBOX")):
		print("ERR - not a EVB File!")
		exit(1)

	indata = indata[0x14:]

	return flz.FLZ(indata).uncompressed_data


if(__name__=="__main__"):
	try:
		f = open(sys.argv[1],'rb')
	except:
		print("Usage %s infile" % sys.argv[0])
		exit(1)

	data = f.read()
	f.close()
	data = extract_bon3(data)

	with open("out.bin",'wb') as g:
		g.write(data)
		