'''
 PA Gamebar Activator by j0n0
'''
import SimpleHTTPServer
import SocketServer

#My Ys7 CD-KEY :)
CD_KEY = "H3D6C-F42MU-VS26M-86N9V-YA77M-VACFC-FAU2Z-4DSFN"
#Parallels
#"YDKH6-3FQRR-6Q3FD-WDAU8-D3B8D-YT7U6-RVQH2-YJ2V8"
#Hack - Big Ass Key

PORT = 99

DEBUG = False

class ServerHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
	#We only need POST
	def do_POST(self):
		
		#Get the XML Data from Paul.dll
		data = self.rfile.read(int(self.headers['Content-Length']))
		#Hack - Keep the Connection open so we can debug.
		#data = self.rfile.read(9999)		
		if(DEBUG == True):
			print(self.headers)
			print(data)
		opcode = ""
		if("<activate_type>1" in data):
			print("Activation Request Received")
			opcode = "Activate_Game"
			response_data = """<?xml version="1.0" encoding="UTF-8"?><response version="1"><result_value>0</result_value><message>%s</message></response>""" % CD_KEY
			print("Sending Activation Code")
		elif("<activate_type>2" in data):
			print("Deactivation Request Received")
			opcode = "Deactivate_Game"
			response_data = """<?xml version="1.0" encoding="UTF-8"?><response version="1"><result_value>0</result_value><message>Fuck You, Securom!</message></response>"""
			print("Sending Dectivation Response")
		if(opcode != ""):
			#If we have a handled request - do it up :)
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			self.wfile.write(response_data)
		print("Done!")
if(__name__=="__main__"):
	print("Ys7 Gamebar Server Simulator")
	print("2014 j0n0")
	Handler = ServerHandler
	httpd = SocketServer.TCPServer(("", PORT), Handler)
	httpd.serve_forever()

