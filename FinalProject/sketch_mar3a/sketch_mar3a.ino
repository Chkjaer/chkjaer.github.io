#include <WiFi.h>
#include <HTTPClient.h> 
#include <ArduinoJson.h> // necessary for convienent parsing of json from API call
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 1 // 4 blocks

// Wi-Fi Credentials
const char* ssid = "treehouse";
const char* password = "balls123";

// OpenWeather API Key and City
const char* apiKey = "a92d3da0f6e66025f6eda9004254222b";
const char* city = "Seattle";  // Change this to your city
const char* country = "US";    // Change country code if needed

// OpenWeather API URL
String openWeatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(country) + "&appid=" + String(apiKey) + "&units=imperial";

// MAX7219 SPI Pins for XIAO ESP32S3
#define CS_PIN 5   // Chip Select
#define CLK_PIN 8  // Clock
#define DATA_PIN 9 // Data In
String currentWeather = "clear sky";

// Initialize MAX7219 LED Matrix  
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Weather Icons (8x8 bitmaps)
// Weather Icons (8x8 sprites)
const uint8_t PROGMEM sunIcon[] = {
  0x18, 0x24, 0x42, 0x81, 0xFF, 0x81, 0x42, 0x24, 0x18, 0x00
};

const uint8_t PROGMEM rainIcon[] = {
  0x00, 0x66, 0x99, 0x00, 0x66, 0x99, 0x00, 0x66, 0x99, 0x00
};

const uint8_t PROGMEM cloudIcon[] = {
  0x00, 0x3C, 0x42, 0x81, 0x81, 0xFF, 0x7E, 0x3C, 0x00, 0x00
};

const uint8_t PROGMEM snowIcon[] = {
  0x24, 0x5A, 0x24, 0x00, 0x24, 0x5A, 0x24, 0x00, 0x24, 0x00
};

const uint8_t PROGMEM thunderIcon[] = {
  0x18, 0x3C, 0x66, 0xC3, 0x18, 0x18, 0x3C, 0x66, 0xC3, 0x00
};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\n Wi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Initializing MAX7219...");
  P.begin();         // initialize the LED Matrix
  P.setIntensity(15); // set the brightness of the LED matrix display (from 0 to 15)
  P.displayClear();  // clear LED matrix display

  // Fetch and display weather data
  getWeatherData();
}

void loop() {
  if (P.displayAnimate()) {
    P.displayReset();
  }
  // Refresh weather data every 10 minutes
  delay(600000); 
  getWeatherData();
}

void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) { //Checks wifi connection
    HTTPClient http; // Creates object allowing for requests to the web
    http.begin(openWeatherUrl); // Requests weather for Seattle, with use of provided API key

    int httpResponseCode = http.GET(); //Returns weather data as JSON
    
    if (httpResponseCode > 0) { // Checks for a valid response
      String payload = http.getString(); //extracts API response as string
      Serial.println("Weather Data Received: " + payload); // Prints to serial monitor to view raw data

      // Parses JSON for Arduino
      DynamicJsonDocument doc(1024); // Creates memory space to store JSON data in ESP32 RAM
      deserializeJson(doc, payload); // Converts raw JSON string into structured object stored in doc on ESP32

      String description = doc["weather"][0]["description"]; // Parses Weather array for weather description
      float temperature = doc["main"]["temp"]; // parses weather array for temperature
      currentWeather = description;  // Store weather condition

      Serial.println(" Weather: " + description);
      Serial.println(" Temperature: " + String(temperature) + "°C");
    } else {
      Serial.println(" Failed to get weather data");
    }
    displayWeatherIcon(currentWeather);
    http.end();
  } else {
    Serial.println(" Not connected to Wi-Fi");
  }
}

// 🔹 Function to Display Icons on MAX7219
 void displayWeatherIcon(String weatherDesc) {
  P.displayClear(); // Clear display

  const uint8_t* icon; // Pointer to selected icon

  if (weatherDesc.indexOf("clear") != -1) {
    Serial.println("☀️ Showing Sun Icon");
    icon = sunIcon;
  } else if (weatherDesc.indexOf("rain") != -1) {
    Serial.println("🌧️ Showing Rain Icon");
    icon = rainIcon;
  } else if (weatherDesc.indexOf("snow") != -1) {
    Serial.println("❄️ Showing Snow Icon");
    icon = snowIcon;
  } else if (weatherDesc.indexOf("cloud") != -1) {
    Serial.println("☁️ Showing Cloud Icon");
    icon = cloudIcon;
  } else if (weatherDesc.indexOf("thunder") != -1) {
    Serial.println("⛈️ Showing Thunder Icon");
    icon = thunderIcon;
  } else {
    Serial.println("🤷 Unknown weather, showing Cloud Icon");
    icon = cloudIcon;
  }
 // Manually write the 8x8 icon
  for (int i = 0; i < 8; i++) {
    P.getZone(0)->setColumn(i, icon[i]);
  }
}