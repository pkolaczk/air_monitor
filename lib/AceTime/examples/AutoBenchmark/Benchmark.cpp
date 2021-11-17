#include <stdint.h>
#include <Arduino.h>
#include <AceTime.h>
#include "Benchmark.h"
#include "ace_time/common/util.h"

using namespace ace_time;
using ace_time::common::printPad3;

#if defined(ARDUINO_ARCH_AVR)
const uint32_t COUNT = 2500;
#elif defined(ARDUINO_ARCH_SAMD)
const uint32_t COUNT = 10000;
#elif defined(ESP8266)
const uint32_t COUNT = 10000;
#elif defined(ESP32)
const uint32_t COUNT = 100000;
#elif defined(TEENSYDUINO)
const uint32_t COUNT = 100000;
#elif defined(UNIX_HOST_DUINO)
// Linux or MacOS
const uint32_t COUNT = 100000;
#else
// A generic Arduino board that we have not looked at.
const uint32_t COUNT = 10000;
#endif

const uint32_t MILLIS_TO_NANO_PER_ITERATION = ((uint32_t) 1000000 / COUNT);

// The FPSTR() macro converts these (const char*) into (const
// __FlashHelperString*) so that the correct version of println() or print() is
// called. ESP8266 and ESP32 already define this. AVR and Teensy do not.
#ifndef FPSTR
#define FPSTR(pstr_pointer) \
      (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#endif

// The following strings are placed into PROGMEM flash memory to prevent them
// from consuming static RAM on the AVR platform. This reduces the static RAM
// usage on an Nano from 1957 bytes to 1059 bytes, which allows this program to
// run on a controller with 2kB of RAM.
const char TOP[] PROGMEM =
  "+--------------------------------------------------+----------+";
const char HEADER[] PROGMEM =
  "| Method                                           |   micros |";
const char ROW_DIVIDER[] PROGMEM =
  "|--------------------------------------------------|----------|";
const char* const BOTTOM = TOP;
const char COL_DIVIDER[] PROGMEM = " |";
const char EMPTY_LOOP_LABEL[] PROGMEM =
  "| Empty loop                                       | ";
const char LOCAL_DATE_FOR_EPOCH_DAYS_LABEL[] PROGMEM =
  "| LocalDate::forEpochDays()                        | ";
const char LOCAL_DATE_TO_EPOCH_DAYS_LABEL[] PROGMEM =
  "| LocalDate::toEpochDays()                         | ";
const char LOCAL_DATE_DAY_OF_WEEK_LABEL[] PROGMEM =
  "| LocalDate::dayOfWeek()                           | ";

const char OFFSET_DATE_TIME_FOR_EPOCH_SECONDS_LABEL[] PROGMEM =
  "| OffsetDateTime::forEpochSeconds()                | ";
const char OFFSET_DATE_TIME_TO_EPOCH_SECONDS_LABEL[] PROGMEM =
  "| OffsetDateTime::toEpochSeconds()                 | ";

const char DATE_TIME_TO_EPOCH_DAYS_LABEL[] PROGMEM =
  "| ZonedDateTime::toEpochDays()                     | ";
const char DATE_TIME_TO_EPOCH_SECONDS_LABEL[] PROGMEM =
  "| ZonedDateTime::toEpochSeconds()                  | ";
const char DATE_TIME_FOR_EPOCH_SECONDS_LABEL[] PROGMEM =
  "| ZonedDateTime::forEpochSeconds(UTC)              | ";
const char DATE_TIME_FOR_EPOCH_SECONDS_BASIC_NO_CACHE[] PROGMEM =
  "| ZonedDateTime::forEpochSeconds(Basic nocache)    | ";
const char DATE_TIME_FOR_EPOCH_SECONDS_BASIC_CACHED[] PROGMEM =
  "| ZonedDateTime::forEpochSeconds(Basic cached)     | ";
const char DATE_TIME_FOR_EPOCH_SECONDS_EXTENDED_NO_CACHE[] PROGMEM =
  "| ZonedDateTime::forEpochSeconds(Extended nocache) | ";
const char DATE_TIME_FOR_EPOCH_SECONDS_EXTENDED_CACHED[] PROGMEM =
  "| ZonedDateTime::forEpochSeconds(Extended cached)  | ";

// The compiler is extremelly good about removing code that does nothing. This
// volatile variable is used to create side-effects that prevent the compiler
// from optimizing out the code that's being tested. Each disableOptimization()
// method should perform 6 XOR operations to cancel each other out when
// subtracted.
volatile uint8_t guard;

void disableOptimization(const LocalDate& ld) {
  guard ^= ld.year();
  guard ^= ld.month();
  guard ^= ld.day();
  guard ^= ld.year();
  guard ^= ld.month();
  guard ^= ld.day();
}

void disableOptimization(const ZonedDateTime& dt) {
  guard ^= dt.year();
  guard ^= dt.month();
  guard ^= dt.day();
  guard ^= dt.hour();
  guard ^= dt.minute();
  guard ^= dt.second();
}

void disableOptimization(const OffsetDateTime& dt) {
  guard ^= dt.year();
  guard ^= dt.month();
  guard ^= dt.day();
  guard ^= dt.hour();
  guard ^= dt.minute();
  guard ^= dt.second();
}

void disableOptimization(uint32_t value) {
  // Two temp variables allows 2 more XOR operations, for a total of 6.
  uint8_t tmp1, tmp2;

  guard ^= value & 0xff;
  guard ^= (value >> 8) & 0xff;
  guard ^= (tmp1 = (value >> 16) & 0xff);
  guard ^= (tmp2 = (value >> 24) & 0xff);
  guard ^= tmp1;
  guard ^= tmp2;
}

// A small helper that runs the given lamba expression in a loop
// and returns how long it took.
template <typename F>
unsigned long runLambda(uint32_t count, F&& lambda) {
  yield();
  unsigned long startMillis = millis();
  while (count--) {
    lambda();
  }
  unsigned long elapsedMillis = millis() - startMillis;
  yield();
  return elapsedMillis;
}

void printPad3(uint16_t val, char padChar) {
  if (val < 100) SERIAL_PORT_MONITOR.print(padChar);
  if (val < 10) SERIAL_PORT_MONITOR.print(padChar);
  SERIAL_PORT_MONITOR.print(val);
}

void printPad4(uint16_t val, char padChar) {
  if (val < 1000) SERIAL_PORT_MONITOR.print(padChar);
  if (val < 100) SERIAL_PORT_MONITOR.print(padChar);
  if (val < 10) SERIAL_PORT_MONITOR.print(padChar);
  SERIAL_PORT_MONITOR.print(val);
}

// Given total elapsed time in millis, print micros per iteration as a floating
// point number (without using floating point operations).
//
// Sometimes, the elapsedMillis is negative. This happens on some benchmarks on
// higher powered CPUs where the thing being measured is so quickly executed
// that the empty loop overhead can take a longer. Print "-0.000" if that
// occurs.
static void printMicrosPerIteration(long elapsedMillis) {
  if (elapsedMillis < 0) {
    SERIAL_PORT_MONITOR.print(F("  -0.000"));
    return;
  }
  unsigned long nanos = elapsedMillis * MILLIS_TO_NANO_PER_ITERATION;
  uint16_t whole = nanos / 1000;
  uint16_t frac = nanos % 1000;
  printPad4(whole, ' ');
  SERIAL_PORT_MONITOR.print('.');
  printPad3(frac, '0');
}

static void runEmptyLoop() {
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long tickMillis = millis();
    disableOptimization(tickMillis);
  });
  SERIAL_PORT_MONITOR.print(FPSTR(EMPTY_LOOP_LABEL));
  printMicrosPerIteration(emptyLoopMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// LocalDate::forEpochDays()
static void runLocalDateForEpochDays() {
  unsigned long localDateForDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    LocalDate localDate = LocalDate::forEpochDays(fakeEpochDays);
    disableOptimization(localDate);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long emptyMillis = millis();
    disableOptimization(emptyMillis);
  });
  long elapsedMillis = localDateForDaysMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(LOCAL_DATE_FOR_EPOCH_DAYS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// LocalDate::toEpochDays()
static void runLocalDateToEpochDays() {
  unsigned long localDateToEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    LocalDate localDate = LocalDate::forEpochDays(fakeEpochDays);
    acetime_t epochDays = localDate.toEpochDays();
    disableOptimization(epochDays);
  });
  unsigned long forEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    LocalDate localDate = LocalDate::forEpochDays(fakeEpochDays);
    disableOptimization(localDate);
  });
  long elapsedMillis = localDateToEpochDaysMillis - forEpochDaysMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(LOCAL_DATE_TO_EPOCH_DAYS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// LocalDate::dayOfWeek()
static void runLocalDateDaysOfWeek() {
  unsigned long localDateDayOfWeekMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    LocalDate localDate = LocalDate::forEpochDays(fakeEpochDays);
    uint8_t dayOfWeek = localDate.dayOfWeek();
    disableOptimization(localDate);
    disableOptimization(dayOfWeek);
  });
  unsigned long forEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    LocalDate localDate = LocalDate::forEpochDays(fakeEpochDays);
    disableOptimization(localDate);
  });
  long elapsedMillis = localDateDayOfWeekMillis - forEpochDaysMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(LOCAL_DATE_DAY_OF_WEEK_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// OffsetDateTime::forEpochSeconds()
static void runOffsetDateTimeForEpochSeconds() {
  unsigned long localDateForDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    OffsetDateTime odt = OffsetDateTime::forEpochSeconds(
        fakeEpochSeconds, TimeOffset());
    disableOptimization(odt);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long emptyMillis = millis();
    disableOptimization(emptyMillis);
  });
  long elapsedMillis = localDateForDaysMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(OFFSET_DATE_TIME_FOR_EPOCH_SECONDS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// OffsetDateTime::toEpochSeconds()
static void runOffsetDateTimeToEpochSeconds() {
  unsigned long localDateToEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    OffsetDateTime odt = OffsetDateTime::forEpochSeconds(
        fakeEpochSeconds, TimeOffset());
    acetime_t epochDays = odt.toEpochSeconds();
    disableOptimization(epochDays);
  });
  unsigned long forEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    OffsetDateTime odt = OffsetDateTime::forEpochSeconds(
        fakeEpochSeconds, TimeOffset());
    disableOptimization(odt);
  });
  long elapsedMillis = localDateToEpochDaysMillis - forEpochDaysMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(OFFSET_DATE_TIME_TO_EPOCH_SECONDS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// ZonedDateTime::forEpochSeconds(seconds)
static void runZonedDateTimeForEpochSeconds() {
  unsigned long forEpochSecondsMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(fakeEpochDays,
        TimeZone());
    disableOptimization(dateTime);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long emptyMillis = millis();
    disableOptimization(emptyMillis);
  });
  long elapsedMillis = forEpochSecondsMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_FOR_EPOCH_SECONDS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// ZonedDateTime::toEpochDays()
