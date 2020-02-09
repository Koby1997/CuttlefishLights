/*
 * This reads the bands from the Spectrum Shield.
 * Changing the strobe changes what band the Spectrum shield
 * will read from. This is why we do the for loop to read
 * all of the bands. Each band also has a left and right
 * side of the band, so we read both and then compare.
 * The highest gets put into our band[] that we will base
 * the lights off of. 
 */
void readSpectrum()
{
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
        if (left > right)
          band[i] = left;
        else
          band[i] = right;  
            
        digitalWrite(strobe, HIGH);
        /* Changing the strobe changes what band the Spectrum shield
         * will read from. This is why we do the for loop to read
         * all of the bands. Each band also has a left and right
         * side of the band, so we read both and then compare.
         * The highest gets put into our band[] that we will base
         * the lights off of.
         */
    }
}


void moveLightsFTB(CRGB leds[], int arrayLength) //Front To Back
{
    for (int i = arrayLength - 1; i > 0; i--)
    {
       leds[i] = leds[i-1];
    }
}

void moveLightsBTF(CRGB leds[], int arrayLength) //Back To Front
{
    for (int i = 0; i < arrayLength; i++)
    {
       leds[i] = leds[i+1];
    }
}


/*
 * This uses a potentiometer to change the time it takes to add a new light.
 * The middle pin of the potentiometer is connected to A2, which is lightDelay.
 * Hooked the potentiometer to the arduino 5V. My readings went from 0 - 100. 
 */
int getDelay() //This is used to change the time it takes to add a new light
{
    int newDelay;
    int x = analogRead(lightDelay);

    if (x < 50)
        newDelay = 0;
    else
        newDelay = x/10;

    return newDelay;
}