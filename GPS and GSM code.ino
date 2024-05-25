#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define GPS_TX_PIN 8 //transmitter pin of Neo GPS module
#define GPS_RX_PIN 12 //receiver pin of Neo GPS module
#define GSM_TX_PIN 10 //transmitter pin of SIM 900A GSM module
#define GSM_RX_PIN 11 //  receiver pin of SIM 900A GSM module

SoftwareSerial gpsSerial(GPS_TX_PIN, GPS_RX_PIN); // GPS connection
SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN); // GSM connection

TinyGPS gps; // GPS object

String phoneNumber = "+97798&&&&&&&&"; // Modify this with your phone number

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  // Initialize GSM module
  delay(2000);
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // AT command to enable call receiving
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
}

void loop() {
  // Read GPS data
  float latitude, longitude;
  unsigned long fix_age;
  
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&latitude, &longitude, &fix_age);
      delay(5000); // Wait for 5 seconds before sending the message
      sendLocation(latitude, longitude);
    }
  }

  // Check if command is received from Serial Monitor
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "send") {
      sendLocation(0, 0); // Send location with default (0, 0) coordinates
    }
  }
}

void sendLocation(float lat, float lon) {
  // Create SMS message
  String message = "Help! I'm in danger. My location is:https://www.google.com/maps/place/Lalitpur/@27.6574688,85.323152,13z";
  message += String(lat, 6);
  message += ",";
  message += String(lon, 6);

  // Send SMS
  gsmSerial.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);
  gsmSerial.println(message);
  delay(100);
  gsmSerial.write(0x1A); // End of message
  delay(1000);

  // Check for "OK" response
  if(gsmSerial.find("OK")) {
    Serial.println("Message sent successfully.");
  } else {
    Serial.println("Failed to send message.");
    Serial.println("Check GSM module and network.");
  }
}
