#include <FS.h>
#include "Log.h"


const unsigned long Log::SECONDS_IN_DAY = 24 * 3600;

Log::Log(const String& dirName, const Clock& clock, const ace_time::TimeZone& tz): 
    fileNamePrefix(dirName),
    timeZone(tz),
    clock(clock),
    logEndTime(0) {
}

void Log::write(const LogRecord& record) {
  acetime_t currentTime = clock.getNow();
  time_t unixTime = LocalDateTime::forEpochSeconds(currentTime).toUnixSeconds();
  String fileName = getFileName(currentTime);
  Serial.print("Appending entry to log file: ");
  Serial.println(fileName);  
  File file = SPIFFS.open(fileName, "a");
  file.write(record.pos + sizeof(currentTime) + 2);
  file.write((char*) &unixTime, sizeof(unixTime));
  file.write((char*) record.buffer, record.pos);
  file.write(0);  // reserved for future use, e.g. checksum
  file.write(0);  // reserved for future use, e.g. checksum
  file.close();
  logEndTime = currentTime;
}

String Log::getFileName(acetime_t currentTime) {
  return fileNamePrefix + getDateStr(currentTime);
}

acetime_t Log::getEndTime() {
  return logEndTime; 
}

String Log::getDateStr(acetime_t epoch) {
  ZonedDateTime time = ZonedDateTime::forEpochSeconds(epoch, timeZone);
  char buf[11];
  sprintf(buf, "%d-%02d-%02d", time.year(), time.month(), time.day());
  return String(buf);
}

