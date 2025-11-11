#include <WiFi.h>

// Wi-Fi 설정
#define WIFI_SSID "onestar"
#define WIFI_PASS "22615015"

// TCP 설정
WiFiClient client;
#define PC_IP "192.168.92.174"
#define PC_PORT 54321

int count = 0;

void setup() {
  Serial.begin(115200);

  // Wi-Fi 연결
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // TCP 서버 연결
  connectToServer();
}

void connectToServer() {
  Serial.println("Connecting to TCP Server...");
  if (client.connect(PC_IP, PC_PORT)) {
    Serial.println("Connected to server!");
  } else {
    Serial.println("Connection failed!");
  }
}

void loop() {
  // 연결 상태 체크 및 재연결
  if (!client.connected()) {
    Serial.println("Disconnected! Reconnecting...");
    client.stop();
    delay(1000);
    connectToServer();
    return;
  }

  // TCP 데이터 전송
  client.printf("Test Packet %d\n", count);
  Serial.printf("Send: Test Packet %d\n", count);
  
  count++;
  
  // 10번마다 구분선 표시 및 2초 대기
  if (count % 10 == 0) {
    Serial.println("========== 10개 전송 완료 ==========");
    delay(2000);
  }
  
  delay(1000);
}