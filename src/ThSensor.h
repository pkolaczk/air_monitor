#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <dhtnew.h>

#include "Log.h"

/** Temperature and humidity sensor */
class ThSensor {
public:

  ThSensor(uint8_t pin);
  void begin();
  void loop();
  bool save(Log& log);

  boolean isReady() const;
  float getTemperature() const;
  float getHumidity() const;
  unsigned long getTimestamp() const;

private:
  DHTNEW dht;
  float temperature;
  float humidity;
  unsigned long timestamp;  

  static const unsigned int READ_INTERVAL_MILLIS;
};

#endif /* TEMPSENSOR_H */
