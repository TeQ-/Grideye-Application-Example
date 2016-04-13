import oscP5.*;
import netP5.*;
import cc.arduino.*;
import processing.serial.*;

OscP5 oscP5;
Serial port;
NetAddress myBroadcastLocation;

PShape square;
PFont font;
float[][] temp;
float[][] filtertemp;
float[][] finaltemp;
float minCel = 900.0;
float maxCel = -900.0;
int predictedClassLabel;
int counter=0;
String data = "";
String[] list;
boolean filtered = false;
boolean exerciseDetected = false;


// Variables for GRT
int pipelineMode = 2; // Timeseries Mode
int numInputs = 64; 
int numOutputs = 1;
int classifierType = 0; // Dynamic Time Wraping
int exerciseCount = 1;
String selectedDataset = "";


void setup() {
  size(1400, 800, P2D);
  frameRate(25);
  port = new Serial(this, "/dev/ttyUSB0", 115200);
  port.bufferUntil(';');
  font = createFont("Arial",14,true);
  textFont(font,14); 
  square = createShape(RECT, 0, 0, 50, 50);
  square.setFill(color(255, 255, 255));
  square.setStroke(false);
  
  // matrices for the grideye data
  temp = new float[8][8];
  filtertemp = new float[8][8];
  finaltemp = new float[8][8];

  // new instance of oscP5, listening for osc messages on port 5001 
  oscP5 = new OscP5(this,5001);
  // the address of the osc broadcast server
  myBroadcastLocation = new NetAddress("127.0.0.1",5000);
}


void draw() {
  background(0,0,0);
  text(String.format("%.2f",minCel), 120,40);
  text(String.format("%.2f",maxCel), 180,40);

  // Draw the current grid
  for(int y=0; y<8; y++) {
     for(int x=0; x<8; x++) {
        //mapping color
        float c = map(finaltemp[x][y], 0, 1, 0.0, 255.0);  // 0 = black, 1 = red
        square.setFill(color(int(c),0,0));       

        //paint square and text
        shape(square, 51*(x+1)+1, 51*(y+1)+1);
        text(String.format("%.2f",finaltemp[x][y]), 10+51*(x+1),40+51*(y+1));
      }
   }
   
   text(selectedDataset, 250, 40);  // print the name of the selected dataset
   stroke(255,0,0);
   strokeWeight(10); 
   
   if (predictedClassLabel == 0){
       line(800, 300, 1000, 500);
       line(800, 500, 1000, 300);
     }
     
   if (selectedDataset == "kniebeuge"){  // if the rigth dataset is selected
     // Classlabel 0 & 3 are noise and 1,2,5,6 are versions of the exercise 
     if (predictedClassLabel == 0){
       exerciseDetected = false;
     }
     if (predictedClassLabel == 3){
       exerciseDetected = false;
     }
     if (predictedClassLabel == 1){
       line(750, 300, 800, 300);
       line(950, 300, 1000, 300);
       line(800, 500, 950, 500);
       line(700, 450, 800, 500);
       line(950, 500, 1050, 450);
       setLED();
     }
     if (predictedClassLabel == 2){
       line(750, 300, 800, 300);
       line(950, 300, 1000, 300);
       line(800, 500, 950, 500);
       line(700, 450, 800, 500);
       line(950, 500, 1050, 450);
       setLED();
     }
     if (predictedClassLabel == 5){
       line(750, 300, 800, 300);
       line(950, 300, 1000, 300);
       line(800, 500, 950, 500);
       line(700, 450, 800, 500);
       line(950, 500, 1050, 450);
       setLED();
     } 
     if (predictedClassLabel == 6){
       line(750, 300, 800, 300);
       line(950, 300, 1000, 300);
       line(800, 500, 950, 500);
       line(700, 450, 800, 500);
       line(950, 500, 1050, 450);
       setLED();
     } 
   }
}

void setLED(){
  if (!exerciseDetected){
    port.write(exerciseCount);
    exerciseCount++;   
  } 
  if (exerciseCount == 6) exerciseCount = 1;  // Reset after last LED
  exerciseDetected = true;
}

