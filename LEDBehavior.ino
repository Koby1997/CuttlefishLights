/*
 * This was the first behavior written. It is just a simple
 * if else statement to follow the beat. It only has
 * 4 lights but actually works very well.
 * The start parameter works with the moveLights functions.
 * If you want the lights to move from FTB, the start should be 0.
 * If you want the lights to move from BTF, the start should be NUM_LEDS - 1.
 */
void OG(int start)
{
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
}

/*
 * Each band is connected to a color, the band with the highest
 * amplitude will get output to the lights.
 * The start parameter works with the moveLights functions.
 * If you want the lights to move from FTB, the start should be 0.
 * If you want the lights to move from BTF, the start should be NUM_LEDS - 1.
 */
void sevenColors(int start)
{
  readSpectrum();

  int tracker = 0;
  band[0] = 3* band[0];
  band[1] = 2* band[1];
  for(int i = 1; i < 7; i++)
  {
    if(band[i] > band[tracker])
      tracker = i;
  }

  switch (tracker)
  {
    case 0:
      leds[start] = CRGB(255, 0, 0);
      break;
    case 1:
      leds[start] = CRGB(255, 127, 0);
      break;
    case 2:
      leds[start] = CRGB(255, 255, 0);
      break;
    case 3:
      leds[start] = CRGB(0, 255, 0);
      break;
    case 4:
      leds[start] = CRGB(0, 255, 255);
      break;
    case 5:
      leds[start] = CRGB(0, 127, 255);
      break;
    case 6:
      leds[start] = CRGB(0, 0, 255);
      break;
  }
}

void allWhite()
{
  FastLED.setBrightness(60); //white takes a lot of power and gets hot
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255,255,255);
  }
}

void switchOnBeat()//whole strip one color, changes to random color when bass hits
{
  readSpectrum();

  int randomR = random(255);
  int randomG = random(255);
  int randomB = random(255);
  int fade = 60;

  while(band[0] + band[1] < 600) //fades to black, easier on eyes
  {
    FastLED.setBrightness(fade);
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
  delay(30);//So it doesn't just flash during periods where bass is up
}

/*
 * For this idea, the first light is lit, then the second, then third etc.
 * Once it gets to the end it restarts over the old lights
 */
void snake()//needs work
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    sevenColors(i);
    FastLED.show();
    delay(20);    
  }
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(75, 0, 150);
  }
}


void forJosiah(int start)
{



readSpectrum();

  int tracker = 0;
  band[0] = 3* band[0];
  band[1] = 2* band[1];
  for(int i = 1; i < 7; i++)
  {
    if(band[i] > band[tracker])
      tracker = i;
  }

  switch (tracker)
  {
    case 2:
      leds[start] = CRGB(128, 0, 255);
      break;
    case 3:
      leds[start] = CRGB(0, 128, 255);
      break;
    case 4:
      leds[start] = CRGB(255, 0, 0);
      break;
    case 5:
      leds[start] = CRGB(255, 150, 0);
      break;
    case 6:
      leds[start] = CRGB(0, 255, 200);
      break;
    case 1://bass bands
      if(band[0] + band[1] > 600)
      {
        CRGB oldColor = bassColor;
        bassColor = CRGB(random(255),random(255),random(255));
        for (int i = 0; i < NUM_LEDS; i++)
        {
          if(leds[i] == oldColor)
          {
            leds[i] = bassColor;
          }
        }
      }
      leds[start] = bassColor;
      delay(15);
    break;
  }








  // if(band[0] + band[1] > 600)
  // {
  //   CRGB oldColor = bassColor;
  //   bassColor = CRGB(random(255),random(255),random(255));
  //   for (int i = 0; i < NUM_LEDS; i++)
  //   {
  //     if(leds[i] == oldColor)
  //     {
  //       leds[i] = bassColor;
  //     }
  //   }
  // }
  // moveLightsFTB(leds, NUM_LEDS);
  // sevenColors(0);
}
