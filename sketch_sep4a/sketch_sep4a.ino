#define BUTTON_PIN 14 // The Arduino Nano ESP32 pin connected to the button
#define LED_PIN    13 // The Arduino Nano ESP32 pin connected to the LED

int button_state = 0;   // variable for reading the pushbutton status

void setup() {
  // Configure the LED pin as a digital output
  pinMode(LED_PIN, OUTPUT);
  // initialize the button pin as an pull-up input (HIGH when the switch is open and LOW when the switch is closed)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // 시리얼 통신 시작 (115200 baud 권장)
  Serial.begin(115200);
  Serial.println("Setup complete. Waiting for button input...");
}

void loop() {
  // read the state of the pushbutton value:
  button_state = digitalRead(BUTTON_PIN);

  // control LED according to the state of button
  if(button_state == LOW) {        // If button is pressed
    digitalWrite(LED_PIN, HIGH);   // turn on LED
    Serial.println("Button pressed → 1");
  } else {                         // otherwise, button is not pressed
    digitalWrite(LED_PIN, LOW);    // turn off LED
    Serial.println("Button released → 0");
  }

  delay(200); // 너무 많이 찍히지 않게 딜레이 추가
}