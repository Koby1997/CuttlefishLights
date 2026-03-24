// LED Graphic Helper
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


// Hardware analog speed reader
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

void daysUntilGraduation(int daysLeft)
{ 
  // ... implementation ...
  safeShow();
  safeDelay(20);
}



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


bool isHit(int b)
{
    // A hit occurs if the band sharply spikes above its recent history (1.5x its EMA)
    long dynamicThreshold = (bandEma[b] * 15L) / 10L; // 1.5x EMA
    
    // We still use global sensitivity as a minimum floor to avoid false positives on quiet noise
    long actualThreshold = (dynamicThreshold > sensitivity) ? dynamicThreshold : sensitivity;
    
    if(band[b] > actualThreshold)
        return true;
    else
        return false;
}

bool isLightHit(int b)
{
    long dynamicThreshold = (bandEma[b] * 12L) / 10L; // 1.2x EMA
    long actualThreshold = (dynamicThreshold > (sensitivity / 2)) ? dynamicThreshold : (sensitivity / 2);

    if(band[b] > actualThreshold)
        return true;
    else
        return false;
}


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
    (band[0] * 1.7) - 110 > 0 ? band[0] = constrain((band[0] * 1.7) - 110, 0, 1023) : band[0] = 0;
    (band[1] * 1.5) - 120 > 0 ? band[1] = constrain((band[1] * 1.5) - 120, 0, 1023) : band[1] = 0;
    band[2] - 90 > 0 ? band[2] = constrain(band[2] - 90, 0, 1023) : band[2] = 0;
    band[3] - 90 > 0 ? band[3] = constrain(band[3] - 90, 0, 1023) : band[3] = 0;
    band[4] - 90 > 0 ? band[4] = constrain(band[4] - 90, 0, 1023) : band[4] = 0;
    band[5] - 90 > 0 ? band[5] = constrain(band[5] - 90, 0, 1023) : band[5] = 0;
    (band[6] * 1.2) - 140 > 0 ? band[6] = constrain((band[6] * 1.2) - 140, 0, 1023) : band[6] = 0; //This doesn't get hit nearly as high either

    for(int i = 0; i < 7; i++)
    {
        totalAmp += band[i];
        
        bandEma[i] = ((long)band[i] * 20L + bandEma[i] * 80L) / 100L;
    }

    emaAmplitude = ((long)totalAmp * 10L + emaAmplitude * 90L) / 100L;

    setSensitivity();
}


void setSensitivity()
{
    // Make sensitivity ~28% of the EMA total amplitude to match original behavior
    sensitivity = (emaAmplitude * 2) / 7; 
    
    if (sensitivity < 60) {
        sensitivity = 60;
    }
    
    // Add a ceiling to prevent "deafness" during universally loud sections.
    if (sensitivity > 750) {
        sensitivity = 750;
    }
}
