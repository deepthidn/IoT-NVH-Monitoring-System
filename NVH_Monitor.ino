#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <MPU6050.h>
#include <U8g2lib.h>

// ---------------- WiFi ----------------
const char* ssid     = "XXXXXXX";
const char* password = "XXXXXXXX";

// ---------------- Twilio ----------------
const char* accountSID = "XXXXXXXXXXXXXXXXXXXXXXXXX";
const char* authToken  = "XXXXXXXXXXXXXXXXXXXXXXXXX";
const char* fromNumber = "+XXXXXXXXXXX";
const char* toNumber   = "+XXXXXXXXXXX";

// ---------------- ThingSpeak ----------------
const char* thingspeakKey = "XXXXXXXXXXXX";

// ---------------- Pins ----------------
#define BUZZER_PIN 12
#define ALERT_PIN  14
#define SOUND_PIN  A0

// ---------------- Thresholds ----------------
#define VIB_THRESHOLD   0.4
#define NOISE_THRESHOLD 50

// ---------------- Timing ----------------
#define SMS_INTERVAL    30000
#define OLED_INTERVAL   800

// ---------------- Objects ----------------
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
MPU6050 mpu;

// ---------------- Timers ----------------
unsigned long lastSMS = 0;
unsigned long lastSend = 0;

// ---------------- Values ----------------
float vibrationVal = 0;
float noiseVal = 0;
float gyroVal = 0;

// ---------------- SMS flags ----------------
bool criticalSMSSent = false;
bool normalSMSSent = false;

// ======================================================
// OLED + SERIAL DISPLAY
// ======================================================
void oledShow(String l1, String l2, String l3) {

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 12, "NVH Monitor");
  oled.drawStr(0, 28, l1.c_str());
  oled.drawStr(0, 44, l2.c_str());
  oled.drawStr(0, 60, l3.c_str());
  oled.sendBuffer();

  Serial.println("----- OLED -----");
  Serial.println(l1);
  Serial.println(l2);
  Serial.println(l3);
  Serial.println("----------------");
}

// ======================================================
//  SMS FUNCTION
// ======================================================
void sendSMS(String msg) {

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://api.twilio.com/**************" +
               String(accountSID) + "/Messages.json";

  String body = "To=" + String(toNumber) +
                "&From=" + String(fromNumber) +
                "&Body=" + msg;

  http.begin(client, url);
  http.setAuthorization(accountSID, authToken);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int code = http.POST(body);

  Serial.print("SMS HTTP CODE: ");
  Serial.println(code);

  Serial.println(http.getString());

  http.end();
}

// ======================================================
//  SETUP
// ======================================================
void setup() {

  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ALERT_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(ALERT_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Wire.begin(4, 5);
  oled.begin();
  mpu.initialize();

  oledShow("System Ready", "", "");
}

// ======================================================
// LOOP
// ======================================================
void loop() {

  int16_t axr, ayr, azr, gxr, gyr, gzr;
  mpu.getMotion6(&axr, &ayr, &azr, &gxr, &gyr, &gzr);

  float ax = axr / 16384.0;
  float ay = ayr / 16384.0;
  float az = azr / 16384.0;

  float gx = gxr / 131.0;
  float gy = gyr / 131.0;
  float gz = gzr / 131.0;

  float vibration = sqrt(ax*ax + ay*ay + az*az) - 1.0;
  float gyro = sqrt(gx*gx + gy*gy + gz*gz);
  int noise = analogRead(SOUND_PIN);

  vibrationVal = vibration;
  noiseVal = noise;
  gyroVal = gyro;

  // ---------------- ALERT LOGIC ----------------
  bool vibAlert = vibration > VIB_THRESHOLD;
  bool noiseAlert = noise > NOISE_THRESHOLD;

  bool criticalAlert = vibAlert && noiseAlert;

  String msg;

  if (criticalAlert) msg = "CRITICAL ALERT!";
  else if (vibAlert) msg = "VIB ALERT";
  else if (noiseAlert) msg = "NOISE ALERT";
  else msg = "NORMAL";

  // ---------------- OUTPUT ----------------
  digitalWrite(BUZZER_PIN, (vibAlert || criticalAlert) ? LOW : HIGH);
  digitalWrite(ALERT_PIN, (noiseAlert || criticalAlert) ? HIGH : LOW);

  // ---------------- OLED ----------------
  static unsigned long lastOledUpdate = 0;
  static String lastMsg = "";

  if (msg != lastMsg || millis() - lastOledUpdate > OLED_INTERVAL) {

    oledShow(
      msg,
      "V:" + String(vibrationVal,2) + " G:" + String(gyroVal,0),
      "N:" + String(noiseVal)
    );

    lastMsg = msg;
    lastOledUpdate = millis();
  }

  // ---------------- RESET ----------------
  if (!vibAlert && !noiseAlert) {
    criticalSMSSent = false;
    normalSMSSent = false;
  }

  
  if (criticalAlert && !criticalSMSSent && millis() - lastSMS > SMS_INTERVAL) {

    sendSMS("CRITICAL ALERT!\nV:" + String(vibrationVal,2) +
            " G:" + String(gyroVal,0) +
            " N:" + String(noiseVal));

    Serial.println("📩 CRITICAL SMS SENT");

    lastSMS = millis();
    criticalSMSSent = true;
  }

  // ---------------- NORMAL SMS  ----------------
  else if (!criticalAlert && (vibAlert || noiseAlert) &&
           !normalSMSSent && millis() - lastSMS > SMS_INTERVAL) {

    String type = vibAlert ? "VIB ALERT" : "NOISE ALERT";

    sendSMS(type +
            "\nV:" + String(vibrationVal,2) +
            " G:" + String(gyroVal,0) +
            " N:" + String(noiseVal));

    Serial.println("📩 NORMAL SMS SENT");

    lastSMS = millis();
    normalSMSSent = true;
  }

  // ---------------- ThingSpeak ----------------
  if (millis() - lastSend > 15000) {

    int flag = criticalAlert ? 2 : (vibAlert || noiseAlert);

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://api.thingspeak*******" +
                 String(thingspeakKey) +
                 "&field1=" + String(vibrationVal) +
                 "&field2=" + String(noiseVal) +
                 "&field3=" + String(flag);

    http.begin(client, url);
    http.GET();
    http.end();

    lastSend = millis();
  }

  delay(50);
}