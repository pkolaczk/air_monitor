#include <Arduino.h>

#include "ThSensor.h"

const unsigned int ThSensor::READ_INTERVAL_MILLIS = 5000;

ThSensor::ThSensor(uint8_t pin): 
  dht(pin), 
  temperature(0.0), 
  humidity(0.0), 
  timestamp(0) {    
}

float ThSensor::getTemperature() const {
  return temperature;    
}

float ThSensor::getHumidity() const {
  return humidity;    
}

unsigned long ThSensor::getTimestamp() const {
  return timestamp;    
}

boolean ThSensor::isReady() const {
  return timestamp > 0;
}

void ThSensor::begin() {  
  dht.setDisableIRQ(true);
  dht.setSuppressError(true);
  timestamp = millis();  
}

void ThSensor::loop() {
  if (millis() - timestamp > READ_INTERVAL_MILLIS) {
    dht.read();
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();
    timestamp = millis();

    Serial.print("temperature = ");
    Serial.println(temperature);
    Serial.print("humidity = ");
    Serial.println(humidity);
  }
}

bool ThSensor::save(Log& log) {
  if (!isReady())
    return false;
  int16_t t = (int16_t) (temperature * 10);
  int16_t h = (int16_t) (humidity * 10);
  LogRecord record;
  record.write(t);
  record.write(h);
  log.write(record);
  return true;    
}
