/**
 NEOPIXEL BEST PRACTICES for most reliable operation:
   + Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
   + MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
   + NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
   + AVOID connecting NeoPixels on a LIVE CIRCUIT. If you do, connect GROUND (-) first, then +, then data.

*/

// NeoPixel Library
#include <Adafruit_NeoPixel.h>

#define LED_PIN  9

#define LED_COUNT 24

//   Arg 3 = Pixel type flags, add together as needed:
//    - NEO_GRB: Pixels are wired for GRB bitstream (most NeoPixel products)
//    - NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
Adafruit_NeoPixel eye1(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Passive Infrared Sensor and state
#define PIR 10
int pirState = LOW;
int inputReading = 0;

int noTwitch = 0;

void setup() {
  randomSeed(analogRead(0));

  eye1.begin(); // initial NeoPixel strip object
  eye1.show();  // turn off all pixels
  eye1.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(PIR, INPUT);
}


// Loop should take no more than 1-2 seconds
// todo: use interupt for PIR to allow for longer execute of randomTwitch()
void loop() {
  // Take PIR reading
  inputReading = digitalRead(PIR);

  // Greet guest with animation when PIR detects motion
  if(inputReading == HIGH){
    digitalWrite(13, HIGH);
    eye1.setBrightness(150);
    entranceAnimation(random(40, 60));
    clearStrip();
    if(pirState== LOW){ pirState = HIGH; } // store HIGH value of PIR
    eye1.setBrightness(random(25, 45));
    delay(4000);            // wait a sec
  }else{
    // when inputReading is LOW, clear strip
    if(pirState == HIGH){
      digitalWrite(13, LOW);
      clearStrip();
      pirState = LOW;         // store LOW value of PIR
    }
  }

  if((noTwitch % 2) == 1){
    // delay on even noTwitches
    delay(random(500, 4000));
    noTwitch++;
  }else if(noTwitch < 10){
    randomTwitch(random(random(1,3),random(4,7)));
    noTwitch++;
  }else{
    noTwitch = random(0,5);
    delay(random(3000, 6000));
  }
}

// todo: paramaterize methods below / take strip name
void clearStrip(){
  int off = eye1.Color(0,0,0);

  for(int i=0; i<LED_COUNT;i++){
    eye1.setPixelColor(i, off);
    eye1.show();
  }
}


void randomTwitch(int twitches){
  clearStrip();

  for(int i = 0; i < twitches; i++){
    int randomWait = random(random(350, 500), random(600, 900));

    for(int j = random(1, 3); j < random(3, 5); j++){
      clearStrip();
      int randomStutter = random(random(1,3), random(5,8)) * random(0.425, 0.825);
      eye1.setPixelColor(random(0, LED_COUNT), eye1.Color(random(1, 256), random(1, 256), random(1, 256)));
      eye1.show();
      delay(randomStutter/j);
    }

    clearStrip();
    delay(randomWait);
  }
}

void entranceAnimation(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      eye1.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c = b; c < eye1.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / eye1.numPixels();
        uint32_t color = eye1.gamma32(eye1.ColorHSV(hue)); // hue -> RGB
        eye1.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      eye1.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
