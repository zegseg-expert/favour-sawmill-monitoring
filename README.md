# 🌲 Favour Sawmill Monitoring System

Real-time air quality monitoring using:
- Sharp GP2Y1014 Dust Sensor
- LoRa 433MHz wireless
- ESP32 Receiver with Buzzer
- Firebase Firestore Database
- GitHub Pages Dashboard

## Live Dashboard
Visit: `https://YOUR_USERNAME.github.io/favour-sawmill-monitoring`

## How it works
1. Transmitter sends dust data via LoRa
2. ESP32 Receiver gets data and forwards to Firebase
3. Dashboard fetches data from Firebase and displays it
