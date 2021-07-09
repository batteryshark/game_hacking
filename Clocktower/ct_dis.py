'''
 Clocktower Scene Disassembler
 		2014 - rFx
'''

'''
This program does a couple of things:
1. Parses all opcodes from CT.ADO/ADT 
2. Translates all text from ClockTower (PC) to English
'''
import os,sys

#My Custom Imports
import ct_files
import codecs
codecs.register(lambda name: codecs.lookup('utf-8') if name == 'cp65001' else None)



if(__name__ == "__main__"):
	print("Clocktower Translator by rFx")
	ct = ct_files.ADOFile("CT_J.ADO","CT_J.ADT")
	ct.translate("en")
	ct.save()
	exit(0)

