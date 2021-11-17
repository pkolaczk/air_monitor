#ifndef OLED_CLOCK_CLOCK_INFO_H
#define OLED_CLOCK_CLOCK_INFO_H

#include <AceTime.h>

/** Information about the clock, mostly independent of rendering. */
struct ClockInfo {
  /** 12:00:00 AM to 12:00:00 PM */
  static uint8_t const kTwelve = 0;

  /** 00:00:00 - 23:59:59 */
  static uint8_t const kTwentyFour = 1;

  /** 12/24 mode */
  uint8_t hourMode;

  /** The desired timezone of the clock. */
  ace_time::TimeZone timeZone;

  /** Current time. */
  ace_time::ZonedDateTime dateTime;
};

#endif
