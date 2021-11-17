#include <AceTime.h>
#include <LiquidCrystal_I2C.h>

class Lcd {

  LiquidCrystal_I2C lcd;
  uint8_t backlightPin;

  ace_time::ZonedDateTime time;
  float temperature = NAN;
  float humidity = NAN;
  uint16_t pm1 = -1;
  uint16_t pm2_5 = -1;
  uint16_t pm10 = -1;
  
  long temperatureUpdateTime = 0;
  long pmsUpdateTime = 0;

  long backlightTime = 0;
  bool backlightOn = false;
  bool needsUpdate = false;

  ace_time::BasicZoneProcessor warsawProcessor;

public:    

  Lcd(uint8_t addr, uint8_t backlightPin);
  void begin();
  void loop();

  void backlight();
  void setTime(ace_time::ZonedDateTime time);
  void setTemperature(float t);
  void setHumidity(float h);
  void setPM1(int value);
  void setPM2_5(int value);
  void setPM10(int value);
};
