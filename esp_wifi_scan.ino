/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>

#define MAX_NETWORKS_DISPLAYED 10

#define RES_HEADER "<h1>Testing ESP8266</h1><h3>misan128</h3><form action='data'><ul>"
#define RES_FOOTER "<li>Password: <input type='password' name='password'></li><li><input type='submit' value='Submit'></li></ul></form>"

#define RES_PAIRED "<h2>Already Paired</h2>"

const char* ssid = "ESP";
const char* password = "123456789";

String inet_ssid = "";
String inet_password = "";

ESP8266WebServer server(80);

String res_items[MAX_NETWORKS_DISPLAYED];
int total_items;
boolean paired = false;

void clean_buffer();
void reset_app();
void scan_network();
void handle_data();
String pairing_response();
String paired_response();

void clean_buffer(){
  for(int i = 0; i < MAX_NETWORKS_DISPLAYED; i++){
    res_items[i] = "";
  }
  total_items = 0;
}

void scan_network(){
  void clean_buffer();
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    total_items = n;
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
//      Serial.print(i + 1);
//      Serial.print(": ");
//      Serial.print(WiFi.SSID(i));
//      Serial.print(" (");
//      Serial.print(WiFi.RSSI(i));
//      Serial.print(")");
//      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      res_items[i] = String("<li><input type='radio' name='ssid' value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</li>");
      //strcpy(res_items[i], String("<li>" + i + "</li>");
      delay(10);
    }

    for (int i = 0; i < n; ++i) {
      Serial.print(res_items[i]);
    }
  }
  Serial.println("");
}

void handle_data(){
  // get the value of request argument "state" and convert it to an int
  if(!paired){
    String ssid = server.arg("ssid");
  
    String password = server.arg("password");
  
    Serial.print("SSID: " + ssid + " - PASS:" + password);
  
    inet_ssid = ssid;
    inet_password = password;
  
    paired = true;
  }
  server_response();
}

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
  scan_network();

  Serial.println("Scan done");

  WiFi.softAP(ssid,password);

  server.on("/", [](){
    server_response();  
  });
  
  server.on("/data", handle_data);

  server.on("/reset", reset_app);

  // Start the server
  server.begin();
}

void reset_app(){
  clean_buffer();
  inet_ssid = "";
  inet_password = "";
  paired = false;
  scan_network();
  server_response();
}

void server_response(){
  String response = paired ? paired_response() : pairing_response();
  server.send(200, "text/html", response);
}

String pairing_response(){
  String response = RES_HEADER;
  for(int i = 0; i < total_items; i++){
    response = String(response + res_items[i]);
  }
  response = String(response + RES_FOOTER);
  return response;
}

String paired_response(){
  String response = RES_PAIRED;
  response = String(response + "<p>Connected with SSID: " + inet_ssid + "</p>");
  return response;
}

void loop() {
  
    //scan_network();
    // Wait a bit before scanning again
  //delay(5000);
  server.handleClient();
  
}
