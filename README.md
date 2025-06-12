# ENGR-103H-Final-Project
My final project is recreating Bop It using the Arduino CPX board. The player has to give the correct input to the commands given by the CPX board. The commands are given through the NeoPixel lights in the CPX board. The inputs will be taken using the switch, the two buttons, and the accelerometer.

The switch is used to toggle between players 1 and 2
The two buttons are used for the Bop It and Pull It functionality.
The accelerometer is used for the Shake it functionality.
These are the possible commands given to the players. They are randomized.

The LED lights will turn purple, representing the "Bop it" command. The player has to hit the right button before the LED light turns off to earn a point
The LED light will turn yellow, representing the "twist it" command. The player has to shake the CPX board before the LED light turns off
The LED light will turn purple, representing the "pull it" command. The player has to press both buttons before the LED lights turn off
When the player correctly executes each command, the score will increment by 1. As the score increases, the speed at which the commands are given will also increase.

2 players can play this game, so the high score of both players will be stored after the end of the game and displayed before every round starts.

Video Demo: https://youtu.be/Njo2zHew4C8
