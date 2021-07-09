'''
DJMax Technika Arcade IO Board Library by Mario Mercaldi
Special Thanks to King Gregory for his majesty's assistance.
'''

import serial,struct,platform

class TGBoard(object):
  def __init__(self):
		#Input Attributes
		self.coin = 0
		self.test = 0
		self.svc = 0
		
		#Light (Output) Attributes
		self.sidelight = self.SideLight()
		self.pclight = self.PCrewLight()
		self.cardlight = self.CardLight()
		self.sblight = self.ScreenBassLight()
		
		#Hardware Port Attributes
		if not EMULATION:
			if(platform.system() == "Windows"):
				self.io = serial.Serial("COM7",115200)
			else:
				self.io = serial.Serial("/dev/ttyUSB0",115200)
		
	def checkInput(self):
		#Get input from serial connection
		incode = self.io.read(3)
		
		if(incode[1] == b'\x01'):
			self.coin = 1
			return
		else:
			self.coin = 0
		if(incode[1] == b'\x03'):
			self.test = 1
			return
		else:
			self.test = 0		
		if(incode[1] == b'\x18'):
			self.svc = 1
			return
		else:
			self.svc = 0
			
	def sendSignal(self,signal):
		self.io.write(signal)
	
	def debugOutput(self):
		print("COIN = %02X TEST = %02X SVC = %02X" % (self.coin,self.test,self.svc))
		print("SideL FX=%02X RGB=%02X,%02X,%02X DL=%02X" % (self.sidelight.state,self.sidelight.r,\
		self.sidelight.g,self.sidelight.b,self.sidelight.delay))
		print("BscrL FX=%02X RGB=%02X,%02X,%02X DL=%02X" % (self.pclight.state,self.pclight.r,\
		self.pclight.g,self.pclight.b,self.pclight.delay))
		print("PCrwL FX=%02X RGB=%02X,%02X,%02X DL=%02X" %(self.cardlight.state,self.cardlight.r,\
		self.cardlight.g,self.cardlight.b,self.cardlight.delay))
		print("CardL FX=%02X RGB=%02X,%02X,%02X DL=%02X"  %(self.sblight.state,self.sblight.r,\
		self.sblight.g,self.sblight.b,self.sblight.delay))

	class TGBLight(object):		
		def getSignal(self):
			signal= b'\xC3\x5A\xA5'
			signal+=struct.pack("B",self.state & 0xFF)
			signal+=struct.pack("B",self.r & 0xFF)
			signal+=struct.pack("B",self.g & 0xFF)
			signal+=struct.pack("B",self.b & 0xFF)
			signal+=struct.pack("B",self.delay & 0xFF)
			return signal
			

	class ScreenBassLight(TGBLight):
		def __init__(self):
			self.fx = {
			"off":0x58,
			"on":0x59,
			"chanswap":0xA0
			}
			self.state = self.fx["off"]
			self.r = 0
			self.g = 0
			self.b = 0
			self.delay = 0x1E
			#Send an init signal to set everything up - default state.
			self.getSignal()
		
	class PCrewLight(TGBLight):
		#This light has a strange color setup, basically this:
		#R Sets Blue values for both sides.
		#G Sets Green Values for Left and Red for Right
		#B Sets Red Values for Left and Green for Right
	
		def __init__(self):
			self.fx = {
			"outwave":0x90,
			"inwave":0x91,
			"pongwave":0x92,
			"converge":0x93,
			"leftright":0x94,
			"on":0x95,
			"pause":0x96
			}
			self.state = self.fx["pause"]
			self.r = 0
			self.g = 0
			self.b = 0
			self.delay = 0x1E


	class CardLight(TGBLight):
		def __init__(self):
			self.fx = {
			"cycle":0x60,
			"red":0x61,
			"yellow":0x62,
			"green":0x63,
			"blue":0x64,
			"darkblue":0x65,
			"purple":0x66,
			"redblink":0x67,
			"off":0x68,
			"white":0x69
			}
			self.state = self.fx["cycle"]
			self.r = 0
			self.g = 0
			self.b = 0
			self.delay = 0x1E

			
	class SideLight(TGBLight):
		def __init__(self):
			self.fx = {
			"pulse":0x80,
			"outwave":0x81,
			"riseflow":0x82,
			"diverge":0x83,
			"fallflow":0x84,
			"ripple":0x85,
			"ripplefade":0x86,
			"rise":0x87,
			"on":0x88,
			"pause":0x89
			}
			self.state = self.fx["pause"]
			self.r = 0
			self.g = 0
			self.b = 0
			self.delay = 0x1E


'''
Example Usage:
EMULATION = "ON"			
ioboard = TGBoard()
ioboard.sendSignal(ioboard.cardlight.getSignal())

while(1):
	ioboard.checkInput()
	ioboard.debugOutput()
	
'''
