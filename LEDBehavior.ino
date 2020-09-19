void allWhite()
{
  FastLED.setBrightness(60); //white takes a lot of power and gets hot
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255,255,255);
  }
  FastLED.show();
}


/*
 * How fast the lights move is dependent on if the bass is hitting or not.
 * Havn't gotten the chance to finish this yet.
 */
//void bassSpeed()
//{
 // readSpectrum();

  //if(band[0] > sensitivity || band[1] > sensitivity)
    //Delay(30);

 // moveLights(true);
 // sevenColors(0);
//}


/*
 * Whenever the bass band hits, a new color will sort of flow from the begining.
 * It will keep flowing this color until the next bass hits.
 */
void bassStartsNewColor(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);
  
  readSpectrum();
  if(isHit(0) == true)
    bassColor = CRGB(random(255),random(255),random(255));

  leds[start] = bassColor;
  FastLED.show();
}


/*
 * Basically the same as the sevenColors except bass color changes randomly
 * Every time the bass notes hit. I gotchu Josiah.
 */
void forJosiah(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);

  readSpectrum();

  int tracker = 0;
  for(int i = 1; i < 7; i++)
  {
    if(band[i] > band[tracker])
      tracker = i;
  }

  if(tracker == 0 || tracker == 1)
    leds[start] = lightSwitch(7);
  else
    leds[start] = lightSwitch(tracker);

  FastLED.show();
}


/*
 * Literally just orange and blue paterns haha Go Gators!
 */
// void goGators()
// {
//   int setting = random(10);

//   switch (setting)
//     {
//         case 0: //Half orange, half blue
//           for(int i = 0; i < NUM_LEDS/2; i++)
//           {
//             leds[i] = CRGB(0, 0, 255); // Blue
//           }
//           for(int i = Num_Leds/2, i < NUM_LEDS; i++)
//           {
//             leds[i] = CRGB(255, 127, 0); //Orange
//           }
//         break;
//         case 1: //Every other, Orange and Blue
//             for(int i = 0, i < NUM_LEDS; i++)
//             {
//               (NUM_LEDS % 2) == 0 ? leds[i] = CRGB(0, 0, 255) : leds[i] = CRGB(255, 127, 0);
//             }
//         break;
//         case 2: //Bands 1-3 = Blue, 4-7 = Orange;
//           readSpectrum();

//         break;
//         case 3:
//             return CRGB(0, 255, 0);
//         break;
//         case 4:
//             return CRGB(0, 255, 255);
//         break;
//         case 5:
//             return CRGB(0, 127, 255);
//         break;
//         case 6:
//             return CRGB(0, 0, 255);
//         break;
// }


/*
 * This was the first behavior written. It is just a simple
 * if else statement to follow the beat. It only has
 * 4 lights but actually works very well.
 * The start parameter works with the moveLights functions.
 * If you want the lights to move from FTB, the start should be 0.
 * If you want the lights to move from BTF, the start should be NUM_LEDS - 1.
 */
void OG(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);
  
  readSpectrum();
  float R = band[0] + band[1];
  float G = band[2] + band[3] + band[4];
  float B = band[5] + band[6];
  //Random equation and priority encoder I came up with. Tuned a little bit
  if ((3*R)/1.25 > (G+B))
    leds[start] = CRGB(250,0,0);
  else if(G > 800)
    leds[start] = CRGB(0,250,0);
  else if(B > 300)
    leds[start] = CRGB(0,0,250);
  else
    leds[start] = CRGB(75,125,0);//yellowish

  FastLED.show();
}


// 255,  G++,  0     //1
// R--,  255,  0     //2
// 0,    255,  B++   //3
// 0,    G--,  255   //4
// R++,  0,    255   //5
// 255,  0,    B--   //6
/*
 * This does a rainbow effect. I just looked at the RGB color picker on google and
 * saw that it starts with Red fully high, increments Green until its full, decrements
 * Red until 0, increments Blue.... etc. 
 * The speed is how fast you want it to switch colors.
 * Future plans are going to be to also adjust the delay.
 */
