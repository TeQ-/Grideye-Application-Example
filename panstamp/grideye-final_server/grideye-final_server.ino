#include "HardwareSerial.h"

#define RFCHANNEL        0       // Let's use channel 0
#define SYNCWORD1        0xB5    // Synchronization word, high byte
#define SYNCWORD0        0x47    // Synchronization word, low byte
#define SOURCE_ADDR      2      // Sender address
#define DESTINATION_ADDR 1       // Receiver address

CCPACKET txPacket;
bool start = false;
int ctr = 0;


void setup() {
  Serial.begin(115200);
  
  panstamp.radio.setChannel(RFCHANNEL);
  panstamp.radio.setSyncWord(SYNCWORD1, SYNCWORD0);
  panstamp.radio.setDevAddress(SOURCE_ADDR);
  panstamp.radio.setCCregs();
  panstamp.radio.disableAddressCheck();
  panstamp.setPacketRxCallback(rfPacketReceived);
}

void rfPacketReceived(CCPACKET *packet)
{ 
  // Start with part 1
  if (packet->data[1] == 1) {
    start = true;
  }
  
  if (start) {
      
      // Print sensor number (1 or 2)
      if (packet->data[1] == 1) {
        if (packet->data[2] == 104){
          Serial.print("Sensor1");
        }
        else Serial.print("Sensor2");
        Serial.print(",");
      }
      // Print the grid values of the packet
      for(int i = 3; i < 34; i++){
        Serial.print(packet->data[i]);
        Serial.print(",");
      }
      
      // Print last Value and ";" if its Part 2/2 (+ line break)
      Serial.print(packet->data[34]);
      if (packet->data[1] == 2) {
        Serial.println(";"); 
      }
      else Serial.print(",");
      
      // incoming Serial data from processing
      while(Serial.available()) {
        // Read the led number (= number of the current led position)
        int content = 0;
        content = Serial.read();
        sendLEDdata(content);
      }
  }
}

void sendLEDdata(int LEDcode){
  CCPACKET testpacket;
  testpacket.length = 2;
  testpacket.data[0] = 3;  // Address of LED-panstamp
  testpacket.data[1] = LEDcode; // Current LED position
  
  // Some packets seem to get lost...send it multiple times to make sure that they are received
  panstamp.radio.sendData(testpacket);
  panstamp.radio.sendData(testpacket);
  panstamp.radio.sendData(testpacket);
  panstamp.radio.sendData(testpacket);
  panstamp.radio.sendData(testpacket);
}

void loop() {
}

