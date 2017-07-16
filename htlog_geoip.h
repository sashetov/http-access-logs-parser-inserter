#ifndef __HTLOG_GEOIP__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "GeoIP.h"
#include "GeoIPCity.h"
#if !defined(_MSC_VER)
#include <sys/time.h>
#endif
#if defined(_WIN32)
  #define DATADIR  SRCDIR "/data/"
#else
  #define DATADIR  "/usr/local/share/GeoIP/"
#endif
#define GEOIP_OPEN(basename, flg)                  \
    do {                                           \
        gip = GeoIP_open(DATADIR basename,           \
                       (flg) | GEOIP_SILENCE);     \
        if (gip == NULL) {                           \
            printf("error: %s%s does not exist\n", \
                   DATADIR, basename);             \
            return NULL;                                \
            /* or a 'longjmp(geo_jmp,1)'? */       \
        }                                          \
    } while (0)
#define __HTLOG_GEOIP__
char * get_geoip_country( int flags, char *ip_string );
#ifdef __cplusplus
}
#endif
#endif
