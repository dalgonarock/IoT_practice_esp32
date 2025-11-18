#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int LDR_PIN = 34;
const char* ssid = "onestar";
const char* password = "22615015";
// Public Broker
const char* mqtt_server = "broker.hivemq.com";  

const int mqtt_port = 1883;
const char* topic = "iot/team7/env";  // 실험 토픽

WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500);
  Serial.print("."); }
  Serial.println("\n[WiFi 연결 완료]");
  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    Serial.print("MQTT 연결 시도...");
    if (client.connect("ESP32Client_teamX")) {
      Serial.println("성공!");
    } else {
      Serial.println("실패! 재시도 대기...");
      delay(2000);
      return;
    }
  }
 int ldr = analogRead(LDR_PIN);
 float temp = dht.readTemperature();
 float hum = dht.readHumidity();

  // JSON 형식으로 데이터 구성
  char payload[100];
  snprintf(payload, sizeof(payload), 
                "{\"temp\":%.1f,\"hum\":%.1f,\"ldr\":%d}",temp, hum, ldr);

  client.publish(topic, payload);
  Serial.printf("[Publish] %s\n", payload);

  client.loop();
  delay(2000);
}
