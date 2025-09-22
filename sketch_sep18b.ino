#define POT_PIN 33          // 가변저항 중앙 핀 → GPIO33
#define SERVO_PIN 32        // 서보모터 신호 핀 → GPIO32

// 서보모터 PWM 설정
#define SERVO_FREQ 50       // 서보모터 주파수 (50Hz)
#define PWM_RESOLUTION 16   // PWM 해상도 (16bit = 0~65535)

// 서보모터 듀티 사이클 값 (16bit 기준)
#define SERVO_MIN_DUTY 1638  // 0도 (약 2.5% 듀티 사이클)
#define SERVO_MAX_DUTY 8192  // 180도 (약 12.5% 듀티 사이클)

void setup() {
  Serial.begin(115200);       // 시리얼 통신 시작
  
  // 서보모터 PWM 설정
  ledcAttach(SERVO_PIN, SERVO_FREQ, PWM_RESOLUTION);
  
  Serial.println("=== 가변저항으로 서보모터 각도 제어 ===");
  Serial.println("시리얼 모니터와 플로터 모두 지원");
  Serial.println("ADC_Value,Voltage_x100,Servo_Angle,Duty_Cycle");
}

void loop() {
  // 가변저항 값 읽기
  int potValue = analogRead(POT_PIN);      // 0~4095 값 읽기
  float voltage = potValue * (3.3 / 4095.0);  // 전압 변환 (0~3.3V)
  
  // 서보모터 각도 계산 (0~4095를 0~180도로 매핑)
  int servoAngle = map(potValue, 0, 4095, 0, 180);
  
  // 각도를 듀티 사이클로 변환
  int dutyCycle = map(servoAngle, 0, 180, SERVO_MIN_DUTY, SERVO_MAX_DUTY);
  float dutyPercent = (dutyCycle / 65535.0) * 100;  // 듀티 사이클 퍼센트
  
  // 서보모터 각도 설정
  ledcWrite(SERVO_PIN, dutyCycle);
  
  // 시리얼 모니터용 출력 (상세 정보)
  Serial.print("ADC: ");
  Serial.print(potValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | Angle: ");
  Serial.print(servoAngle);
  Serial.print("° | Duty: ");
  Serial.print(dutyPercent, 2);
  Serial.print("% | ");
  
  // 시리얼 플로터용 출력 (콤마로 구분된 숫자만)
  Serial.print("ADC:");
  Serial.print(potValue);
  Serial.print(",Voltage_x100:");
  Serial.print(voltage * 100, 1);  // 전압에 100을 곱해서 보기 쉽게
  Serial.print(",Angle:");
  Serial.print(servoAngle);
  Serial.print(",Duty_x10:");
  Serial.println(dutyPercent * 10, 1);  // 듀티 사이클에 10을 곱해서 보기 쉽게

  delay(500);                    // 0.5초 간격으로 출력
}