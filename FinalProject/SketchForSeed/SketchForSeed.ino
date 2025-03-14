#include <WiFi.h> // Allows for Wifi capabilities of Seed ESP32S3
#include <HTTPClient.h> // Allows for API calls and responses
#include <ArduinoJson.h> // Allows for easy JSON parsing in Arduino
#include <MD_Parola.h> // Drives the text effects for temperature readings on LED matrix
#include <MD_MAX72xx.h> //  Drives the writing of individual pixels on the LED matrix, used for weather icons
#include <SPI.h> // Needed to drive communication between seed and LED matrix, used by two libraries above

// Hardware Configuration for LED Matrix and Seed ESP32S3
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 1
#define CS_PIN 5   
#define CLK_PIN 8  
#define DATA_PIN 9 
#define RX_PIN RX
#define TX_PIN TX

//*NOTE These are for the same LED matrix, but act as two separate objects controlling the LED matrix.
// Creatse Parola object for scrolling text
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// Creates MAX72XX object for icon display
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Wi-Fi Credentials
const char* ssid = "FAKEWIFI";
const char* password = "FAKEPASS";

// OpenWeather API Setup
const char* apiKey = ""; // Key removed before I pushed to github. Generate one at https://openweathermap.org/api
const char* city = "Seattle";  
const char* country = "US";    
String openWeatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + 
                        String(city) + "," + String(country) + 
                        "&appid=" + String(apiKey) + "&units=imperial";

// Stores current weather data
String currentWeatherCondition = "";
float currentTemperature = 0;

bool systemOn = false;  // Tracks whether the weather display should be ON or OFF

// Weather Icons
const uint8_t sunIcon[8] = {
  0b00000000,
  0b00111100,
  0b01111110,
  0b01111110,
  0b01111110,
  0b00111100,
  0b00000000,
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
  0b00011110,
  0b00011000,
  0b01100000,
  0b00000000
};

void setup() {
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // Starts communication with Arduino over serial,
  Serial.begin(115200); // Starts communication with ArduinoIDE serial monitor for debugging
  WiFi.begin(ssid, password); // Initializes WiFi
  while (WiFi.status() != WL_CONNECTED) {
      delay(500); // If isn't connected, will retry every half second
  }

  //*NOTE This is the same LED matrix, but two separate objects 
  // are needed as they perform two different functions and use two different libraries
  P.begin(); // Starts LED matrix object for text
  P.setIntensity(5); // Sets brightness
  P.displayClear(); // Clears the display for this object

  mx.begin(); // Starts LED matrix object for icons
  mx.control(MD_MAX72XX::INTENSITY, 5); // Sets brightness
  mx.clear(); // Clears the display for this object

  // Fetches weather data
  getWeatherData();
}

void loop() {
  Serial.print("Waiting...."); // Prints to serial monitor for debugging
  if (Serial1.available()) { // Reads System Status data over Serial from Arduino 
      Serial.println("received!"); // for debugging 
      String message = Serial1.readStringUntil('\n'); // Ensures only one line is read, OFF/ON messages are separated by a "\n"
      Serial.println(message); // for debugging
      message.trim();  // Removes any extra spaces or newlines

      if (message == "ON") { // If motion is detected, Seed will receive "ON" and turn on the LED matrix
          systemOn = true;
      } else if (message == "OFF") {  // If motion is no longer detected for 5 seconds, Seed will receive "OFF" and turn off the LED matrix
          systemOn = false;
          // Clears the LED matrix 
          P.displayClear(); 
          mx.clear();
      }
  }

  if (systemOn) { // Turns on display when motion is detected
    displayWeatherIcon(currentWeatherCondition);
    delay(3000); // Shows weather icon for 3 seconds
    displayTemperature(currentTemperature); // Shows temperature as text that scrolls across screen
    delay(1000); // 1 second pause after temperature animation finishes
  } 

  // Refreshes Weather Data Every 10 Minutes
  static unsigned long lastWeatherUpdate = 0;
  if (millis() - lastWeatherUpdate > 600000) { 
      getWeatherData();
      lastWeatherUpdate = millis();
  }
}

// Fetch Weather Data from OpenWeather
void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) { 
    HTTPClient http; 
    http.begin(openWeatherUrl); // Initializes the request to the OpenWeather API
    int httpResponseCode = http.GET(); 
    if (httpResponseCode > 0) { // If API call is successful, this processes the JSON repsonse
      String response = http.getString(); 
      DynamicJsonDocument doc(1024); // Creates JSON document with 1024 bytes of memory to parse the response
      deserializeJson(doc, response); // Converts JSON string into object that allows access for the request weather information

      //Access weather conditions and temperature from JSON data
      currentWeatherCondition = String(doc["weather"][0]["description"].as<const char*>()); // Conditions stored as a string
      currentTemperature = doc["main"]["temp"].as<float>(); // Temperature stored as a float
    } 
    http.end();
  } 
}

// Display Weather Icon on LED Matrix
void displayWeatherIcon(String weatherDesc) {
  const uint8_t* icon;
  // Matches icon to weather condition received from API
  if (weatherDesc.indexOf("clear") != -1) icon = sunIcon; 
  else if (weatherDesc.indexOf("rain") != -1) icon = rainIcon; 
  else if (weatherDesc.indexOf("snow") != -1) icon = snowIcon;
  else if (weatherDesc.indexOf("cloud") != -1) icon = cloudIcon;
  else icon = thunderIcon;
  mx.clear(); // Clears display of previous icon
  for (int row = 0; row < 8; row++) { // Manually sets each individual pixel of matrix to display appropriate weather icon 
    mx.setRow(0, row, icon[row]); 
  }
}

// Scroll Temperature on LED Matrix
void displayTemperature(float temperature) {
  char tempStr[6];  
  snprintf(tempStr, sizeof(tempStr), "%.1fF", temperature); // Formats temperature reading as __._F
  P.displayClear(); // Clears display of previous text
  P.setZoneEffect(0, true, PA_FLIP_LR); // Inverts text horizontally, added to fix bug where text was mirrored by default
  P.displayText(tempStr, PA_CENTER, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT); // Scrolls the temperature across the matrix
  while (!P.displayAnimate()) {
    // Keep animating until done
  }
}



