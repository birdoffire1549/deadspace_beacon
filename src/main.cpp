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

#ifndef AP_SSID
  #define AP_SSID "Deadspace"
  #define AP_PSK "SetThePassword"
#endif

const char *ssid = AP_SSID;
const char *password = AP_PSK;

// TODO: Webserver for setting changes...

void setup() {
  delay(1000);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println("AP Setup in progress...");
  WiFi.setHostname("deadspace-alpha");
  WiFi.softAP(ssid, password);
  IPAddress myIp = WiFi.softAPIP();
  Serial.printf("Ap's IP is: %s\n", (myIp.toString().c_str()));

  Serial.println("AP Setup is Complete.");
  digitalWrite(2, LOW);
}

void loop() {
  delay(1000);
}