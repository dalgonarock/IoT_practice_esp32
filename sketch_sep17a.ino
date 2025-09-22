#define LDR_PIN 33                       // LDR → ADC 핀
#define LED_PIN 32                       // LED → GPIO16 (PWM 지원)
#define SAMPLES 5                        // 평균값을 위한 샘플 수

// LED 밝기 제어 변수
int currentBrightness = 0;               // 현재 LED 밝기 (0-255)
int targetBrightness = 0;                // 목표 LED 밝기 (0-255)
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 30; // 밝기 업데이트 간격 (ms)

// 조도 임계값 설정 (LDR 값이 높을수록 어두움)
const int DARK_THRESHOLD = 1500;         // 어두움 판단 기준값 (이 값보다 높으면 어두움)
const int BRIGHT_THRESHOLD = 1000;       // 밝음 판단 기준값 (이 값보다 낮으면 밝음)

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // ADC 설정
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  Serial.println("=== 조도 센서 PWM LED 제어 ===");
  Serial.println("어두우면 서서히 켜지고, 밝으면 서서히 꺼집니다");
  Serial.println("--------------------------------------------");
  delay(100);
}

void loop() {
  // LDR 값 읽기 (여러 번 측정 후 평균)
  long ldrSum = 0;
  for(int i = 0; i < SAMPLES; i++) {
    ldrSum += analogRead(LDR_PIN);
    delay(3);
  }
  float avgLdrValue = ldrSum / (float)SAMPLES;

  // 목표 밝기 결정 (히스테리시스 적용으로 깜빡임 방지)
  if (avgLdrValue > DARK_THRESHOLD) {
    // LDR 값이 높을 때: 어두움 → LED 켜기
    targetBrightness = 255;
  } 
  else if (avgLdrValue < BRIGHT_THRESHOLD) {
    // LDR 값이 낮을 때: 밝음 → LED 끄기
    targetBrightness = 0;
  }
  // 중간값일 때는 현재 목표 밝기 유지 (깜빡임 방지)

  // 부드러운 밝기 변화 적용
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    
    // 현재 밝기를 목표 밝기로 점진적으로 변경
    if (currentBrightness < targetBrightness) {
      currentBrightness += 3;  // 켜지는 속도 (숫자가 클수록 빠름)
      if (currentBrightness > targetBrightness) {
        currentBrightness = targetBrightness;
      }
    } 
    else if (currentBrightness > targetBrightness) {
      currentBrightness -= 2;  // 꺼지는 속도 (켜지는 것보다 약간 느리게)
      if (currentBrightness < targetBrightness) {
        currentBrightness = targetBrightness;
      }
    }
    
    // LED 밝기 적용
    analogWrite(LED_PIN, currentBrightness);
    lastUpdate = currentTime;
  }

  // 시리얼 출력 (상태 모니터링)
  Serial.print("LDR: ");
  Serial.print(avgLdrValue, 0);
  
  // 조도 상태 및 LED 동작 표시
  if (avgLdrValue > DARK_THRESHOLD) {
    Serial.print(" (어두움 - LED 켜짐 🌙)");
  } else if (avgLdrValue < BRIGHT_THRESHOLD) {
    Serial.print(" (밝음 - LED 꺼짐 ☀️)");
  } else {
    Serial.print(" (중간 - 상태 유지 🌤️)");
  }
  
  Serial.print(" | LED: ");
  Serial.print(currentBrightness);
  Serial.print("/255 (");
  Serial.print((currentBrightness * 100) / 255);
  Serial.print("%)");
  
  // 목표 밝기와 현재 밝기 비교
  if (currentBrightness < targetBrightness) {
    Serial.print(" ↗️ 점점 밝아짐");
  } else if (currentBrightness > targetBrightness) {
    Serial.print(" ↘️ 점점 어두워짐");
  } else {
    Serial.print(" ✅ 목표 달성");
  }
  
  // 시각적 밝기 바
  Serial.print(" | ");
  int barLength = currentBrightness / 10;  // 0-25 길이
  for(int i = 0; i < 25; i++) {
    if(i < barLength) {
      Serial.print("█");
    } else {
      Serial.print("░");
    }
  }
  Serial.println();

  delay(100);  // 시리얼 출력 간격
}