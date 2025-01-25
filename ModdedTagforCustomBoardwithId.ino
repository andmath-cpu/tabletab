/*

For ESP32 UWB or ESP32 UWB Pro

*/

#include <SPI.h>
#include "DW1000Ranging.h"
#include <WiFi.h>
#include <math.h> // Include the math library for mathematical functions

// Define anchor1range to be updated continuously
float anch1Range;
float anch2Range;

float x = 0.0, y = 0.0;

// Replace with your Wi-Fi credentials
const char* ssid = "Bruno WiFi";
const char* password = "Andres0909";

// Server IP and port (replace with your computer's IP)
const char* host = "10.0.0.224";  // Replace with your computer's IP
const int port = 12345;

// Unique ID for the tracker
const char* trackerID = "tracker2";  // You can modify this as needed

#define SPI_SCK 21
#define SPI_MISO 19
#define SPI_MOSI 18
#define DW_CS 17

// connection pins
const uint8_t PIN_RST = 16; // reset pin
const uint8_t PIN_IRQ = 22; // irq pin

// Timer for controlling the frequency of data sending
unsigned long lastSendTime = 0; // Stores the last time data was sent
const unsigned long sendInterval = 750; // 500 milliseconds (half a second)

void setup()
{

    anch1Range = 0.0;
    anch2Range = 0.0;

    Serial.begin(115200);
    delay(1000);

    // Connect to Wi-Fi
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");

    // Init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, DW_CS, PIN_IRQ); // Reset, CS, IRQ pin
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
}

unsigned long lastUpdateTime = 0; // Tracks the last time the function was called
const unsigned long updateInterval = 1000; // Interval in milliseconds (1 second)

void tagPos(float a, float b, float c, float &x, float &y) {
    // Calculate the cosine of angle 'a' using the law of cosines
    float cos_a = (sq(b) + sq(c) - sq(a)) / (2 * b * c);  // Correct calculation for cos_a

    // Calculate x and y based on the law of cosines and Pythagorean theorem
    x = b * cos_a;
    y = b * sqrt(1 - sq(cos_a));  // Correct calculation for y

    // Ensure y is a valid number
    if (isnan(y)) {
        y = 0.0; // Set y to 0 if the calculation results in NaN
    }

    // Round to one decimal place
    x = round(x * 10) / 10.0;
    y = round(y * 10) / 10.0;

    // Print the results for debugging
    Serial.print("x: ");
    Serial.print(x);
    Serial.print(", y: ");
    Serial.println(y);
}


void loop() {
  DW1000Ranging.loop();

  // Example variables for demonstration
  float c = 1.0;         // Distance between anchors (fixed)

  // Get the current time
  unsigned long currentTime = millis();

  // Check if the specified interval has passed
  if (currentTime - lastUpdateTime >= updateInterval) {
    // Call the tagPos function
    tagPos(anch1Range, anch2Range, c, x, y);

    // Update the last update time
    lastUpdateTime = currentTime;
  }
}


void newRange()
{
    unsigned long currentTime = millis(); // Get the current time

    // Check if it's time to send data
    if (currentTime - lastSendTime >= sendInterval) {
        lastSendTime = currentTime; // Update the last send time

        // Get the range and anchorId values
        float range = DW1000Ranging.getDistantDevice()->getRange();
        float anchorId = DW1000Ranging.getDistantDevice()->getShortAddress();

        /* Print to Serial Monitor for debugging
        Serial.print("from: ");
        Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
        Serial.print("\t Range: ");
        Serial.print(range);
        Serial.print(" m");
        Serial.print("\t RX power: ");
        Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
        Serial.println(" dBm");
        */

        if (anchorId == 6022.00){
          //Serial.print("updating beacon 1");
          anch1Range = range;
          //Serial.print(anch1Range);
        }
        else{
          if (anchorId = 5922.00){
            //Serial.print("updating beacon 2");
            anch2Range = range;
          }
        }

        // Connect to the server
        WiFiClient client;
        if (client.connect(host, port)) {
            // Send the tracker ID, range, and anchorId to the server
            client.printf("%s; %.2f; %.2f\n", trackerID, x, y);
            client.stop();  // Close the connection
            Serial.println("Data sent: " + String(trackerID) + "; " + x + "; " + y);
        } else {
            Serial.println("Connection to server failed");
        }
    }
}

void newDevice(DW1000Device *device)
{
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}
