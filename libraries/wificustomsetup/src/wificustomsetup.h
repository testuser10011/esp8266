/*
  wificustomsetup.h
  Created by GC 2021
  Released into the public domain.
*/
#ifndef wificustomsetup_h
#define wificustomsetup_h

/* includes section*/
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <wificustomsetup.h>


class WifiCustomSetup
{
  private:
    /* wifi related */
    char* ssid;
    char* password;

    /*clientId*/
    String clientId;

    /*Over the air update parameters*/
    char * OTAPassword = NULL ;
    int OTAPort = 0;
    boolean useOTA = true ;
    int initOTA(char * OTAPassword, int OTAPort);
  
  /*networking related*/
    byte mac[6];
    void setupWifi();
    void displayMac();
    int wifiConnectionRetryTimer = 1000;


  public:
    // WifiCustomSetup(char* ssid, char* password);
    //  void ovverideDhcp(String ip);

    void serviceStart();
    void handleOTAClient();
    String setClientID(String cid);
    
    /*constructors*/
    WifiCustomSetup(char* ssid, char* password, boolean useOTA, char * OTAPassword, int OTAPort);
   
};
#endif
