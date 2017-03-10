#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>

/*
 * JAWS - Just another weather station v0.1
 * BMP180 + WeMos D1
 * http://avrland.it
 */
 
/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 *  
 *  
 *  Gateway for Android ip: 192.168.43.1
 *  Mask 255.255.255.0
 */


Adafruit_BMP085 bmp;
const char* ssid = "LG_L90";
const char* password = "bobek123";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

IPAddress ip(192, 168, 43, 20);
IPAddress gateway(192, 168, 43, 1); 
IPAddress mask(255, 255, 255, 0);

void setup() {
  Serial.begin(9600);
    if (!bmp.begin()) {
      Serial.println("Could not find a valid BMP085 sensor, check wiring!");
      while (1) {}
  }
  delay(10);
  WiFi.config(ip, gateway, mask);
  // prepare GPIO2
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    }
    
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
    
    // Match the request
    int val;
    if (req.indexOf("/gpio/1") != -1)
      val = 0;
    else if (req.indexOf("/gpio/0") != -1)
      val = 1;
    else {
      client.flush();
      String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nTemperatura: ";
      s += bmp.readTemperature();
      s += " *C";  
      s += "      Cisnienie = " ;
      s += (1800+ bmp.readPressure())/100; 
      s += " hPa";   
      s += "</html>\n";
      // Send the response to the client
      client.print(s);
      delay(1);
      Serial.println("Client disonnected");
      return;
    }
  
    // Set GPIO2 according to the request
    digitalWrite(LED_BUILTIN, val);
    
    client.flush();
  
    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
    s += (val)?"low":"high";
    s += "</html>\n";
  
    // Send the response to the client
    client.print(s);
    delay(1);
    Serial.println("Client disonnected");
  }
}

