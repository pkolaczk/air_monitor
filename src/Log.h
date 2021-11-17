#ifndef LOGGER_H
#define LOGGER_H

#include <AceTime.h>
#include <Arduino.h>

using namespace ace_time;
using namespace ace_time::clock;


class LogRecord {
  friend class Log;
  public:

    template<typename T>
    bool write(const T& value) {
      if (pos + sizeof(value) > MAX_SIZE)
        return false;  
      memcpy(buffer + pos, &value, sizeof(value));
      pos += sizeof(value);
      return true;
    }

  private: 
    static const size_t MAX_SIZE = 16;
    byte pos = 0;
    byte buffer[MAX_SIZE];
};

class Log {
  public:
    Log(const String& logPrefix, const Clock& clock, const TimeZone& tz);
    void write(const LogRecord& record);
    acetime_t getEndTime();

  private:
    static const unsigned long SECONDS_IN_DAY;

    const String fileNamePrefix;
    const Clock& clock;
    const TimeZone& timeZone;

    acetime_t logEndTime;    
    String getFileName(acetime_t time);
    String getDateStr(acetime_t time);  
};


#endif /* LOGGER_H */
