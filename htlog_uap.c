#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
regmatch_t * get_regex_matches( char * str, char * regex_str ) {
  regex_t * regex = malloc(sizeof(regex_t));
  regmatch_t * matches = malloc(sizeof(regmatch_t));
  char msgbuf[100];
  int reti = regcomp( regex, regex_str, 0); //compile
  if( reti ){
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }
  reti = regexec( regex, str, 0, matches, 0); //run
  if( !reti ){
    regfree(regex);
    return matches;
  }
  if( reti == REG_NOMATCH ) {
    fprintf(stderr, "no match for string %s and regex %s\n", str, regex_str );
    exit(1);
  }
  else {
    regerror( reti, regex, msgbuf, sizeof(msgbuf) );
    fprintf( stderr, "Regex match failed: %s\n", msgbuf );
    exit(1);
  }
}
#endif
