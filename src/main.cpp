/*
  Written By: Scott Griffis
  Date: 4/5/2024
  
  Description: This code was written for the ESP8266. The purpose of this
  code is to enable the ESP8266 to act as an adhoc network for devices to
  connect to, so that they can talk to eachother on that network but have 
  no internet or network access except to other devices which are also 
  attached to that same network.

  In short... This causes the ESP8266 to act as a kind of wireless networking
  switch. 

  I created this because I have devices I need to wirelessly put on the same
  network but I don't want them on my home network or anywhere that has Internet
  access.
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>
#include "ExampleSecrets.h"
#include "Secrets.h" // <-- Your secrets here using vars: server_cert and server_key

#ifndef AP_SSID
  #define AP_SSID "Deadspace"
  #define AP_PSK "Password" //"SetThePassword"
#endif

const char *ssid = AP_SSID;
const char *password = AP_PSK;

BearSSL::ESP8266WebServerSecure webServer(443);
BearSSL::ServerSessions serverCache(5);

IPAddress initWiFi();
void initWebServer();
String getServerStatusByNumber(int value);

void setup() {
  system_update_cpu_freq(160);
  delay(1000);
  pinMode(2, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();
  
  IPAddress myIp = initWiFi();
  initWebServer();
  
  Serial.printf("\nAP Info is:\n\tSSID=%s\n\tIP=%s\n\n", ssid, (myIp.toString().c_str()));

  Serial.println("AP Setup is Complete.");
  digitalWrite(2, LOW);
}

void loop() {
  webServer.handleClient();
}

IPAddress initWiFi() {
  Serial.print("AP Setup in progress...");
  WiFi.setHostname("deadspace");
  WiFi.softAP(ssid, password);
  Serial.println("Complete.");

  return WiFi.softAPIP();
}

void initWebServer() {
  Serial.println("Initializing Web Server...");
  Serial.println("Configuring SSL... ");
  #ifdef Secrets_h
    webServer.getServer().setRSACert(new BearSSL::X509List(server_cert), new BearSSL::PrivateKey(server_key));
  #else
    #ifdef ExampleSecrets_h
      #pragma message("DO NOT USE THE SAMPLE CERT AND KEY IN YOU OWN PROJECT!!!")
      webServer.getServer().setRSACert(new BearSSL::X509List(SAMPLE_SERVER_CERT), new BearSSL::PrivateKey(SAMPLE_SERVER_KEY));
      Serial.println("\nWARNING!!! Using insecure Cert and Key!\n");
    #else
      #error No Secrets found to supply the server Certificate or Key!!!
      Serial.println("FATAL!!! No SSL Cert or Key found!");
      exit(1);
    #endif
  #endif
  webServer.getServer().setCache(&serverCache);
  Serial.println(F("SSL configuration Complete."));
  
  Serial.println(F("Registering web endpoints..."));
  webServer.on("/", [] () {
      webServer.send(200, "text/html", "<html><body>Hello from WebServer8266!</body></html>");
  });
  webServer.onNotFound([] () {
      webServer.send(404, "text/plain", "Oops, Not Found!!!");
  });
  Serial.println(F("Endpoint registration Complete."));
  Serial.println(F("Web Server initialization Complete."));

  Serial.println(F("\nStarting Web Server..."));
  webServer.begin();
  Serial.printf("Web Server started with a status code of '%s'.\n", getServerStatusByNumber(webServer.getServer().status()).c_str());
  
  Serial.println("\nWeb Server startup is complete.");
}

String getServerStatusByNumber(int value) {
  switch (value) {
    case 0:
      return "CLOSED";
    case 1:
      return "LISTEN";
    case 2:
      return "SYN_SENT";
    case 3:
      return "SYN_RCVD";
    case 4:
      return "SYN_RCVD";
    case 5:
      return "ESTABLISHED";
    case 6:
      return "FIN_WAIT_1";
    case 7:
      return "FIN_WAIT_2";
    case 8:
      return "CLOSE_WAIT";
    case 9:
      return "LAST_ACK";
    case 10:
      return "TIME_WAIT";
    default:
      return "UNKNOWN";
  }
}