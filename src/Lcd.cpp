#include <Arduino.h>

#include "Lcd.h"

static const int LCD_BACKLIGHT_ON_LEVEL = 1024;
static const int LCD_BACKLIGHT_DIM_LEVEL = 64;
static const int LCD_BACKLIGHT_DURATION_MILLIS = 15 * 1000;
static const char* MONTHS[] = {"sty", "lut", "mar", "kwi", "maj", "cze", "lip", "sie", "wrz", "paz", "lis", "gru"};

using namespace ace_time;
using namespace ace_time::clock;

Lcd::Lcd(uint8_t addr, uint8_t backlightPin): 
  lcd(addr, 20, 4), 
  backlightPin(backlightPin),
  time(ZonedDateTime::forError()) {
}

void Lcd::begin() {
  pinMode(backlightPin, OUTPUT);
  backlight();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  lcd.print("      Miernik ");
  lcd.setCursor(0, 2);
  lcd.print(" Jakosci Powietrza");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
  backlightTime = millis();
}

void Lcd::loop() {
  if (needsUpdate) {    
    lcd.setCursor(0, 0);
    if (time.isError())
      lcd.printf("--:--:--      -- ---");    
    else  
      lcd.printf("%02d:%02d:%02d      %2d %s", time.hour(), time.minute(), time.second(), time.day(), MONTHS[time.month() - 1]);    
    lcd.setCursor(0, 1);
    lcd.printf("          %4d ug/m3", this->pm10);      
    lcd.setCursor(0, 2);
    lcd.printf("%4.1f %cC   %4d ug/m3", this->temperature, (char) 223, this->pm2_5);  
    lcd.setCursor(0, 3);
    lcd.printf("%4.1f %%    %4d ug/m3", this->humidity, this->pm1); 
    needsUpdate = false;
  }

  if (backlightOn && millis() - backlightTime > LCD_BACKLIGHT_DURATION_MILLIS) {
    analogWrite(backlightPin, LCD_BACKLIGHT_DIM_LEVEL);
    backlightOn = false;
  }
}

void Lcd::backlight() {  
  if (!backlightOn) {
    backlightOn = true;
    backlightTime = millis();    
    analogWrite(backlightPin, LCD_BACKLIGHT_ON_LEVEL);
  }
}

void Lcd::setTime(ZonedDateTime time) {
  needsUpdate = needsUpdate || this->time != time;
  this->time = time;    
}

void Lcd::setTemperature(float t) {
  temperatureUpdateTime = millis();
  needsUpdate = needsUpdate || this->temperature != t;
  temperature = t;  
}

void Lcd::setHumidity(float h) {
  needsUpdate = needsUpdate || this->humidity != h;
  humidity = h;
}

void Lcd::setPM1(int value) {
  pmsUpdateTime = millis();
  needsUpdate = needsUpdate || this->pm1 != value;
  pm1 = value;  
}

void Lcd::setPM2_5(int value) {
  needsUpdate = needsUpdate || this->pm2_5 != value;
  pm2_5 = value;
}

void Lcd::setPM10(int value) {
  needsUpdate = needsUpdate || this->pm10 != value;
  pm10 = value;    
}
