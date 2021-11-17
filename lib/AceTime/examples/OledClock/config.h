#ifndef OLED_CLOCK_CONFIG_H
#define OLED_CLOCK_CONFIG_H

//------------------------------------------------------------------
// Configuration parameters.
//------------------------------------------------------------------

#define CLOCK_VERSION_STRING "0.2"

#define EEPROM_SIZE 32

// Set to 1 to print debugging info to SERIAL_PORT_MONITOR
#define ENABLE_SERIAL 0

// Set to 1 to force the ClockInfo to its initial state
#define FORCE_INITIALIZE 0

// Determine whether "auto" time zone uses Basic or Extended. Extended is too
// big for a Nano or Pro Micro, but will work on an ESP8266 or ESP32.
#define TIME_ZONE_TYPE_MANUAL 0
#define TIME_ZONE_TYPE_BASIC 1
#define TIME_ZONE_TYPE_EXTENDED 2
#define TIME_ZONE_TYPE TIME_ZONE_TYPE_BASIC

#define TIME_SOURCE_TYPE_NONE 0
#define TIME_SOURCE_TYPE_DS3231 1
#define TIME_SOURCE_TYPE_NTP 2
#define TIME_SOURCE_TYPE_BOTH 3 

#ifndef AUNITER
  // Arduino IDE in interactive mode
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP false
#elif defined(AUNITER_NANO)
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP false
#elif defined(AUNITER_MICRO) || defined(AUNITER_MICRO_MINDER)
  // Pro Micro does not have enough Flash to use Basic TimeZone
  // (28884 bytes) so use Manual TimeZone (23984 bytes).
  #undef TIME_ZONE_TYPE
  #define TIME_ZONE_TYPE TIME_ZONE_TYPE_MANUAL
  #define MODE_BUTTON_PIN 8
  #define CHANGE_BUTTON_PIN 9
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP true
#elif defined(AUNITER_MINI_MINDER)
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP false
#elif defined(AUNITER_MEGA)
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP false
#elif defined(AUNITER_SAMD)
  #define MODE_BUTTON_PIN 11
  #define CHANGE_BUTTON_PIN 10
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP true
#elif defined(AUNITER_ESP8266) || defined(AUNITER_ESP_MINDER)
  #define MODE_BUTTON_PIN D4
  #define CHANGE_BUTTON_PIN D3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP false
#elif defined(AUNITER_ESP_MINDER2)
  #define MODE_BUTTON_PIN D4
  #define CHANGE_BUTTON_PIN D3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_DS3231
  #define OLED_REMAP true
#elif defined(AUNITER_ESP32)
  #define MODE_BUTTON_PIN 4
  #define CHANGE_BUTTON_PIN 3
  #define TIME_SOURCE_TYPE TIME_SOURCE_TYPE_NTP
  #define OLED_REMAP true
#else
  #error Unknown AUNITER environment
#endif

//------------------------------------------------------------------
// Button state transition nodes.
//------------------------------------------------------------------

const uint8_t MODE_UNKNOWN = 0; // uninitialized
const uint8_t MODE_DATE_TIME = 1;
const uint8_t MODE_TIME_ZONE = 2;
const uint8_t MODE_ABOUT = 3;

const uint8_t MODE_CHANGE_YEAR = 10;
const uint8_t MODE_CHANGE_MONTH = 11;
const uint8_t MODE_CHANGE_DAY = 12;
const uint8_t MODE_CHANGE_HOUR = 13;
const uint8_t MODE_CHANGE_MINUTE = 14;
const uint8_t MODE_CHANGE_SECOND = 15;

#if TIME_ZONE_TYPE == TIME_ZONE_TYPE_MANUAL
const uint8_t MODE_CHANGE_TIME_ZONE_OFFSET = 20;
const uint8_t MODE_CHANGE_TIME_ZONE_DST = 21;
#else
const uint8_t MODE_CHANGE_TIME_ZONE_NAME = 20;
#endif

#endif
