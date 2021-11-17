# Copyright 2019 Brian T. Park
#
# MIT License

import logging
from zone_specifier import ZoneSpecifier

class BufSizeEstimator:
    """Estimate the ExtendedZoneSpecifier::TransitionStorage buffer size for
    each zone.
    """

    def __init__(self, zone_infos, zone_policies, start_year, until_year):
        """
        Args:
            zone_infos (dict): {full_name -> zone_info{} }
            zone_policies (dict): {policy_name ->zone_policy{} }
        """
        self.zone_infos = zone_infos
        self.zone_policies = zone_policies
        self.start_year = start_year
        self.until_year = until_year

    def estimate(self):
        """Calculate the (dict) of {full_name -> buf_size} where buf_size is one
        more than the estimate from ZoneSpecifier.get_buffer_sizes(). Return
        the tuple of (buf_sizes, max_size).
        """
        buf_sizes = {}
        max_size = 0
        for zone_name, zone_info in self.zone_infos.items():
            zone_specifier = ZoneSpecifier(zone_info)
            (max_actives, max_buffer_size) = zone_specifier.get_buffer_sizes(
                self.start_year, self.until_year)

            # The TransitionStorage size should be one more than the estimate
            # because TransitionStorage.getFreeAgent() needs one slot even if
            # it's not used.
            buf_size = max_buffer_size[0] + 1

            # The estimate is off for Asia/Atyrau. zone_specifier.py returns
            # max_buffer_size[0]==4 which means 5 should be enough, but
            # TransitionStorage.getHighWater() says that 6 is required. Not sure
            # why.
            if zone_name == 'Asia/Atyrau':
                buf_size += 1

            buf_sizes[zone_name] = buf_size
            if buf_size > max_size:
                max_size = buf_size

        return (buf_sizes, max_size)
