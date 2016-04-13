#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            3
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      5
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Panstamp
#define RFCHANNEL        0       // Let's use channel 0
#define SYNCWORD1        0xB5    // Synchronization word, high byte
#define SYNCWORD0        0x47    // Synchronization word, low byte
#define SOURCE_ADDR      1       // Sender address
#define DESTINATION_ADDR 2       // Receiver address

int LEDpos = 0;
bool exerciseStarted = false;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  panstamp.radio.setChannel(RFCHANNEL);
  panstamp.radio.setSyncWord(SYNCWORD1, SYNCWORD0);
  panstamp.radio.setDevAddress(SOURCE_ADDR);
  panstamp.radio.setCCregs();
  panstamp.radio.disableAddressCheck();
  panstamp.setPacketRxCallback(rfPacketReceived);
  
  pixels.begin(); // This initializes the NeoPixel library.
}

void rfPacketReceived(CCPACKET *packet)
{ 
  // If addressed to LED-panstamp
  if (packet->data[0] == 3){
    LEDpos = packet->data[1] - 1;
    SetLED(LEDpos);
  }   
}

void SetLED(int pos){
  //Check if LED Position is between 0 and 4
  if (pos >=0 && pos <=4){
    
    // For the first 4 LEDs:
    if (pos <= 3){
      exerciseStarted = true;
      
      // change color of all LEDs until the current position to green
      for (int i = 0; i<=pos; i++){
        pixels.setPixelColor(pos, pixels.Color(0,150,0));
        pixels.show();
      }  
    }
    
    // The last LED:
    if (pos == 4 && exerciseStarted){
      // last LED green for 100ms
      pixels.setPixelColor(pos, pixels.Color(0,150,0));
      pixels.show();
      delay(100);
      
      // Winning animation (Green blinking of all LEDs)
      for (int j = 0; j< 5; j++){
        for (int i = 0; i<= NUMPIXELS - 1; i++){ 
          // change to green
          pixels.setPixelColor(i, pixels.Color(0,150,0));
        }
        pixels.show();
        delay(200);
        for (int i = 0; i<= NUMPIXELS - 1; i++){ 
          // turn off the LEDs
          pixels.setPixelColor(i, pixels.Color(0,0,0));
        }
        pixels.show();
        delay(200);
      }
      exerciseStarted = false;
    }
  }
}
    
  
void loop() {
  // If not started -> all LEDs red, 1st is blinking
  if (!exerciseStarted){
    for (int i = 1; i<= NUMPIXELS - 1; i++){
      pixels.setPixelColor(i, pixels.Color(150,0,0));
    } 
    pixels.setPixelColor(0, pixels.Color(150,0,0));
    pixels.show();
    delay(100);
    pixels.setPixelColor(0, pixels.Color(0,0,0));
    pixels.show();
    delay(100);
  } 
  
  // If exercise is started -> exercises left = number of red LEDs, next one is blinking
  else {
    for (int i = LEDpos +2 ; i<= NUMPIXELS - 1; i++){
      // All LEDs after the next one are red
      pixels.setPixelColor(i, pixels.Color(150,0,0));
    } 
    pixels.setPixelColor(0, pixels.Color(0,150,0));  
    // Next LED is blinking red
    pixels.setPixelColor(LEDpos+1, pixels.Color(150,0,0));
    pixels.show();
    delay(100);
    pixels.setPixelColor(LEDpos+1, pixels.Color(0,0,0));
    pixels.show(); 
    delay(100); 
  }
} 

