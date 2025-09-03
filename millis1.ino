int led_pin = 25;
int led1_pin = 26;
unsigned long ppap = 0;
bool ledstate = LOW;
void setup() {
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  pinMode(led1_pin, OUTPUT);

}

void loop() {
  unsigned long cmillis = millis();
  if(cmillis - ppap >= 1000){
    ppap = cmillis;
    ledstate = !ledstate;
    digitalWrite(led_pin, ledstate);
    digitalWrite(led1_pin, ledstate);
  }

}
