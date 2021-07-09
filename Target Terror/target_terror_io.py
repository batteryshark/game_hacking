# Target Terror JAMMA/GUN I/O Emulator

import subprocess,serial,time,binascii,struct
from threading import Thread
import evdev
import Xlib
import Xlib.display



GAME_WINDOW = {
"height":0,
"width":0,
"x":0,
"y":0,
}

def scale_value(rfrom,rto,old_value):
    old_range = rfrom[1] - rfrom[0]
    new_range = rto[1] - rto[0]
    return (((old_value - rfrom[0]) * new_range) / old_range) + rto[0]

def get_mouse_pos(window):
    global GAME_WINDOW
    qp = window.query_pointer()
    real_x = qp.win_x
    real_y = qp.win_y  #WTF
    # We're going to constrain the mouse return coords to the window
    if(real_x > GAME_WINDOW["width"]):
        real_x = GAME_WINDOW["width"]
    if(real_y > GAME_WINDOW["height"]):
        real_y = GAME_WINDOW["height"]
    if(real_x < 0):
        real_x = 0
    if(real_y < 0):
        real_y = 0

    bm = {"button_0":"1","button_1":"0","button_2":"0","x":"0000","y":"0000"}
    # We will put the values in the format that the game expects...
    real_x = scale_value((0,GAME_WINDOW["width"]),(0,640),real_x)
    real_y = scale_value((0,GAME_WINDOW["height"]),(0,640),real_y)
    bm["x"] = binascii.hexlify(struct.pack(">H",real_x)).upper()
    bm["y"] = binascii.hexlify(struct.pack(">H",real_y)).upper()
    
    # Now, we need to deal with the buttons...
    msk = qp.mask
    
    if(msk & 0x100):
        bm["button_0"] = "0"
    else:
        bm["button_0"] = "1"
    if(msk & 0x200):
        bm["button_1"] = "1"
    else:
        bm["button_1"] = "0"
    if(msk & 0x400):
        bm["button_2"] = "1"
    else:
        bm["button_2"] = "0"

    return bm


def check_window(window):
    global EXIT
    global GAME_WINDOW
    try:
        window_geo = window.get_geometry()
    except:
        EXIT = True
        print("Window Closed")
        return
    # Pass Geometry to our global
    GAME_WINDOW["height"] = window_geo.height
    GAME_WINDOW["width"] = window_geo.width
    GAME_WINDOW["x"] = window_geo.x
    GAME_WINDOW["y"] = window_geo.y

def wait_for_game(display,root):
    print("Waiting for Game Window...")
    window_id = ""
    while 1:
        windowIDs = root.get_full_property(display.intern_atom('_NET_CLIENT_LIST'), Xlib.X.AnyPropertyType).value
        for windowID in windowIDs:
            window = display.create_resource_object('window', windowID)
            name = window.get_wm_name() # Title
            pid = window.get_full_property(display.intern_atom('_NET_WM_PID'), Xlib.X.AnyPropertyType)
            if(name == "Target Terror: GOLD"):
                print("Game Window Found!")
                window_id = windowID
                break
        if(window_id != ""):
            break

    return window_id

# Global Exit - Used to kill all the damn threads.

EXIT = False

SWITCH_DB = {
    "trigger_0":"1", #Triggers are inverted
    "trigger_1":"1",
    "reload_0":"0",
    "reload_1":"0",
    "start_0":"0",
    "start_1":"0",
    "coin_0":"0",
    "coin_1":"0",
    "volume_up":"0",
    "volume_down":"0",
    "switch_test":"0",
    "switch_service":"0",
    "tilt_0":"1",
    "tilt_1":"1"
}

def process_text():
    return ""

switches_updated = 0

io_active = 0
def keyboard_input_loop():
    global switches_updated
    global SWITCH_DB
    global EXIT
    global LAST_SWITCH_DB
    keyboard_device = evdev.InputDevice("/run/kbdhook")
    while True:
        if(EXIT == True):
            return
        if(io_active == 0):
            continue
        for event in keyboard_device.read_loop():
            if event.type == evdev.ecodes.EV_KEY:
                switches_updated = 1
                kc = evdev.ecodes.KEY[event.code]
                if(kc == "KEY_ESC"):
                    proc.kill()
                    EXIT = True
                    return
                elif(kc == "KEY_F2"):
                    if(event.value):
                        SWITCH_DB["switch_test"] = "1"
                    else:
                        SWITCH_DB["switch_test"] = "0"
                elif(kc == "KEY_F3"):
                    if(event.value):
                        SWITCH_DB["switch_service"] = "1"
                    else:
                        SWITCH_DB["switch_service"] = "0"
                elif(kc == "KEY_1"):
                    if(event.value):
                        SWITCH_DB["start_0"] = "1"
                    else:
                        SWITCH_DB["start_0"] = "0"
                elif(kc == "KEY_2"):
                    if(event.value):
                        SWITCH_DB["start_1"] = "1"
                    else:
                        SWITCH_DB["start_1"] = "0"
                elif(kc == "KEY_5"):
                    if(event.value):
                        SWITCH_DB["coin_0"] = "1"
                    else:
                        SWITCH_DB["coin_0"] = "0"
                elif(kc == "KEY_6"):
                    if(event.value):
                        SWITCH_DB["coin_1"] = "1"
                    else:
                        SWITCH_DB["coin_1"] = "0"
                elif(kc == "KEY_8"):
                    if(event.value):
                        SWITCH_DB["volume_down"] = "1"
                    else:
                        SWITCH_DB["volume_down"] = "0"
                elif(kc == "KEY_9"):
                    if(event.value):
                        SWITCH_DB["volume_up"] = "1"
                    else:
                        SWITCH_DB["volume_up"] = "0"
                elif(kc == "KEY_Z"):
                    if(event.value):
                        SWITCH_DB["trigger_0"] = "0"
                    else:
                        SWITCH_DB["trigger_0"] = "1"
                elif(kc == "KEY_C"):
                    if(event.value):
                        SWITCH_DB["trigger_1"] = "0"
                    else:
                        SWITCH_DB["trigger_1"] = "1"
                elif(kc == "KEY_X"):
                    if(event.value):
                        SWITCH_DB["reload_0"] = "0"
                    else:
                        SWITCH_DB["reload_0"] = "1"
                elif(kc == "KEY_V"):
                    if(event.value):
                        SWITCH_DB["reload_1"] = "0"
                    else:
                        SWITCH_DB["reload_1"] = "1"
                elif(kc == "KEY_3"):
                    if(event.value):
                        SWITCH_DB["tilt_0"] = "1"
                    else:
                        SWITCH_DB["tilt_0"] = "0"
                elif(kc == "KEY_4"):
                    if(event.value):
                        SWITCH_DB["tilt_1"] = "1"
                    else:
                        SWITCH_DB["tilt_1"] = "0"
    pass


