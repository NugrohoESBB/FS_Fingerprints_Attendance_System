#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
#include <elapsedMillis.h>

const char* ssid        = "UGMURO-1";
const char* password    = "Piscok2000";
String GOOGLE_SCRIPT_ID = "AKfycbwD7BqfT8yeQlPuCZLVykr0pzbZiqz0cS-tGpS0NNjDppoe4h75IvCqd2F3aEnM-3jpdw";

#define RX_PIN D3
#define TX_PIN D4

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger(&mySerial);
WiFiClientSecure client;

elapsedMillis timeSinceLastSend;
const unsigned long sendInterval = 5000;

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Sensor sidik jari siap digunakan!");
  } else {
    Serial.println("Gagal menghubungkan ke sensor sidik jari.");
    while (1);
  }
}

void loop() {
  int fingerID = getFingerprintID();
  if (fingerID != -1) {
    Serial.println("Sidik jari ditemukan dengan ID: " + String(fingerID));
    sendDataToReport(fingerID, "Dibaca");
  } else {
    Serial.println("Sidik jari tidak ditemukan.");
    delay(2000);
  }
}

int getFingerprintID() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

void sendDataToReport(int id, String status) {
  if (WiFi.status() == WL_CONNECTED) {
    if (timeSinceLastSend > sendInterval) {
      timeSinceLastSend = 0;
      String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "ID_Siswa=" + String(id) + "&Status=" + status;
      Serial.println("API: " + urlFinal);

      HTTPClient http;
      client.setInsecure();
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);;
      http.begin(client, urlFinal);
      int httpCode = http.GET();
      Serial.println("HTTP Status Code: " + String(httpCode));

      String payload;
      if (httpCode >= -30) {
        payload = http.getString();
        Serial.println("Payload: " + payload);
      } else {
        Serial.println("HTTP request failed");
      }
      http.end();
    }
  }
  //delay(1000);
}
