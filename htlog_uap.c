#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
#endif
str_container_t *** get_regex_matches( char * str, char * regex_str, size_t num_groups, size_t max_matches ) {
  int i=0, j=0, k=0;
  char * cursor = str;
  int container_size = (sizeof(str_container_t));
  regex_t * regex = (regex_t *)malloc(sizeof(regex_t));
  str_container_t *** results =
    (str_container_t ***) malloc(num_groups * max_matches * container_size);
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
  free(regex);
  return results;
}
char * get_device_version_string( char * model, char * brand){
  char * vt1= "%s";
  char * vt2= "%s %s";
  char * version_str;
  if( strcmp( model, "" ) == 0 ){
    version_str = strdup((char *) "none");
  }
  else if( strcmp( brand, "" ) == 0 ){
    version_str = (char *) malloc( snprintf( NULL, 0 , vt1, model) + 1);
    sprintf( version_str, vt1, model );
  }
  else {
    version_str = (char *) malloc( snprintf( NULL, 0 , vt2, model, brand) + 1);
    sprintf( version_str, vt2, model, brand );
  }
  return version_str;
}
char * get_version_string( char * major, char * minor, char * patch, char * patch_minor ){
  char * vt1= "%s";
  char * vt2= "%s.%s";
  char * vt3= "%s.%s.%s";
  char * vt4= "%s.%s.%s.%s";
  char * version_str;
  if( strcmp( major, "" ) == 0 ){
    version_str = strdup((char *) "none");
  }
  else if( strcmp( minor, "" ) == 0 ){
    version_str = (char *) malloc( snprintf( NULL, 0 , vt1, major ) + 1);
    sprintf( version_str, vt1, major );
  }
  else if( strcmp( patch, "" ) == 0 ){
    version_str = (char *) malloc( snprintf( NULL, 0 , vt2, major, minor ) + 1);
    sprintf( version_str, vt2, major, minor );
  }
  else if( strcmp( patch_minor, "" ) == 0 ){
    version_str = (char *) malloc( snprintf( NULL, 0 , vt3, major, minor, patch ) + 1);
    sprintf( version_str, vt3, major, minor, patch );
  }
  else {
    version_str = (char *) malloc( snprintf( NULL, 0 , vt4, major, minor, patch, patch_minor ) + 1);
    sprintf( version_str, vt4, major, minor, patch, patch_minor );
  }
  return version_str;
}
ua_agent_t * init_ua_agent( char * family, char * major, char * minor, char * patch, char * patch_minor ){
  ua_agent_t * res = (ua_agent_t *) malloc( sizeof(ua_agent_t ) );
  //res->family = strdup(family);
  memcpy(res->family, family, strlen(family));
  res->family[strlen(family)] = '\0';
  //res->major = strdup(major);
  memcpy(res->major, major, strlen(major));
  res->major[strlen(major)] = '\0';
  //res->minor = strdup(minor);
  memcpy(res->minor, minor, strlen(minor));
  res->minor[strlen(minor)] = '\0';
  //res->patch = strdup(patch);
  memcpy(res->patch, patch, strlen(patch));
  res->patch[strlen(patch)] = '\0';
  //res->patch_minor = strdup(patch_minor);
  memcpy(res->patch_minor, patch_minor, strlen(patch_minor));
  res->patch_minor[strlen(patch_minor)] = '\0';
}
ua_device_t * init_ua_device( char * family, char * model, char * brand ){
  ua_device_t * res = (ua_agent_t *) malloc( sizeof(ua_device_t) );
  //res->family = strdup(family);
  memcpy(res->family, family, strlen(family) );
  res->family[strlen(family)] = '\0';
  //res->model = strdup(model);
  memcpy(res->model, model, strlen(model) );
  res->model[strlen(model)] = '\0';
  //res->brand = strdup(brand);
  memcpy(res->brand, brand, strlen(brand) );
  res->brand[strlen(brand)] = '\0';

}
void free_c_ua( ua_t * user_agent ) {
  //free(user_agent->device->family);
  //free(user_agent->device->model);
  //free(user_agent->device->brand);
  free(user_agent->device);
  //free(user_agent->os->family);
  //free(user_agent->os->major);
  //free(user_agent->os->minor);
  //free(user_agent->os->patch);
  //free(user_agent->os->patch_minor);
  free(user_agent->os);
  //free(user_agent->browser->family);
  //free(user_agent->browser->major);
  //free(user_agent->browser->minor);
  //free(user_agent->browser->patch);
  //free(user_agent->browser->patch_minor);
  free(user_agent->browser);
}
/*str_container_t *** matches = get_regex_matches( "___ abc123def ___ ghi456 ___", "[a-z]*([0-9]+)([a-z]*)", 3, 20 );*/
