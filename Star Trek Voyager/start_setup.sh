#!/bin/bash
source ./start_sound.sh > /dev/null 2>/dev/null
LD_PRELOAD=./stv_setup.so ./stvserv.e4
sh ./kill_sound.sh
