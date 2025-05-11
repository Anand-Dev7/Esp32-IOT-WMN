#include <Wire.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON_LEFT 23
#define BUTTON_RIGHT 4
#define BUTTON_SELECT 19
#define BUTTON_BACK 18
#define BUTTON_CLEAR 13
#define BATTERY_PIN 35

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const String apiKey = "";  // Enter your apiKey  
const String city = "Hyderabad"; // Enter your city name  
const String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 190000;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

int screenIndex = 0;
String notes = "";

bool isPlaying = false;
int trackIndex = 0;
const int maxTracks = 5;
String lastAction = "";
unsigned long lastActionTime = 0;
const unsigned long actionDisplayDuration = 1000;

bool isSelectingLetter = false;
int selectedIndex = 0;
const char characterSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz_-0123456789!@#$%^&*()";
const int characterCount = sizeof(characterSet) - 1;
unsigned long clearPressStart = 0;

bool inAudioControlMode = true;

void drawLayout();

String getWeather();
String get12HourTime();

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer not found.");
    while (true) delay(100);
  }

  myDFPlayer.volume(30);

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);
  pinMode(BUTTON_CLEAR, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }

  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("ESP32-IOT")) {
    delay(3000);
    ESP.restart();
  }

  timeClient.begin();
  timeClient.update();
  drawLayout();
}

void loop() {
  timeClient.update();

  if (millis() - lastWeatherUpdate > weatherUpdateInterval) {
    getWeather();
    drawLayout();
    lastWeatherUpdate = millis();
  }

  if (!isSelectingLetter && screenIndex != 1) {
    if (digitalRead(BUTTON_LEFT) == LOW) {
      if (screenIndex > 0) screenIndex--;
      if (screenIndex == 1) inAudioControlMode = true;
      delay(200);
      drawLayout();
    }
    if (digitalRead(BUTTON_RIGHT) == LOW) {
      if (screenIndex < 2) screenIndex++;
      if (screenIndex == 1) inAudioControlMode = true;
      delay(200);
      drawLayout();
    }
  }

  if (screenIndex == 1) {
    if (inAudioControlMode) {
      if (digitalRead(BUTTON_SELECT) == LOW) {
        if (isPlaying) {
          myDFPlayer.pause();
          lastAction = "⏸ Pause";
        } else {
          myDFPlayer.play(trackIndex + 1);
          lastAction = "▶ Play";
        }
        isPlaying = !isPlaying;
        lastActionTime = millis();
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_RIGHT) == LOW) {
        trackIndex = (trackIndex + 1) % maxTracks;
        myDFPlayer.play(trackIndex + 1);
        isPlaying = true;
        lastAction = "⏭ Next";
        lastActionTime = millis();
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_LEFT) == LOW) {
        trackIndex = (trackIndex - 1 + maxTracks) % maxTracks;
        myDFPlayer.play(trackIndex + 1);
        isPlaying = true;
        lastAction = "⏮ Prev";
        lastActionTime = millis();
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_BACK) == LOW) {
        myDFPlayer.stop();
        isPlaying = false;
        inAudioControlMode = false;
        lastAction = "⏹ Stopped";
        lastActionTime = millis();
        delay(200);
        drawLayout();
      }
    } else {
      if (digitalRead(BUTTON_LEFT) == LOW) {
        if (screenIndex > 0) screenIndex--;
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_RIGHT) == LOW) {
        if (screenIndex < 2) screenIndex++;
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_SELECT) == LOW) {
        inAudioControlMode = true;
        delay(200);
        drawLayout();
      }
    }
  }

  if (screenIndex == 2) {
    if (digitalRead(BUTTON_SELECT) == LOW) {
      if (!isSelectingLetter) {
        isSelectingLetter = true;
      } else {
        notes += characterSet[selectedIndex];
      }
      delay(200);
      drawLayout();
    }
    if (digitalRead(BUTTON_BACK) == LOW) {
      isSelectingLetter = false;
      delay(200);
      drawLayout();
    }
    if (isSelectingLetter) {
      if (digitalRead(BUTTON_LEFT) == LOW) {
        if (selectedIndex > 0) selectedIndex--;
        delay(200);
        drawLayout();
      }
      if (digitalRead(BUTTON_RIGHT) == LOW) {
        if (selectedIndex < characterCount - 1) selectedIndex++;
        delay(200);
        drawLayout();
      }
    }
  }

  if (digitalRead(BUTTON_CLEAR) == LOW) {
    if (clearPressStart == 0) {
      clearPressStart = millis();
    }
    if (millis() - clearPressStart >= 5000) {
      notes = "";
      clearPressStart = 0;
      drawLayout();
    }
  } else {
    if (clearPressStart > 0 && millis() - clearPressStart < 5000) {
      if (notes.length() > 0) {
        notes.remove(notes.length() - 1);
        drawLayout();
      }
    }
    clearPressStart = 0;
  }

  // Check if current track ended
  if (screenIndex == 1 && inAudioControlMode && isPlaying) {
    if (myDFPlayer.readState() == 0) {
      // Move to next track automatically
      trackIndex = (trackIndex + 1) % maxTracks;
      myDFPlayer.play(trackIndex + 1);
      lastAction = "⏭ Auto Next";
      lastActionTime = millis();
      drawLayout();
    }
  }
}

