#include <WiFi.h>
#include <HTTPClient.h> 
#include <ArduinoJson.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// 🔹 **Hardware Configuration**
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 1
#define CS_PIN 5   
#define CLK_PIN 8  
#define DATA_PIN 9 

// Create Parola object for scrolling text
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Create MAX72XX object for icon display
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// 🔹 **Wi-Fi Credentials**
const char* ssid = "Christians iPhone";
const char* password = "balls123";

// 🔹 **OpenWeather API Setup**
const char* apiKey = "a92d3da0f6e66025f6eda9004254222b";
const char* city = "Seattle";  
const char* country = "US";    
String openWeatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + 
                        String(city) + "," + String(country) + 
                        "&appid=" + String(apiKey) + "&units=imperial";

// 🔹 **Weather Data Storage**
String currentWeatherCondition = "";
float currentTemperature = 0;

// 🔹 **TX Signal from Arduino**
#define ARDUINO_TX_PIN 44  // Motion signal from Arduino TX pin

// 🔹 **Weather Icons (8x8 Bitmaps)**
const uint8_t sunIcon[8] = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};

const uint8_t rainIcon[8] = {
  0b00000000,
  0b00111100,
  0b01111110,
  0b01111110,
  0b00100100,
  0b01001000,
  0b10010000,
  0b00000000
};

const uint8_t cloudIcon[8] = {
  0b00000000,
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00000000
};

const uint8_t snowIcon[8] = {
  0b10010010,
  0b01010100,
  0b00111000,
  0b01111100,
  0b00111000,
  0b01010100,
  0b10010010,
  0b00000000
};

const uint8_t thunderIcon[8] = {
  0b00011000,
  0b00111100,
  0b01100110,
  0b00011000,
  0b00011000,
  0b00011000,
  0b01100110,
  0b00000000
};

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("🔌 Initializing LED Matrix...");
    P.begin();
    P.setIntensity(5);
    P.displayClear();

    mx.begin();
    mx.control(MD_MAX72XX::INTENSITY, 5);
    mx.clear();

    pinMode(ARDUINO_TX_PIN, INPUT);  // TX signal from Arduino (motion detection)

    // Fetch and display weather data
    getWeatherData();
}

void loop() {
    // Read motion detection signal from Arduino
    int systemState = digitalRead(ARDUINO_TX_PIN);

    if (systemState == HIGH) {
        Serial.println("🔴 Motion detected → Turning ON LED Matrix...");
        
        // Show Weather Icon
        displayWeatherIcon(currentWeatherCondition);
        delay(5000);

        // Show Temperature
        displayTemperature(currentTemperature);
        delay(1000);
    } else {
        Serial.println("⚫ No motion detected → Turning OFF LED Matrix...");
        P.displayClear();
        mx.clear();
    }

    // Refresh weather data every 10 minutes
    static unsigned long lastWeatherUpdate = 0;
    if (millis() - lastWeatherUpdate > 600000) {
        getWeatherData();
        lastWeatherUpdate = millis();
    }
}

// 🔹 **Fetch Weather Data from OpenWeather**
void getWeatherData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(openWeatherUrl);

        int httpResponseCode = http.GET();
        Serial.println("HTTP Response Code: " + String(httpResponseCode));

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("✅ Weather Data Received: " + payload);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            // ✅ **Fix: Explicitly cast JSON data to String**
            currentWeatherCondition = String(doc["weather"][0]["description"].as<const char*>());
            currentTemperature = doc["main"]["temp"].as<float>();

            Serial.println("🌤️ Weather: " + currentWeatherCondition);
            Serial.println("🌡️ Temperature: " + String(currentTemperature) + "°F");
        } else {
            Serial.println("❌ Failed to get weather data");
        }
        http.end();
    } else {
        Serial.println("❌ Not connected to Wi-Fi");
    }
}

// 🔹 **Display Weather Icon on LED Matrix**
void displayWeatherIcon(String weatherDesc) {
    Serial.println("🔍 Displaying Weather Icon...");

    const uint8_t* icon;
    if (weatherDesc.indexOf("clear") != -1) icon = sunIcon;
    else if (weatherDesc.indexOf("rain") != -1) icon = rainIcon;
    else if (weatherDesc.indexOf("snow") != -1) icon = snowIcon;
    else if (weatherDesc.indexOf("cloud") != -1) icon = cloudIcon;
    else icon = thunderIcon;

    mx.clear();

    for (int row = 0; row < 8; row++) {
        mx.setRow(0, row, icon[row]);
    }

    Serial.println("✅ Weather Icon Displayed!");
}

// 🔹 **Scroll Temperature on LED Matrix**
void displayTemperature(float temperature) {
    char tempStr[6];  
    snprintf(tempStr, sizeof(tempStr), "%.1fF", temperature);  // Format as "72.5°F"

    Serial.print("Displaying Temperature: ");
    Serial.println(tempStr);

    P.displayClear();
     P.setZoneEffect(0, true, PA_FLIP_LR);
    P.displayText(tempStr, PA_CENTER, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    while (!P.displayAnimate()) {
        // Keep animating until done
    }
}
