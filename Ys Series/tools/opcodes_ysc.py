import struct


def RepresentsInt(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False

def RepresentsFloat(s):
	if(s.endswith("f")):
		s = s[:-1]
	try:
		float(s)
		return True
	except ValueError:
		return False
		
		
DTYPE_DB = {
	"\x01\x00":{'name':'"!="','datasize':0,'type':'conditionalopr'},
	"\x02\x00":{'name':'"!"','datasize':0,'type':'conditionalopr'},
	"\x03\x00":{'name':'"*"','datasize':0,'type':'conditionalopr'},
	"\x04\x00":{'name':'"/"','datasize':0,'type':'conditionalopr'},
	"\x05\x00":{'name':'"%"','datasize':0,'type':'conditionalopr'},
	"\x06\x00":{'name':'"+"','datasize':0,'type':'conditionalopr'},
	"\x07\x00":{'name':'"-"','datasize':0,'type':'conditionalopr'},
	"\x08\x00":{'name':'">>"','datasize':0,'type':'conditionalopr'},
	"\x09\x00":{'name':'">="','datasize':0,'type':'conditionalopr'},
	"\x0A\x00":{'name':'">"','datasize':0,'type':'conditionalopr'},
	"\x0B\x00":{'name':'"<<"','datasize':0,'type':'conditionalopr'},
	"\x0C\x00":{'name':'"<="','datasize':0,'type':'conditionalopr'},
	"\x0D\x00":{'name':'"<"','datasize':0,'type':'conditionalopr'},
	"\x0E\x00":{'name':'"=="','datasize':0,'type':'conditionalopr'},
	"\x0F\x00":{'name':'"="','datasize':0,'type':'conditionalopr'},
	"\x10\x00":{'name':'"&&"','datasize':0,'type':'conditionalopr'},
	"\x11\x00":{'name':'"&"','datasize':0,'type':'conditionalopr'},
	"\x12\x00":{'name':'"||"','datasize':0,'type':'conditionalopr'},
	"\x13\x00":{'name':'"|"','datasize':0,'type':'conditionalopr'},
	"\x14\x00":{'name':'"~"','datasize':0,'type':'conditionalopr'},
	"\x15\x00":{'name':'"^"','datasize':0,'type':'conditionalopr'},
	"\x1A\x00":{'name':'lddwrd','datasize':4,'type':'int'},
	"\x1B\x00":{'name':'ldflt','datasize':4,'type':'float'},
	"\x1C\x00":{'name':'ldstr0','datasize':4,'type':'float'},
	"\x1D\x00":{'name':'endexp','datasize':0,'type':'null'},
	"\x1F\x00":{'name':'FLAG','datasize':0,'type':'conditionalopr'},
	"\x20\x00":{'name':'WORK','datasize':0,'type':'conditionalopr'},
	"\x21\x00":{'name':'CHRWORK','datasize':0,'type':'conditionalopr'},
	"\x22\x00":{'name':'ITEMWORK','datasize':0,'type':'conditionalopr'},
	"\x23\x00":{'name':'ALLITEMWORK','datasize':0,'type':'conditionalopr'},
	"\x24\x00":{'name':'TEMP','datasize':0,'type':'conditionalopr'},
	"\x25\x00":{'name':'abs','datasize':0,'type':'conditionalopr'},
	"\x26\x00":{'name':'INT','datasize':0,'type':'conditionalopr'},
	"\x27\x00":{'name':'FLOAT','datasize':0,'type':'conditionalopr'},
	"\x28\x00":{'name':'ACTIONFLAG','datasize':0,'type':'conditionalopr'},
	"\x29\x00":{'name':'rand','datasize':0,'type':'conditionalopr'},
	"\x2A\x00":{'name':'randf','datasize':0,'type':'conditionalopr'},
	"\x2B\x00":{'name':'SETFLAG_TIME','datasize':0,'type':'conditionalopr'},
	"\x2C\x00":{'name':'ldstr','datasize':4,'type':'strlen'},
	"\x2D\x00":{'name':'UNK_0x2D','datasize':0,'type':'conditionalopr'},
	"\x30\x00":{'name':'GetNo','datasize':0,'type':'conditionalopr'},
	"\x31\x00":{'name':'IsChange','datasize':0,'type':'conditionalopr'},
	"\x32\x00":{'name':'IsKey','datasize':0,'type':'conditionalopr'},
	"\x33\x00":{'name':'IsMoving','datasize':0,'type':'conditionalopr'},
	"\x34\x00":{'name':'IsPlayer','datasize':0,'type':'conditionalopr'},
	"\x35\x00":{'name':'IsPartyIn','datasize':0,'type':'conditionalopr'},
	"\x36\x00":{'name':'IsGuestOK','datasize':0,'type':'conditionalopr'},
	"\x37\x00":{'name':'NextAnimation','datasize':0,'type':'conditionalopr'},
	"\x38\x00":{'name':'PrevAnimation','datasize':0,'type':'conditionalopr'},
	"\x39\x00":{'name':'IsWalking','datasize':0,'type':'conditionalopr'},
	"\x3A\x00":{'name':'IsEquip','datasize':0,'type':'conditionalopr'},
	"\x3B\x00":{'name':'GetSpeedRatio','datasize':0,'type':'conditionalopr'},
	"\xDD\x82":{'name':'lddwrdp','datasize':4,'type':'int'},
	"\xDE\x82":{'name':'ldfltp','datasize':4,'type':'float'},
	"\xDF\x82":{'name':'ldstrp','datasize':4,'type':'strlen'},
	"\xE0\x82":{'name':'expr','datasize':4,'type':'expr'},
}
		
DTYPE_BYNAME_DB = {}
for dk in DTYPE_DB.keys():
	DTYPE_BYNAME_DB[DTYPE_DB[dk]['name']] = dk
# Return the binary representation of the param list.
def get_binargs(params):
	bdata = ""
	for p in params:
		if(not "expr(" in p):
			if(p.endswith("f") and RepresentsFloat(p)):
				p = p[:-1]
				bdata += "\xDE\x82"
				bdata += struct.pack("f",float(p))
			elif(p in DTYPE_BYNAME_DB.keys()):
				bdata += DTYPE_BYNAME_DB[p]
				
			elif(RepresentsInt(p)):
				bdata += "\xDD\x82"
				bdata += struct.pack("<i",int(p))
			elif("GOTO_" in p):
				bdata += "\xDD\x82\x00\x00\x00\x00"
					
			else:
				bdata += "\xDF\x82"
				p = p.replace("[tab]","\x09")
				p = p.replace("sz__","")
				if(p == "[szNULL]"):
					p = p.replace("[szNULL]","")
				bdata += struct.pack("<I",len(p))
				bdata += p
		else:
			p = p.replace("expr(","").replace(")","")
			bdata +="\xE0\x82"
			edata = ""
			if(not "," in p):
				if(p.endswith("f") and RepresentsFloat(p)):
					p = p[:-1]
					edata+= "\x1B\x00"
					edata+= struct.pack("f",float(p))
				elif(RepresentsInt(p)):
					edata += "\x1A\x00"
					edata += struct.pack("<i",int(p))					
			else:
				vals = p.split(",")

				for v in vals:
					if(v in DTYPE_BYNAME_DB.keys()):
						edata += DTYPE_BYNAME_DB[v]
					elif("GOTO_" in p):
						edata += "\xDD\x82\x00\x00\x00\x00"
					elif(v.endswith("f") and RepresentsFloat(v)):
						v = v[:-1]
						edata+= "\x1B\x00"
						edata+= struct.pack("f",float(v))
					elif(RepresentsInt(v)):
						edata += "\x1A\x00"
						edata += struct.pack("<i",int(v))
					elif(not v.endswith('f') and not RepresentsInt(v)):
						edata+= "\x2C\x00"
						edata+= struct.pack("<I",len(v))
						edata+= v
			edata +="\x1D\x00"
			bdata += struct.pack("<I",len(edata))
			bdata += edata
	return bdata

# Reads the expression and returns all operands in 'expr()' format.
def get_expr_val(f,expr_len):
	end_len = f.tell()+expr_len
	operand = ""
	exp_lst = []
	while f.tell()< end_len:
		ov = get_value(f)
		if(ov != ""):
			if(ov == "szNULL"):
				ov = ""
			exp_lst.append(ov)
	return "expr("+','.join(exp_lst)+")"
	
# Reads the datatype and returns the operand.
def get_value(f):
	dt = f.read(2)
	if(dt not in DTYPE_DB):
		print("ERROR - Unknown Datatype 0x%2X @ offset %x " % (struct.unpack("<H",dt)[0],f.tell()-2))
		exit(1)
	operand = f.read(DTYPE_DB[dt]['datasize'])
	if(DTYPE_DB[dt]['type'] == 'int'):
		operand = str(struct.unpack("<i",operand)[0])
	if(DTYPE_DB[dt]['type'] == 'float'):
		operand = "%.6ff" % struct.unpack("f",operand)[0]
	if(DTYPE_DB[dt]['type'] == 'strlen'):
		#Get the length of the string.
		strlen = struct.unpack("<I",operand)[0]
		#Quote it for the list.
		operand = f.read(strlen)
		#HACK - Fucking Tabs in strings, I'll kill all of them!
		operand = operand.replace("\x09","[tab]")
		if(RepresentsInt(operand)):
			operand="sz__"+operand
		elif(RepresentsFloat(operand)):
			operand="sz__"+operand
		if(operand == ""):
			operand = "[szNULL]"
	if(DTYPE_DB[dt]['type'] == 'expr'):
		# Get the length of the expression.
		expr_len = struct.unpack("<I",operand)[0]
		operand = get_expr_val(f,expr_len)
	if(DTYPE_DB[dt]['type'] == 'null'):
		return ""
	if(DTYPE_DB[dt]['type'] == 'conditionalopr'):
		return DTYPE_DB[dt]['name']
	return operand
	

def std_operand_handler(f,opcode):
	operand_lst = []
	for i in range(0,OPCODE_DB[opcode]['params']):
		operand_lst.append(get_value(f))
	return operand_lst	

OPCODE_DB = {
	0x8000:{'name':'return','params':0,'handler':std_operand_handler},
	0x8001:{'name':'LoadArg','params':1,'handler':std_operand_handler},
	0x8002:{'name':'LoadTex','params':1,'handler':std_operand_handler},
	0x8003:{'name':'LoadTexAsync','params':1,'handler':std_operand_handler},
	0x8004:{'name':'LoadTexWait','params':1,'handler':std_operand_handler},
	0x8005:{'name':'ReleaseTex','params':0,'handler':std_operand_handler},
	0x8006:{'name':'ChangeAnimation','params':4,'handler':std_operand_handler},
	0x8007:{'name':'Message','params':1,'handler':std_operand_handler},
	0x8008:{'name':'Wait','params':1,'handler':std_operand_handler},
	0x8009:{'name':'WaitFade','params':0,'handler':std_operand_handler},
	0x800A:{'name':'WaitMenu','params':1,'handler':std_operand_handler},
	0x800B:{'name':'SetChrPos','params':4,'handler':std_operand_handler},
	0x800C:{'name':'Move','params':6,'handler':std_operand_handler},
	0x800D:{'name':'MoveTo','params':6,'handler':std_operand_handler},
	0x800E:{'name':'MoveToChr','params':4,'handler':std_operand_handler},
	0x800F:{'name':'MoveEx','params':8,'handler':std_operand_handler},
	0x8010:{'name':'MoveToEx','params':8,'handler':std_operand_handler},
	0x8011:{'name':'MoveToEx2','params':8,'handler':std_operand_handler},
	0x8012:{'name':'MoveToChrEx','params':6,'handler':std_operand_handler},
	0x8013:{'name':'Turn','params':3,'handler':std_operand_handler},
	0x8014:{'name':'Turn2','params':4,'handler':std_operand_handler},
	0x8015:{'name':'TurnTo','params':5,'handler':std_operand_handler},
	0x8016:{'name':'TurnToVec','params':5,'handler':std_operand_handler},
	0x8017:{'name':'TurnToChr','params':3,'handler':std_operand_handler},
	0x8018:{'name':'Jump','params':2,'handler':std_operand_handler},
	0x8019:{'name':'Dash','params':3,'handler':std_operand_handler},
	0x801A:{'name':'ForceDash','params':3,'handler':std_operand_handler},
	0x801B:{'name':'MoveZ','params':5,'handler':std_operand_handler},
	0x801C:{'name':'MoveRotate','params':10,'handler':std_operand_handler},
	0x801D:{'name':'ResetChrPos','params':1,'handler':std_operand_handler},
	0x801E:{'name':'ClearDebris','params':0,'handler':std_operand_handler},
	0x801F:{'name':'MoveCameraAt','params':5,'handler':std_operand_handler},
	0x8020:{'name':'RotateCamera','params':3,'handler':std_operand_handler},
	0x8021:{'name':'ChangeCameraZoom','params':3,'handler':std_operand_handler},
	0x8022:{'name':'ChangeCameraPers','params':3,'handler':std_operand_handler},
	0x8023:{'name':'ChangeCameraElevation','params':3,'handler':std_operand_handler},
	0x8024:{'name':'ChangeCameraDistance','params':3,'handler':std_operand_handler},
	0x8025:{'name':'MoveCamera','params':5,'handler':std_operand_handler},
	0x8026:{'name':'SaveCamera','params':0,'handler':std_operand_handler},
	0x8027:{'name':'RestoreCamera','params':2,'handler':std_operand_handler},
	0x8028:{'name':'SetStopFlag','params':1,'handler':std_operand_handler},
	0x8029:{'name':'ResetStopFlag','params':1,'handler':std_operand_handler},
	0x802A:{'name':'SetChrInfoFlag','params':2,'handler':std_operand_handler},
	0x802B:{'name':'ResetChrInfoFlag','params':2,'handler':std_operand_handler},
	0x802C:{'name':'SetFlag','params':2,'handler':std_operand_handler},
	0x802D:{'name':'SetChrWork','params':3,'handler':std_operand_handler},
	0x802E:{'name':'SetWork','params':2,'handler':std_operand_handler},
	0x802F:{'name':'JoinParty','params':1,'handler':std_operand_handler},
	0x8030:{'name':'JoinGuest','params':1,'handler':std_operand_handler},
	0x8031:{'name':'SeparateParty','params':1,'handler':std_operand_handler},
	0x8032:{'name':'FadeOut','params':2,'handler':std_operand_handler},
	0x8033:{'name':'FadeIn','params':2,'handler':std_operand_handler},
	0x8034:{'name':'CrossFade','params':1,'handler':std_operand_handler},
	0x8035:{'name':'MenuReset','params':0,'handler':std_operand_handler},
	0x8036:{'name':'MenuAdd','params':2,'handler':std_operand_handler},
	0x8037:{'name':'MenuOpen','params':7,'handler':std_operand_handler},
	0x8038:{'name':'MenuClose','params':2,'handler':std_operand_handler},
	0x8039:{'name':'YesNoMenu','params':3,'handler':std_operand_handler},
	0x803A:{'name':'MiniGame','params':1,'handler':std_operand_handler},
	0x803B:{'name':'PlayBGM','params':2,'handler':std_operand_handler},
	0x803C:{'name':'StopBGM','params':1,'handler':std_operand_handler},
	0x803D:{'name':'PlaySE','params':3,'handler':std_operand_handler},
	0x803E:{'name':'PlaySE3D','params':6,'handler':std_operand_handler},
	0x803F:{'name':'SetName','params':1,'handler':std_operand_handler},
	0x8040:{'name':'TalkMes','params':4,'handler':std_operand_handler},
	0x8041:{'name':'Message2','params':13,'handler':std_operand_handler},
	0x8042:{'name':'WaitPrompt','params':0,'handler':std_operand_handler},
	0x8043:{'name':'ClosePrompt','params':0,'handler':std_operand_handler},
	0x8044:{'name':'Portrait_Load','params':2,'handler':std_operand_handler},
	0x8045:{'name':'Portrait_Unload','params':1,'handler':std_operand_handler},
	0x8046:{'name':'Portrait_Create','params':10,'handler':std_operand_handler},
	0x8047:{'name':'Portrait_Close','params':1,'handler':std_operand_handler},
	0x8048:{'name':'Portrait_Anime','params':7,'handler':std_operand_handler},
	0x8049:{'name':'Portrait3D_Set','params':4,'handler':std_operand_handler},
	0x804A:{'name':'Portrait3D_Release','params':1,'handler':std_operand_handler},
	0x804B:{'name':'Portrait3D_Create','params':8,'handler':std_operand_handler},
	0x804C:{'name':'Portrait3D_Close','params':1,'handler':std_operand_handler},
	0x804D:{'name':'Portrait3D_Anime','params':8,'handler':std_operand_handler},
	0x804E:{'name':'ExecuteCmd','params':2,'handler':std_operand_handler},
	0x804F:{'name':'ExecuteFunc','params':2,'handler':std_operand_handler},
	0x8050:{'name':'WaitThread','params':1,'handler':std_operand_handler},
	0x8051:{'name':'WaitThread2','params':1,'handler':std_operand_handler},
	0x8052:{'name':'StopThread','params':1,'handler':std_operand_handler},
	0x8053:{'name':'StopThread2','params':1,'handler':std_operand_handler},
	0x8054:{'name':'EventCue','params':1,'handler':std_operand_handler},
	0x8055:{'name':'HP_Heal','params':2,'handler':std_operand_handler},
	0x8056:{'name':'HP_Recover','params':1,'handler':std_operand_handler},
	0x8057:{'name':'HPMax_Recover','params':1,'handler':std_operand_handler},
	0x8058:{'name':'SP_Recover','params':1,'handler':std_operand_handler},
	0x8059:{'name':'SPMax_Recover','params':0,'handler':std_operand_handler},
	0x805A:{'name':'RepairSkillCount','params':0,'handler':std_operand_handler},
	0x805B:{'name':'SetCameraMode','params':1,'handler':std_operand_handler},
	0x805C:{'name':'SetCameraZPlane','params':2,'handler':std_operand_handler},
	0x805D:{'name':'ResetCameraZPlane','params':0,'handler':std_operand_handler},
	0x805E:{'name':'ChangeCamera','params':1,'handler':std_operand_handler},
	0x805F:{'name':'Effect','params':9,'handler':std_operand_handler},
	0x8060:{'name':'Effect2','params':10,'handler':std_operand_handler},
	0x8061:{'name':'ChrEffect','params':12,'handler':std_operand_handler},
	0x8062:{'name':'ChrEffect2','params':10,'handler':std_operand_handler},
	0x8063:{'name':'ChrSpecular','params':10,'handler':std_operand_handler},
	0x8064:{'name':'Hagitori','params':0,'handler':std_operand_handler},
	0x8065:{'name':'Hagitori2','params':0,'handler':std_operand_handler},
	0x8066:{'name':'ChangeFace','params':4,'handler':std_operand_handler},
	0x8067:{'name':'HeadYes','params':3,'handler':std_operand_handler},
	0x8068:{'name':'HeadNo','params':3,'handler':std_operand_handler},
	0x8069:{'name':'Look','params':3,'handler':std_operand_handler},
	0x806A:{'name':'LookChr','params':2,'handler':std_operand_handler},
	0x806B:{'name':'LookTo','params':4,'handler':std_operand_handler},
	0x806C:{'name':'LookVec','params':4,'handler':std_operand_handler},
	0x806D:{'name':'LookReset','params':1,'handler':std_operand_handler},
	0x806E:{'name':'Emotion','params':6,'handler':std_operand_handler},
	0x806F:{'name':'MapHide','params':2,'handler':std_operand_handler},
	0x8070:{'name':'MapAnime','params':2,'handler':std_operand_handler},
	0x8071:{'name':'MapAnimeIndex','params':3,'handler':std_operand_handler},
	0x8072:{'name':'MapAnimeLast','params':2,'handler':std_operand_handler},
	0x8073:{'name':'ShopOpen','params':1,'handler':std_operand_handler},
	0x8074:{'name':'ShowMinipop','params':2,'handler':std_operand_handler},
	0x8075:{'name':'ChangeItemState','params':1,'handler':std_operand_handler},
	0x8076:{'name':'EventBox','params':2,'handler':std_operand_handler},
	0x8077:{'name':'EventBox2','params':2,'handler':std_operand_handler},
	0x8078:{'name':'EventAreaEnable','params':2,'handler':std_operand_handler},
	0x8079:{'name':'ChangeItemSlot','params':2,'handler':std_operand_handler},
	0x807A:{'name':'DeleteItem','params':2,'handler':std_operand_handler},
	0x807B:{'name':'GetItem','params':2,'handler':std_operand_handler},
	0x807C:{'name':'GetItemMessage','params':2,'handler':std_operand_handler},
	0x807D:{'name':'AddItemLog','params':3,'handler':std_operand_handler},
	0x807E:{'name':'ResetMotion','params':2,'handler':std_operand_handler},
	0x807F:{'name':'PlayMMV','params':3,'handler':std_operand_handler},
	# Functions with Jumps Start
	0x8080:{'name':'if','params':2,'handler':std_operand_handler},
	0x8081:{'name':'else if','params':2,'handler':std_operand_handler},
	0x8082:{'name':'else','params':1,'handler':std_operand_handler},
	0x8083:{'name':'switch','params':1,'handler':std_operand_handler},
	0x8085:{'name':'jmpto','params':1,'handler':std_operand_handler},
	0x8086:{'name':'break','params':1,'handler':std_operand_handler},
	0x8087:{'name':'implicit_break','params':0,'handler':std_operand_handler},
	0x8088:{'name':'case','params':2,'handler':std_operand_handler},
	0x8089:{'name':'default','params':1,'handler':std_operand_handler},
	# Functions with Jumps End
	0x808A:{'name':'FreeMove','params':6,'handler':std_operand_handler},
	0x808B:{'name':'ResetFollowPoint','params':0,'handler':std_operand_handler},
	0x808C:{'name':'ResetPartyPos','params':0,'handler':std_operand_handler},
	0x808D:{'name':'else CurePoison','params':0,'handler':std_operand_handler},
	0x808E:{'name':'EarthQuake','params':3,'handler':std_operand_handler},
	0x808F:{'name':'SetLevel','params':2,'handler':std_operand_handler},
	0x8090:{'name':'EquipWeapon','params':2,'handler':std_operand_handler},
	0x8091:{'name':'EquipArmor','params':2,'handler':std_operand_handler},
	0x8092:{'name':'EquipShield','params':2,'handler':std_operand_handler},
	0x8093:{'name':'EquipAccessory','params':3,'handler':std_operand_handler},
	0x8094:{'name':'Entry','params':1,'handler':std_operand_handler},
	0x8095:{'name':'EntryOn','params':2,'handler':std_operand_handler},
	0x8096:{'name':'SetEventParam','params':7,'handler':std_operand_handler},
	0x8097:{'name':'SetCheckPoint','params':1,'handler':std_operand_handler},
	0x8098:{'name':'GetCheckPoint','params':2,'handler':std_operand_handler},
	0x8099:{'name':'WarpMenu','params':1,'handler':std_operand_handler},
	0x809A:{'name':'TopicMenu','params':0,'handler':std_operand_handler},
	0x809B:{'name':'DestroyObj','params':5,'handler':std_operand_handler},
	0x809C:{'name':'GetSkill','params':3,'handler':std_operand_handler},
	0x809D:{'name':'SetSkillShortCut','params':3,'handler':std_operand_handler},
	0x809E:{'name':'SetLeverState','params':2,'handler':std_operand_handler},
	0x809F:{'name':'SetLeverNotChange','params':2,'handler':std_operand_handler},
	0x80A0:{'name':'BtlPopup','params':2,'handler':std_operand_handler},
	0x80A1:{'name':'AddEX','params':1,'handler':std_operand_handler},
	0x80A2:{'name':'SetPartyMember','params':3,'handler':std_operand_handler},
	0x80A3:{'name':'SavePartyMember','params':0,'handler':std_operand_handler},
	0x80A4:{'name':'RestorePartyMember','params':0,'handler':std_operand_handler},
	0x80A5:{'name':'SetEventPartyChr','params':1,'handler':std_operand_handler},
	0x80A6:{'name':'LoadEventPartyChr','params':0,'handler':std_operand_handler},
	0x80A7:{'name':'ReleaseEventPartyChr','params':0,'handler':std_operand_handler},
	0x80A8:{'name':'TimeStop','params':1,'handler':std_operand_handler},
	0x80A9:{'name':'PartyMenu','params':0,'handler':std_operand_handler},
	0x80AA:{'name':'ResetCameraObserver','params':1,'handler':std_operand_handler},
	0x80AB:{'name':'DoorOpen','params':3,'handler':std_operand_handler},
	0x80AC:{'name':'DoorClose','params':3,'handler':std_operand_handler},
	0x80AD:{'name':'ChrAlpha','params':3,'handler':std_operand_handler},
	0x80AE:{'name':'ChrColor','params':10,'handler':std_operand_handler},
	0x80AF:{'name':'MapColor','params':3,'handler':std_operand_handler},
	0x80B0:{'name':'MapDelProp','params':2,'handler':std_operand_handler},
	0x80B1:{'name':'MapSetProp','params':2,'handler':std_operand_handler},
	0x80B2:{'name':'Blur','params':3,'handler':std_operand_handler},
	0x80B3:{'name':'GameOver','params':0,'handler':std_operand_handler},
	0x80B4:{'name':'LoadEffect','params':1,'handler':std_operand_handler},
	0x80B5:{'name':'ReleaseEffect','params':1,'handler':std_operand_handler},
	0x80B6:{'name':'TimeAtkStart','params':1,'handler':std_operand_handler},
	0x80B7:{'name':'TimeAtkEnd','params':0,'handler':std_operand_handler},
	0x80B8:{'name':'TimeAtkMenuReset','params':0,'handler':std_operand_handler},
	0x80B9:{'name':'TimeAtkMenuAdd','params':2,'handler':std_operand_handler},
	0x80BA:{'name':'TimeAtkMenuOpen','params':1,'handler':std_operand_handler},
	0x80BB:{'name':'AutoSave','params':0,'handler':std_operand_handler},
	0x80BC:{'name':'ReturnTitle','params':0,'handler':std_operand_handler},
	0x80BD:{'name':'StopEffect','params':3,'handler':std_operand_handler},
	0x80BE:{'name':'StopEmotion','params':1,'handler':std_operand_handler},
	0x80BF:{'name':'PartyChrHide','params':1,'handler':std_operand_handler},
	0x80C0:{'name':'GetItemMessageEx','params':3,'handler':std_operand_handler},
	0x80C1:{'name':'CopyStatus','params':2,'handler':std_operand_handler},
	0x80C2:{'name':'Movie','params':3,'handler':std_operand_handler},
	0x80C3:{'name':'WaitMovie','params':0,'handler':std_operand_handler},
	0x80C4:{'name':'EmotionEx','params':8,'handler':std_operand_handler},
	0x80C5:{'name':'SetFumiSwitch','params':1,'handler':std_operand_handler},
	0x80C6:{'name':'StopSE','params':1,'handler':std_operand_handler},
	0x80C7:{'name':'SetEnvSEPlayState','params':2,'handler':std_operand_handler},
	0x80C8:{'name':'SetFog','params':13,'handler':std_operand_handler},
	0x80C9:{'name':'GetItemMessageExPlus','params':6,'handler':std_operand_handler},
	0x80CA:{'name':'ResetPresetCamera','params':0,'handler':std_operand_handler},
	0x80CB:{'name':'Cure','params':1,'handler':std_operand_handler},
	0x80CC:{'name':'CampMenu','params':1,'handler':std_operand_handler},
	0x80CD:{'name':'ResetMoveVec','params':1,'handler':std_operand_handler},
	0x80CE:{'name':'PlayBGMB','params':2,'handler':std_operand_handler},
	0x80CF:{'name':'RemovePartyEquip','params':2,'handler':std_operand_handler},
	0x80D0:{'name':'AddStatus','params':3,'handler':std_operand_handler},
	0x80D1:{'name':'SetPlaceName','params':1,'handler':std_operand_handler},
	0x80D2:{'name':'YesNoMenuEx','params':4,'handler':std_operand_handler},
	0x80D3:{'name':'SetPartyPosEntryOut','params':0,'handler':std_operand_handler},
	0x80D4:{'name':'MPMenu','params':2,'handler':std_operand_handler},
	0x80D5:{'name':'SetMapMarker','params':8,'handler':std_operand_handler},
	0x80D6:{'name':'DelMapMarker','params':5,'handler':std_operand_handler},
	0x80D7:{'name':'DeleteArgMarker','params':1,'handler':std_operand_handler},
	0x80D8:{'name':'SetEventNPC','params':4,'handler':std_operand_handler},
	0x80D9:{'name':'PuzzleOpen','params':1,'handler':std_operand_handler},
	0x80DA:{'name':'SetPortal','params':1,'handler':std_operand_handler},
	0x80DB:{'name':'CreateKoma','params':14,'handler':std_operand_handler},
	0x80DC:{'name':'CloseKoma','params':6,'handler':std_operand_handler},
	0x80DD:{'name':'Portrait_SetKoma','params':2,'handler':std_operand_handler},
	0x80DE:{'name':'SetKomaInfo','params':3,'handler':std_operand_handler},
	0x80DF:{'name':'SetKomaColor','params':7,'handler':std_operand_handler},
	0x80E0:{'name':'MoveKoma','params':6,'handler':std_operand_handler},
	0x80E1:{'name':'MoveKomaEx','params':12,'handler':std_operand_handler},
	0x80E2:{'name':'VibratePortrait','params':9,'handler':std_operand_handler},
	0x80E3:{'name':'VibrateKoma','params':8,'handler':std_operand_handler},
	0x80E4:{'name':'SetFadeColor','params':3,'handler':std_operand_handler},
	0x80E5:{'name':'Portrait_Change','params':3,'handler':std_operand_handler},
	0x80E6:{'name':'HP_ORecover','params':2,'handler':std_operand_handler},
	0x80E7:{'name':'ChangeHDRBaseLum','params':3,'handler':std_operand_handler},
	0x80E8:{'name':'ChangeHDRSoftFocus','params':3,'handler':std_operand_handler},
	0x80E9:{'name':'ChangeHDRDoF','params':3,'handler':std_operand_handler},
	0x80EA:{'name':'ResetHDRParam','params':2,'handler':std_operand_handler},
	0x80EB:{'name':'MapNodeEvent','params':2,'handler':std_operand_handler},
	0x80EC:{'name':'EventAttack','params':5,'handler':std_operand_handler},
	0x80ED:{'name':'EventDamage','params':2,'handler':std_operand_handler},
	0x80EE:{'name':'WaitAnimation','params':3,'handler':std_operand_handler},
	0x80EF:{'name':'MoveToEx3','params':8,'handler':std_operand_handler},
	0x80F0:{'name':'EvTimeShift','params':2,'handler':std_operand_handler},
	0x80F1:{'name':'SetDiaryFlag','params':2,'handler':std_operand_handler},
	0x80F2:{'name':'QuestBoard','params':1,'handler':std_operand_handler},
	0x80F3:{'name':'ReturnMemoryMenu','params':0,'handler':std_operand_handler},
	0x80F4:{'name':'Set3DParticle','params':15,'handler':std_operand_handler},
	0x80F5:{'name':'Change3DParticleParam','params':6,'handler':std_operand_handler},
	0x80F6:{'name':'Change3DParticleVec','params':13,'handler':std_operand_handler},
	0x80F7:{'name':'Stop3DParticle','params':1,'handler':std_operand_handler},
	0x80F8:{'name':'SetOverlay','params':8,'handler':std_operand_handler},
	0x80F9:{'name':'StopOverlay','params':1,'handler':std_operand_handler},
	0x80FA:{'name':'SetParent','params':3,'handler':std_operand_handler},
	0x80FB:{'name':'ChrNodeHide','params':4,'handler':std_operand_handler},
	0x80FC:{'name':'ForceChangeCamera','params':1,'handler':std_operand_handler},
	0x80FD:{'name':'RollCamera','params':3,'handler':std_operand_handler},
	0x80FE:{'name':'CheckChrAngle','params':5,'handler':std_operand_handler},
	0x80FF:{'name':'CheckChrDistance','params':5,'handler':std_operand_handler},
	0x8100:{'name':'SetMapViewPos','params':2,'handler':std_operand_handler},
	0x8101:{'name':'ResetTracks','params':0,'handler':std_operand_handler},
	0x8102:{'name':'DiscoverMemory','params':1,'handler':std_operand_handler},
	0x8103:{'name':'StartZapping','params':2,'handler':std_operand_handler},
	0x8104:{'name':'StopZapping','params':1,'handler':std_operand_handler},
	0x8105:{'name':'ChrLightState','params':4,'handler':std_operand_handler},
	0x8106:{'name':'ChrLightRadius','params':8,'handler':std_operand_handler},
	0x8107:{'name':'ChrLightColor','params':6,'handler':std_operand_handler},
	0x8108:{'name':'CreateLight','params':4,'handler':std_operand_handler},
	0x8109:{'name':'SetLightState','params':2,'handler':std_operand_handler},
	0x810A:{'name':'SetLightRadius','params':8,'handler':std_operand_handler},
	0x810B:{'name':'SetLightColor','params':6,'handler':std_operand_handler},
	0x810C:{'name':'CureAll','params':0,'handler':std_operand_handler},
	0x810D:{'name':'SetCondition','params':2,'handler':std_operand_handler},
	0x810E:{'name':'PlaySELabel','params':4,'handler':std_operand_handler},
	0x810F:{'name':'PlaySE3DLabel','params':7,'handler':std_operand_handler},
	0x8110:{'name':'StopSELabel','params':1,'handler':std_operand_handler},
	0x8111:{'name':'FadeSELabel','params':5,'handler':std_operand_handler},
	0x8112:{'name':'FadeBGM','params':2,'handler':std_operand_handler},
	0x8113:{'name':'SetWeaponKillScript','params':4,'handler':std_operand_handler},
	0x8114:{'name':'Timer','params':2,'handler':std_operand_handler},
	0x8115:{'name':'VisualName','params':2,'handler':std_operand_handler},
	0x8116:{'name':'SetArgInitPos','params':4,'handler':std_operand_handler},
	0x8117:{'name':'PlayVoice','params':2,'handler':std_operand_handler},
	0x8118:{'name':'PlayVoice3D','params':5,'handler':std_operand_handler},
	0x8119:{'name':'StopVoice','params':1,'handler':std_operand_handler},
	0x811A:{'name':'SetShadowBound','params':2,'handler':std_operand_handler},
	0x811B:{'name':'ResetShadowBound','params':0,'handler':std_operand_handler},
	0x811C:{'name':'GetTrophy','params':1,'handler':std_operand_handler},
	0x811D:{'name':'MemoryMenu','params':1,'handler':std_operand_handler},
	0x811E:{'name':'RemoveEquipAll','params':0,'handler':std_operand_handler},
	0x811F:{'name':'EndTimeAtk','params':0,'handler':std_operand_handler},
}


	
#SetFlag has some post-processing to do - two operands (flag_number, and value to set)
# e.g. SetFlag expr(413) expr(2) is SetFlag expr(SF_ADOL_JOINOK) expr(2)

BRANCH_INSTRUCTIONS = ['if','else if','else','case','break','default','jmpto']





