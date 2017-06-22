#ifndef __HTLOG_UAP__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>

regmatch_t * get_regex_matches(char * str, char * regex_str);

#ifdef __cplusplus
}
#endif
#endif
