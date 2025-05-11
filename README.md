# Esp32-IOT-WMN(Weather Music Notes):

The component that i have used are ESP32, 0.96 inch Oled and DFplayer mini module

This project uses an ESP32 microcontroller to display weather information, control audio playback, and manage notes on an OLED screen. It integrates various libraries and hardware components to provide a multifunctional IoT device.

---

## Features

- **Weather Display**: Fetches and displays real-time weather information using the OpenWeatherMap API.
- **Audio Player**: Controls audio playback using the DFPlayer Mini module.
- **Notes Management**: Allows users to create and manage notes using buttons.
- **Battery Monitoring**: Displays battery voltage and percentage.
- **WiFi Connectivity**: Automatically connects to WiFi using WiFiManager.
- **Time Display**: Displays the current time in 12-hour format using NTP.

---

## Libraries Used

The following libraries are required for this project:

1. **[Wire](https://www.arduino.cc/en/Reference/Wire)**: For I2C communication.
2. **[WiFi](https://www.arduino.cc/en/Reference/WiFi)**: For WiFi connectivity.
3. **[WiFiManager](https://github.com/tzapu/WiFiManager)**: For managing WiFi connections.
4. **[Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)**: For graphics rendering on the OLED.
5. **[Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)**: For controlling the OLED display.
6. **[NTPClient](https://github.com/arduino-libraries/NTPClient)**: For fetching the current time from an NTP server.
7. **[WiFiUdp](https://www.arduino.cc/en/Reference/WiFiUdp)**: For UDP communication.
8. **[HTTPClient](https://www.arduino.cc/en/Reference/HTTPClient)**: For making HTTP requests.
9. **[ArduinoJson](https://arduinojson.org/)**: For parsing JSON responses from APIs.
10. **[DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini)**: For controlling the DFPlayer Mini audio module.

---

## Hardware Requirements

1. **ESP32**: The main microcontroller.
2. **OLED Display (128x64)**: For displaying information (I2C interface).
3. **DFPlayer Mini**: For audio playback.
4. **Push Buttons**: For navigation and control.
   - **BUTTON_LEFT**: GPIO 23
   - **BUTTON_RIGHT**: GPIO 4
   - **BUTTON_SELECT**: GPIO 19
   - **BUTTON_BACK**: GPIO 18
   - **BUTTON_CLEAR**: GPIO 13
5. **Battery**: For powering the ESP32 and monitoring voltage (connected to GPIO 35).
6. **Resistors**: For voltage divider (100k–100k) to measure battery voltage.

---

## Circuit Connections

| Component       | ESP32 Pin |
| --------------- | --------- |
| OLED SDA        | GPIO 21   |
| OLED SCL        | GPIO 22   |
| DFPlayer RX     | GPIO 16   |
| DFPlayer TX     | GPIO 17   |
| BUTTON_LEFT     | GPIO 23   |
| BUTTON_RIGHT    | GPIO 4    |
| BUTTON_SELECT   | GPIO 19   |
| BUTTON_BACK     | GPIO 18   |
| BUTTON_CLEAR    | GPIO 13   |
| Battery Voltage | GPIO 35   |

---

## Setup Instructions

1. **Install Required Libraries**:

   - Use the Arduino Library Manager to install the libraries listed above.

2. **Configure OpenWeatherMap API**:

   - Replace the `apiKey` and `city` variables in the code with your OpenWeatherMap API key and city name:
     ```cpp
     const String apiKey = "YOUR_API_KEY";
     const String city = "YOUR_CITY_NAME";
     ```

3. **Upload the Code**:

   - Connect your ESP32 to your computer and upload the code using the Arduino IDE.

4. **Connect to WiFi**:

   - On first boot, the ESP32 will create a WiFi access point named `ESP32-IOT`. Connect to it and configure your WiFi credentials.

5. **Power the Device**:
   - Connect the battery and ensure all components are properly powered.

---

## Usage

- **Screen Navigation**:
  - Use the `BUTTON_LEFT` and `BUTTON_RIGHT` to navigate between screens (Time/Weather, Audio Player, Notes).
- **Audio Player**:
  - Use `BUTTON_SELECT` to play/pause audio.
  - Use `BUTTON_LEFT` and `BUTTON_RIGHT` to switch tracks.
  - Use `BUTTON_BACK` to stop playback.
- **Notes**:
  - Use `BUTTON_SELECT` to add characters to notes.
  - Use `BUTTON_LEFT` and `BUTTON_RIGHT` to select characters.
  - Use `BUTTON_BACK` to exit character selection.
  - Hold `BUTTON_CLEAR` for 5 seconds to clear all notes.
- **Battery Monitoring**:
  - The battery percentage is displayed on the top-right corner of the screen.

---

## Screens

1. **Time and Weather**:
   - Displays the current time, weather, and WiFi status.
2. **Audio Player**:
   - Controls for playing, pausing, and switching tracks.
3. **Notes**:
   - Allows users to create and manage notes.

---

## Troubleshooting

- **OLED Not Displaying**:
  - Check the I2C connections and ensure the address is set to `0x3C`.
- **WiFi Not Connecting**:
  - Ensure the WiFi credentials are correct and the ESP32 is within range of the router.
- **DFPlayer Not Working**:
  - Ensure the DFPlayer is properly connected and the SD card contains valid audio files.
