/****************/
/* System Definitions and setup  */
/****************/
#include <FastLED.h>
#include <EEPROM.h>

#define NUM_LEDS 13
#define DATA_PIN 5 // the pin that the LED strip is connected its data line to

/****************/
/* constants */
/****************/

#define BAUD_RATE 9600
#define BRIGHTNESS "b"
#define COLOR "c"
#define ON "on"
#define OFF "off"

/* This is the index address where in memory the data of each of them will be stored */
#define BRIGHTNESS_EEPROM_INDEX 0
#define RED_EEPROM_INDEX 1
#define GREEN_EEPROM_INDEX 2
#define BLUE_EEPROM_INDEX 3

/****************/
/* variables that are changable */
/* default variables that will be changed throughout runtime*/
/****************/
String action;
int brightness;
int offState = 0;
int onState = brightness;
int red, green, blue;


CRGB leds[NUM_LEDS];

/****************/
/* setup on controller startup */
/****************/
void setup() {
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

  // listen for serial input and see if there is data coming in
  while(Serial.available() == 0){}

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
    Serial.println("Changing BRIGHTNESS...");
    changeBrightness(command_action.toInt());
  }

  if(command_type == COLOR) {
    Serial.println("Changing COLOR...");
    changeColor(command_action);
  }
}

/**
 * changes the current rightness
 */
void changeBrightness(int brightVal) {
  if(!isnan(brightVal)) {
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
 * display light
 */
void renderLights() {
  for(int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot].setRGB(red, green, blue);
    FastLED.setBrightness(brightness);
    FastLED.show();
  }
}


/**
 * EEPROM save to EEPROM state
 */

void initBuiltInMem() {
  brightness = EEPROM.read(BRIGHTNESS_EEPROM_INDEX);
  red = EEPROM.read(RED_EEPROM_INDEX);
  green = EEPROM.read(GREEN_EEPROM_INDEX);
  blue = EEPROM.read(BLUE_EEPROM_INDEX);


  //consecutive reset will clear everything in memory if we need to reset everthing
  EEPROM.write(BRIGHTNESS_EEPROM_INDEX, 255);
  EEPROM.write(RED_EEPROM_INDEX, 255);
  EEPROM.write(GREEN_EEPROM_INDEX, 255);
  EEPROM.write(BLUE_EEPROM_INDEX, 255);
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