static void runZonedDateTimeToEpochDays() {
  unsigned long toEpochDaysMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(fakeEpochDays,
        TimeZone());
    acetime_t epochDays = dateTime.toEpochDays();
    disableOptimization(epochDays);
  });
  unsigned long forEpochSecondsMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(fakeEpochDays,
        TimeZone());
    disableOptimization(dateTime);
  });
  long elapsedMillis = toEpochDaysMillis - forEpochSecondsMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_TO_EPOCH_DAYS_LABEL));
  printMicrosPerIteration(elapsedMillis < 0 ? 0 : elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// ZonedDateTime::toEpochSeconds()
static void runZonedDateTimeToEpochSeconds() {
  unsigned long toEpochSecondsMillis = runLambda(COUNT, []() {
    unsigned long tickMillis = millis();
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(tickMillis,
        TimeZone());
    acetime_t epochSeconds = dateTime.toEpochSeconds();
    disableOptimization(epochSeconds);
  });
  unsigned long forEpochSecondsMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochDays = millis();
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(fakeEpochDays,
        TimeZone());
    disableOptimization(dateTime);
  });
  long elapsedMillis = toEpochSecondsMillis - forEpochSecondsMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_TO_EPOCH_SECONDS_LABEL));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

static const acetime_t kTwoYears = 2 * 365 * 24 * 3600L;

