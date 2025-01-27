/*

For ESP32 UWB or ESP32 UWB Pro

*/

#include <SPI.h>
#include "DW1000Ranging.h"
#include <WiFi.h>

// Replace with your Wi-Fi credentials
const char* ssid = "Bruno WiFi";
const char* password = "Andres0909";

// Server IP and port (replace with your computer's IP)
const char* host = "10.0.0.224";  // Replace with your computer's IP
const int port = 12345;

// Unique ID for the tracker
const char* trackerID = "tracker1";  // You can modify this as needed

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// Timer for controlling the frequency of data sending
unsigned long lastSendTime = 0; // Stores the last time data was sent
const unsigned long sendInterval = 750; // 500 milliseconds (half a second)

void setup()
{
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

void loop()
{
    DW1000Ranging.loop();
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

        // Print to Serial Monitor for debugging
        Serial.print("from: ");
        Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
        Serial.print("\t Range: ");
        Serial.print(range);
        Serial.print(" m");
        Serial.print("\t RX power: ");
        Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
        Serial.println(" dBm");

        // Connect to the server
        WiFiClient client;
        if (client.connect(host, port)) {
            // Send the tracker ID, range, and anchorId to the server
            client.printf("%s; %.2f; %.2f\n", trackerID, range, anchorId);
            client.stop();  // Close the connection
            Serial.println("Data sent: " + String(trackerID) + "; " + String(range) + "; " + String(anchorId));
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