last_mouse = {"button_0":"1","button_1":"0","button_2":"0","x":"0000","y":"0000"}
# 0 is left, 1 is right.
def process_gun(player,window):
    global EXIT
    global last_mouse
    global SWITCH_DB
    response = "G"
    if(player == 0):
        response = "g"
    check_window(window)
    try:
        bm = get_mouse_pos(window)
    except:
        print("Window Not Detected, Shutting down...")
        EXIT = True
        return

    if(player == 0):
        SWITCH_DB["trigger_0"] = bm["button_0"]
        SWITCH_DB["reload_0"] = bm["button_2"]
    else:
        SWITCH_DB["trigger_1"] = bm["button_0"]
        SWITCH_DB["reload_1"] = bm["button_2"]
    response +="0000"+bm["y"]+bm["x"]+"0000"

    return response
    #Check if they're the same as last time.

last_response = ""
def process_switches():
    global switches_updated
    global SWITCH_DB
    global last_response
    sdb = SWITCH_DB
    if(switches_updated == 0):
        return ""
    else:
        response = "S00"
        response+=sdb["trigger_0"]+sdb["trigger_1"]+sdb["reload_0"]
        response+=sdb["reload_1"]+sdb["start_0"]+sdb["start_1"]
        response+=sdb["coin_0"]+sdb["coin_1"]+sdb["volume_up"]
        response+=sdb["volume_down"]+sdb["switch_test"]+sdb["switch_service"]
        response+=sdb["tilt_0"]+sdb["tilt_1"]
        
        if(response == last_response):
            return ""
        else:
            print(response)
            last_response = response
            return response
def process_dipswitches():
    return ""


def jamma_read(ser):
    global SWITCH_CONFIG
    global io_active
    global EXIT
    s_count = 0
    while EXIT == False:
        if(EXIT == True):
            return
        line = ""
        while ser.inWaiting() > 0:
            line+=ser.read()

        if(len(line) < 2):
            continue
        if(line != ""):
            pass
        if(line[0] != "\x0a" and line[0] != "*"):
            print("Bad Packet:%s" % line)
            continue
        if(line[1] == "v"):
            send_response("v1337")
        elif(line[1] == "s"):
            send_response("s00000000"+SWITCH_CONFIG)
            s_count+=1
            if(s_count == 12):
                t_input.start()
                io_active = 1
        elif(line[1] == "*"):
            print(line[1:-1])

# Have to do some locking because the serial read thread is going
# to be responding along with the main response thread for I/O
write_lock = False
def send_response(msg):
    global write_lock
    global EXIT
    while write_lock == True:
        continue
    if(EXIT == True):
        return
    write_lock = True
    try:
        ser.write("\x0a"+msg+"\x0d")
    except:
        EXIT = True
        return
    write_lock = False

# Dip switch bits are inverted.
SWITCH_CONFIG = "FF"
s_offset = 0
t_input = Thread(target=keyboard_input_loop)
if(__name__=="__main__"):
    cmd=['/usr/bin/socat','-d','-d','PTY,link=%s,raw,echo=0' % './ttytt', 'PTY,link=%s,raw,echo=0' % '/dev/ttyS0']
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(1)
    ser = serial.Serial(port='./ttytt', baudrate=115200)
    print("Target Terror I/O Ready")

    display = Xlib.display.Display()
    root = display.screen().root
    window_id = wait_for_game(display,root)
    window = display.create_resource_object('window', window_id)
    

    sr_thread = Thread(target=jamma_read,args=[ser])
    sr_thread.start()
    while EXIT == False:
        # Handle any of the IO calls needed.
        # Send a response if needed.
        gun_0 = process_gun(0,window)
        if(EXIT == True):
            break
        if(gun_0 != ""):
            send_response(gun_0)
        gun_1 = process_gun(1,window)
        if(EXIT == True):
            break
        if(gun_1 != ""):
            send_response(gun_1)
        switches = process_switches()
        if(EXIT == True):
            break
        if(switches != ""):
            send_response(switches)
        dip_switches = process_dipswitches()
        if(dip_switches != ""):
            send_response(dip_switches)
        text = process_text()
        if(text != ""):
            send_response(text)
    exit(0)
        



        