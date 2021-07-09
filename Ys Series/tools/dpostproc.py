# Disassembler PostProc
from flags import *

def SetFlag(entry_lst):
	# Param 1 is a flag value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[1]):
		exf = True
	fv = int(entry_lst[1].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[1] = "expr("+FLAG_DB[fv]+")"
	else:
		entry_lst[1] = FLAG_DB[fv]
	return entry_lst
	
def SetWork(entry_lst):
	# Param 2 is a wk value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[2]):
		exf = True
	fv = int(entry_lst[2].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[2] = "expr("+WK_DB[fv]+")"
	else:
		entry_lst[2] = WK_DB[fv]
	return entry_lst
	
def ResetInfoFlag(entry_lst):
	# Param 2 is an INFO_FLAG value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[2]):
		exf = True
	fv = int(entry_lst[2].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[2] = "expr("+INFO_FLAG_DB[fv]+")"
	else:
		entry_lst[2] = INFO_FLAG_DB[fv]
	return entry_lst	
	
def SetChrInfoFlag(entry_lst):
	# Param 2 is an INFO_FLAG value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[2]):
		exf = True
	fv = int(entry_lst[2].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[2] = "expr("+INFO_FLAG_DB[fv]+")"
	else:
		entry_lst[2] = INFO_FLAG_DB[fv]
	return entry_lst

def SetEnvSEPlayState(entry_lst):
	# Param 2 is an ENV_SE value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[2]):
		exf = True
	fv = int(entry_lst[2].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[2] = "expr("+ENV_SE_DB[fv]+")"
	else:
		entry_lst[2] = ENV_SE_DB[fv]
	return entry_lst	
	
def PlaySE(entry_lst):
	# Param 1 is a flag value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[1]):
		exf = True
	fv = int(entry_lst[1].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[1] = "expr("+SE_DB[fv]+")"
	else:
		entry_lst[1] = SE_DB[fv]
	return entry_lst	
	
def PlayBGM(entry_lst):
	# Param 1 is a flag value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[1]):
		exf = True
	fv = int(entry_lst[1].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[1] = "expr("+MUSIC_DB[fv]+")"
	else:
		entry_lst[1] = MUSIC_DB[fv]
	return entry_lst
	
def FadeBGM(entry_lst):
	# Param 1 is a flag value.
	#Strip expr if found.
	exf = False
	if(expr in entry_lst[1]):
		exf = True
	fv = int(entry_lst[1].replace("expr(","").replace(")",""))
	if(exf == True):
		entry_lst[1] = "expr("+MUSIC_DB[fv]+")"
	else:
		entry_lst[1] = MUSIC_DB[fv]
	return entry_lst