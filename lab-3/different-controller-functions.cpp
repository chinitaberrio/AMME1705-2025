// BANG-BANG controller
// this function runs every 100ms (operating at 10Hz)
unsigned int CalculateControlAction(unsigned int &currentCount, float &setpoint, float &accumulated_error) {

  // bang-bang controller
  if (currentCount < setpoint)
    return maximum_control_action;
  else
    return minimum_control_action;
}


// Proportional only controller
// this function runs every 100ms (operating at 10Hz)
unsigned int CalculateControlAction(unsigned int &currentCount, float &setpoint, float &accumulated_error) {

  float Kp = 3;  // How much to weight the proportional control of the motor
   
  // How far is the current speed from the set point (the error)
  float error = setpoint - currentCount;

  return Kp * error;
}


// Integral only controller
// this function runs every 100ms (operating at 10Hz)
unsigned int CalculateControlAction(unsigned int &currentCount, float &setpoint, float &accumulated_error) {

    float Ki = 1;  // How much to weight the accumulated_error control of the motor
    
    // How far is the current speed from the set point (the error)
    float error = setpoint - currentCount;

    // accumulate the error (this is the integration of the error - the I term)
    accumulated_error += error;
    
    // Clamp the accumulated_error term to prevent windup
    accumulated_error = constrain(accumulated_error, min_accumulated_error, max_accumulated_error);
    
    return Ki * accumulated_error;
}



// PI controller
// this function runs every 100ms (operating at 10Hz)
unsigned int CalculateControlAction(unsigned int &currentCount, float &setpoint, float &accumulated_error) {

  // Combine proportional control with integral control (PI controller)
  float Kp = 2.0;  // How much to weight the proportional control of the motor
  float Ki = 0.4;  // How much to weight the accumulated_error (integral) control of the motor
    
  // How far is the current speed from the set point (the error)
  float error = setpoint - currentCount;

  // accumulate the error (this is the integration of the error - the I term)
  accumulated_error += error;
    
  // Clamp the accumulated_error term to prevent windup
  accumulated_error = constrain(accumulated_error, min_accumulated_error, max_accumulated_error);
    
  return Kp * error + Ki * accumulated_error;
}

