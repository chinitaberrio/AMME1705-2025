// Measure Oscilloscope pin from analog input
const int oscilloscopePin = A0;  // This is our oscilloscope 'probe'

long last_measurement_us = 0; // last measurement time in microseconds
long sample_rate_us = 50000; // time between samples in microseconds

int measurementVoltage = 0; // measurement read from pin oscilloscopePin

void setup() {
  Serial.begin(9600); // initialise serial communications at 9600 baud rate
}

void loop() {
  if (last_measurement_us != 0) {
    // wait for the correct time for the next measurement
    while (micros() - last_measurement_us < sample_rate_us) {
    } 
    // make the reading straight away (it would be very close to this)
    last_measurement_us += sample_rate_us;
  }
  else {
    last_measurement_us = micros(); // This is the first measurement
  }
    
  // Read the voltage on pin oscilloscopePin (analog input)
  measurementVoltage = analogRead(oscilloscopePin);

  // map this to a voltage (0 is 0V, 1024 is 5V)
  float measurementValueFloat = 5. * (measurementVoltage / 1024.);
  float timeInSeconds = (float)micros() / (float)1e6;

  // output a comma separated string (CSV) to the serial port
  Serial.print(timeInSeconds,4);
  Serial.print(",");
  Serial.println(measurementValueFloat, 4);
}
