/*
 * MIT License
 * Copyright (c) 2019 Brian T. Park
 */

#ifndef ACE_TIME_ZONE_MANAGER_H
#define ACE_TIME_ZONE_MANAGER_H

#include "ZoneProcessorCache.h"
#include "ZoneRegistrar.h"
#include "TimeZoneData.h"
#include "TimeZone.h"

namespace ace_time {

/**
 * Returns the TimeZone given the zoneInfo, zoneName, or zoneId. Looks up the
 * ZoneInfo in the ZoneRegistrar. If an existing ZoneProcessor exists in the
 * ZoneProcessorCache, then it is used. If not, another ZoneProcessor is picked
 * from the cache in a round-robin fashion. The type of the TimeZone will be
 * assigned to be the type of the ZoneProcessorCache, which will be either
 * kTypeBasicManaged or kTypeExtendedManaged.
 *
 * @tparam ZI type of ZoneInfo (basic::ZoneInfo or extended::ZoneInfo) which
 *    make up the zone registry
 * @tparam ZR class of ZoneRegistrar
 * @tparam ZSC class of ZoneProcessorCache
 */
template<typename ZI, typename ZR, typename ZSC>
class ZoneManager {
  public:
    const ZR& getRegistrar() const { return mZoneRegistrar; }

    TimeZone createForZoneInfo(const ZI* zoneInfo) {
      if (! zoneInfo) return TimeZone::forError();
      return TimeZone(zoneInfo, &mZoneProcessorCache);
    }

    TimeZone createForZoneName(const char* name) {
      const ZI* zoneInfo = mZoneRegistrar.getZoneInfoForName(name);
      return createForZoneInfo(zoneInfo);
    }

    TimeZone createForZoneId(uint32_t id) {
      const ZI* zoneInfo = mZoneRegistrar.getZoneInfoForId(id);
      return createForZoneInfo(zoneInfo);
    }

    TimeZone createForZoneIndex(uint16_t index) {
      const ZI* zoneInfo = mZoneRegistrar.getZoneInfoForIndex(index);
      return createForZoneInfo(zoneInfo);
    }

    /**
     * Create from the TimeZoneData created by TimeZone::toTimeZoneData().
     * kTypeBasic is converted into a kTypeBasicManaged, and kTypeExtended is
     * converted into a kTypeExtendedManaged.
     */
    TimeZone createForTimeZoneData(const TimeZoneData& d) {
      switch (d.type) {
        case TimeZone::kTypeError:
          return TimeZone::forError();
        case TimeZone::kTypeManual:
          return TimeZone::forTimeOffset(
              TimeOffset::forMinutes(d.stdOffsetMinutes),
              TimeOffset::forMinutes(d.dstOffsetMinutes));
        case TimeZone::kTypeBasic:
        case TimeZone::kTypeExtended:
          return createForZoneId(d.zoneId);
        default:
          return TimeZone();
      }
    }

    uint16_t indexForZoneName(const char* name) const {
      const ZI* zoneInfo = mZoneRegistrar.getZoneInfoForName(name);
      if (! zoneInfo) return 0;
      return (zoneInfo - mZoneRegistrar.getZoneInfoForIndex(0));
    }

    uint16_t indexForZoneId(uint32_t id) const {
      const ZI* zoneInfo = mZoneRegistrar.getZoneInfoForId(id);
      if (! zoneInfo) return 0;
      return (zoneInfo - mZoneRegistrar.getZoneInfoForIndex(0));
    }

  protected:
    ZoneManager(uint16_t registrySize, const ZI* const* zoneRegistry):
        mZoneRegistrar(registrySize, zoneRegistry),
        mZoneProcessorCache() {}

  private:
    // disable copy constructor and assignment operator
    ZoneManager(const ZoneManager&) = delete;
    ZoneManager& operator=(const ZoneManager&) = delete;

    const ZR mZoneRegistrar;
    ZSC mZoneProcessorCache;
};

#if 1
/**
 * @tparam SIZE size of the BasicZoneProcessorCache
 */
template<uint16_t SIZE>
class BasicZoneManager: public ZoneManager<basic::ZoneInfo,
    BasicZoneRegistrar, BasicZoneProcessorCache<SIZE>> {
  public:
    BasicZoneManager(uint16_t registrySize,
        const basic::ZoneInfo* const* zoneRegistry):
        ZoneManager<basic::ZoneInfo, BasicZoneRegistrar,
            BasicZoneProcessorCache<SIZE>>(registrySize, zoneRegistry) {}
};

/**
 * @tparam SIZE size of the ExtendedZoneProcessorCache
 */
template<uint16_t SIZE>
class ExtendedZoneManager: public ZoneManager<extended::ZoneInfo,
    ExtendedZoneRegistrar, ExtendedZoneProcessorCache<SIZE>> {
  public:
    ExtendedZoneManager(uint16_t registrySize,
        const extended::ZoneInfo* const* zoneRegistry):
        ZoneManager<extended::ZoneInfo, ExtendedZoneRegistrar,
            ExtendedZoneProcessorCache<SIZE>>(registrySize, zoneRegistry) {}
};

#else

// NOTE: The following typedef seems shorter and easier to maintain. The
// problem is that it makes error messages basically impossible to decipher
// because the immensely long full template class name is printed out. There
// seems to be no difference in code size between the two. The compiler seems
// to optimize away the vtables of the parent and child classes.

template<uint8_t SIZE>
using BasicZoneManager = ZoneManager<basic::ZoneInfo,
    BasicZoneRegistrar, BasicZoneProcessorCache<SIZE>>;

template<uint8_t SIZE>
using ExtendedZoneManager = ZoneManager<extended::ZoneInfo,
    ExtendedZoneRegistrar, ExtendedZoneProcessorCache<SIZE>>;

#endif

}

#endif