static const basic::ZoneInfo* const kBasicZoneRegistry[] ACE_TIME_PROGMEM = {
  &zonedb::kZoneAmerica_Chicago,
  &zonedb::kZoneAmerica_Denver,
  &zonedb::kZoneAmerica_Los_Angeles,
  &zonedb::kZoneAmerica_New_York,
};

static const uint16_t kBasicZoneRegistrySize =
    sizeof(kBasicZoneRegistry) / sizeof(kBasicZoneRegistry[0]);

// ZonedDateTime::forEpochSeconds(seconds, tz), uncached
static void runZonedDateTimeForEpochSecondsBasicZoneManager() {
	BasicZoneManager<2> manager(kBasicZoneRegistrySize, kBasicZoneRegistry);
  acetime_t offset = 0;

  unsigned long forEpochSecondsMillis = runLambda(COUNT, [&offset, &manager]() {
    offset = (offset) ? 0 : kTwoYears;
    unsigned long fakeEpochSeconds = millis() + offset;
    TimeZone tzLosAngeles = manager.createForZoneInfo(
        &zonedb::kZoneAmerica_Los_Angeles);
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(
        fakeEpochSeconds, tzLosAngeles);
    disableOptimization(dateTime);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    disableOptimization(fakeEpochSeconds);
  });
  long elapsedMillis = forEpochSecondsMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_FOR_EPOCH_SECONDS_BASIC_NO_CACHE));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// ZonedDateTime::forEpochSeconds(seconds, tz) cached
