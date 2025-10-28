#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID      "onestar"
#define WIFI_PASSWORD  "22615015"

#define SMTP_HOST      "smtp.gmail.com"
#define SMTP_PORT      587

#define AUTHOR_EMAIL    "ings0514@gmail.com"
#define AUTHOR_PASSWORD "zkvu ubur veom wxqf"

// 초음파 센서 핀 설정
#define TRIG_PIN 5
#define ECHO_PIN 18

String strSender          = "star";
String strRecipientName   = "정동현";
String strRecipientMail   = "whdehd338@gmail.com";
String strSubject         = "택배 도착 알림";
String strMsg             = "택배가 도착했습니다!";

SMTPSession smtp;

bool mailSent = false;  // 중복 전송 방지

// 거리 측정 함수
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // cm 단위
  
  return distance;
}

void sendEmail() {
  smtp.debug(1);
  smtp.callback([](SMTP_Status status) {
    Serial.println(status.info());
  });

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = F("mydomain.net");

  session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  session.time.gmt_offset = 9;
  session.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = strSender;
  message.sender.email = AUTHOR_EMAIL;
  message.subject = strSubject;
  message.addRecipient(strRecipientName, strRecipientMail);

  message.text.content = strMsg;
  message.text.charSet = F("utf-8");
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));

  if (!smtp.connect(&session)) {
    Serial.println("❌ SMTP 연결 실패");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("❌ 메일 전송 실패: " + smtp.errorReason());
  } else {
    Serial.println("✅ 메일 전송 성공");
    mailSent = true;  // 전송 완료 표시
  }

  smtp.closeSession();
}

void setup() {
  Serial.begin(115200);
  
  // 초음파 센서 핀 설정
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.print("WiFi 연결 중");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.println(" ✅ WiFi 연결됨");
  Serial.print("IP 주소: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float distance = getDistance();
  
  Serial.print("거리: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // 30cm 이내에 물체 감지 && 아직 메일 안 보냈으면
  if (distance < 30 && distance > 0 && !mailSent) {
    Serial.println("🎁 택배 감지! 이메일 전송 중...");
    sendEmail();
  }
  
  // 물체가 사라지면 다시 감지 가능하도록 초기화
  if (distance > 50) {
    mailSent = false;
  }
  
  delay(1000);  // 1초마다 측정
}