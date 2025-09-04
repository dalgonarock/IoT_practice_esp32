#define BUTTON_PIN 11 // The Arduino Nano ESP32 pin connected to the button
#define LED_PIN    13 // The Arduino Nano ESP32 pin connected to the LED

int button_state = 0;   // variable for reading the pushbutton status

void setup() {
  // Configure the LED pin as a digital output
  pinMode(LED_PIN, OUTPUT);
  // initialize the button pin as an pull-up input (HIGH when the switch is open and LOW when the switch is closed)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // read the state of the pushbutton value:
  button_state = digitalRead(BUTTON_PIN);

  // control LED according to the state of button
  if(button_state == LOW)         // If button is pressing
    digitalWrite(LED_PIN, HIGH); // turn on LED
  else                           // otherwise, button is not pressing
    digitalWrite(LED_PIN, LOW);  // turn off LED
}