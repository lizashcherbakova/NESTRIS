![alt text](https://raw.githubusercontent.com/Elanif/NESTRIS/master/preview.png) 

# NESTRIS

## NESTRIS is a nes tetris clone that aims to be a perfect recreation in every competitive aspect, additionally it aims to lower the input delay to the lowest amount possible.

### Some of its characteristics:
- c++: I went for c++ since it's the language I know best and since it's known for its optimization.
- delays: Playing on an original console with a crt yields 1/2 frame of lag from the crt, 2 frames lag built in the nes, plus controller lag. Playing on a normal emulator yields 4 frames of lag, plug monitor lag and controller lag. Retroarch's run ahead option reduces lag to 2 frames.
- fps: One of the reasons why I chose c++ over a web application is that I couldn't get a broswer game to have exactly 60.0988 fps at all times.
### Why I created this project:
- It's hard to find a ntsc nes and crt if you live in the EU and vice versa.
- Crt's aren't healthy to watch, and they're very hard to move around.
- The NES, its controllers, cartridges, aren't on production anymore, so sooner or later most of the pieces scattered around the world will break (the NES classic is a shitty emulator).
- It's really easy to set up NESTRIS on any pc, no need to configure emulators or download illegal files (I guess this project is illegal tho).
### TO-DO list
- Accuracy of the program must be checked for 1 frame discrepancies.
- Console manager should be changed to a proper (hidable) window.
- A real use interface on a separate window is needed for the settings, right now settings can only be changed through the settings.ini.
- Quality of life.
- Pause (pausing invalidates a run).
- Sound: I tried to understand how the nes sounds work, and while I got the basics I couldn't understand how to emulate sounds from a cartridge.
- More game modes.
- Modding options?
- Fullscreen option: fullscreen could reduce input lag by 1 frame.
- Implement this glitch: when you clear the top line it also clears the bottom line.
- 2 Player mode
- Statistics option: long piece drought, tetris %, etc.
### Issues
- the nes had a 4x16 color palette, but sometimes black is considered transparent ("The first 2 colors of every entry are black and white. However, the first color is actually ignored; regardless of its value, it is treated as a transparent color through which the solid black background is exposed."). My program doesn't have a consistent way to handle this.
- Do all the shader.setVariables things.
### meatfighter.com
This project was only possible thanks to https://meatfighter.com/nintendotetrisai/
