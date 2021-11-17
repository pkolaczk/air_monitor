#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "ThSensor.h"
#include "PmSensor.h"

class WebServer {
  public:
    WebServer(
      uint16_t port, 
      ThSensor& thSensor, 
      PmSensor& pmSensor);

    void begin();
    void loop();

  private:
    ESP8266WebServer server;
    ThSensor& thSensor;
    PmSensor& pmSensor;

    void handleIndex();
    void handleSensor();
    void handleFileRead();
};
#endif /* WEBSERVER_H */
