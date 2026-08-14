#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <LoRa.h>

// --- LoRa Pins ---
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define BUZZER_PIN 26

// --- WiFi ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- Firebase ---
#define FIREBASE_API_KEY "YOUR_API_KEY"
#define FIREBASE_PROJECT_ID "YOUR_PROJECT_ID"
#define FIREBASE_DATABASE_URL "YOUR_DATABASE_URL"  // Not needed for Firestore

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- Alert Threshold ---
#define ALERT_THRESHOLD 100

void setup() {
  Serial.begin(9600);
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
  
  // Firebase
  config.api_key = FIREBASE_API_KEY;
  auth.user.email = "your-email@gmail.com";  // Optional
  auth.user.password = "your-password";      // Optional
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // LoRa
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa failed!");
    while(1);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  Serial.println("✅ LoRa ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }
    
    int commaIndex = received.indexOf(',');
    if (commaIndex > 0) {
      int pm25 = received.substring(0, commaIndex).toInt();
      int fanStatus = received.substring(commaIndex + 1).toInt();
      
      Serial.print("PM2.5: "); Serial.print(pm25); Serial.print(" µg/m³ | Fans: "); Serial.println(fanStatus ? "ON" : "OFF");
      
      // Buzzer
      if (pm25 > ALERT_THRESHOLD) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }
      
      // Send to Firebase
      if (Firebase.ready()) {
        FirebaseJson json;
        json.add("sawmill", "FAVOUR SAWMILL");
        json.add("pm25", pm25);
        json.add("fan_active", fanStatus);
        json.add("alert", (pm25 > ALERT_THRESHOLD) ? 1 : 0);
        
        String docPath = "sawmill_readings/" + String(millis());
        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", docPath, json)) {
          Serial.println("✅ Sent to Firebase");
        } else {
          Serial.println("❌ Firebase error: " + fbdo.errorReason());
        }
      }
    }
  }
  delay(100);
}
