/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include "common/util.h"
#include "common/DateStrings.h"
#include "ZonedDateTime.h"

namespace ace_time {

using common::printPad2;

// Print ZonedDateTime in ISO 8601 format
void ZonedDateTime::printTo(Print& printer) const {
  if (isError()) {
    printer.print(F("<Invalid ZonedDateTime>"));
    return;
  }

  mOffsetDateTime.printTo(printer);
  printer.print('[');
  mTimeZone.printTo(printer);
  printer.print(']');
}

}
