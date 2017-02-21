// MQTT Servo LED controller
// For use with ESP8266
// By Michael Huggins (Skaako) 2016
// michael@cmtech.co.nz

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Servo.h> 

Servo myservo;  // create servo object to control a servo
 
// Connect to the WiFi
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "MQTT_SERVER";   // Can be name or IP address
 
WiFiClient espClient;
PubSubClient client(espClient);

int signal1G = 12; // GPIO12               
int signal1A = 5; // GPIO5
int signal1R = 4; // GPIO4
int ledPin3 = 2; // GPIO2

int flash = 0;                            // Initial settings for flashing
int greenFlash = 0;
int redFlash = 0;
int amberFlash = 0;

unsigned long previousMillis = 0;         // will store last time LED was updated
const long interval = 500;                // interval at which to flash LED's (milliseconds)
 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);

  if (strcmp (topic,"signal01") == 0) {     // When character 'G' is sent to topic 'signal01' sets the Green LED only. Flash off.
    if (receivedChar == 'G'){
     digitalWrite(signal1G, LOW);
     digitalWrite(signal1A, HIGH);
     digitalWrite(signal1R, HIGH);
     redFlash = 0;
     greenFlash = 0;
    }
    if (receivedChar == 'A'){
     digitalWrite(signal1G, HIGH);
     digitalWrite(signal1A, LOW);
     digitalWrite(signal1R, HIGH);
     redFlash = 0;
     greenFlash = 0;
    }
    if (receivedChar == 'R'){
     digitalWrite(signal1R, LOW);
     digitalWrite(signal1A, HIGH);
     digitalWrite(signal1G, HIGH);
     redFlash = 0;
     greenFlash = 0;
    } 
    if (receivedChar == 'O'){
     digitalWrite(signal1R, HIGH);
     digitalWrite(signal1A, HIGH);
     digitalWrite(signal1G, HIGH);
     redFlash = 0;
     greenFlash = 0;
    } 
    if (receivedChar == 'H'){
     digitalWrite(signal1R, HIGH);
     digitalWrite(signal1A, HIGH);
     digitalWrite(signal1G, LOW);
     redFlash = 0;
     greenFlash = 1;
    } 
    if (receivedChar == 'S'){
     digitalWrite(signal1R, LOW);
     digitalWrite(signal1A, HIGH);
     digitalWrite(signal1G, HIGH);
     redFlash = 1;
     greenFlash = 0;
    }
  }

  if (strcmp (topic,"points01") == 0) {          // When 0 or 1 is sent to the topic 'points01' it will set the servo.
    if (receivedChar == '0'){
     myservo.attach(14);                         // Attach to servo.. Delay.. Set servo.. Delay.. Detach. Save power and stop servo hum. Allows for multiple servo's from one board.
     delay(20);
     myservo.write(68);                          // This is the low value for my servo. Adjust to suit your servo.
     delay(100);
     myservo.detach();
    }
    if (receivedChar == '1'){                    
     myservo.attach(14);
     delay(20);
     myservo.write(85);                           // This is the high value for my servo. Adjust to suit your servo.
     delay(100);
     myservo.detach();
     
    } 
  }
  
  }
  Serial.println();
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    
    if (client.connect("ESP8266 Client 2")) {
      Serial.println("connected");
      // ... and subscribe to topic
      client.subscribe("points01");                // Topics that are subscribed to. Add more or change as required.
      client.subscribe("signal01");
  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
 Serial.begin(115200);
 
 client.setServer(mqtt_server, 1883);            // Default port of MQTT server is 1883
 client.setCallback(callback);
 
 pinMode(signal1G, OUTPUT);
 pinMode(signal1A, OUTPUT);
 pinMode(signal1R, OUTPUT);
 pinMode(ledPin3, OUTPUT);

 //myservo.attach(14);  // unused. Attach and detach from Servo when switching
 
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  unsigned long currentMillis = millis();                    // All this code is to allow LED's to flash at a set rate and allow code to continue. If delays are too long ESP with freeze WIFI functions.

  if (currentMillis - previousMillis >= interval) {      

    if (flash == 0) {      
      if (redFlash == 1) digitalWrite(signal1R, LOW);
      if (greenFlash == 1) digitalWrite(signal1G, LOW);
      flash = 1;
    }
    else if (flash == 1) {
      if (redFlash == 1) digitalWrite(signal1R, HIGH);
      if (greenFlash == 1) digitalWrite(signal1G, HIGH);
      flash = 0;
    }
    
    previousMillis = currentMillis;
  }
 
  client.loop();
}
