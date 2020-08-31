/*
* ESPBBQ
* Made by Roly van Leersum
* Github URL: https://github.com/roly197/ESPBBQ
* 
* Pinout details:
* 
*      ESP32       
*    DEVKIT V1
*    |------|    R1
*    | 3.3V |_--^^^^---|
*    |      |          |
*    |   VP |----------|
*    |      |  R0(NTC) |
*    |  GND |---^^^^---|
*    |------|
* 
*     NTC B3950 Thermistor
*     the formula for temp in kelvin is
*                     1
*     T = ----------------------------
*         1/To + (1/beta) * ln(Rt/Ro)
*    
*     To determine Beta:
*     R(t0) = resistance @ 273.15 Kelvin; Ice water (or 0 Celcius) 
*     R(t1) = resistance @ 373.15 Kelvin: Boiling water (or 100 Celcius)
*     Then: 
*         Beta = ln(Rt1/Rt0)/(1/t1-1/t0)
*    
*     https://en.wikipedia.org/wiki/Thermistor
*/

#include <Arduino.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>

AsyncWebServer server(80);

const char* ssid = "Bigdragon"; // Your WiFi SSID
const char* password = "Unreal!!!"; // Your WiFi Password

int ThermistorPin;
double adcMax, Vs;

double R1 = 32600.0;   // voltage divider resistor value (33k)
double Beta = 3950.0;  // Beta value
double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
double Ro = 81000.0;   // Resistance of Thermistor at 25 degree Celsius


void setup() {
    Serial.begin(115200);

    ThermistorPin = A0;
    adcMax = 4095.0;   // ADC resolution 10-bit (0-1023)
    Vs = 3.3;          // supply voltage

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
      double Vout, Rt = 0;
      double T, Tc, Tf = 0;

      Vout = analogRead(ThermistorPin) * Vs/adcMax;
      Rt = R1 * Vout / (Vs - Vout);
      T = 1/(1/To + log(Rt/Ro)/Beta);  // Temperature in Kelvin
      Tc = T - 273.15;                 // Celsius
      Tf = Tc * 9 / 5 + 32;            // Fahrenheit
      Serial.println(Tc);
      ESPDash.updateTemperatureCard("temp1", int (Tc));
      Serial.println(analogRead(ThermistorPin));

      delay(2000);
}
