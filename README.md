# ESP32 Fingerprint Attendance System

---

## Overview

The ESP32 Fingerprint Attendance System is an embedded IoT firmware developed using the ESP32 and AS608 fingerprint sensor.  

The system performs biometric enrollment, fingerprint identification, session-based time validation using NTP, and transmits attendance records to a backend server via HTTP.

This firmware was developed as part of a final year engineering project focused on embedded systems and IoT integration.

---

## System Architecture

The system combines biometric authentication, time validation, and network communication in a single embedded device.

**Main Functional Blocks:**

- Fingerprint acquisition and matching
- Push-button triggered enrollment
- NTP time synchronization
- Session-based attendance validation
- HTTP communication with backend server
- LCD feedback interface
- LED status indicators

---

## Hardware Components

| Component | Description |
|------------|-------------|
| ESP32 DevKit V4 | Main microcontroller with WiFi capability |
| AS608 Fingerprint Sensor | Biometric fingerprint module |
| 16x2 LCD (I2C) | User feedback display |
| Push Button | Triggers fingerprint enrollment |
| Green LED | Successful authentication indicator |
| Red LED | Failed authentication indicator |

---

## Features

- Fingerprint enrollment via physical button
- Automatic fingerprint ID increment
- Biometric identification and search
- Real-time clock synchronization using NTP
- Session-based attendance control
- WiFi connectivity
- HTTP POST data transmission to backend
- LCD-based user interaction
- Visual status indication via LEDs

---

## System Workflow

1. The ESP32 initializes peripherals and connects to WiFi.
2. Time is synchronized using an NTP server.
3. If the enrollment button is pressed:
   - The system captures two fingerprint scans.
   - A fingerprint model is created and stored.
4. When a fingerprint is scanned:
   - The system searches for a matching template.
   - If found, the fingerprint ID is retrieved.
   - The current session is determined based on time.
   - If a valid session is active, attendance data is sent to the backend server.
5. If the fingerprint is not recognized:
   - The system displays an error message.
   - The red LED is activated.

---

## Session Time Configuration

Attendance validation is based on predefined time ranges:

| Session Number | Time Range        | Total Minutes Range |
|----------------|------------------|--------------------|
| 1 | 08:30 – 10:00 | 510 – 600 |
| 2 | 10:00 – 11:30 | 600 – 690 |
| 3 | 12:30 – 14:00 | 750 – 840 |
| 4 | 14:00 – 15:30 | 840 – 930 |
| 5 | 15:30 – 17:30 | 930 – 1050 |

If the current time does not fall within any defined session, attendance transmission is skipped.

---

## Software Stack

| Layer | Technology |
|--------|------------|
| Firmware | C++ (Arduino Framework for ESP32) |
| Networking | WiFi.h |
| HTTP Communication | HTTPClient.h |
| Time Synchronization | NTPClient |
| Biometric Library | Adafruit_Fingerprint |
| Display Interface | LiquidCrystal_I2C |

---

## Backend Communication

The ESP32 sends attendance data via HTTP POST request to the backend server.

**Payload Format:**

```
fingerprint_id=<ID>&session_number=<SESSION>
```

The backend server processes and stores attendance records in a database.

---

## Configuration

Before uploading the firmware, update the following parameters:

```
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String url = "http://YOUR_SERVER_IP:3000/api/fingerprint";
```

Do not upload real credentials to public repositories.

---

## Engineering Focus

This project demonstrates:

- Embedded firmware development
- UART communication
- Peripheral interfacing
- Biometric system integration
- Real-time logic implementation
- HTTP-based IoT communication
- Time-based control systems
- Full-stack embedded-to-server interaction

---

## Project Context

This firmware represents the embedded layer of a complete smart attendance system composed of:

- ESP32 biometric device
- Node.js backend server
- MySQL database
- Web interface for attendance tracking

The project integrates hardware, firmware, networking, and backend development into a unified IoT solution.

---
