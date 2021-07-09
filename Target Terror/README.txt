Target Terror Gold 2.12

Description: 
This - this was a fun one, cleaned up nicely. The game originally ran on some old PC-based slackware system with 512MB of ram and some old NVIDIA card.
Protection of the GOLD version consisted of a RockKey dongle, but was only checking for its presence... not for any values or anything.
Protection of the ORIGINAL version of the game appeared to be hasp, they actually packed the old executable (ZZZUB$) with the hasp envelope, it appears.
The game had 3 partitions (OS, game partition mounted on /tt, and SWAP) - all readables were ReiserFS.
The game requests files through a somewhat windows-like path setup (e.g. "avi\blah\blah.avi"), but takes that path, 
runs Bob Jenkin's lookup2 hash on the string: (http://burtleburtle.net/bob/hash/doobs.html), then converts that uint32 to ASCII 6bit to come up with a name
and reads the garbled file that way.

Two IO devices were supported - USB and Serial. The Serial device was easier to emulate which is why I emulated that one - they basically pass ASCII packets
encased in \n and \r to signal the beginning and end. If you want more details, look at "target_terror_io.py"

Notes:

- libpulse on modern systems doesn't work well with gettimeofday,
copy the libpulse.so.0 included in this archive to /usr/lib/i386-linux/gnu/pulseaudio/
if you get an assertion error.

- Some dependencies may be required for the game (such as freeglut3, etc...)

- unknown.zip are files the game doesn't use and can't be renamed at the moment... they appear to be random garbage development files or files from the previous version.

- The left/right player is currently supported with a mouse, further implementations (wiimote zappers, multiple players, etc.) are left as an exercise to the reader.

- The game directory expects to be /tt/* - paths have been patched, but if there are any problems, put the game there.

- Keyboard bindings require a symlink at /run/kbdhook, type this: "ln -s /dev/input/by-path/*event-kbd /run/kbdhook"

- To run in fullscreen mode, type export TTG_FULLSCREEN=1

- Run the IO emulator with "python ./target_terror_io.py" - you might need python xlib, python-evdev, and pyserial to get it going.

- To run the game, at the game directory, type LD_PRELOAD=./tt_emu.so ./game

- Sometimes the multi-threaded io python runtime gets bitchy when you close the game, just pkill it until I get around to making it die with some grace.

- Controls:

F2 - Test Switch
F3 - Service Switch
1  - 1P Start
2  - 2P Start
5  - 1P Coin
6  - 2P Coin
Left Mouse -  P1/P2 Trigger
Right Mouse - P1/P2 Reload
Mouse_X/Mouse_Y - Controls both guns
8  - Volume Up
9  - Volume Down

- All source code for the patch (tt_emu.so) has been included in the "src" directory, I used CLion to write it - go nuts :)


Love,

rFx

