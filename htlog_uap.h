#ifndef __HTLOG_UAP__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>

char *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches );
#ifdef __cplusplus
}
#endif
#endif
