# ENGR-103-Final
Rhythm Game

# Dependencies
[Adafruit CircutPlayground](https://github.com/adafruit/Adafruit_CircuitPlayground)

[Async Delay](https://github.com/stevemarple/AsyncDelay)

# Inputs

 - Right button
 - Left button
 - Slide switch
 
# Outputs
 - NeoPixels
 - Speaker
 - Serial Monitor

# Rules
When you first boot up the game, you will be in the level select menu. Use the left and right buttons to move to the level you wish to play, which will be displayed both in the number of NeoPixels lit up and Serial Monitor, then use the slide switch to start that level.

When the game has started, there will be several different colors of light that move across the NeoPixels, hitting the button corresponding with the color of the light when it reaches the end will score you points depending on how on-time your press was, with points being subtracted for being slightly before or after the light.

Red indicates a right button press, blue indicates a left button press, and purple indicates that you should press both of the buttons (will only register if you press them within 20ms of each other).

After each time you score, the Serial Monitor will update telling you how many points you scored and what your new total score is.

After you reach the end of the level, indicated by a white light, you will get a small celebration, with the Serial Monitor showing you how many points you scored that run. The game will then send you back to the menu, ready for you to pick out another level to play.
