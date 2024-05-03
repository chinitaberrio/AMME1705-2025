// BANG-BANG controller
// this function runs every 50ms (operating at 20Hz)
int CalculateControlAction(int &currentCount, float &setpoint, float &error_sum) {

  // bang-bang controller
  if (currentCount < setpoint)
    return maximum_control_action;
  else
    return minimum_control_action;
}


// Proportional only controller
// this function runs every 50ms (operating at 20Hz)
int CalculateControlAction(int &currentCount, float &setpoint, float &error_sum) {

  float Kp = 3;  // How much to weight the proportional control of the motor
   
  // How far is the current speed from the set point (the error)
  float error = setpoint - currentCount;

  return Kp * error;
}


// Integral only controller
// this function runs every 50ms (operating at 20Hz)
int CalculateControlAction(int &currentCount, float &setpoint, float &error_sum) {

    float Ki = 1;  // How much to weight the accumulated_error control of the motor
    
    // How far is the current speed from the set point (the error)
    float error = setpoint - currentCount;

  // accumulate the error (this is the integration of the error - the I term)
  error_sum += error;
    
  // Clamp the accumulated_error term to prevent windup
  if (error_sum < min_accumulated_error)
    error_sum = min_accumulated_error;
  else if (error_sum > max_accumulated_error)
    error_sum = max_accumulated_error;

  return Ki * error_sum;
}



// PI controller
// this function runs every 50ms (operating at 20Hz)
int CalculateControlAction(int &currentCount, float &setpoint, float &error_sum) {

  // Combine proportional control with integral control (PI controller)
  float Kp = 2.0;  // How much to weight the proportional control of the motor
  float Ki = 0.4;  // How much to weight the accumulated_error (integral) control of the motor
    
  // How far is the current speed from the set point (the error)
  float error = setpoint - currentCount;

  // accumulate the error (this is the integration of the error - the I term)
  error_sum += error;
    
  // Clamp the accumulated_error term to prevent windup
  if (error_sum < min_accumulated_error)
    error_sum = min_accumulated_error;
  else if (error_sum > max_accumulated_error)
    error_sum = max_accumulated_error;

  return Kp * error + Ki * error_sum;
}

