# IoT-Based Noise, Vibration and Harshness (NVH) Monitoring System

## Overview

This project presents an IoT-based Noise, Vibration, and Harshness (NVH) Monitoring System designed for real-time condition monitoring of industrial equipment. The system continuously measures vibration, motion, and noise using an ESP8266 microcontroller, an MPU6050 accelerometer and gyroscope, and an analog sound sensor. It provides local alerts through a buzzer, LED, and OLED display, uploads sensor data to the ThingSpeak cloud platform, and sends SMS notifications using the Twilio API whenever abnormal operating conditions are detected.

---

## Features

* Real-time vibration monitoring using the MPU6050 sensor
* Noise level detection using an analog sound sensor
* Live sensor data displayed on an SH1106 OLED display
* Buzzer and LED alerts for abnormal conditions
* SMS notifications using the Twilio REST API
* Cloud-based data logging using ThingSpeak
* Multi-level fault detection:

  * Normal
  * Vibration Alert
  * Noise Alert
  * Critical Alert (Noise + Vibration)

---

## Hardware Components

* ESP8266 NodeMCU
* MPU6050 Accelerometer and Gyroscope
* Analog Sound Sensor
* SH1106 128×64 OLED Display
* Active Buzzer
* LED
* Breadboard
* Jumper Wires

---

## Software and Technologies

* Arduino IDE
* C++
* ESP8266 WiFi Library
* HTTPClient Library
* Wire Library
* U8g2 Graphics Library
* MPU6050 Library
* ThingSpeak Cloud
* Twilio REST API

---

## System Workflow

1. The ESP8266 reads vibration and motion data from the MPU6050 sensor.
2. Noise levels are measured using the analog sound sensor.
3. Sensor values are compared with predefined threshold values.
4. The OLED display shows the current system status and sensor readings.
5. The buzzer and LED provide immediate local alerts when abnormal conditions occur.
6. SMS notifications are sent through Twilio for vibration, noise, or critical alerts.
7. Sensor data is uploaded to ThingSpeak every 15 seconds for remote monitoring and analysis.

---

## Alert Conditions

| Condition      | System Response                                  |
| -------------- | ------------------------------------------------ |
| Normal         | OLED displays live values                        |
| High Vibration | Buzzer activated and SMS notification            |
| High Noise     | LED activated and SMS notification               |
| Critical Alert | Buzzer, LED, SMS notification, and cloud logging |


---

## Future Enhancements

* Telegram or WhatsApp notifications
* Mobile application for remote monitoring
* MQTT-based communication
* Machine learning for predictive maintenance
* FFT-based vibration analysis
* Support for multiple monitoring nodes

---

## Contributors

**Deepthi D Nayak**
**Deekshith Rajesh Rao**
**Dhanush D Shetty**
B.Tech in Electronics and Communication Engineering

---

## License

This project is intended for educational and academic purposes.
