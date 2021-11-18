#define ESP8266  // Needed by AceTime to include proper Wifi libs

#include <AceTime.h>
#include <Arduino.h>
#include <Astra.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>
#include <RH_ASK.h>
#include <SoftwareSerial.h>

#include "Pins.h"
#include "Lcd.h"
#include "Log.h"
#include "ThSensor.h"
#include "PmSensor.h"
#include "Publisher.h"
#include "WebServer.h"

using namespace ace_time;
using namespace ace_time::clock;

static BasicZoneProcessor tzProcessor;

const time_t LOG_INTERVAL_SECONDS = 60 * 10;

TimeZone timeZone = TimeZone::forZoneInfo(&zonedb::kZoneEurope_Warsaw, &tzProcessor);
NtpClock ntpClock("2.pl.pool.ntp.org");
SystemClockLoop systemClock(&ntpClock, nullptr);

Log thLog("/log/th/", systemClock, timeZone);
Log pmLog("/log/pm/", systemClock, timeZone);
ThSensor thSensor(PIN_D7);
PmSensor pmSensor(PIN_D5, PIN_D6);
WebServer server(80, thSensor, pmSensor);
Lcd lcd(0x27, PIN_D3);
Publisher publisher(thSensor, pmSensor, systemClock);

RH_ASK receiver(2000, PIN_D8);
char buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);



void setupWiFi() {
  Serial.println("Reading network credentials SPIFFS...");    
  fs::File secret = SPIFFS.open("/secret/wifi.txt", "r");
  String ssid = secret.readStringUntil('\n');
  String password = secret.readStringUntil('\n');  
  secret.close();  

  Serial.println("Connecting to network " + ssid + "..."); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("*");
  }

  Serial.println("");
  Serial.println("Connected successfully. Local IP is " + WiFi.localIP().toString());
}

void maybeAppendPmLog() {
  if (systemClock.getNow() - pmLog.getEndTime() > LOG_INTERVAL_SECONDS - pmSensor.WARM_UP_DELAY_MILLIS / 1000) {    
    pmSensor.wakeUp();
  }
  if (systemClock.getNow() - pmLog.getEndTime() > LOG_INTERVAL_SECONDS) {
    if (pmSensor.save(pmLog)) {
      Serial.println("Appended PM log");    
    }
  }
}

void maybeAppendThLog() {
  if (systemClock.getNow() - thLog.getEndTime() > LOG_INTERVAL_SECONDS) {
    if (thSensor.save(thLog)) {
      Serial.println("Appended temperature/humidity log");    
    }
  }  
}

void setup() {
  pinMode(PIN_D0, INPUT_PULLDOWN_16);
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D8, INPUT);
  Serial.begin(115200);  
  lcd.begin();
  SPIFFS.begin();
  thSensor.begin();
  pmSensor.begin();
  setupWiFi();
  ntpClock.setup();  
  systemClock.setup();
  server.begin();
  if (receiver.init()) 
    Serial.println("RF433 receiver initialized ok");
  publisher.init();
}


void loop() {
  systemClock.loop();
  lcd.loop();
  server.loop();
  thSensor.loop();
  pmSensor.loop();  
  maybeAppendThLog();
  maybeAppendPmLog();  
  publisher.loop();

  if (receiver.recv((uint8_t*) buf, &buflen)) 
    Serial.printf("Received temperature: %d.%d\n", buf[0] - 100, buf[1]);

  if (digitalRead(PIN_D0) == 1) {
    lcd.backlight();
  }

  lcd.setTime(ZonedDateTime::forEpochSeconds(systemClock.getNow(), timeZone));
  lcd.setTemperature(thSensor.getTemperature());
  lcd.setHumidity(thSensor.getHumidity());
  lcd.setPM1(pmSensor.getPm1());
  lcd.setPM2_5(pmSensor.getPm2_5());
  lcd.setPM10(pmSensor.getPm10());  
  
}

