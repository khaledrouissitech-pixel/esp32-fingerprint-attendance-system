#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define BUTTON_PIN 4
#define RX_PIN 16
#define TX_PIN 17
#define GREEN_LED 18
#define RED_LED 19

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

HardwareSerial mySerial(2); // UART2
Adafruit_Fingerprint finger(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // 3600 seconds = 1 hour
uint8_t nextID = 1;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Initialisation..");

  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);
  if (finger.verifyPassword()) {
    lcd.setCursor(0, 1);
    lcd.print("Capteur pret!");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Cap non trouve");
    while (1) delay(1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion WiFi");
  }

  timeClient.begin();

  Serial.println("Connecte au WiFi");
  delay(2000);
  lcd.clear();
}

void loop() {
  timeClient.update();

  if (digitalRead(BUTTON_PIN) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ajout ID:");
    lcd.print(nextID);
    enrollFingerprint(nextID);
    nextID++;
    delay(1000);
    return;
  }

  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_OK) {
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return;

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      int foundID = finger.fingerID;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bienvenue ID :");
      lcd.setCursor(12, 0);
      lcd.print(foundID);

      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);

      sendAttendanceData(foundID);
      delay(3000);

      digitalWrite(GREEN_LED, LOW);
      lcd.clear();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Doigt inconnu");

      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      delay(2000);

      digitalWrite(RED_LED, LOW);
      lcd.clear();
    }
  }
}

void enrollFingerprint(uint8_t id) {
  int p = -1;
  lcd.setCursor(0, 1);
  lcd.print("Placez le doigt");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p == FINGERPRINT_OK) break;
    lcd.setCursor(0, 1);
    lcd.print("Erreur d'image");
    return;
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 1);
    lcd.print("Echec image→Tz");
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("Retirez le doigt");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  lcd.setCursor(0, 1);
  lcd.print("2nd scan...");
  while (finger.getImage() != FINGERPRINT_OK);
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 1);
    lcd.print("Echec 2e img→Tz");
    return;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 1);
    lcd.print("Echec enreg");
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.setCursor(0, 1);
    lcd.print("enregistre ID:");
    lcd.setCursor(10, 1);
    lcd.print(id);
    sendAttendanceData(id);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Echec enreg");
  }

  delay(2000);
  lcd.clear();
}

void sendAttendanceData(int user_id) {
  if (WiFi.status() == WL_CONNECTED) {
    int sessionNumber = getSessionNumber();

    // 🧪 Debugging output
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    Serial.print("Time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.println(minutes);
    Serial.print("Session: ");
    Serial.println(sessionNumber);

    // ❌ If no session is active, skip sending
    if (sessionNumber == 0) {
      lcd.setCursor(0, 1);
      lcd.print("Hors session");
      delay(2000);
      lcd.clear();
      return;
    }

  String url = "http://YOUR_SERVER_IP:3000/api/fingerprint";

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = "fingerprint_id=" + String(user_id) + "&session_number=" + String(sessionNumber);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
      Serial.println("Attendance recorded successfully!");
    } else {
      Serial.print("Error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error: WiFi not connected");
  }
}

int getSessionNumber() {
  timeClient.update(); // Ensure time is fresh
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int totalMinutes = hours * 60 + minutes;

  if (totalMinutes >= 510 && totalMinutes < 600) return 1;   // 08:30–10:00
  if (totalMinutes >= 600 && totalMinutes < 690) return 2;   // 10:00–11:30
  if (totalMinutes >= 750 && totalMinutes < 840) return 3;   // 12:30–14:00
  if (totalMinutes >= 840 && totalMinutes < 930) return 4;   // 14:00–15:30
  if (totalMinutes >= 930 && totalMinutes < 1050) return 5;  // 15:30–17:30 (Extended for testing)

  return 0;
}