void rainbow(bool forward, int speed)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;

  for(int i = 0; i < 256; i+=speed)    //1
  {
    moveLights(forward);
    leds[start] = CRGB(255, i, 0);
    FastLED.show();
    delay(20);
  }

  for(int i = 255; i >= 0; i-=speed)   //2
  {
    moveLights(forward);
    leds[start] = CRGB(i, 255, 0);
    FastLED.show();
    delay(20);
  }

  for(int i = 0; i < 256; i+=speed)    //3
  {
    moveLights(forward);
    leds[start] = CRGB(0, 255, i);
    FastLED.show();
    delay(20);
  }

  for(int i = 255; i >= 0; i-=speed)   //4
  {
    moveLights(forward);
    leds[start] = CRGB(0, i, 255);
    FastLED.show();
    delay(20);
  }

  for(int i = 0; i < 256; i+=speed)    //5
  {
    moveLights(forward);
    leds[start] = CRGB(i, 0, 255);
    FastLED.show();
    delay(20);
  }

  for(int i = 255; i >= 0; i-=speed)   //6
  {
    moveLights(forward);
    leds[start] = CRGB(255, 0, i);
    FastLED.show();
    delay(20);
  }
  return;
}


/*
 * Name kinda explains it. Picks a color and slowly changes to that color.
 */
void randomTransition()
{
  int goalR = random(255);
  int goalG = random(255);
  int goalB = random(255);

  int r = leds[0].red;
  int g = leds[0].green;
  int b = leds[0].blue;

  while(leds[0] != CRGB(goalR, goalG, goalB))
  {
    if(r < goalR)
      r++;
    if(r > goalR)
      r--;

    if(g < goalG)
      g++;
    if(g > goalG)
      g--;

    if(b < goalB)
      b++;
    if(b > goalB)
      b--;

    for(int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(r,g,b);
    }
    FastLED.show();
    delay(5); //Most likely going to need this so it isn't so crazy. Can make it variable too
  }
}


/*
 * Strip split into 7 sections, one for each band.
 * Each band is a specific color. How many lights that turn on in each section depends on how high
 * that band's amplitude is.
 * splitting my strand into 7 bands gives me 42 LEDs per band.
 * I separate each light by a white light, just because there is a remainder of 6 and it works
 *
 * Can add parameter in the future to make the number of sections variable.
 * Changes would be to switch the parameter for 7, adjust start and end.
 * Will need to get rid of white lights, that doesn't work with every number.
 */
void sevenBounce()
{
  readSpectrum();
  
  int howManyToLight;
  int sectionlength = NUM_LEDS/7;//42
  float temp;

  for(int i = 0; i < 7; i++)
  {
    temp = ((float) band[i]/(float) sensitivity);
    howManyToLight = temp * sectionlength;

    howManyToLight < 10 ? howManyToLight = 1 : NULL;
    howManyToLight > sectionlength ? howManyToLight = sectionlength : NULL;

    int start = (i*sectionlength) + i; //These need to change if parameter is added
    int end = ((i+1) * sectionlength) + i - 1;
    int lastToLight = start + howManyToLight;

    bounce(start, end, lastToLight, i);
  }

  for(int i = 1; i < 7; i++)  //For the white lights in between each segment
  {
    leds[(((i*sectionlength) + i) - 1)] = CRGB(100,100,100);
  }  

// 0-41, 42, 43-84, 85, 86-127, 128, 129-170, 171, 172-213, 214, 215-256, 257-299
}


/*
 * Each band is connected to a color, the band with the highest
 * amplitude will get output to the lights.
 * The start parameter works with the moveLights functions.
 * If you want the lights to move from FTB, the start should be 0.
 * If you want the lights to move from BTF, the start should be NUM_LEDS - 1.
 */
void sevenColors(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);

  readSpectrum();
  leds[start] = lightSwitch(highestBand());
}


