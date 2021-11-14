/*
* This is the code that runs on the -cart- for Adam
* it runs on an arduino nano
* it has two servos and 92 GRB LEDs
* this code needs to handle the LED display, servos, 
*           and recieve UART data from the controller
*/

#include <Arduino.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// Fast LED prototype to be used for the infinity mirror
// this is configured for the 10 led test strip, but the mirror will have much more. 
// HARDWARE: currently running on a arduino nano clone

// From the Fast LED library
void juggle();

// Hue values defined for readibility.
// 100 is green 150 is blue
// 50 is yellow 0 is red/pink
uint8_t button_commanded_color;
#define GREEN 100
#define BLUE 150
#define YELLOW 50
#define RED 0

// UART coms stuff
void recvWithEndMarker();
const byte numChars = 6;
char receivedChars[numChars];   /* an array to store the received data */
boolean newData = false;

// Definitions for LED
#define DATA_PIN           A5     /* chosen becuase its close to the 5v and gnd */
#define LED_TYPE           WS2811 /* led driver (specific per strip) */
#define COLOR_ORDER        GRB    /* green red blue order */
#define NUM_LEDS           92     /* count how many leds are in the strip */
CRGB leds[NUM_LEDS];              /* array for the fast led library to control everything with */
#define BRIGHTNESS         100    /* exactly what it sounds like */
#define FRAMES_PER_SECOND  120    /* if this is too low, the transitions are choppy */

void setup() {
  delay(1000);  

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // UART coms
  // Arduino nano only has one serial channel
  // it is also used for programming, so make 
  // sure things are unplugged if you can't flash the board
  Serial.begin(9600);
}

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // juggle animation for the mirror
  juggle();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // getting the serial command
  recvWithEndMarker();

  // Check if the buttons are pressed 
  // updates button color for the juggle animation to use
  if (newData){
    newData = false;
    String s = receivedChars;
    if (s.indexOf('g') > 0){
      // green button pressed
      button_commanded_color = GREEN;     
    }
    if (s.indexOf('r') > 0){
      // red button pressed
      button_commanded_color = RED;
    }
    if (s.indexOf('y') > 0){
      // yellow button pressed
      button_commanded_color = YELLOW;
    }
    if (s.indexOf('b') > 0){
      // blue button pressed
      button_commanded_color = BLUE;
    }
  }
}

// this function takes the serial messages comming in and 
// updates the receivedChars global vairable so that we can 
// check it for incomming button presses
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = 'k';  // you can use whatever you want, I just chose k
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
                rc = endMarker;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

// colored dots, weaving in and out of sync with each other
void juggle() {
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  dothue = button_commanded_color; 
  for( int i = 0; i < 12; i++) {
    leds[beatsin16( i+15, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
  }
}