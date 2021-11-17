#include "PmSensor.h"
#include "Pins.h"

const time_t PmSensor::SLEEP_DELAY_MILLIS = 30000;   // must be longer than WARM_UP_DELAY_MILLIS
const time_t PmSensor::WARM_UP_DELAY_MILLIS = 20000;

PmSensor::PmSensor(uint8_t rxPin, uint8_t txPin): 
  pmsSerial(rxPin, txPin), 
  pms(pmsSerial),
  active(true),
  ready(false) {    
}

void PmSensor::begin() {
  pmsSerial.begin(9600);
  pms.wakeUp();
  time_t currentTime = millis();
  lastDataReceivedTime = 0;
  lastWakeUpRequestTime = currentTime;
  initTime = currentTime;
  digitalWrite(PIN_D4, LOW);
}

void PmSensor::loop() {
  if (pms.read(pmsData)) {
    if (!active || millis() - lastWakeUpRequestTime > SLEEP_DELAY_MILLIS)
      sleep();
    else if (!ready && millis() - initTime > WARM_UP_DELAY_MILLIS) {
      Serial.println("PM sensor ready and warmed up");    
      ready = true;
    }

    lastDataReceivedTime = millis();
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(pmsData.PM_AE_UG_1_0);
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(pmsData.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(pmsData.PM_AE_UG_10_0);  
  } 
  // If we didn't receive any data from the sensor since initTime, but active is set to true, let's try to wakeup the sensor once again:
  if (active && lastDataReceivedTime < initTime && millis() - initTime > 5000) {    
    Serial.println("No response from PM sensor for 5s, waking it up again...");
    pms.wakeUp();
    lastWakeUpRequestTime = millis();
    initTime = millis();
  }
}

void PmSensor::sleep() {
  Serial.println("Suspending the PM sensor...");
  pms.sleep();
  active = false;  
  ready = false;
  digitalWrite(PIN_D4, HIGH);
}

void PmSensor::wakeUp() {
  digitalWrite(PIN_D4, LOW);
  if (!active) {
    Serial.println("Waking up the PM sensor...");
    pms.wakeUp();
    active = true;
    initTime = millis();
  }
  lastWakeUpRequestTime = millis();
}

bool PmSensor::isReady() const {
  return active && ready;    
}

bool PmSensor::save(Log& log) {
  if (!isReady()) 
    return false;    
  LogRecord record;
  record.write(pmsData.PM_AE_UG_1_0);
  record.write(pmsData.PM_AE_UG_2_5);
  record.write(pmsData.PM_AE_UG_10_0);
  log.write(record);
  return true;
}

uint16_t PmSensor::getPm10() const {
  return pmsData.PM_AE_UG_10_0;
}

uint16_t PmSensor::getPm2_5() const {
  return pmsData.PM_AE_UG_2_5;
}

uint16_t PmSensor::getPm1() const {
  return pmsData.PM_AE_UG_1_0;
}