/*
 * This basically creates RGB base on bands 0,3,6.
 * After we find the sensitivity, we can use that as sort of a proportion
 * to know how bright we should light the R, G, and B individually.
 * 
 * (band Reading)/(Sensitivity) = (???Value we want???)/255
 *
 * Doesn't look as good as I thought. I don't know if that is becasue I can
 * code it better and tune it, or if this is the best it gets.
 */
void smooth(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);

  // float oldR = (band[0]) * (255/(float)sensitivity);
  // float oldG = (band[3]) * (255/(float)sensitivity);
  // float oldB = (band[6]) * (255/(float)sensitivity);

  readSpectrum();

  float R = (band[0]) * (255/(float)sensitivity);
  float G = (band[3]) * (255/(float)sensitivity);
  float B = (band[6]) * (255/(float)sensitivity);


// if(oldR > R)
//   R = R - 10;
// if(oldG > G)
//   G = G - 10;
// if(oldB > B)
//   B = B - 10;

if(R > 255) //if over 255, it restarts. So like 257 acts like 2
  R=255;
if(G > 255)
  G=255;
if(B > 255)
  B=255;

  leds[start] = CRGB(R,G,B);
  FastLED.show();
}


/*
 * For this idea, the first light is lit, then the second, then third etc.
 * Once it gets to the end it restarts over the old lights
 */
void snake(bool forward)//needs work
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    readSpectrum();
    leds[i] = lightSwitch(highestBand());
    FastLED.show();
    delay(20);    
  }
  for (int i = 0; i < NUM_LEDS; i++) //defaults to a specific color, just messing around right now
  {
    leds[i] = CRGB(75, 0, 150);
  }
}


/*
 * Uses seperate array to keep track of colors.
 * If you use the CHSV object instead of CRGB, 
 * you can make a color by chaging the Hue
 * so you can sort the seperate colors array array
 * and constantly update the leds[] with the colors[]
 */
void sort(int algorithm)
{

  for(int i = 0; i < NUM_LEDS - 1; i++)
  {
    colors[i] = random(255);
    leds[i] = CHSV(colors[i],255,255);
  }
  FastLED.show();


  switch (algorithm)
    {
        case 0:
          //mergeSort(0, NUM_LEDS - 1);
        break;

        case 1:
          selectionSort(20);
        break;

        case 2:
          bubbleSort(0);
        break;

        case 3:
          insertionSort(0);
        break;

        case 4:
          heapSort(0);
        break;
        
    }
}


/*
 * Allows you to split the LEDs into even sections and have the lights start at different sections.
 * For example, if I split(2), the strip will be cut in half. The lights will seem to flow
 * from the end of the strip and from the middle. The remainder will just be added to last strip.
 * Made for Summer.
 */
// void split(bool forward, int numberOfSections)//Right now is just doing sevenColor multiple times
// {
//   int start;

//   for(int i = 0; i < numberOfSections; i++)
//   {
//     start = ((NUM_LEDS/numberOfSections) * i);
//     if(forward == false)
//       start = NUM_LEDS - start;

//     sevenColors(start);
//   }  
// }// THIS IS BROKE RIGHT NOW BECAUSE I CHANGED HOW sevenColors() WORKS.


/*
 * Whole strip one color, changes to random color when bass hits
 */
void switchOnBeat()
{
  readSpectrum();

  int randomR = random(255);
  int randomG = random(255);
  int randomB = random(255);
  int fade = 60;

  while(!isHit(0) || !(highestBand() == 0))
  {
    FastLED.setBrightness(fade);//fades, easier on eyes
    if(fade > 20)
      fade = fade - 1;
    FastLED.show();
    readSpectrum();
  }
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(randomR, randomG, randomB);
  }
  FastLED.setBrightness(100);
  FastLED.show();

   while(isHit(0) || highestBand() == 0)//This helps it to not constantly flash when the amplitude is over the sensitivity
   {
     //wait
     readSpectrum();
   }
}
