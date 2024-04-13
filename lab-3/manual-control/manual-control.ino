/*
  This code reads a value from analog input (the exact pin is set by analogInPin)
  and outputs a PWM signal (through the analogWrite command) to the pin specified by
  analogOutPin.
  
  Modified from 
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/

// Parameters to change
const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to


// Code to read analog and output PWM
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // Start serial communication. 
  // NOTE: the baud rate needs to be set in the serial monitor/plotter as 115200
  Serial.begin(115200);
}

void loop() {
  // read the analog in value:
  int sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out - this is required as the analog input
  // reads a value from 0 to 1023, and the PWM output takes a value between 0 to 255.
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  // change the analog out value:
  analogWrite(analogOutPin, outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
