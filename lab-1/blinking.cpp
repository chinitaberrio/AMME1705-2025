const int LED_PIN = 7;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500); // Wait for 500 millisecond(s)
  digitalWrite(LED_PIN, LOW);
  delay(500); // Wait for 500 millisecond(s)
}
