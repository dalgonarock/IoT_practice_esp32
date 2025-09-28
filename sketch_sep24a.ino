#define TX_PIN 32  // UART2 TX 핀 (수신측에서는 사용하지 않지만 정의)
#define RX_PIN 33  // UART2 RX 핀
#define LED_PIN 15  // LED 핀

String receivedData = "";
unsigned long lastReceiveTime = 0;
unsigned long noDataTimeout = 5000; // 5초간 데이터 없으면 통신 오류로 판단

void setup() {
  // USB 시리얼 초기화 (디버깅용)
  Serial.begin(115200);
  
  // UART2 초기화 (GPIO 16번 핀으로 수신)
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  
  // LED 핀 초기화
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("=== ESP32 UART 수신기 시작 ===");
  Serial.println("UART2 대기 중... (GPIO33-RX)");
  Serial.println("LED 핀: GPIO34");
  Serial.println("");
  
  lastReceiveTime = millis();
}

void loop() {
  // UART2로부터 데이터 수신 체크
  if (Serial2.available()) {
    receivedData = Serial2.readStringUntil('\n');
    receivedData.trim(); // 앞뒤 공백 제거
    lastReceiveTime = millis();
    
    Serial.println("[수신] 원본 데이터: " + receivedData);
    
    // 수신된 명령 처리
    if (receivedData == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("✓ LED 켜짐");
    }
    else if (receivedData == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("✓ LED 꺼짐");
    }
    else {
      Serial.println("✗ 알 수 없는 명령: " + receivedData);
      Serial.println("  예상 명령: LED_ON, LED_OFF");
      
      // Baud rate 불일치로 깨진 데이터인지 체크
      if (receivedData.length() > 0) {
        Serial.print("  ASCII 코드: ");
        for (int i = 0; i < receivedData.length(); i++) {
          Serial.print((int)receivedData[i]);
          Serial.print(" ");
        }
        Serial.println("");
      }
    }
    Serial.println("");
  }
  
  // 통신 상태 모니터링
  if (millis() - lastReceiveTime > noDataTimeout) {
    Serial.println("⚠️  통신 오류: 5초간 데이터 수신 없음");
    Serial.println("   - Baud rate 설정 확인 필요");
    Serial.println("   - 연결 상태 확인 필요");
    Serial.println("");
    
    // LED 깜박임으로 오류 표시
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
    
    lastReceiveTime = millis(); // 재설정하여 반복 방지
  }
  
  delay(100);
}