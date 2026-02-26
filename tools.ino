/*
 * So this is like an LED graphic. The higher the band, the more lights get lit.
 * If you need less lights lit, it tries to slow that down by turning off one
 * light at a time. Can use a delay in the main loop to slow it down more.
 * The delay helps it flow more smoothly, not fast and frantic.
 */
void bounce(int start, int end, int lastToLight, int pick)
{
    int alreadyLit = 0;
    for(int i = start; i < end; i++)
    {
        if(leds[i] != CRGB(0,0,0))
            alreadyLit = i;      
        leds[i] != CRGB(0,0,0) ? alreadyLit = i : NULL;
    }

    lastToLight < alreadyLit ? lastToLight = (alreadyLit - 1) : NULL;

    for(int i = start; i < lastToLight; i++)
    {
        leds[i] = lightSwitch(pick);
    }

    for(int i = lastToLight; i < end + 1; i++)
    {
        leds[i] = CRGB(0,0,0);
    }
}


/*
 * This uses a potentiometer to change the time it takes to add a new light.
 * The middle pin of the potentiometer is connected to A2, which is lightDelay.
 * Hooked the potentiometer to the arduino 5V. My readings went from 0 - 1000. 
 * The Delay() function is read in ms so keep that in mind.
 */
int getDelay()
{
    int newDelay;
    int x = analogRead(lightDelay);

    if (x < 50)
        newDelay = 0;
    else
        newDelay = x/10;

    return newDelay;
}

// ... (Rest of file logic is safe as long as explicit delays are replaced)

// NOTE: bounce() and logic functions are fine.

void daysUntilGraduation(int daysLeft)
{ 
  // ... implementation ...
  safeShow();
  safeDelay(20);
}



/*
 * finds which band is the highest
 */
int highestBand()
{
    int highest = 0;
    for(int i = 1; i < 7; i++)
    {
        if(band[i] > band[highest])
        highest = i;
    }
    return highest;
}


/*
 * Just sees if the band given is higher than the sensitivity
 */
bool isHit(int b)
{
    if(band[b] > sensitivity)
        return true;
    else
        return false;
}

bool isLightHit(int b)
{
    if(band[b] > (sensitivity / 2))
        return true;
    else
        return false;
}


/*
 * Simple switch-case statement to pic different colors for different behaviors
 */
CRGB lightSwitch(int pick)//name for the lols
{
    switch (pick)
    {
        case 0:
            return CRGB(255, 0, 0);//Red
        break;
        case 1:
            return CRGB(255, 127, 0);//Orange
        break;
        case 2:
            return CRGB(255, 255, 0);//Yellow
        break;
        case 3:
            return CRGB(0, 255, 0);//Green
        break;
        case 4:
            return CRGB(0, 0, 255);//Blue
        break;
        case 5:
            return CRGB(145, 0, 255);//Purple
        break;
        case 6:
            return CRGB(255, 0, 210);//Pink
        break;
        //From now on, sort of like special cases

        case 7: //Josiah's case
            if(band[0] > sensitivity || band[1] > sensitivity)
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
            return bassColor;
        break;
  }
}


/*
 * This moves the lights either from Front to Back, or Back to Front.
 */
void moveLights(bool forward)
{
    if(forward == true)
    {
        for(int i = NUM_LEDS - 1; i > 0; i--)
        {
            leds[i] = leds[i-1];
        }
        return;
    }
    else
    {
        for(int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = leds[i+1];
        }
        return;
    }
}


/*
 * This reads the bands from the Spectrum Shield.
 * Changing the strobe changes what band the Spectrum shield will read from.
 * This is why we do the for loop to read all of the bands.
 * Each band also has a left and right side of the band, 
 * so we read both and then compare.
 * The highest gets put into our band[] that we will base the lights off of. 
 */
void readSpectrum()
{
    int totalAmp = 0;
    int left;
    int right;
    digitalWrite(reset, HIGH);
    digitalWrite(reset, LOW);
    for(int i=0; i <7; i++)
    {
        digitalWrite(strobe, LOW);
        delayMicroseconds(30);

        left = analogRead(audio1);
        right = analogRead(audio2);
        //left > right ? band[i] = left : band[i] = right;
        if (left > right)
        {
            band[i] = left;
        }
        else
        {
            band[i] = right;
        }
            
        digitalWrite(strobe, HIGH);
        /* Changing the strobe changes what band the Spectrum shield will read from.
         * This is why we do the for loop to read all of the bands.
         * Each band also has a left and right side of the band, so we read both and then compare.
         * The highest gets put into our band[] that we will base the lights off of.
         */
    }
    /* Bass bands do not get read as high, in order for our sensitivity fix
     * we need to bump up their readings a little. Look at the excell sheet
     * on gitHub if you wanna know more on why. It shows the data.
     * Also, when paused or muted, there is still readings from the bands. 
     * Some higher than others, so I also initially subtract from the band readings
     * to try to make it start at 0
     */
    (band[0] * 1.7) - 110 > 0 ? band[0] = (band[0] * 1.7) - 110 : band[0] = 0;
    (band[1] * 1.5) - 120 > 0 ? band[1] = (band[1] * 1.5) - 120 : band[1] = 0;
    band[2] - 90 > 0 ? band[2] = band[2] - 90 : band[2] = 0;
    band[3] - 90 > 0 ? band[3] = band[3] - 90 : band[3] = 0;
    band[4] - 90 > 0 ? band[4] = band[4] - 90 : band[4] = 0;
    band[5] - 90 > 0 ? band[5] = band[5] - 90 : band[5] = 0;
    (band[6] * 1.2) - 140 > 0 ? band[6] = (band[6] * 1.2) - 140 : band[6] = 0;//This doesn't get hit nearly as high either

    for(int i = 0; i < 7; i++)
    {
        totalAmp += band[i];
    }

    // EMA Optimization: replaces [20] int array and 50-iteration loop.
    // Smoothing factor sets how fast it adapts. New = 10% new + 90% old.
    // Explicitly cast to (long) and use 'L' literals because UNO uses 16-bit ints,
    // and totalAmp*100 easily overflows 32,767, crushing sensitivity to 0.
    emaAmplitude = ((long)totalAmp * 10L + emaAmplitude * 90L) / 100L;

    setSensitivity();
}


/*
 * Sensitivity uses an Exponential Moving Average of total amplitude across all 7 bands.
 * We multiply by 2 (or divide by 3.5 depending on math) to make sensitivity a little higher
 * than average so specific actions trigger on hard hits, not average noise.
 * Original math: (sum of 50 totalAmps / 350) * 2
 * Which simplifies to: (average totalAmp / 7) * 2 = average totalAmp * (2/7) = average totalAmp * 0.28
 */
void setSensitivity()
{
    // Make sensitivity ~28% of the EMA total amplitude to match original behavior
    sensitivity = (emaAmplitude * 2) / 7; 
    
    // Add a strict noise floor minimum. 
    // When music is paused, raw MSGEQ7 noise float can plummet sensitivity to near 0,
    // which causes division-by-tiny-number blowouts, rendering the strip fully lit on pure static.
    if (sensitivity < 60) {
        sensitivity = 60;
    }
}
