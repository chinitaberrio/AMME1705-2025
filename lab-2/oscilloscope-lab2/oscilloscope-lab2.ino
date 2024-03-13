// options for the Arduino oscilloscope
bool include_time_in_data = true;
bool use_button_to_start_logging = false;
long sample_rate_us = 10000; // time between samples in microseconds

// Measure Oscilloscope pin from analog input
const int oscilloscopePin1 = A0;  // This is our oscilloscope 'probe' 1
const int oscilloscopePin2 = A1;  // This is our oscilloscope 'probe' 2
const int oscilloscopePin3 = A2;  // This is our oscilloscope 'probe' 3

const int buttonPin = 2;  // This is our oscilloscope 'probe'

long last_measurement_us = 0; // last measurement time in microseconds

int measurementVoltage1 = 0; // measurement read from pin oscilloscopePin
int measurementVoltage2 = 0; // measurement read from pin oscilloscopePin
int measurementVoltage3 = 0; // measurement read from pin oscilloscopePin

int buttonState = HIGH;

void setup() {
  Serial.begin(1000000); // initialise serial communications at 115200
  
  //Serial.println("Press a button connected to pin 2 to start");
  pinMode(buttonPin, INPUT);
}

void loop() {
  if (last_measurement_us != 0) {
    while (micros() - last_measurement_us < sample_rate_us) {
       // wait for the correct time for the next measurement
    } 
    // make the reading straight away (it would be very close to this)
    last_measurement_us += sample_rate_us;
  }
  else {
    last_measurement_us = micros(); // This is the first measurement
  }
  
  if (use_button_to_start_logging) {
    // Wait until the button is detected as 'pressed'
    do{
        buttonState = digitalRead(buttonPin);
    } while (buttonState == HIGH);
  }
  
  // Read the voltage on pin oscilloscopePin (analog input)
  measurementVoltage1 = analogRead(oscilloscopePin1);
  measurementVoltage2 = analogRead(oscilloscopePin2);
  measurementVoltage3 = analogRead(oscilloscopePin3);

  // map this to a voltage (0 is 0V, 1024 is 5V)
  float measurementValueFloat1 = 5. * (measurementVoltage1 / 1024.);
  float measurementValueFloat2 = 5. * (measurementVoltage2 / 1024.);
//  float measurementValueFloat3 = 5. * (measurementVoltage3 / 1024.);
  float timeInSeconds = (float)micros() / (float)1e6;

  // output a comma separated string (CSV) to the serial port
  if (include_time_in_data) {
    Serial.print(timeInSeconds,4);
    Serial.print(",");
  }
  Serial.print(measurementValueFloat1, 3);
  Serial.print(",");
  Serial.println(measurementValueFloat2, 3);
 // Serial.print(",");
//  Serial.println(measurementValueFloat3, 3);
}