void drawLayout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (screenIndex == 0) {
    display.setCursor(0, 0);
    display.print("Time: ");
    display.println(get12HourTime());

    display.setCursor(0, 16);
    display.print("Weather: ");
    display.println(getWeather());

    display.setCursor(0, 32);
    display.print("WiFi:");
    display.println(WiFi.isConnected() ? WiFi.SSID() : "Not Connected");

  } else if (screenIndex == 1) {
    display.setCursor(0, 0);
    display.print("Audio Player");

    display.setCursor(0, 16);
    display.print(isPlaying ? "Playing" : "Paused");

    // if (millis() - lastActionTime < actionDisplayDuration) {
    //   display.setCursor(0, 32);
    //   display.print("Action: ");
    //   display.println(lastAction);
    // }

    int centerY = 40;  //48
    display.fillTriangle(10, centerY, 10, centerY + 10, 0, centerY + 5, SSD1306_WHITE);
    display.fillTriangle(30, centerY, 30, centerY + 10, 40, centerY + 5, SSD1306_WHITE);
    display.fillRect(60, centerY, 3, 10, SSD1306_WHITE);
    display.fillRect(66, centerY, 3, 10, SSD1306_WHITE);
    display.fillTriangle(90, centerY, 90, centerY + 10, 100, centerY + 5, SSD1306_WHITE);

  } else if (screenIndex == 2) {
    display.setCursor(0, 0);
    display.print("Notes: ");
    display.setCursor(0, 16);
    display.println(notes);
    display.setCursor(0, 48);
    display.print("Select: ");
    display.print(characterSet[selectedIndex]);
  }

  display.setCursor(0, 56);
  display.print(isSelectingLetter ? "<<Pre Let" : "<<");
  // Calculate right-aligned label
  String rightLabel = isSelectingLetter ? "Next Let>>" : ">>";
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(rightLabel, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(SCREEN_WIDTH - w, 56);
  display.print(rightLabel);


  float batteryV = getBatteryVoltage();
  int batteryPct = getBatteryPercentage(batteryV);
  display.setCursor(SCREEN_WIDTH - 40, 0);
  // display.print("🔋");
  display.print(batteryPct);
  display.print("%");


  display.display();
}

String getWeather() {
  String weatherInfo = "N/A";
  if (WiFi.isConnected()) {
    HTTPClient http;
    http.begin(weatherUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        float temp = doc["main"]["temp"];
        String condition = doc["weather"][0]["main"].as<String>();
        weatherInfo = String(temp, 1) + "C " + condition;
      } else {
        weatherInfo = "JSON Err";
      }
    } else {
      weatherInfo = "Err " + String(httpCode);
    }
    http.end();
  } else {
    weatherInfo = "WiFi Disconnected";
  }
  return weatherInfo;
}

String get12HourTime() {
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  String meridian = "AM";
  if (hours == 0) {
    hours = 12;
  } else if (hours == 12) {
    meridian = "PM";
  } else if (hours > 12) {
    hours -= 12;
    meridian = "PM";
  }
  char timeStr[12];
  sprintf(timeStr, "%02d:%02d %s", hours, minutes, meridian.c_str());
  return String(timeStr);
}

float getBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3;  // Convert ADC value to voltage (ADC range 0–3.3V)
  voltage = voltage * 2.0;               // Because of 100k–100k voltage divider
  return voltage;
}

int getBatteryPercentage(float voltage) {
  if (voltage >= 4.2) return 100;
  if (voltage >= 3.95) return 75;
  if (voltage >= 3.7) return 50;
  if (voltage >= 3.5) return 25;
  return 10;
}