void serialEvent (Serial port) // incoming serial data
{
  //find the end
  data = port.readStringUntil(';');

  /* Convert the incoming serial data to a String
     and afterwards copy them into the temp matrix */
     
  //only consider the grid values (cut off "Sensor1:" in the beginning and ";" at the end)
  data = data.substring(10, data.length() - 1);
  
  //split data string
  list = split(data, ",");

  //get minCel, maxCel and fill temp matrix
  minCel = 900.0;
  maxCel = -900.0;
  int z=63;
  for(int x=0; x<8; x++){
    for(int y=0; y<8; y++){
      if (float(list[z]) > maxCel) {
        maxCel = float(list[z])+0.5;
      }
      if (float(list[z]) < minCel) {
        minCel = float(list[z])-0.5;
      }
      temp[y][x] = float(list[z]);
      z--;
    }
  }
  
  filter(temp);   // filter the grid values
  sendOSCData();  // send them to GRT as OSC message
}

void sendOSCData(){
  OscMessage myOscMessage = new OscMessage("/Data");
  for (int i=0; i<8; i++){
    for (int j=0; j<8; j++){
      myOscMessage.add(finaltemp[i][j]);
    }
  }
  // send an OSC message containing all filtered values of the current grid
  oscP5.send(myOscMessage, myBroadcastLocation);
} 

void filter(float[][] temp){
   if (filtered){
     for(int x=0; x<8; x++){
       for(int y=0; y<8; y++){
         if (temp[x][y] < filtertemp[x][y]){
           finaltemp[x][y] = 0.0;
         }
         else finaltemp[x][y] = 1.0;
       }
     }
   }   
   else {        // not filtered
     for(int x=0; x<8; x++){
       for(int y=0; y<8; y++){
         finaltemp[x][y] = temp[x][y];
       }
     }
   }
}

void oscEvent(OscMessage osc) {
  if (osc.checkAddrPattern("/Prediction") == true) {
    predictedClassLabel = osc.get(0).intValue();
    println(predictedClassLabel);
  }
}

void keyPressed(){
  
  switch(key){
    
    // GRT Setup Message
    case 's':
      sendSetupMessage();
      break;
      
    case 'c':
      sendClassifier();
      break;
    
    // Select an existing Dataset  
    case '1':
      selectDataset("orientierung_v2");
      startTraining();
      break;  
    case '2':
      selectDataset("wischenfilter");
      startTraining();
      break; 
    case '3':
      selectDataset("fingergesten");
      startTraining();
      break; 
    case '4':
      selectDataset("kniebeuge");
      startTraining();
      break; 
      
    // Filter all following grids regarding the current value of every single pixel
    case 'f':
      for(int x=0; x<8; x++){
        for(int y=0; y<8; y++){
          // Threshold for every single pixel is its value in the moment of filtering +3
          filtertemp[x][y] = temp[x][y]+3;
        }
      } 
      filtered = true;   
      break;   
    case 'g':
      for(int x=0; x<8; x++){
        for(int y=0; y<8; y++){
          // Threshold is the overall maxium temperature in the moment of filtering -1
          filtertemp[x][y] = maxCel-1;
        }
      }
      filtered = true; 
      break;
    
    // Take a screenshot by pressing space  
    case ' ':
      saveFrame("screenshot"+str(hour())+str(minute())+str(second())+".png");
      break;
      
    // Manually change the LED position (exercise count) by pressing q-w-e-r-t-z (1-2-3-4-5) 
    case 'q':
      port.write(1);
      break;
    case 'w':
      port.write(2);
      break;
    case 'e':
      port.write(3);
      break;
    case 'r':
      port.write(4);
      break;
    case 't':
      port.write(5);
      break;
    case 'z':
      port.write(5);
      break;
    default:
      break;
  }
  
}

// Automatically sets the right settings in GRT
void sendSetupMessage(){
  OscMessage msg = new OscMessage("/Setup");
  msg.add(pipelineMode);
  msg.add(numInputs);
  msg.add(numOutputs);
  oscP5.send(msg, myBroadcastLocation);  
}

// Sets the right classifier in GRT
void sendClassifier(){
  OscMessage classifier = new OscMessage("/SetClassifier"); 
  classifier.add(classifierType);  // Classifier
  classifier.add(false);           // useScaling
  classifier.add(false);           // useNullRejection
  classifier.add(0.0);             // nullRejectionThreshold
  oscP5.send(classifier, myBroadcastLocation);
}  

// Loads an existing dataset in GRT
void selectDataset (String setname){
  selectedDataset = setname;
  OscMessage dataset = new OscMessage("/LoadTrainingDatasetFromFile");
  dataset.add(setname);
  oscP5.send(dataset, myBroadcastLocation);
}

// Start training of the selected dataset
void startTraining(){
  OscMessage msg = new OscMessage("/Train");
  msg.add(1);
  oscP5.send(msg, myBroadcastLocation);
}
