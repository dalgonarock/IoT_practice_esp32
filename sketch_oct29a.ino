#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "onestar"
#define WIFI_PASSWORD "22615015"
#define FIREBASE_HOST "esp32-onestar-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "h16OVmxjcCOuPWcpE3nzv3sVJpCvpc7Q3I7FP04B"

// 부저 핀 설정
#define BUZZER_PIN 4     // 부저 핀 → GPIO4 (원하는 핀으로 변경 가능)

// FirebaseConfig와 FirebaseAuth 객체 선언
FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 5000; // 5초마다 heartbeat 전송

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);  // 부저 핀
  digitalWrite(BUZZER_PIN, LOW); // 부저 초기 상태 OFF

  // WiFi 연결
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println();
  
  // Firebase 설정
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  // Firebase 초기화
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("✅ Firebase Connected");
  
  // 초기 설정
  Firebase.setString(fbdo, "/device/buzzer", "off"); // 부저 초기값
  
  // 🟢 ESP32 온라인 상태로 설정
  Firebase.setString(fbdo, "/device/status", "online");
  Firebase.setInt(fbdo, "/device/lastSeen", millis());
  Serial.println("🟢 Device Status: ONLINE");
}

void loop() {
  // 🔄 주기적으로 heartbeat 전송 (ESP32가 살아있음을 알림)
  if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    Firebase.setInt(fbdo, "/device/lastSeen", millis());
    lastHeartbeat = millis();
    Serial.println("💓 Heartbeat sent");
  }

  // 부저 제어 🔊
  if (Firebase.getString(fbdo, "/device/buzzer")) {
    String buzzerCmd = fbdo.stringData();
    if (buzzerCmd == "on") {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("🔊 부저 ON");
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("🔇 부저 OFF");
    }
  }

  delay(1000);
}