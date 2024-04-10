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
#include "WebPages.h"
#include "Settings.h"
#include "ExampleSecrets.h"
#include "Secrets.h" // <-- Your secrets here using vars: server_cert and server_key

#ifndef APP_VERSION
  #define APP_VERSION "2.0.0"
#endif

const char *ssid = SSID;
const char *password = PSK;

BearSSL::ESP8266WebServerSecure webServer(443);
BearSSL::ServerSessions serverCache(5);

/* ***************** *
 * Function Protypes *
 * ***************** */
IPAddress initWiFi();
void initWebServer();
String getServerStatusByNumber(int value);
void endpointHandlerRoot();
void endpointHandlerAdmin();
void resourceNotFoundHandler();

/**
 * ## SETUP FUNCTION ##
 * This is the setup function. 
 * Per the Arduino Framework setup runs first and only once at the start 
 * of the program just prior to the main looping part of the program.
*/
void setup() {
  system_update_cpu_freq(160);
  delay(1000);
  pinMode(2, OUTPUT);
  
  Serial.begin(SERIAL_SPEED);
  Serial.println();
  Serial.println("===========================");
  Serial.printf("Firmware Version: %s\n", APP_VERSION);
  Serial.println("===========================\n");
  
  IPAddress myIp = initWiFi();
  initWebServer();
  
  Serial.printf("\nAP Info is:\n\tSSID=%s\n\tIP=%s\n\n", ssid, (myIp.toString().c_str()));

  Serial.println("AP Setup is Complete.");
  digitalWrite(2, LOW);
}

/**
 * ## LOOP FUNCTION ##
 * This is the loop function.
 * Per the Arduino Framework, this is where the primary execution of the
 * software takes place. The loop function runs repeatedly as long as the
 * device is active.
*/
void loop() {
  webServer.handleClient();
}

/**
 * Function handles the initialization of the WiFi.
 * 
 * @returns Returns the device's IP Address.
*/
IPAddress initWiFi() {
  Serial.print("AP Setup in progress...");
  WiFi.setHostname(HOST_NAME.c_str());
  WiFi.softAPConfig(DEVICE_IP, GATEWAY_IP, SUBNET_MASK);
  WiFi.softAP(ssid, password);
  Serial.println("Complete.");

  return WiFi.softAPIP();
}

/**
 * Handles the initialization of the internal SSL WebServer.
 * 
*/
void initWebServer() {
  Serial.println("Initializing Web Server...");

  /* Configure WebServer's SSL */
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
  
  /* Register Endpoints */
  Serial.println(F("Registering web endpoints..."));
  webServer.on("/", HTTP_GET, endpointHandlerRoot);
  webServer.on("/admin", endpointHandlerAdmin);
  webServer.onNotFound(resourceNotFoundHandler);
  Serial.println(F("Endpoint registration Complete."));

  Serial.println(F("Web Server initialization Complete."));

  /* Start WebServer */
  Serial.println(F("\nStarting Web Server..."));
  webServer.begin();
  Serial.printf("Web Server started with a status code of '%s'.\n", getServerStatusByNumber(webServer.getServer().status()).c_str());
  
  Serial.println("\nWeb Server startup is complete.");
}

/**
 * UTILITY FUNCTION
 * This function acts as a utility for deriving the human readable 
 * name of a status given its numeric value.
 * 
 * @param value The value to fetch the name of as int.
 * @returns The name of the value as a String.
*/
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

/**
 * #### ENDPOINT HANDLER "/" ####
 * This is the endpoint handler function for calls to the Root.
 * 
*/
void endpointHandlerRoot() {
  String rootPage = ROOT_PAGE;

  rootPage.replace("${rootWebTitle}", ROOT_WEB_TITLE);
  rootPage.replace("${hostName}", HOST_NAME);
  rootPage.replace("${firmwareVersion}", APP_VERSION);
  rootPage.replace("${chipId}", String(ESP.getChipId()));
  rootPage.replace("${coreVersion}", ESP.getCoreVersion());
  rootPage.replace("${cpuFreq}", String(ESP.getCpuFreqMHz()));
  rootPage.replace("${freeHeap}", String(((float)ESP.getFreeHeap()) / (float)1000));

  webServer.send(200, "text/html", rootPage);
}

/**
 * #### ENDPOINT HANDLER "/admin" ####
 * This is the endpoint handler function for calls to the /admin path.
 * 
*/
void endpointHandlerAdmin() {
  // FIXME: Lots of work to do here...
  String loginPage = LOGIN_PAGE;
  String adminPage = "";

  webServer.send(200, "text/html", loginPage);
}

/**
 * #### RESOURCE NOT FOUND ####
 * This function handles the response whenever a request is made to the WebServer
 * for a resource that doesn't exist.
 * 
*/
void resourceNotFoundHandler() {
  webServer.send(404, "text/html", NOT_FOUND_PAGE);
}