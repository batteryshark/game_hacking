#!/bin/bash
source ./start_sound.sh > /dev/null 2>/dev/null
LD_PRELOAD=./stv_emu.so ./trek.e4 >/dev/null 2>/dev/null
sh ./kill_sound.sh
