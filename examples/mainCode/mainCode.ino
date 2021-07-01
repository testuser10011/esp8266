#include <wificustomsetup.h>
#include <customhttptools.h>
char* ssid = "SET_WIFI_SSID";
char* WIFIPassword =  "SET_WIFI_PASSWORD";
int pin = 3;
String clientId = "SET_ID_HERE";
char* OTAPassword = "SET_OTA_PASSWORD";
int OTAPort = 8080;
int HTTPPort = 80;
int numReads = 100;



WifiCustomSetup wifiObject(ssid, WIFIPassword, true, OTAPassword, OTAPort);
RelayWebService httpObject(HTTPPort, clientId, pin);
/*Temperature*/
//TemperatureWebService httpObject(HTTPPort, clientId,pin);


void setup() {
  Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY); //added to use GPIO 3 RX as input for ESP8266-01
  Serial.print(F("Booting "));
  Serial.println(clientId);
  wifiObject.setClientID(clientId);
  wifiObject.serviceStart();
  httpObject.serviceStart();
}

void loop() {
  /*Relay */

  httpObject.updateMDNS();
  wifiObject.handleOTAClient();
  /*Temperature*/
  httpObject.readMultipleValues(numReads,300000);
  httpObject.server.handleClient();
}
