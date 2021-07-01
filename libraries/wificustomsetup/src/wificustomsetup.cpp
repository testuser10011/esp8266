/*
  wificustomsetup.h
  Created by GC 2021
  Released into the public domain.
*/

/* includes section*/
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <wificustomsetup.h>


WifiCustomSetup::WifiCustomSetup(char* ssid, char* password, boolean useOTA, char * OTAPassword, int OTAPort)
    {
      this->ssid = ssid;
      this->password = password;
      this->useOTA = useOTA;
      this->OTAPassword = OTAPassword;
      this->OTAPort = OTAPort;
    }

/*set clientID*/
String WifiCustomSetup::setClientID(String cid)
{
  if (cid == "" ) {
    this->clientId = WiFi.hostname().c_str();
  }
  else {
    this->clientId = cid;
  }
  return this->clientId;
}

/* display WIFI mac address */
void WifiCustomSetup::displayMac() {
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);
}

int WifiCustomSetup::initOTA(char * OTAPassword = NULL, int OTAPort = 0) {

  if (OTAPort > 0) ArduinoOTA.setPort(OTAPort);
  if (!OTAPassword == NULL) ArduinoOTA.setPassword(OTAPassword);

  // Hostname for OTA
  ArduinoOTA.setHostname(clientId.c_str());

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
    return error;
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  return 0;
}

/*handle OTA requests*/
void WifiCustomSetup::handleOTAClient()
{
    return ArduinoOTA.handle();
}


/*initiate wifi connection*/
void WifiCustomSetup::setupWifi() {
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(clientId);
  // ovverideDhcp();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(wifiConnectionRetryTimer);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  return;
}
/*start services*/
void WifiCustomSetup::serviceStart() {
  this->setupWifi() ;
  displayMac();
  if (useOTA) {
    initOTA(OTAPassword, OTAPort);
  }
  return;
}