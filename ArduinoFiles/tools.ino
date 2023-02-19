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
            delay(15);
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

    recentValues[recentCount] = totalAmp;
    recentCount == 49 ? recentCount = 0 : recentCount++;

    setSensitivity();

    //Saved for testing purposes. I hate when I have to write all of this out haha
    // Serial.println("These are the the bands: ");
    // Serial.print("Band 0:  ");
    // Serial.print(band[0]);
    // Serial.print("   Band1:  ");
    // Serial.print(band[1]);
    // Serial.print("   Band1:  ");
    // Serial.print(band[2]);
    // Serial.print("   Band3:  ");
    // Serial.print(band[3]);
    // Serial.print("   Band4:  ");
    // Serial.print(band[4]);
    // Serial.print("   Band5:  ");
    // Serial.print(band[5]);
    // Serial.print("   Band6:  ");
    // Serial.print(band[6]);
    // Serial.print("  Sensitivity:  ");
    // Serial.println(sensitivity);
}


/*
 * So this is my answer to the Sensitivity issue right now. Basically
 * we keep track of the 50 most recent readings from the spectrum.
 * We are reading from all 7 bands.
 * Then we find the average of those readings and we make the sensitivity
 * a little higher. We want it higher because we want specific actions to take
 * place when they hit harder, not when they are average.
 * To find the average we need to divide by 350 because we are reading
 * 50 integers from 7 different bands (7*50 is 350). We want the average
 * for one band.
 */
void setSensitivity()//sensitivity for first 50 readings will be weird
{
    long numerator = 0;
    int max = 0;
    for(int i = 0; i < 50; i++)
    {
        max < recentValues[i] ? max = recentValues[i] : NULL;
        numerator += recentValues[i];
    }

    sensitivity = (numerator / 350) * 2;//higher than average, but not too high. After testing this seems to work.
    //kinda weird at really low volumes, but actually works pretty good.
}
