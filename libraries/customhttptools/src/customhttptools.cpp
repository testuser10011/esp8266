/*
  customhttptools.h
  Created by GC 2021
  Released into the public domain.
*/

#include <customhttptools.h>




WebService::WebService(int httpPort = 80, String clientId = "") {
  /*to do rework this part for error messaging */
  this->httpPort = httpPort;
  this->clientId = clientId;
}

/*define mDNS setter*/
void  WebService::setMDNS(bool value) {
  this->useMDNS = value;
}
/*define mDNS getter*/
bool  WebService::getMDNS() {
  return this ->useMDNS;
}
/*start MDNS service ; returns 0 if succesfull*/
int  WebService::startMDNS() {
  if (int i = MDNS.begin(clientId)) {             // Start the mDNS responder for esp8266.local
    Serial.println(i + " Error setting up MDNS responder!");
    return i;
  }
  else {
    Serial.println("mDNS responder started");
    return 0;
  }
  MDNS.addService("http", "tcp", this ->httpPort);
}


/*update MDNS service used for periodic update in loop functions */
int  WebService::updateMDNS() {
  return MDNS.update();
}


/* starts all services */
void WebService::serviceStart() {
  server.on("/", std::bind(&WebService::handleRoot, this));
  server.on("/json", std::bind(&WebService::handleOutput, this));
  server.on("/RELAY", std::bind(&WebService::handleCommand, this));
  server.onNotFound(std::bind(&WebService::handleNotFound, this));
  server.begin(httpPort);
  if (getMDNS()) {
    startMDNS();
  }
}


/* return error for http failure on handle*/
void WebService::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


RelayWebService::RelayWebService(int httpPort, String clientId, int pin) {
  this->httpPort = httpPort;
  this->clientId = clientId;
  this->pin = pin;
}


/*relay control*/
/* commands relay ON via Serial */
void RelayWebService::start()
{
  Serial.write(relON, sizeof(relON));// CLOSE RELAY
  Serial.println("relay command was to set to on.");

}

/* commands relay OFF via Serial */
void RelayWebService::stop()
{
  Serial.write(relOFF, sizeof(relOFF));
  Serial.println("relay command was to set to off.");
}

/*Toggles relay ON / OFF like a push switch*/
void RelayWebService::startStop(int relayDelay = 300)
{

  Serial.println("***************startStop called***************");
  start();
  delay(relayDelay);
  stop();
  Serial.println("***************startStop call completed***************");
}
/* reads IO status */
int RelayWebService::getState(int pin)
{
  this->lastPinState =  digitalRead(pin);
  return lastPinState;
}

/*http server control*/
void RelayWebService::handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/RELAY\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
}


void RelayWebService::handleCommand() {                          // If a POST request is made to URI /LED
  startStop();
  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser => home page again
  server.send(303);                         // Send it back to the browser with an HTTP status redirect
}

void RelayWebService::handleOutput() {//Outputs contact status
  String message = "{";
  message += "\"door\":\"";
  message += String(getState(pin));
  message += "\",\"id\":\"";
  message += this->clientId ;
  message += "\"}";
  server.send(200, "application/json", message);
}


TemperatureWebService::TemperatureWebService(int httpPort = 80, String clientId = "",int tPin = 2) {
  this->httpPort = httpPort;
  this->clientId = clientId;
    //tPin unused to do in another release
  dht.begin();
  this->new_time = millis();
  this->old_time = new_time ;
}
char* TemperatureWebService::intToStr(int data) {
  String strData = String(data);
  char* temp = new char[strData.length() + 1];
  strcpy(temp, strData.c_str());
  return temp;
}


/* reads defined number of times values for temperature,humididy in a time frame and provides as output an average and heat index */
void TemperatureWebService::readMultipleValues(int numValues = 1, int timeFrame = period)

{
  int i = 0 ;
  float ahumidity, atemperature;
  ahumidity = 0;
  atemperature = 0;

  new_time = millis();
  if ( new_time - timeFrame < old_time )
  {
    return;
  }

  while (i < numValues)
  {
    ahumidity = ahumidity + dht.readHumidity();
    // Read temperature as Celsius (the default)
    atemperature = atemperature + dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (!checkValues(ahumidity, atemperature, 0)) {
      return;
    }
    delay(100);
    i++;
  }
  temperature = atemperature / numValues * multiplyFactor;
  humidity = ahumidity / numValues * multiplyFactor;
  heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  old_time = new_time;
  /*
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");
    Serial.print("Heat index: ");
    Serial.print(heatIndex);
    Serial.println(" *C ");
  */
}
/* protects output for NaN error*/
bool TemperatureWebService::checkValues(int humidity, int temperature, int heatIndex)
{
  if (isnan(humidity) || isnan(temperature) || isnan(heatIndex)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }
  return true;
}

void TemperatureWebService::handleRoot() {
  server.send(200, "text/html", "<h1>ESP8266 - Temperature and Humidity</h1>");
}


void TemperatureWebService::handleOutput() {
  // Check if any reads failed and exit early (to try again).
  if (!checkValues(this->humidity, this->temperature, this->heatIndex)) {
    return;
  }

  String message = "{";
  message += "\"temperature\":\"";
  message += String(this->temperature);
  message += "\",\"humidity\":\"";
  message += String(this->humidity);
  message += "\",\"heatIndex\":\"";
  message += String(this->heatIndex);
  message += "\",\"id\":\"";
  message += this->clientId ;
  message += "\"}";
  //Serial.println(message);
  server.send(200, "application/json", message);
}

void TemperatureWebService::handleCommand() { // Implement If a POST request is needed
 return;
}