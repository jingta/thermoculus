
// (Based on Ethernet's WebClient Example)

#include <SPI.h>
#include <WiFly.h>
#include <SoftwareSerial.h>

#include "Credentials.h"

SoftwareSerial Thermal(5, 6);
int heatTime = 80;
int heatInterval = 255;
char printDensity = 15; 
char printBreakTime = 15;



//byte server[] = { 66, 249, 89, 104 }; // Google

//Client client(server, 80);
// server address:
//char server[] = "www.arduino.cc";
//char server[] = "google.com";
//char server[] = "checkip.dyndns.com";
char server[] = "thermoculus.herokuapp.com";
char token[] = "someguid";
//IPAddress server(64,131,82,241);
//WiFlyClient client("google.com", 80);
WiFlyClient client(server, 80);


String readString = String();
unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 5*1000;  // delay between updates, in milliseconds
boolean current_line_is_blank = true;
boolean is_body = false;

void setup() {
  Serial.begin(115200);
  Thermal.begin(19200); // to write to our new printer
  initPrinter();
 
  WiFly.begin();
  Serial.print("Attempting to join SSID: ");
  Serial.println(ssid);
  while (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed. Retry in 10 seconds.");
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Joined.");
  printWifiStatus();
}

void initPrinter()
{
  Serial.println("Printer init"); 
 //Modify the print speed and heat
 Thermal.write(27);
 Thermal.write(55);
 Thermal.write(7); //Default 64 dots = 8*('7'+1)
 Thermal.write(heatTime); //Default 80 or 800us
 Thermal.write(heatInterval); //Default 2 or 20us
 //Modify the print density and timeout
 Thermal.write(18);
 Thermal.write(35);
 int printSetting = (printDensity<<4) | printBreakTime;
 Thermal.write(printSetting); //Combination of printDensity and printBreakTime
 Serial.println();
 Serial.println("Printer ready"); 
// Thermal.println("Printer ready"); 
// Thermal.write(10);//Sends the LF to the printer, advances the paper
// Thermal.write(10); 
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  
  while (client.available()) {
    char c = client.read();
    //Serial.write(c);
    
    if (c == '\n') {
      if (!is_body) {
        Serial.write("Header: ");
      } else {
        Serial.write("Body: ");
        Thermal.println(readString); 
        //Thermal.write(10);//Sends the LF to the printer, advances the paper
        //Thermal.write(10); 
      }
      Serial.println(readString);
      readString="";
      if (current_line_is_blank){
        is_body = true;
        Serial.println("-------------------new line and line is blank");
      }
      current_line_is_blank = true;
    } else if (c != '\r') {
      current_line_is_blank = false;
      readString = readString + c;
    }
  }
  
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    current_line_is_blank = true;
    is_body = false;
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}
// this method makes a HTTP connection to the server:
void httpRequest() {
  // if there's a successful connection:
  if (client.connect()) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    //client.println("GET /latest.txt HTTP/1.1");
    //client.println("Host: www.arduino.cc");
    client.println("GET /" + token + " HTTP/1.1");
    client.println("Host: thermoculus.herokuapp.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  //Serial.print("SSID: ");
  //Serial.println(WiFly.ssid());

  // print your WiFi shield's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFly.ip());

  // print the received signal strength:
  //long rssi = WiFly.rssi();
  //Serial.print("signal strength (RSSI):");
  //Serial.print(rssi);
  //Serial.println(" dBm");
}
