import os,sys,struct,binascii
from translate import Translator


infile = open("xlate.txt","rb")
lines = infile.readlines()
translator = Translator(to_lang="en")
outfile = open("xlate_en.txt","wb")

for line in lines:
	offset_str,t_str = line.split("\t")
	t_str = t_str.decode("SHIFT_JIS").encode("UTF-8")
	t_str = translator.translate(t_str).encode("SHIFT_JIS")
	print(t_str)
	outfile.write("%s\t%s\n" % (offset_str,t_str))
outfile.close()