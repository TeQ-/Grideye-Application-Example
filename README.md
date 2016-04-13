# Grideye-Application-Example
This is an application for testing the Panasonic GridEye that we devoloped in the students project "GridEye infrared array: Advanced low cost sensors" in WS15/16 at Bauhaus-Universität Weimar.

The idea of this software was to encourage people to do sports in public. In this simple Application, the Grideye have to be mounted in the height of 1.6m, whereas the participant has to stay 1m away from it.
If he does a squat, the software will detect that and one of the LEDs will light up green. <br/>
This application is very elementary, because our focus was more on testing and investigating than on creating a software!

Needed Hardware: 3 panstamps, GridEye Device with 1 GridEye sensor, LED Device with 5 Neopixels

Steps to get the software running: <br/>

1. Upload the Sketches to the panstamps <br/>

2. Copy the dataset in the directory of the Gesture Recognition Toolkit GUI <br/>

3. Start the Processing sketch <br/>

4. Start the GRT GUI and adjust the following options: <br/>
  - Choose Timeseries Classification Mode <br/>
  - Number of Inputs = 64 <br/>
  - switch to tab "Data Manager" -> Load the dataset "kniebeuge" <br/>
  - switch to tab "Training Tool" -> Train <br/>
  
5. Switch back to Processing and press "1" to confirm the dataset

6. Press "f" for filtering the Values

-> Detection should be running now
  
