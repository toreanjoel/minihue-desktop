#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    15
#define BRIGHTNESS 100
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define PIN13 13

CRGB leds[NUM_LEDS];
int RED = 255;
int GREEN = 255;
int BLUE = 255;

void setup() {
    Serial.begin(9600);
    // set pin mode so we can use the LED - buildin LED
    pinMode(LED_PIN, OUTPUT);
    // setup the fast LED library
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
}

void loop() {
  // activate the leds with the the updated value where needed for the color
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(RED, GREEN, BLUE);
  }
  FastLED.show();
  serialListener();
}


/**
 * This will be the listerner that will run as long as there is a stream of serial data
 */
void serialListener() {
  while (Serial.available() > 0) {
    String recieved = Serial.readStringUntil('\n');
    char* stringCast;
    char* redString;
    char* greenString;
    char* blueString;
    
    stringCast = recieved.c_str();
    redString = strtok(stringCast, ",");
    greenString = strtok(NULL, ",");
    blueString = strtok(NULL, ",");

    int red = atoi(redString);
    int green = atoi(greenString);
    int blue = atoi(blueString);

    // Process message
    if(recieved != "") {
      RED = red;
      GREEN = green;
      BLUE = blue;
    }
  }
}
