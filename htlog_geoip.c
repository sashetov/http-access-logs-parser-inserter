#ifndef __HTLOG_GEOIP__
#include "htlog_geoip.h"
char * get_geoip_country( int flags, char *ip_string ) {
  GeoIP *gip = NULL;
  GeoIPRecord *gipr = NULL;
  GEOIP_OPEN("GeoLiteCity.dat", flags);
  gipr = GeoIP_record_by_addr(gip, ip_string);
  char * ip_country = (char *)strdup(gipr->country_name);
  GeoIPRecord_delete(gipr);
  GeoIP_delete(gip);
  return ip_country;
}
#endif
