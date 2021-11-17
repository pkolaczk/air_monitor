#include <FS.h>

#include "WebServer.h"


WebServer::WebServer(
      uint16_t port, 
      ThSensor& thSensor, 
      PmSensor& pmSensor): 
      thSensor(thSensor), 
      pmSensor(pmSensor), 
      server(port) { }

void WebServer::begin() {
    Serial.println("Starting server..."); 
  server.on("/", std::bind(&WebServer::handleIndex, this));
  server.on("/sensor", std::bind(&WebServer::handleSensor, this));
  server.serveStatic("/graphs.html", SPIFFS, "/ui/graphs.html");  
  server.serveStatic("/thermometer.svg", SPIFFS, "/ui/thermometer.svg");
  server.serveStatic("/droplet.svg", SPIFFS, "/ui/droplet.svg"); 
  server.serveStatic("/gas-mask.svg", SPIFFS, "/ui/gas-mask.svg");
  server.onNotFound(std::bind(&WebServer::handleFileRead, this));
  server.begin();
  Serial.println("Server started");
}

void WebServer::loop() {
  server.handleClient();
}

void WebServer::handleIndex() {
  Serial.println("Received a request for /");
  File index = SPIFFS.open("/ui/index.html", "r");
  String content = index.readString();
  index.close();
  content.replace("$temperature", String(thSensor.getTemperature()));
  content.replace("$humidity", String(thSensor.getHumidity()));
  content.replace("$pm10", String(pmSensor.getPm10()));
  content.replace("$pm2_5", String(pmSensor.getPm2_5()));
  content.replace("$pm1", String(pmSensor.getPm1()));
  content.replace("$pmready", String(pmSensor.isReady()));
  server.send(200, "text/html", content);    
}

void WebServer::handleSensor() {
  Serial.println("Received a request for /sensor");
  pmSensor.wakeUp();

  server.send(200, "application/json", 
  String("{ \"temperature\":") + thSensor.getTemperature() + 
         ", \"humidity\":" + thSensor.getHumidity() + 
         ", \"pm10\":" + pmSensor.getPm10() + 
         ", \"pm2_5\":" + pmSensor.getPm2_5() + 
         ", \"pm1\":" + pmSensor.getPm1() +  
         ", \"pmready\":" + pmSensor.isReady() + 
         " }\n");     
}

void WebServer::handleFileRead() {
  String uri = server.uri();
  if (SPIFFS.exists(uri) && (uri.startsWith("/ui/") || uri.startsWith("/log/"))) {
    String contentType = "application/octet-stream";
    if (uri.endsWith(".html"))
      contentType = "text/html";
    else if (uri.endsWith(".js"))
      contentType = "text/javascript";          
    else if (uri.endsWith(".svg"))
      contentType = "image/svg+xml";

    File file = SPIFFS.open(uri, "r");
    server.streamFile(file, contentType);
    file.close();
  }
  else {
    server.send(404, "text/plain", "Not found: " + uri);
  }
}
