#ifndef PMSENSOR_H
#define PMSENSOR_H

#include <PMS.h>
#include <SoftwareSerial.h>

#include "Log.h"

class PmSensor {
public:
  PmSensor(uint8_t rxPin, uint8_t txPin);
  void begin();
  void loop();
  bool save(Log& log);

  uint16_t getPm10() const;
  uint16_t getPm2_5() const;
  uint16_t getPm1() const;

  void sleep();
  void wakeUp();
  bool isReady() const;

  static const time_t SLEEP_DELAY_MILLIS;
  static const time_t WARM_UP_DELAY_MILLIS;

private:
  SoftwareSerial pmsSerial;
  PMS pms;  
  PMS::DATA pmsData;
  time_t initTime;
  time_t lastWakeUpRequestTime;
  time_t lastDataReceivedTime;
  bool active;
  bool ready;  

};


#endif /* PMSENSOR_H */
