

# Behavior Descriptions
Quick notes: 
The parameter **bool forward** is in a lot of the behaviors. This changes what direction the lights move in.
Some of the gifs look the same or don't show the behavior that well because they are changing based on music that you can't hear, so you put some imagination in it haha.
Sadly I had to take the cuttlefish down when I moved so that is why not all the gifs are with the cuttlefish. For the gifs with the lights on my desk, the "start" of the lights is usually at the back right of the desk.

## allWhite()

Every led will be white. Purpose is supposed to be used as an actual light. White takes a looooot of power though. Soon I will change the color and brightness to be variable.

## bassStartsNewColor(bool forward)

Whenever the bass band hits, a new random color will sort of flow from the beginning. It will keep flowing this color until the next bass hits.

![](gifs/bassStartsNewColor.gif)
 

## forJosiah(bool forward)

Basically the same as the sevenColors() except bass color changes randomly every time a bass note hits. It is kinda hard to see, but thats what it does haha. I gotchu Josiah.

![](gifs/forJosiah.gif)


## OG(bool forward)

This was the first behavior written. It is just a simple if else statement to follow the beat. It only has 4 lights but actually works very well. A color will flow if certain bands reach a high enough reading. Basically red is the bass color, green is midrange, and blue is high notes. If no requirements are met to output one of those 3 colors, it defaults to outputting yellow.

![](gifs/OG.gif)
 

## rainbow(bool forward, int speed)

 1)	255,  G++,  0
 2)	R--,  255,  0
 3)	0,    255,  B++
 4)	0,    G--,  255
 5)	R++,  0,    255
 6)	255,  0,    B--
 
This does a rainbow effect. I just looked at the RGB color picker on google and saw that it starts with Red fully high, increments Green until its full, decrements Red until 0, increments Blue.... etc. The speed is how fast you want it to switch colors. Future plans are going to be to also adjust the delay. Also now that I have worked on this project way more, I know I can write this better, just haven't gotten to it.

![](gifs/rainbow.gif)


## randomTransition()

 Name kinda explains it. Picks a color and slowly changes to that color.
 
![](gifs/randomTransition.gif)


## sevenBounce()

Strip split into 7 sections, one for each band. makes it like a music spectrum analyzer.
Each band is a specific color. How many lights that turn on in each section depends on how high that band's amplitude is. splitting my strand into 7 bands gives me 42 LEDs per band. I separate each light by a white light, just because there is a remainder of 6 and it works. Can add parameter in the future to make the number of sections variable. Changes would be to switch the parameter for 7, adjust start and end.  Will need to get rid of white lights, that doesn't work with every number.

![](gifs/sevenBounce.gif)
 

## sevenColors(bool forward)

Each band is connected to a color, the band with the highest amplitude will get output to the lights. Very similar to OG. Soon I want to allow the user to pick which color goes with which band.

![](gifs/sevenColors.gif)


## smooth(bool forward)

This basically creates a RGB color based on bands 0,3,6. After we find the sensitivity, we can use that as sort of a proportion to know how bright we should light the R, G, and B individually.

 (band Reading)/(Sensitivity) = (???Value we want???)/255
 
 Doesn't look as good as I thought. I don't know if that is because I can  code it better and tune it, or if this is the best it gets.
 
![](gifs/smooth.gif)

 
## snake(bool forward)

For this idea, the first light is lit, then the second, then third etc. Once it gets to the end it defaults all to one color and then restarts. This is like the opposite of **sevenColors()** in a way.

![](gifs/snake.gif)


## sort(int algorithm)

Uses separate array to keep track of colors. If you use the CHSV object instead of CRGB, you can make a color by charging the Hue so you can sort the separate colors array array and constantly update the leds[] with the colors[]. The **int algorithm** chooses which sort method is used.

0)	mergeSort()	//currently broken

1)	selectionSort()

![](gifs/selectionSort.gif)

2)	bubbleSort()

![](gifs/bubbleSort.gif)

3)	insertionSort()

![](gifs/insertionSort.gif)

4)	heapSort()

![](gifs/heapSort.gif)

5)	quickSort()	// currently broken



## switchOnBeat()

Whole strip one color, changes to random color when bass hits. The brightness is maxed when the color changes, but then fades a bit to give a cool effect.

![](gifs/switchOnBeat.gif)


 
