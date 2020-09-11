# Cuttlefish Lights notes:

![](7Colors.gif)![](SwitchOnBeat.gif)

First things first. These started off as my personal notes and how I thought of the project. I might not explain something perfectly, or even right, but I will explain it with how I see how it works and hey, thats good enough for me. This isn't the best formatted README and is very plain, but I hope you find this helpful!
____________________________________________________________________________

## HARDWARE:

Hardware required:

 - Arduino UNO
 - Spectrum Shield: [https://www.sparkfun.com/products/13116](https://www.sparkfun.com/products/13116)
 - ALITOVE WS2812B : https://www.amazon.com/gp/product/B018X04ES2/ref=ppx_yo_dt_b_asin_title_o00_s01?ie=UTF8&psc=1
 - Header pins to stack Spectrum on top of Arduino
 - breadboard wires : male to male, male to female
 - 2 aux cords
 - An aux splitter, one end male, the other two female
 - capacitor 1000uF

Hardware recomended:
5 Volt 10 Amp power supply for light strip : https://www.amazon.com/gp/product/B07Q26YG61/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1

Extras:
Small bread board to add extra functionalities
Potentiometer : can be use to control qualities such as brightness or speed
Other buttons or things you think you might need to add functionalities. You can think of this after you read through this or finish the project.
___________________________________________________________________________

## QUICK NOTE ABOUT SPECTRUM SHIELD:

So what you need to know about the spectrum shield is that it takes in audio from the aux input and splits up the audio into 7 bands. These 7 bands are related to 7 frequency ranges. So lower bands are like the bass notes and higher bands are like high notes. The bands amplitude is related to how frequent that frequency range is during that specific time. So if there is a ton of bass, the lower band's amplitude will increase. This is what the Arduino is reading, the amplitude from the bands. Using this information, you can do many things with the LEDs. If you want to know more about it and read more detailed explanations to get a better understanding or you just want to message me saying how I am dumb and how I described it wrong,  here is the link you need: https://www.sparkfun.com/products/13116
It has some great visuals as well as some code you can look at.
_________________________________________________________________________

## SETUP:

**(Make wires as short as possible!)**

Solder header pins onto left and right side of the Spectrum Shield so that it can stack onto the Arduino's original header pins.
If you don't have header pins and need to connect them a different way (Like I had to at first), the important pins are -

    Arduino --->         Spectrum Shield
    
    Analog in A0  --->      Analog in A0
    
    Analog in A1   --->     Analog in A1
    
    Digital PWM 4   --->    Right side pin 4
    
    Digital PWM 5  -- ->    Right side pin 5
    
    5V        --->          5V
    
    GND        --->         GND

Now the LEDs have 5 total wires. The 3 wires that are together are red(power) green(data line) and black(ground).
So if you do not have the 5 Volt 10 Amp power supply for the light strip and you are powering the lights through the Arduino, you connect the red wire to 5V and the black to Ground.
The data line gets connected to the Digital in 6 pin (It can really be any other pin, I just chose this one to keep my wires organized).
I am not sure, but I think that the data line should only go one way. What I mean by that is that the arduino should connect to the female
side of the arduino. I read in some places that it is bad to try it the other way.
If you connect your wires this way, just know that the Arduino is not powerful enough to power the LED strip to its full potential.
If you try, you will see that when trying to light the strip, the LEDs will start to turn red and just stop.
If you want to see for yourself, this will happen when you try to make every LED white. To make the light white, the Red, Blue, and Green light have to be turned on all the way in the LED.
With evey individual LED drawing the most power it can, it will be too much for the Arduino. If you do not have the power supply for the light strip it is not the end of the world.
To be able to do most things with your light strip, you can just change your brightness down. This is a setting in the code. Setting it around 60% should be good, but not perfect.

If you do have the power supply you need to wire it differently. There are 2 extra wires, one red(power again) and one black(ground again) on both ends of the light strip.
From the 3 wires we talked about before, you only want to connect the green one to pin 6 (remember to connect to the female end of the strip). Leave the power and ground alone.
Now I am assuming your power supply has an adapter to take in normal wires. You need to connect your extra black wire to ground, and extra red wire to power.
You also need to connect the arduino ground to the ground on the adapter.
Lastly, when researching I found out it is a good idea to have a capacitor on the adaptor. The + on the capacitor goes to power, and the - goes to ground. (the shorter leg is ground). I read that it should be a 1000uF capacitor, but I only had 100uF and it works fine (so far haha).
This is supposed to protect the lights from the initial start up. When the LEDs get power they all flash on for a brief second.
What is happening when you connect this power supply is the Arduino doesn't have to worry about powering the LED strip now and can just focus on sending the strip information needed on how to light the LEDs.

Now connect your aux splitter to whatever is playing the music (your phone, laptop, radio, etc.) and then connect one aux from that splitter to your speaker.
Connect the other aux from the splitter to the input of the spectrum shield.
Note: you might think "Why do I need the splitter? The Spectrum has an output aux?". Ok, go ahead and try using that just for fun. You will see it works, but the audio is not nearly as good.


Boom! all of the basic stuff is set up! If you want to add extra features, you gotta figure that out yourself. Most things will be as simple as place a part on a breadboard, power it, ground it, pin it to a new pin in the arduino and add some code. Thats all I did for the delay speed in the code. I just added a potentiometer and read the voltage. The higher the voltage the longer the delay.


### NOTES FOR CONNECTING MORE THAN ONE STRIP:

So the strips I am using have 300 LEDs each strip. If you are connecting more than one than you definitly need an extra power supply. But to connect more than one you just need to set the first strip up like described above. There is connectors on each end of the leds that attach together, and again, it has three wires. What I did was actually cut the power and ground wire on the connector for the second LED strip. I did this so only the data line would pass to the second strip. I then powered the second strip with the power supply like I describe above. The reason I cut the power and ground wires was to separate the light strips into different circuits. I am not the best at circuits honestly haha and I didn't want to overload the lights. By cutting those wires, I know that each strip has 5 volts / 10 amps and that is what I wanted.
Some things to note with having more than one strip. The main thing is if you are using the lights I am using, you won't be able to light all of the LEDs if you are treating them as one long strip(I found this out the hard way). The arduino only has so much memory allocated for Global variables. In the code you are basically making your LEDs an array and you need it to be global. So long story short, the arduino doesn't have enough memory for 600 LEDs. I found that I could get around 550 LEDs working before they stopped working properly and I could get around 590 before the arduino wouldn't upload the code. Its ok though becasue one thing you can do is actually send that same Digital pin 6 to both LEDs. If you do this then you actually don't want to connect them together and just have them be completly seperate. 
Lastly, the more LEDs you have the slower they will be if you are making them move down the strip.
___________________________________________________________________________________

## IN THE CODE:

So one thing that is very useful is the FastLED library. That makes the LEDs super easy to work with.
But at the top of the musicVisualizer.ino is a lot of important settings that you can change depending on your set up.
For example, NUM_LEDS is the number of LEDs you have stringed together. If you have half a strip, or 2 strips together, just changes this number.
You should not have to change anything else in the code to get it to work.

Think of the line "CRGB leds[NUM_LEDS]" as making your LED strip. In the FastLED library, CRGB is an object that represents one RGB pixle, so this is creating an array
of objects that is as long as your strip. Each compartment of the array holds information about that specific LED.
Note: another thing with the FastLED library is that FastLED is also an object. It is the controller and gives us the ability to call certain functions for the lights.

Next we have some variables that we use throughout the code. The first is strobe and this is for the Spectrum Shield. When it is swithced from HIGH to LOW, it changes
what band you can read from. If you look at the readSpectrum() method, that is exactly what we are doing. When you read a band you actually read from 2 ends of the band.
That is why we have audio1 and audio2. I later filter it down to one number with a new array called band, but that was just for organization for me.

Now we have our setup() method. here we declare our pins. The line " FastLED.addLeds<...." is basically setting up the settings for each CRGB object in your array. It is telling the array what type of lights you are using, what pin to read from.
Then we clear the LEDs with the for loop just to make sure all lights start off.
The Serial input setup is for debugging.

The void loop() is where our functionality starts. you can think of this function as our "main" function. (Really the way the arduino works is it has a main function and that just continually calls the loop function, but if you want to learn about that, you can google it). Here is basically where you can decide what behavior your leds have. That leads us to our LEDBehavior.ino.

This is made just for organization to keep all the behaviors separated. This is where different functions are made to make the lights dance on beat, or stay one color, or only change on bass notes etc. whatever you want it to do. These behaviors use the tools.ino file so lets get into that.

Again, this was made just for organization. These are the functions that you can used to build a different behavior. If you need to move the lights, boom there are two functions. If you just need to read the spectrum, boom that's there too.

So basically you can make tools (tools.ino) to build behaviors (LEDBehavior.ino) that your lights will adapt to in musicVisualizer.ino

We are done! WOHOO! So what now?...
MAKE IT BETTER!! There are so many things I want to do to make this better because this was only from one weekend. I want to add so many things, so there has to be things you would want to add. I have a list and I even put it at the bottom.

One thing about the code I feel I should say is that you can change the lights in different ways. You can actually do waaaaay more than what I do. If you look up the documentation and read some you will see.

________________________________________________________________________________________

## SENSITIVITY PROBLEM :

Another important thing to know is that when you change the volume on your computer or phone or whatever is outputting music, this will change the amplitude of the bands that are read from the spectrum. However, if you change the volume on your speaker, it will not change the amplitude of the bands. I wanted to find a way to not have the volume affect the lights but this was the best I could get, so just keep that in mind. This sensitivity issue has an effect on almost every behavior so I am definitely trying to find a way to solve it. After going through some test I found out that the amplitude of the bands and the volume of the music is not connected linearly. Also, the lower bass bands do not get amplified as much as the higher bands when the music is turned up. It is close to linear though, so I think that will help find a solution.

Important: Throughout testing this, I have played music from my computer. After some test, the sensitivity issue also affects music coming from a phone. The phone output is not as high as the computers. Hopefully after fixing the sensitivity issue, this will not matter.

### Update: 
I have worked on the sensitivity problem and I believe I have found a fix. I don't want to delete this yet until I know I am happy with the result.
________________________________________________________________________________________


## LIST OF ADDITIONS:

Variable brightness like the variable speed

Add a way to switch between different programs (I don't know what other word to use. Different way to light up the lights). I'll probably do some hardware first and then add it to the UI.

Way more colors, this was hacky code just to get it started. I want Smooth transition between colors.

Bass slows down. mess with speed

Bass changes directions that lights flow

Gator stuff

Make UI for whole project

scratch:
for lights smoothly: keep track of highest amplitude for each band for a long time over multiple songs. Use that as the denominator of a fraction and read new values.
these values are top of the fracion. do : input/highest = RGB/255
Do for each RGB to create new color
________________________________________________________________________________________


## TROUBLESHOOTING:


    Lights will flash randomly, or are blinking --->         Most likely your wires are too small / too long. It can't handle all the amperage it needs to correctly. Possible bad connections. Example, ground and power wire are hanging and every now and then touching eachother and shorting the circuit.
    
    Lights turn on but go crazy, they show random colors, mainly white  --->      You are not grounded correctly. Your power source is connected to your LED strip correctly, but now you need to also ground it to the arduino
    
    Lights will begin to dim and turn red when more lights get lit up. Sometimes it even stops working completly and stays stuck on red1   --->     Your power source is not strong enough.
