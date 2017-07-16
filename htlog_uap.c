#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
#endif
str_container_t *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches ) {
  int i=0, j=0, k=0;
  char * cursor = str;
  int container_size = (sizeof(str_container_t *));
  regex_t * regex = (regex_t *)malloc(sizeof(regex_t));
  str_container_t *** results = (str_container_t ***)
    malloc(num_groups * max_matches * container_size);
  regmatch_t group_matches[num_groups];
  if ( regcomp( regex, regex_str, REG_EXTENDED ) ) {
    printf("Could not compile regular expression.\n");
    exit(1);
  }
  unsigned int m = 0;
  for( m =0; m < max_matches; m++ ) {
    if(regexec( regex, cursor, num_groups, group_matches, 0) ){
      break;
    }
    unsigned int g = 0;
    unsigned int offset = 0;
    results[m] = (str_container_t **) malloc( num_groups * container_size );
    for (g = 0; g < num_groups; g++) {
      if (group_matches[g].rm_so == (size_t)-1) {
        break;
      }
      if ( !g ) {
        offset = group_matches[g].rm_eo;
      }
      results[m][g] = (str_container_t *) malloc( container_size );
      char str_copy[strlen(cursor) + 1];
      strcpy( str_copy, cursor );
      str_copy[group_matches[g].rm_eo] = 0;
      int cur_m = snprintf(
          NULL, 0, "%s", str_copy + group_matches[g].rm_so ) + 1;
      results[m][g]->str = (char *)malloc( cur_m );
      sprintf( results[m][g]->str , "%s", str_copy + group_matches[g].rm_so );
    }
    cursor += offset;
  }
  return results;
}
ua_t * parse_user_agent( char * ua_str ){
  ua_t * user_agent = parse_to_c_ua( ua_str );
  return user_agent;
}
/*str_container_t *** matches = get_regex_matches( "___ abc123def ___ ghi456 ___", "[a-z]*([0-9]+)([a-z]*)", 3, 20 );*/
