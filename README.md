# OmniDice

## *Uses accelerometer and hall effect sensors to provide both a tactile experience and proper seed randomization to simulate various dice rolls*

This project started as a random thought which then took over my week employing leftover scraps from a lightsaber and HID project. So, it's not the most *polished* project but it got the job done and I'll probably do a version 2 at some point with fixes and improvements from what I learned. All that to say, this is definitely a personal quick fun project, not some serious engineering effort.

**

## The idea

I wanted something that I could wield like a tabletop dice, but allowed any number of any faces of dice I'd run across. But virtual dice have some drawbacks I don't like. Specifically, I like something tactile, and computers suck as random

For anyone somehow here unaware, computers don't _do_ random. Rather they fake random through various means to get something functionally random enough. And at the core of most of these systems is something called a 'seed' which ideally you generate via an actual random factor. 

Humans can be random.

So what if I had a device I could pick up, shake around and get a value? And ideally it has some level of feedback to make that act satisfactory. As a bonus, that action should contribute to making the value actually random.

And thus the OmniDice was born! 

**

## How it works

The OmniDice V1 is an Arduino Nano with a small OLED screen, an accelerometer (an Adafruit MPU6050 in this case) to track rapid acceleration, two Hall effect sensors to measure magnetic fields and a neodymium magnet that bounces around the two in a container.

When the device is shaken, the MPU6050 sensor data is used to both trigger the seed generation as well be part of the seed generation. On top of that, a small container with a Hall effect sensor on each side.

<img src="https://github.com/raegun2k/OmniDice/blob/main/images/IMG_0285.jpg" width="400" /> 

There are also 8 buttons on one side that allow you to set the display mode(more on that in a bit) and the amount of dice as well as the number of faces the dice have for that roll. In TTRPG talk this would be something like "1d6" (roll a 6 sided dice) or "3d4" (3 four 4 sided dice) and so on. There's also a "shortcut Fn" button that's kind of like holding Fn or Shift on a keyboard in that it changes the functions of the button to a secondary function, which is a shortcut to common TTRPG dice


The buttons are as follows [default/shortcut]

[Mode / d2]
[subtract 1 to how many dice rolled / d4]
[add 1 to how many dice rolled / d6]
[subtract one from number of faces on the dice / d8]
[add one to number of faces on the dice / d10]
[subtract ten from number of faces on the dice / d12]
[add ten to number of faces on the dice / d20]
[shortcut Fn]

<img src="https://github.com/raegun2k/OmniDice/blob/main/images/DiceSelectorSm.JPG" width="400">

## Display Modes

There are two display modes available. Clicking the "Mode" button will switch between the two.

 1. Total
		 This mode only displays the total of all dice rolled. So if you rolled 2d6 and the two dice produced a 4 and a 3, it would display only 7, the sum of both. This is also the mode that's easiest to read given the small 128px x 30px OLED screen in use


		  
 2. Detail
		 This mode shows the total but also a punctuation delimited list of all the rolls made. So, for example, if you're playing a TTRPG that asks you to roll two d20s with advantage and pick the highest, you'll see both roles listed. If you roll more dice than the screen can display, a truncation icon of [...] will be shown.

