#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>

const char* ssid = "UGMURO-1";
const char* password = "Piscok2000";
String GOOGLE_SCRIPT_ID = "AKfycbwD7BqfT8yeQlPuCZLVykr0pzbZiqz0cS-tGpS0NNjDppoe4h75IvCqd2F3aEnM-3jpdw";

#define RX_PIN D3
#define TX_PIN D4

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger(&mySerial);
HTTPClient http;
WiFiClientSecure client;

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
  Serial.println("Masukkan ID untuk sidik jari baru (1-1000):");
  while (Serial.available() == 0); // Tunggu input ID
  int id = Serial.parseInt();
  if (id < 1 || id > 1000) {
    Serial.println("ID tidak valid. Gunakan ID antara 1-1000.");
    return;
  }

  if (enrollFingerprint(id)) {
    Serial.println("Sidik jari berhasil didaftarkan!");
    sendDataToSheet(id, "REGISTERED");
  } else {
    Serial.println("Gagal mendaftarkan sidik jari.");
    sendDataToSheet(id, "NOT REGISTERED");
  }

  delay(2000);
}

bool enrollFingerprint(int id) {
  int p;

  Serial.println("Letakkan jari di sensor...");
  while ((p = finger.getImage()) != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      continue;
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Kesalahan komunikasi.");
      return false;
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Gagal menangkap gambar.");
      return false;
    }
  }

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Gagal mengonversi gambar ke template.");
    return false;
  }

  Serial.println("Hapus jari dari sensor.");
  delay(2000);

  Serial.println("Letakkan jari yang sama di sensor lagi...");
  while ((p = finger.getImage()) != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      continue;
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Kesalahan komunikasi.");
      return false;
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Gagal menangkap gambar.");
      return false;
    }
  }

  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Gagal mengonversi gambar ke template.");
    return false;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Gagal membuat model sidik jari.");
    return false;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println("Gagal menyimpan model sidik jari.");
    return false;
  }

  return true;
}

void sendDataToSheet(int id, String status) {
  if (WiFi.status() == WL_CONNECTED) {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec";
    urlFinal += "?ID_Siswa=" + String(id);
    urlFinal += "&Status=" + status;

    Serial.println("Mengirim ke URL: " + urlFinal);

    client.setInsecure();
    http.begin(client, urlFinal);
    int httpCode = http.POST(urlFinal);

    if (httpCode >= -11) {
      Serial.println("HTTP Status Code: " + String(httpCode));
      String payload = http.getString();
      Serial.println("Payload: " + payload);
    } else {
      Serial.println("HTTP request gagal, kode: " + String(httpCode));
    }
  }
  // http.end();
}
