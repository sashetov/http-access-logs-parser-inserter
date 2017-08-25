#ifndef __HTLOG_URLS__
#include "htlog_urls.h"
#endif
char * get_params_str( char * path ){
  int qmatch = 0, plen = 0;
  char q = '?';
  char * params_cursor = path;
  char * params_str;
  params_cursor = strchr(params_cursor, q);
  if(strlen(path)==0){
    return "";
  }
  if(params_cursor!=NULL){
    //printf("params_cursor:%d\n",params_cursor);
    qmatch = (params_cursor+1-path);
    //printf("qmatch:%d\n",qmatch);
    plen = (strlen(path)-qmatch+1);
    params_str = (char*) malloc( plen );
    memcpy(params_str,&path[qmatch],plen-1);
    params_str[plen-1]='\0';
    //printf("params_str:%s\n",params_str);
  }
  else {
    return "";
  }
  return params_str;
}
int get_num_params( char * params_str ){
  char * cursor = params_str;
  char psep = '&';
  char kvsep = '=';
  int i=0;
  cursor = strchr(cursor, psep);
  while( cursor != NULL ) {
    i++;
    cursor = strchr(cursor+1, psep);
  }
  cursor = params_str;
  cursor = strchr(cursor, kvsep);
  if( i == 0 && cursor ){
    i = 1;
  }
  //printf("num_params: %d\n", i);
  return i;
}
url_params_t ** url_params_init( char * params_str, int num_params ){
  url_params_t ** params= (url_params_t **) malloc( sizeof(url_params_t*) * num_params);
  int i = 0;
  char t = '&', pt='=';
  char * cursor = params_str;
  int positions_nums = num_params == 0 ? 2 : num_params+1;
  int * positions = (int * ) malloc(sizeof(int) * positions_nums );
  char ** params_strs = (char **) malloc(sizeof(char *) * num_params );
  char * params_cursor;
  int position;
  int substr_len;
  cursor= strchr( cursor, t);
  //printf("params_str:%s num_params:%d position_nums:%d\n", params_str, num_params, positions_nums );
  positions[0] = 0;
  if( cursor == NULL && num_params == 1 ){
    params_cursor = params_str;
    params_cursor = strchr(params_cursor, pt);
    if( params_cursor != NULL) {
      position = params_cursor - params_str;
      positions[1] = position +1;
      substr_len = positions[1]-positions[0];
      params[0]->key = (char*) malloc(substr_len);
      memcpy( params[0]->key, &params_str[0], substr_len-1);
      params[0]->key[substr_len-1] = '\0';
      //printf("params[%d].key=%s\n",0,params[0]->key);
      params_cursor++; //skip the = char
      params[0]->value= strdup(params_cursor);
      //printf("params[%d].value=%s\n",0,params[0]->value);
    }
  }
  while( cursor != NULL ){
    i++;
    params[i-1] = (url_params_t *) malloc(sizeof(url_params_t));
    position = cursor - params_str;
    positions[i] = position + 1;
    substr_len = positions[i]-positions[i-1];
    params_strs[i-1] = (char *)malloc(substr_len);
    memcpy( params_strs[i-1], &params_str[positions[i-1]], substr_len-1 );
    params_strs[i-1][substr_len -1] = '\0';
    //printf("params_strs[%d]=%s\n",(i-1),params_strs[i-1]);
    params_cursor = params_strs[i-1];
    params_cursor = strchr(params_cursor, pt);
    if( params_cursor != NULL) {
      position = params_cursor - params_strs[i-1];
      substr_len = position + 1;
      params[i-1]->key = (char*) malloc(substr_len);
      memcpy( params[i-1]->key, &params_strs[i-1][0], substr_len-1);
      params[i-1]->key[substr_len-1] = '\0';
      //printf("params[%d].key=%s\n",i-1,params[i-1]->key);
      params_cursor++; //skip the = char
      params[i-1]->value= strdup(params_cursor);
      //printf("params[%d].value=%s\n",i-1,params[i-1]->value);
    }
    free( params_strs[i-1] );
    cursor = strchr( cursor + 1, t );
  }
  free( positions );
  free( params_strs );
  return params;
}
void url_params_free( url_params_t ** params, int len ){
  int i;
  for( i = 0; i < len; i++ ){
    if( strlen( params[i]->key ) ){
      free( params[i]->key );
    }
    if( strlen( params[i]->value ) ){
      free( params[i]->value );
    }
    free( params[i] );
  }
  free (params);
}
referer_url_t * referer_url_init( int internal, char * hostname, char * path_str, char * params_str ){
  referer_url_t * ref = (referer_url_t *) malloc(sizeof(referer_url_t));
  ref->is_internal = internal;
  ref->hostname = strdup(hostname);
  ref->path_str = strdup(path_str);
  ref->params_str = strdup(params_str);
  ref->n_params = get_num_params(ref->params_str);
  ref->params = url_params_init( ref->params_str, ref->n_params);
  return ref;
}
void  free_referer_url(referer_url_t* ref){
  if(ref){
    url_params_free(ref->params,ref->n_params);
    free(ref->hostname);
    free(ref->path_str);
    free(ref->params_str);
    free(ref);
  }
}
referer_url_t * parse_referer_str( char * referer_str, int nih, char ** internal_hostnames, int nsh, char ** search_hostnames){
  referer_url_t * ref=NULL;
  int i=0;
  char slash= '/'; // for splitting path by /
  char params_target ='?';
  char *internal_host_path= referer_str;
  char *params_str = get_params_str(referer_str);
  char *search_host_path  = referer_str;
  char *external_host_cursor = referer_str;
  char *external_host_path;
  char * hostname;
  int internal_match =0, search_match = 0, external_match = 0;
  if(strlen(referer_str)){
    for(i=0; i<nih; i++){
      if( ( internal_host_path = strstr(
              internal_host_path, internal_hostnames[i] ) ) !=NULL){
        internal_host_path += strlen(internal_hostnames[i])+1;
        internal_match = i+1; // >0 in case of first match
        hostname = strdup(internal_hostnames[i]);
        break;
      }
    }
    if( !internal_match ) {
      for(i =0; i<nsh; i++){
        if((search_host_path= strstr(search_host_path,search_hostnames[i])) !=NULL){
          search_host_path+=strlen(search_hostnames[i])+1;
          search_match = i+1; // to be > 0
          hostname = strdup(search_hostnames[i]);
          break;
        }
      }
    }
    if( !internal_match && !search_match ){
      i = 0;
      int external_host_end=0;
      // look for second '/'
      external_host_cursor = strchr( referer_str, slash);
      while(external_host_cursor !=NULL){
        i++;
        if( i == 2 ){
          external_match = external_host_cursor - referer_str + 1;
        }
        if( i == 3 ){
          external_host_end = external_host_cursor - referer_str;
          break;
        }
        external_host_cursor = strchr(external_host_cursor+1, slash);
      }
      if( external_match ){
        if(!external_host_end){
          external_host_end = strlen(referer_str);
        }
        int hlen = external_host_end - external_match + 1;
        hostname = (char *) malloc( hlen );
        memcpy( hostname, &referer_str[external_match], hlen - 1 );
        hostname[hlen-1] = '\0';
        int hplen = strlen(referer_str) - hlen + 1;
        external_host_path = (char*) malloc( hplen );
        memcpy(external_host_path, &referer_str[external_match+hlen], hplen-1);
        external_host_path[hplen-1] = '\0';
      }
    }
    if(internal_match) {
      ref = referer_url_init( URL_INTERNAL, hostname, (char*) internal_host_path, (char*) params_str);
    }
    else if(search_match) {
      ref = referer_url_init( URL_SEARCH, hostname, (char*)search_host_path, (char*)params_str);
    }
    else if(external_match) {
      ref = referer_url_init( URL_INTERNAL, hostname, (char*)external_host_path, (char*)params_str);
      free(external_host_path);
    }
    free(hostname);
  }
  return ref;
}