static void runZonedDateTimeForEpochSecondsBasicZoneManagerCached() {
	BasicZoneManager<2> manager(kBasicZoneRegistrySize, kBasicZoneRegistry);

  unsigned long forEpochSecondsMillis = runLambda(COUNT, [&manager]() {
    unsigned long fakeEpochSeconds = millis();
    TimeZone tzLosAngeles = manager.createForZoneInfo(
        &zonedb::kZoneAmerica_Los_Angeles);
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(
        fakeEpochSeconds, tzLosAngeles);
    disableOptimization(dateTime);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    disableOptimization(fakeEpochSeconds);
  });
  long elapsedMillis = forEpochSecondsMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_FOR_EPOCH_SECONDS_BASIC_CACHED));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

static const extended::ZoneInfo* const kExtendedZoneRegistry[]
    ACE_TIME_PROGMEM = {
  &zonedbx::kZoneAmerica_Chicago,
  &zonedbx::kZoneAmerica_Denver,
  &zonedbx::kZoneAmerica_Los_Angeles,
  &zonedbx::kZoneAmerica_New_York,
};

static const uint16_t kExtendedZoneRegistrySize =
    sizeof(kExtendedZoneRegistry) / sizeof(kExtendedZoneRegistry[0]);

// ZonedDateTime::forEpochSeconds(seconds, tz), uncached
static void runZonedDateTimeForEpochSecondsExtendedZoneManager() {
	ExtendedZoneManager<2> manager(
      kExtendedZoneRegistrySize, kExtendedZoneRegistry);
  acetime_t offset = 0;

  unsigned long forEpochSecondsMillis = runLambda(COUNT, [&offset, &manager]() {
    offset = (offset) ? 0 : kTwoYears;
    unsigned long fakeEpochSeconds = millis() + offset;
    TimeZone tzLosAngeles = manager.createForZoneInfo(
        &zonedbx::kZoneAmerica_Los_Angeles);
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(
        fakeEpochSeconds, tzLosAngeles);
    disableOptimization(dateTime);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    disableOptimization(fakeEpochSeconds);
  });
  long elapsedMillis = forEpochSecondsMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_FOR_EPOCH_SECONDS_EXTENDED_NO_CACHE));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

// ZonedDateTime::forEpochSeconds(seconds, tz) cached ExtendedZoneManager
static void runZonedDateTimeForEpochSecondsExtendedZoneManagerCached() {
	ExtendedZoneManager<2> manager(
      kExtendedZoneRegistrySize, kExtendedZoneRegistry);

  unsigned long forEpochSecondsMillis = runLambda(COUNT, [&manager]() {
    unsigned long fakeEpochSeconds = millis();
    TimeZone tzLosAngeles = manager.createForZoneInfo(
        &zonedbx::kZoneAmerica_Los_Angeles);
    ZonedDateTime dateTime = ZonedDateTime::forEpochSeconds(
        fakeEpochSeconds, tzLosAngeles);
    disableOptimization(dateTime);
  });
  unsigned long emptyLoopMillis = runLambda(COUNT, []() {
    unsigned long fakeEpochSeconds = millis();
    disableOptimization(fakeEpochSeconds);
  });
  long elapsedMillis = forEpochSecondsMillis - emptyLoopMillis;

  SERIAL_PORT_MONITOR.print(FPSTR(DATE_TIME_FOR_EPOCH_SECONDS_EXTENDED_CACHED));
  printMicrosPerIteration(elapsedMillis);
  SERIAL_PORT_MONITOR.println(FPSTR(COL_DIVIDER));
}

void runBenchmarks() {
  SERIAL_PORT_MONITOR.println(FPSTR(TOP));
  SERIAL_PORT_MONITOR.println(FPSTR(HEADER));
  SERIAL_PORT_MONITOR.println(FPSTR(ROW_DIVIDER));

  runEmptyLoop();
  SERIAL_PORT_MONITOR.println(FPSTR(ROW_DIVIDER));

  runLocalDateForEpochDays();
  runLocalDateToEpochDays();
  runLocalDateDaysOfWeek();

  runOffsetDateTimeForEpochSeconds();
  runOffsetDateTimeToEpochSeconds();

  runZonedDateTimeToEpochSeconds();
  runZonedDateTimeToEpochDays();

  runZonedDateTimeForEpochSeconds();
  runZonedDateTimeForEpochSecondsBasicZoneManager();
  runZonedDateTimeForEpochSecondsBasicZoneManagerCached();
  runZonedDateTimeForEpochSecondsExtendedZoneManager();
  runZonedDateTimeForEpochSecondsExtendedZoneManagerCached();

  SERIAL_PORT_MONITOR.println(FPSTR(BOTTOM));

  SERIAL_PORT_MONITOR.print(F("Number of iterations per run: "));
  SERIAL_PORT_MONITOR.println(COUNT);
}
