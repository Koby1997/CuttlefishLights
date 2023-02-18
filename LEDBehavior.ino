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

void randomTransition2(int wait, bool forward)//think of better name later
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

    leds[0] = CRGB(r,g,b);
    moveLights(forward);

    FastLED.show();
    delay(wait); //Most likely going to need this so it isn't so crazy. Can make it variable too
  }

}


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


void sevenColors(bool forward)
{
  int start;
  forward == true ? start = 0 : start = NUM_LEDS -1;
  moveLights(forward);

  readSpectrum();
  leds[start] = lightSwitch(highestBand());
}

//Mix between sevenBounce and switchOnBeat. Can't use the brightness this time though because that would effect all sections.
void sevenFade()
{
  readSpectrum();
  int sectionLength = NUM_LEDS/7;//42

  for(int section = 0; section < 7; section++)//check each section
  {

    int start = (section * sectionLength) + section;
    int end = start + sectionLength;

    if(!isLightHit(section))//if section is not a hit, lower all RGB values
    {
      Serial.print(section);
      Serial.println(" Is not a hit");
      CRGB currentColor = leds[start];

      //fade 10% each time it doesn't hit
      currentColor.r = currentColor.r * 0.9;
      currentColor.g = currentColor.g * 0.9;
      currentColor.b = currentColor.b * 0.9;

      for(int i = start; i < end; i++)
      {
        leds[i] = currentColor;
      }

    }
    else //section is a hit, set back to max color
    {
      for(int i = start; i < end; i++)
      {
        leds[i] = lightSwitch(section);
      }
    }
  }

  // for(int i = 1; i < 7; i++)  //if you want white lights in between each segment
  // {
  //   leds[(((i*sectionLength) + i) - 1)] = CRGB(255,255,255);
  // } 

  FastLED.setBrightness(100);
  FastLED.show();
  delay(25);

}


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


void snake(bool forward)//needs work
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    readSpectrum();
    leds[i] = lightSwitch(highestBand());
    FastLED.show();
    delay(20);    
  }
  int randomR = random(255);
  int randomG = random(255);
  int randomB = random(255);
  for (int i = 0; i < NUM_LEDS; i++) //defaults to a specific color, just messing around right now
  {
    leds[i] = CRGB(50, 50, 50);
  }
}


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

        case 5:
          //quickSort(0, NUM_LEDS - 1, 0);
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
  FastLED.setBrightness(60);
  FastLED.show();

   while(isHit(0) || highestBand() == 0)//This helps it to not constantly flash when the amplitude is over the sensitivity
   {
     delay(25);
     readSpectrum();
   }
}


void daysUntilGraduation(int daysLeft)
{ 
  int start = 230 - daysLeft;
  moveLights(true);
  leds[start] = CRGB(random(255),random(255),random(255));

  for(int i = 0; i < start; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  for(int i = 231; i < NUM_LEDS-1; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  
  FastLED.show();
  delay(20);
}

void daysUntilGraduation2(int daysLeft)
{ 
  int start = 230;
  leds[start] = CRGB(random(255),random(255),random(255));
  start = start - 7;
  leds[start] = CRGB(0,0,255);
  start = start - 7;
  leds[start] = CRGB(0,85,255);
  start = start - 7;
  leds[start] = CRGB(0,170,255);
  start = start - 7;
  leds[start] = CRGB(0, 255,255);
  start = start - 7;
  leds[start] = CRGB(0, 255,170);
  start = start - 7;
  leds[start] = CRGB(0, 255,85);
  start = start - 7;
  leds[start] = CRGB(0,255,0);
  start = start - 7;
  leds[start] = CRGB(85,255,0);
  start = start - 7;
  leds[start] = CRGB(170,255,0);
  start = start - 7;
  leds[start] = CRGB(255,255,0);
  start = start - 7;
  leds[start] = CRGB(255,170,0);
  start = start - 7;
  leds[start] = CRGB(255,85,0);
  start = start - 7;
  leds[start] = CRGB(255,0,0);
  

  moveLights(false);

  for(int i = 0; i < 230-daysLeft; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  for(int i = 231; i < NUM_LEDS-1; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  
  FastLED.show();
  delay(50);
}