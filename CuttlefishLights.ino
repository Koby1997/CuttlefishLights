#include <FastLED.h>
#include <SoftwareSerial.h>

#define NUM_LEDS 300 //How many LEDs are on your strip
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define BRIGHTNESS 60 // Just the default for now, going to create a setting to make it variable

CRGB leds[NUM_LEDS];
int colors[NUM_LEDS];//used for sorting behaviors. Uses CHSV instead of CRGB

CRGB bassColor = new CRGB(0,0,0); //used for some behaviors that use random colors
int recentValues[50];
int recentCount = 0;
long sensitivity;
bool aboveSensitivity;

// AUDIO INPUT SETUP
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int lightDelay = A2;
int band[7];


//For Bluetooth
SoftwareSerial hc06(2,3);
String cmd="";




void setup() 
{ 
  pinMode(audio1, INPUT);
  pinMode(audio2, INPUT);
  pinMode(lightDelay, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  digitalWrite(strobe, HIGH);
  
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  //Clear the LEDs
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  // Serial and input setup
  Serial.begin(115200);
  Serial.println("\nListening...");

  //Initialize Bluetooth Serial Port
  hc06.begin(115200);
}


void loop()
{
  //Change numbers for fun if you want to mess around. Also change between true and false to change direction.
  //Also, some need music, some don't. So if it is acting weird add music.


  //allWhite();

  //bassSpeed();                //not made yet, don't use

  bassStartsNewColor(true);

  //forJosiah(true);            //hard to explain, weird one tho, kinda broke

  //goGators();                 //not made yet, don't use

  //OG(true);

  //rainbow(true,10);           //try changing the number
  
  //randomTransition();

  //randomTransition2(0, true);

  //sevenBounce();
  //delay(25);
  //FastLED.show();
  
  //sevenColors(true);
  //FastLED.show();

  //sevenFade();

  //smooth(true);
  //delay(5);

  //snake(true);

  //split(true, 5);              //Currently Broke

  //switchOnBeat();              //need to work on

  //sort(1);                     //can change from 1-4

  //daysUntilGraduation(0);//LETS GOOOO

  //daysUntilGraduation2(0);//LETS GOOOO




//   //Read data from HC06
//  	while(hc06.available()>0)
//   {
//  			cmd+=(char)hc06.read();
//  	}

//   //if there is something
//   if(cmd!="")
//   {
//  			Serial.print("Command recieved : ");
//  			Serial.println(cmd);
 			
//  			if(cmd=="RED")
//       {
//  							chooseRGB(1);
//  			}
//       else if(cmd=="GREEN")
//       {
//  							chooseRGB(2);
//  			}else
//       {
//  							chooseRGB(3);
//  			}
//  			cmd=""; //reset cmd
//  	}



// }


// void chooseRGB(int RGB)
// {
//   FastLED.setBrightness(60); //white takes a lot of power and gets hot

//   if(RGB == 1)
//   {
//     for(int i = 0; i < NUM_LEDS; i++)
//     {
//       leds[i] = CRGB(255,0,0);
//     }
//   }
//   else if(RGB == 2)
//   { 
//     for(int i = 0; i < NUM_LEDS; i++)
//     {
//       leds[i] = CRGB(0,255,0);
//     }
//   }
//   else
//   {
//     for(int i = 0; i < NUM_LEDS; i++)
//     {
//       leds[i] = CRGB(0,0,255);
//     }
//   }

//   FastLED.show();


 }