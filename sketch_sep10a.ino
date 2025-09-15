#define BUTTON_PIN 33

void setup() {
  Serial.begin(230400);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 플로터 헤더
  Serial.println("Button_State,Chattering_Level,Stability");
}

void loop() {
  // 더 세밀한 채터링 분석
  int high_count = 0;
  int low_count = 0;
  int transitions = 0;
  int last_reading = digitalRead(BUTTON_PIN);
  
  // 연속된 동일 값의 최대 길이 (안정성 측정)
  int max_stable_sequence = 0;
  int current_stable_sequence = 1;
  
  // 5000번 샘플링으로 더 정밀하게 측정
  for(int i = 0; i < 5000; i++) {
    int current_reading = digitalRead(BUTTON_PIN);
    
    if(current_reading == HIGH) high_count++;
    else low_count++;
    
    if(current_reading != last_reading) {
      transitions++;
      max_stable_sequence = max(max_stable_sequence, current_stable_sequence);
      current_stable_sequence = 1;
    } else {
      current_stable_sequence++;
    }
    
    last_reading = current_reading;
    delayMicroseconds(1);
  }
  
  // 분석 결과
  int dominant_state = (high_count > low_count) ? 0 : 1;
  
  // 채터링 레벨: transitions를 0-10 스케일로 변환
  int chattering_level = min(transitions / 5, 10);  // 5번 전환당 1레벨
  
  // 안정성: 가장 긴 안정 구간의 비율
  int stability = (max_stable_sequence * 100) / 5000;
  
  // 시리얼 플로터 출력
  Serial.print(dominant_state);      // 0 또는 1
  Serial.print(",");
  Serial.print(chattering_level);    // 0-10 (높을수록 심한 채터링)
  Serial.print(",");
  Serial.println(min(stability, 100)); // 0-100% (높을수록 안정적)
  
  delay(5);  // 더 빠른 업데이트
}