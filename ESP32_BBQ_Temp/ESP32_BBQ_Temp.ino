/*
* ESPBBQ
* Made by Roly van Leersum
* Github URL: https://github.com/roly197/ESPBBQ
*/

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <Adafruit_MAX31855.h>

AsyncWebServer server(80);

const char* ssid = ""; // Your WiFi SSID
const char* password = ""; // Your WiFi Password

// Creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   19
#define MAXCS   5
#define MAXCLK  18

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);


void setup() {
    Serial.begin(115200);

    //Setup MAX31855
    Serial.println("MAX31855 test");
    // wait for MAX chip to stabilize
    delay(500);
    Serial.print("Initializing sensor...");
    if (!thermocouple.begin()) {
      Serial.println("ERROR.");
      while (1) delay(10);
    }
    Serial.println("DONE.");

    //Setup Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    //Setup Dash (Web framework)
    ESPDash.init(server);   // Initiate ESPDash and attach your Async webserver instance
    ESPDash.addTemperatureCard("temp1", "Meat Temp", 0, 0);
    server.begin();
}

void loop() {
    // basic readout test, just print the current temp
   Serial.print("Internal Temp = ");
   Serial.println(thermocouple.readInternal());

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
     //Debug: display temp although reading is wrong... (remove later)
     ESPDash.updateTemperatureCard("temp1", (int) c);
   } else {
     Serial.print("C = ");
     Serial.println(c);
     ESPDash.updateTemperatureCard("temp1", (int) c);
   }

   delay(1000);

}
