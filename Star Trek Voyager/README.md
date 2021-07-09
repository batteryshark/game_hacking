Star Wars Voyager Arcade/PC Crack

[How to Play]
Optional: run "source ./start_setup.sh" to get to the operator menu
1. If you want fullscreen mode: "export STV_FULL=1"
2. Run "ln -s /dev/input/by-path/*event-kbd /run/kbdhook" 
3. in a terminal, type: "source ./start_game.sh"

[Controls]
F1: Test
F2: Service
F5-F7: Dip Switches 1-4
1: 1P Start / Reload
2: 2P Start / Reload
5: 1P Coin
6: 2P Coin
Mouse: 1P Light Gun / 2P Light Gun

[Notes]
- I've included the source of both the patch and the original driver. From that, you should be able to write support for your own lightgun if you have one.
- The game has an old janky-ass way to do vsync... it might be a bit too fast or slow on some machines. There's a turbo mode in the options if its too slow.
  If that still doesn't fix it, recompile and adjust the frame counter cooldown or figure out a better way to do it. 
  
  
Have Fun - and may the force be with you!