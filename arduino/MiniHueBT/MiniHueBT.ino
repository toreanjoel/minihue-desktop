/****************/
/* System Definitions and setup  */
/****************/
#include <FastLED.h>
#include <EEPROM.h>

#define NUM_LEDS 13
#define DATA_PIN 5 // the pin that the LED strip is connected its data line to
#define FRAMES_PER_SECOND  120

/****************/
/* constants */
/****************/

#define BAUD_RATE 9600
#define BRIGHTNESS "b"
#define COLOR "c"
#define MODE "m"
#define ON "on"
#define OFF "off"
#define PING "ping"
#define ANIMATE_STATE 1
#define CUSTOM_STATE 0

/* This is the index address where in memory the data of each of them will be stored */
#define BRIGHTNESS_EEPROM_INDEX 0
#define RED_EEPROM_INDEX 1
#define GREEN_EEPROM_INDEX 2
#define BLUE_EEPROM_INDEX 3
#define MODE_INDEX_INDEX 4

/****************/
/* variables that are changable */
/* default variables that will be changed throughout runtime*/
/****************/
String action;

int state = 0;
int brightness = 255;
int offState = 0;
int onState = brightness;
int red, green, blue;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CRGB leds[NUM_LEDS];

/****************/
/* setup on controller startup */
/****************/
void setup() {
  delay(3000); // 3 second delay for recovery
  //init the memory so we know what to set the variables to after power down
  initBuiltInMem();
  
  // setup the LED strip with the values in variables
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  Serial.begin(BAUD_RATE); // Default connection rate for my BT module
}

/****************/
/* Loop while running */
/****************/
void loop() {  
  //this will display the lights
  renderLights();

  if (Serial.available()) {
    // set the data recieved to the action variable as a string
    action = Serial.readString();
  
    // split the command type and action
    String action_type = getValue(action,'?', 0);
    String action_command = getValue(action,'?', 1);
  
    //check if user turned on or off all the time
    checkOnOffAction();
    
    //check the command type and action
    runCommand(action_type, action_command);
  }
}

/**
 * checkOnOffAction
 * This is where we handle the on and off commands of the arduino
 */
void checkOnOffAction() {
  if (action == OFF) {
    Serial.println("Turning OFF...");
    brightness = offState;
  }
  
  if (action == ON) {
    Serial.println("Turning ON...");
    brightness = onState;
  }
}

/**
 * this will run the commands based on the type of command passed
 */

void runCommand(String command_type, String command_action) {
  if(command_type == BRIGHTNESS) {
    changeBrightness(command_action.toInt());
  }

  if(command_type == COLOR) {
    changeColor(command_action);
  }

  if(command_type == MODE) {
    checkCustomAnimate(command_action);
  }

  // just general status for the current instance and the variables status
  if (command_type == PING) {
    Serial.println("\n");
    Serial.println("---------");
    Serial.println("Brightness");
    Serial.println(brightness);
    Serial.println("---------");
    Serial.println("Red");
    Serial.println(red);
    Serial.println("---------");
    Serial.println("Green");
    Serial.println(green);
    Serial.println("---------");
    Serial.println("Blue");
    Serial.println(blue);
    Serial.println("---------");
    Serial.println("State");
    Serial.println(state);
    Serial.println("\n");
  }
}

/**
 * changes the current rightness
 */
void changeBrightness(int brightVal) {
  if(!isnan(brightVal)) {
    Serial.println("Changing BRIGHTNESS...");
    onState = brightVal;
    brightness = brightVal;
    //here we write to memory the values
    EEPROM.write(BRIGHTNESS_EEPROM_INDEX, brightVal);
  }
}

/**
 * changeColor
 * Here we set the values of the LED strip
 */
void changeColor(String color) {
  String r = getValue(color,'.', 0); // first value is attached to the delimiter for some reason
  String g = getValue(color,'.', 1);
  String b = getValue(color,'.', 2);
  if(r != "" && g != "" && b != "") { // check if they are not ignored or empty
    Serial.println("Changing COLOR...");
    // convert the values to intergers as we use this to set the color state
    int RED = r.toInt();
    int GREEN= g.toInt();
    int BLUE = b.toInt();
    //update the color global variables
    red = RED;
    green = GREEN;
    blue = BLUE;

    //here we write to memory the values
    EEPROM.write(RED_EEPROM_INDEX, RED);
    EEPROM.write(GREEN_EEPROM_INDEX, GREEN);
    EEPROM.write(BLUE_EEPROM_INDEX, BLUE);
  }
}


/**
 * changeState
 * Here we set the state between manual and animation
 */

void checkCustomAnimate(String mode) {
  String selectedMode = getValue(mode,'?', 0); // first value is attached to the delimiter for some reason
  if(selectedMode != "") { // check if they are not ignored or empty
    Serial.println("Changing MODE...");
    int currentMode = selectedMode.toInt();
    state = currentMode;
    EEPROM.write(MODE_INDEX_INDEX, currentMode);
  }
}

/**
 * display light
 */
void renderLights() {
  if (state == ANIMATE_STATE) {
    //  rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm
    bpm();
    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }
  
  if (state == CUSTOM_STATE) {
    for(int dot = 0; dot < NUM_LEDS; dot++) {
      leds[dot].setRGB(red, green, blue);
    }
  }

  FastLED.delay(1000/FRAMES_PER_SECOND);
  FastLED.setBrightness(brightness);
  FastLED.show();
}


/**
 * EEPROM save to EEPROM state
 */

void initBuiltInMem() {
  brightness = EEPROM.read(BRIGHTNESS_EEPROM_INDEX);
  red = EEPROM.read(RED_EEPROM_INDEX);
  green = EEPROM.read(GREEN_EEPROM_INDEX);
  blue = EEPROM.read(BLUE_EEPROM_INDEX);
  state = EEPROM.read(MODE_INDEX_INDEX);


  //consecutive reset will clear everything in memory if we need to reset everthing
  EEPROM.write(BRIGHTNESS_EEPROM_INDEX, 255);
  EEPROM.write(RED_EEPROM_INDEX, 255);
  EEPROM.write(GREEN_EEPROM_INDEX, 255);
  EEPROM.write(BLUE_EEPROM_INDEX, 255);
  EEPROM.write(MODE_INDEX_INDEX, 0);
}

/****************/
/* HELPER METHODS */
/****************/

/**
 * split by delimeter
 */
 String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


/*************/
/* ANIMATIONS */
/*************/

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 25, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
