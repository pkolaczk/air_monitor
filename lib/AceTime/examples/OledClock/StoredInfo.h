#ifndef OLED_CLOCK_STORED_INFO_H
#define OLED_CLOCK_STORED_INFO_H

#include <AceTime.h>

/** Data that is saved to and retrieved from EEPROM. */
struct StoredInfo {
  /** 12:00:00 AM to 12:00:00 PM */
  static uint8_t const kTwelve = 0;

  /** 00:00:00 - 23:59:59 */
  static uint8_t const kTwentyFour = 1;

  /** Either kTwelve or kTwentyFour. */
  uint8_t hourMode;

  /** TimeZone serialization. */
  ace_time::TimeZoneData timeZoneData;
};

#endif
