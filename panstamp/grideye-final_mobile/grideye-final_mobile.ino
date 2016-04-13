#include <Wire.h>
#include "HardwareSerial.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Panstamp
#define RFCHANNEL        0       // Let's use channel 0
#define SYNCWORD1        0xB5    // Synchronization word, high byte
#define SYNCWORD0        0x47    // Synchronization word, low byte
#define SOURCE_ADDR      1       // Sender address
#define DESTINATION_ADDR 2       // Receiver address

// Neopixel Pin
#define PIN            6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

byte part1 = 1;
byte part2 = 2;
byte pixelTempL;
byte pixelTempH;
float celsius;
float celsiusTherm;
int buttonPin = 3;
int buttonpress = 0;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(buttonPin, INPUT);    // button as input
  digitalWrite(buttonPin, HIGH);
  
  panstamp.radio.setChannel(RFCHANNEL);
  panstamp.radio.setSyncWord(SYNCWORD1, SYNCWORD0);
  panstamp.radio.setDevAddress(SOURCE_ADDR);
  panstamp.radio.setCCregs();
  panstamp.radio.disableAddressCheck();
  
  pixels.begin(); // This initializes the NeoPixel library.
}


void loop() {
  
  // 1st Grideye
  read_temp(104);
  delay(10);
  
  // 2nd Grideye
  //read_temp(105);
  //delay(10);
}


//read out temprerature data from grid-eye sensor with addr
void read_temp(int addr) {
  
  CCPACKET txPacket1;
  CCPACKET txPacket2;
    
  pixelTempL=0x80;
  pixelTempH=0x81;
  
  // Panstamp packet 1 - first 32 grid-values
  txPacket1.length = 35;
  txPacket1.data[0] = DESTINATION_ADDR;
  txPacket1.data[1] = part1;
  txPacket1.data[2] = addr;
  
  // Panstamp packet 2 - last 32 grid-values
  txPacket2.length = 35;
  txPacket2.data[0] = DESTINATION_ADDR;
  txPacket2.data[1] = part2;
  txPacket2.data[2] = addr;
    
  //read pixels for packet 1
  for(int pixel = 0; pixel < 32; pixel++){
  
    Wire.beginTransmission(addr);
    Wire.write(pixelTempL);
    Wire.endTransmission();
    Wire.requestFrom(addr,2);
    byte lowerLevel = Wire.read();
    byte upperLevel = Wire.read(); 
  
    int temperature = ((upperLevel << 8) | lowerLevel);
    if (temperature > 2047){
      temperature = temperature - 4096;
    }
    celsius = temperature*0.25;
    
    txPacket1.data[pixel+3] = celsius;
    
    pixelTempL=pixelTempL+2;
  }
  
  //read pixels for part 2
  for(int pixel = 32; pixel < 64; pixel++){
  
    Wire.beginTransmission(addr);
    Wire.write(pixelTempL);
    Wire.endTransmission();
    Wire.requestFrom(addr,2);
    byte lowerLevel = Wire.read();
    byte upperLevel = Wire.read(); 
    
    int temperature = ((upperLevel << 8) | lowerLevel);
    if (temperature > 2047){
      temperature = temperature - 4096;
    }    
    celsius = temperature*0.25;

    txPacket2.data[pixel-29] = celsius;
    
    pixelTempL=pixelTempL+2;
  }
  
  // Send both Packets
  panstamp.radio.sendData(txPacket1);
  panstamp.radio.sendData(txPacket2);
}
