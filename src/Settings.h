#ifndef Settings_h
    #define Settings_h
    
    #include <ESP8266WiFi.h>
    #include "Secrets.h"

    /* *********************** *
     * Device Related Settings *
     * *********************** */
    const unsigned long SERIAL_SPEED = 115200ul;
    const String HOST_NAME = "deadspace001";

    /* ********************* *
     * WiFi Related Settings *
     * ********************* */
    #ifdef WIFI_SSID // Desired SSID is defined in Secrets.h...
        const char *SSID = WIFI_SSID;
    #else // Desired SSID is not defined; So set it here...
        const char *SSID = "Deadspace001";
    #endif
    #ifdef WIFI_PSK // Desired PSK is defined in Secrets.h...
        const char *PSK = WIFI_PSK;
    #else // Desired PSK is not defined; So set it here...
        const char *PSK = "Pa$$w0rd"; // <-- TODO: Change me!!!
    #endif

    /* ************************ *
     * Network Related Settings *
     * ************************ */
    const IPAddress DEVICE_IP = IPAddress(192, 168, 1, 1);
    const IPAddress GATEWAY_IP = IPAddress(0, 0, 0, 0);
    const IPAddress SUBNET_MASK = IPAddress(255, 255, 255, 0);

    /* ************************ *
     * WebPage Related Settings *
     * ************************ */
    const String ROOT_WEB_TITLE = "Deadspace AP";

#endif
