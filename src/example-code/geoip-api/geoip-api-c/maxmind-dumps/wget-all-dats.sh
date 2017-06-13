#!/bin/bash
declare -a FILES=(GeoIP.dat GeoIPASNum.dat GeoIPCity.dat GeoIPDomain.dat GeoIPISP.dat GeoIPNetSpeedCell.dat GeoIPOrg.dat GeoIPRegion.dat GeoIPv6.dat GeoLiteCityIPv6.dat)
URL="https://github.com/maxmind/geoip-api-php/raw/master/tests/data/"
for FILE in "${FILES[@]}"; do {
  wget $URL$FILE;
}; done;
