/*
  wificustomsetup.h
  Created by GC 2021
  Released into the public domain.
*/
#ifndef customhttptools_h
#define customhttptools_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <DHT.h>
#include <ESP8266mDNS.h>

class WebService
{
  protected:
    int httpPort;
    /*clientId*/
    String clientId;

    /*boolean to activate mDNS service*/
    bool useMDNS = true;
    /*start MDNS service */
    int startMDNS();
  public :
    ESP8266WebServer server;


    virtual void handleRoot()= 0;
    virtual void handleCommand() = 0;
    virtual void handleOutput()= 0;
    void handleNotFound();
    WebService(int httpPort, String clientId);
    //startServices
    void serviceStart();
    //later to implement serviceStop
    /*define mDNS setter*/
    void setMDNS(bool value);
    /*define mDNS getter*/
    bool getMDNS();
    int  updateMDNS();

};


class RelayWebService : public WebService
{
  protected:
    const byte relON[4] = {0xA0, 0x01, 0x01, 0xA2};  // Hex command to send to serial for open relay
    const byte relOFF[4] = {0xA0, 0x01, 0x00, 0xA1}; // Hex command to send to serial for close relay
    int pin = 3; // PIN on ESP8266 board used for returning status
    /*variable storing IO status*/
    int lastPinState = HIGH;

    /*relay control*/
    /* commands relay ON via Serial */
    void start();

    /* commands relay OFF via Serial */
    void stop();

    /*Toggles relay ON / OFF like a push switch*/
    void startStop(int relayDelay);
    /*http handles */
    void handleOutput();
    void handleCommand();
    void handleRoot();

  public :

    RelayWebService(int httpPort, String clientId, int pin);

    /* reads IO status */
    int getState(int pin);

};

class TemperatureWebService : public WebService
{
#define DHTTYPE DHT22
#define DHTPIN 2

  protected:
    DHT dht{DHTPIN, DHTTYPE}; //hardcoded for module => to be reworked on need
    float temperature = 0.0;
    float humidity = 0.0;
    float heatIndex = 0.0;
    unsigned long old_time;
    unsigned long new_time;
    /*factors influencing reading average over a time frame due to sensor variance*/
    static const int period = 300000;//5 minutes * 60 seconds * 1000 ( miliseconds)
    const float multiplyFactor = 0.87;// error factor HW specific
    int numReads = 100;

    /*http handles */
    void handleOutput();
    void handleCommand();
    void handleRoot();


    bool checkValues(int humidity, int temperature, int heatIndex);

  public :

    TemperatureWebService(int httpPort, String clientId,int tPin);
    /* convert integer to String*/
    char* intToStr(int data);
    /* reads defined number of times values for temperature,humididy in a time frame and provides as output an average and heat index */
    void readMultipleValues(int numValues, int timeFrame);

};


#endif
