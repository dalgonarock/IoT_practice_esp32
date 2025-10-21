#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE UUID 정의
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID_WRITE     "beb5483e-36e1-4688-b7f5-ea07361b26a8"  // LED 제어용 (Write)
#define CHAR_UUID_NOTIFY    "cba1d466-344c-4be3-ab3f-189f80dd7518"  // 센서값 전송용 (Notify)

BLECharacteristic *pWriteChar;   // LED 제어용
BLECharacteristic *pNotifyChar;  // 센서값 전송용

int lightSensorPin = 32;  // 조도센서 → ADC 핀
int ledPin = 25;          // LED → GPIO25 (PWM 가능)

// PWM 설정
const int pwmFreq = 5000;       // PWM 주파수 5kHz
const int pwmResolution = 8;    // 8비트 해상도 (0-255)

bool deviceConnected = false;
bool manualControl = false;     // 수동 제어 모드 플래그
bool reverseMode = true;        // 역방향 모드 (어두울수록 밝게)
unsigned long lastNotifyTime = 0;
const int notifyInterval = 500;  // 500ms마다 센서값 전송

// 연결 상태 관리 콜백
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("========================================");
    Serial.println("✓ 클라이언트 연결됨!");
    Serial.println("========================================");
  }
  
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("========================================");
    Serial.println("✗ 클라이언트 연결 해제됨!");
    Serial.println("========================================");
    // 재연결을 위해 advertising 재시작
    BLEDevice::startAdvertising();
  }
};

// LED 제어 콜백 (Write)
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      String command = String(value.c_str());
      Serial.print("받은 명령: ");
      Serial.println(command);
      
      if (command == "on") {
        manualControl = true;
        ledcWrite(ledPin, 255);  // 최대 밝기
        Serial.println("→ LED ON (최대 밝기)");
      } 
      else if (command == "off") {
        manualControl = true;
        ledcWrite(ledPin, 0);    // LED 끄기
        Serial.println("→ LED OFF");
      }
      else if (command == "auto") {
        manualControl = false;
        Serial.println("→ 자동 모드 (조도센서 제어)");
      }
      else if (command == "reverse") {
        reverseMode = !reverseMode;
        Serial.print("→ 모드 전환: ");
        Serial.println(reverseMode ? "역방향 (어두울수록 밝게)" : "정방향 (밝을수록 밝게)");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  
  // PWM 설정 (ESP32 Arduino Core 3.x 버전용)
  ledcAttach(ledPin, pwmFreq, pwmResolution);
  ledcWrite(ledPin, 0);  // 초기값 0 (꺼진 상태)
  
  Serial.println("========================================");
  Serial.println("  BLE 통신 + 조도센서 LED 자동제어");
  Serial.println("========================================");
  Serial.println("📌 모드: 역방향 (어두울수록 LED 밝게)");
  Serial.println("========================================");
  
  // BLE 초기화
  BLEDevice::init("dont touch me");
  
  // BLE Server 생성
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // BLE Service 생성
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // 1. Write Characteristic (스마트폰 → ESP32: LED 제어)
  pWriteChar = pService->createCharacteristic(
    CHAR_UUID_WRITE,
    BLECharacteristic::PROPERTY_WRITE
  );
  pWriteChar->setCallbacks(new MyCallbacks());
  
  // 2. Notify Characteristic (ESP32 → 스마트폰: 센서값 전송)
  pNotifyChar = pService->createCharacteristic(
    CHAR_UUID_NOTIFY,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  
  // Service 시작
  pService->start();
  
  // Advertising 설정 및 시작
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("✓ BLE 서버 준비 완료!");
  Serial.println("스마트폰에서 'dont touch me' 검색 후 연결하세요.");
  Serial.println("========================================");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // 500ms마다 센서값 읽고 처리
  if (currentTime - lastNotifyTime >= notifyInterval) {
    lastNotifyTime = currentTime;
    
    // 조도센서 값 읽기 (0~4095)
    // 높은 값 = 밝음, 낮은 값 = 어두움
    int lightValue = analogRead(lightSensorPin);
    
    // 조도 값을 PWM 값으로 변환
    int pwmValue;
    if (reverseMode) {
      // 역방향: 어두울수록 LED 밝게 (자동 조명 효과)
      pwmValue = map(lightValue, 0, 4095, 255, 0);
    } else {
      // 정방향: 밝을수록 LED 밝게
      pwmValue = map(lightValue, 0, 4095, 0, 255);
    }
    
    // 자동 모드일 때만 조도센서로 LED 제어
    if (!manualControl) {
      ledcWrite(ledPin, pwmValue);
    }
    
    // 밝기 판단
    String brightness;
    if (lightValue > 3000) {
      brightness = "매우 밝음 ☀️";
    } else if (lightValue > 2000) {
      brightness = "밝음 🌤️";
    } else if (lightValue > 1000) {
      brightness = "보통 ☁️";
    } else if (lightValue > 500) {
      brightness = "어두움 🌙";
    } else {
      brightness = "매우 어두움 🌑";
    }
    
    // 시리얼 모니터 출력
    Serial.print("💡 조도센서: ");
    Serial.print(lightValue);
    Serial.print(" (");
    Serial.print(map(lightValue, 0, 4095, 0, 100));
    Serial.print("%) - ");
    Serial.print(brightness);
    Serial.print(" → LED PWM: ");
    Serial.print(pwmValue);
    
    if (manualControl) {
      Serial.print(" [수동 모드]");
    } else {
      Serial.print(" [자동 모드");
      Serial.print(reverseMode ? "-역방향]" : "-정방향]");
    }
    Serial.println();
    
    // 연결된 상태에서만 스마트폰에 전송
    if (deviceConnected) {
      // JSON 형식으로 데이터 전송
      String sensorData = "{\"light\":" + String(lightValue) + 
                         ",\"pwm\":" + String(pwmValue) + 
                         ",\"percent\":" + String(map(lightValue, 0, 4095, 0, 100)) + 
                         ",\"brightness\":\"" + brightness + "\"}";
      
      pNotifyChar->setValue(sensorData.c_str());
      pNotifyChar->notify();
      
      Serial.println("→ 핸드폰으로 전송 완료!");
    }
    
    Serial.println();
  }
  
  delay(10);
}