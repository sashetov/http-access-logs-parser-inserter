#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
char *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches ) {
  char * cursor = str;
  char *** results;
  regex_t * regex = malloc(sizeof(regex_t));
  regmatch_t group_matches[num_groups];
  if (regcomp(regex, regex_str, REG_EXTENDED)) {
    printf("Could not compile regular expression.\n");
    exit(1);
  }
  unsigned int m = 0;
  results = (char ***) malloc( max_matches * num_groups * sizeof(char **));
  for( m =0; m < max_matches; m++ ) {
    results = (char ***) realloc( results,
        (m+1) * num_groups * sizeof(char **) );
    if(regexec( regex, cursor, num_groups, group_matches, 0) ){
      break;
    }
    //results[m] = malloc( num_groups * sizeof(char **) );
    unsigned int g = 0;
    unsigned int offset = 0;
    for (g = 0; g < num_groups; g++) {
      if (group_matches[g].rm_so == (size_t)-1) {
        break;// No more groups
      }
      if ( !g ) {
        offset = group_matches[g].rm_eo;
      }
      char str_copy[strlen(cursor) + 1];
      strcpy(str_copy, cursor);
      str_copy[group_matches[g].rm_eo] = 0;
      char * match; 
      match = malloc ( snprintf( NULL, 0, "%s",
            str_copy + group_matches[g].rm_so ) + 1 );
      sprintf( match, "%s", str_copy + group_matches[g].rm_so );
      results[m][g] = match;
    }
    cursor += offset;
  }
  return results;
}
#endif
