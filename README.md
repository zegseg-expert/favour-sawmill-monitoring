# 🌲 Favour Sawmill Monitoring System

Real-time air quality monitoring system for sawmill dust detection.

## Features
- Sharp GP2Y1014 Dust Sensor
- LoRa 433MHz Wireless Communication
- ESP32 Receiver with Buzzer Alarm
- Google Firestore Database
- Live Dashboard

## Hardware
- Transmitter: Arduino Nano + Dust Sensor + Fans + LoRa
- Receiver: ESP32 + LoRa + Buzzer

## API Endpoints
- `POST /api/data` - Receive data from ESP32
- `GET /api/latest` - Get latest reading
- `GET /api/history` - Get last 20 readings

## Deployment
Deployed on Google Cloud Run
