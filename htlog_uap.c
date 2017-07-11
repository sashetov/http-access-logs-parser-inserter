#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
#endif
str_container_t *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches ) {
  char * cursor = str;
  str_container_t *** results = init_2d_str_cont(max_matches,num_groups,100);
  regex_t * regex = malloc(sizeof(regex_t));
  regmatch_t group_matches[num_groups];
  if (regcomp(regex, regex_str, REG_EXTENDED)) {
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
    for (g = 0; g < num_groups; g++) {
      if (group_matches[g].rm_so == (size_t)-1) {
        break;
      }
      if ( !g ) {
        offset = group_matches[g].rm_eo;
      }
      char str_copy[strlen(cursor) + 1];
      strcpy( str_copy, cursor );
      str_copy[group_matches[g].rm_eo] = 0;
      int cur_m = snprintf(
          NULL, 0, "%s", str_copy + group_matches[g].rm_so ) + 1;
      results[m][g]->str = malloc( cur_m );
      sprintf( results[m][g]->str , "%s", str_copy + group_matches[g].rm_so );
    }
    cursor += offset;
  }
  return results;
}
str_container_t *** init_2d_str_cont(int x, int y, int max_str_size){
  int i =0, j = 0,k =0;
  int container_size = (sizeof(str_container_t *));
  str_container_t *** matrix = (str_container_t ***)malloc(x*y*container_size);
  for( i=0; i<x; i++ ) {
    matrix[i] = (str_container_t **)malloc(y*container_size);
    for(j=0; j<y; j++) {
      matrix[i][j] = (str_container_t *)malloc(container_size);
      matrix[i][j]->str = malloc( (max_str_size) * sizeof(char) );
    }
  }
  return matrix;
}
