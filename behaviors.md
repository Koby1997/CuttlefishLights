# Behavior Descriptions

These behaviors are now controlled seamlessly through the Cuttlefish Lights React desktop application. Parameters like speed, brightness, direction, and specific visual tweaks are handled live via sliders rather than hardcoded variables!

*Note: Some of the gifs might look similar or hard to decipher without the accompanying music, but they showcase the core mechanics of each mode.*

## Solid Color (Formerly allWhite)
Sets the entire strip to a single continuous color. Originally just white, it now supports a full HEX color picker through the dashboard so you can select any color you want.

## Bass Flow (Formerly bassStartsNewColor)
Whenever a heavy bass note hits, a new random color will spawn and flow down the strip. It will keep flowing this color until the next bass note hits.

![](gifs/bassStartsNewColor.gif)

## Music Blend (Formerly smooth)
Blends the raw audio data into a single, cohesive flowing color. Bass frequencies drive the Red values, Mid frequencies drive Green, and High frequencies drive Blue. 

![](gifs/smooth.gif)

## React: OG
This was the first behavior written! A color will flow down the strip if certain frequency bands reach a high enough reading. Bass notes output Red, mid-range notes output Green, and high notes output Blue. If no specific band dominates, it defaults to Yellow.

![](gifs/OG.gif)

## Rainbow
A purely aesthetic flow that cycles through the entire RGB spectrum flawlessly. The speed and density of the rainbow can both be controlled from the dashboard.

![](gifs/rainbow.gif)

## Color Drift (Formerly randomTransition)
A purely ambient mode. It gracefully picks a new random color and the entire strip slowly crossfades to that color.

![](gifs/randomTransition.gif)

## Seven Bounce
The strip is split into 7 distinct sections, one for each frequency band from the MSGEQ7. It acts as a live music spectrum analyzer. The amplitude of each band controls how many lights bounce up within its section!

![](gifs/sevenBounce.gif)

## Seven Colors
Cycles through 7 distinct colors based heavily on which audio frequency is currently the most dominant. Similar to the OG mode, but utilizes the full extended color palette (Red, Orange, Yellow, Green, Blue, Purple, Pink).

![](gifs/sevenColors.gif)

 
## Snake
A single "snake" of light that slithers across the strip. The length of the snake and its color style can be dynamically adjusted. Once it reaches the end, it wraps around and repeats.

![](gifs/snake.gif)

## Sort Visualizer
Turns the LED strip into a live computer science visualizer! It scrambles the pixels into random colors and physically sorts them back into a perfect gradient based on their Hue. You can choose the sorting algorithm from the dashboard.

**1) Selection Sort**
![](gifs/selectionSort.gif)

**2) Bubble Sort**
![](gifs/bubbleSort.gif)

**3) Insertion Sort**
![](gifs/insertionSort.gif)

**4) Heap Sort**
![](gifs/heapSort.gif)

*(Note: Merge Sort and Quick Sort were experimental and removed to favor the stable algorithms above).*

## Switch On Beat
The whole strip illuminates as one solid color, and instantly snaps to a completely new random color whenever a heavy bass beat is heavily detected.

![](gifs/switchOnBeat.gif) 
