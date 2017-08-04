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
  int i=0;
  cursor = strchr(cursor, psep);
  while( cursor != NULL ) {
    i++;
    cursor = strchr(cursor+1, psep);
  }
  //printf("num_params: %d\n", i);
  return i;
}

url_params_t ** url_params_init( char * params_str, int num_params ){
  url_params_t ** params= (url_params_t **) malloc( sizeof(url_params_t*) * num_params);
  int i = 0;
  char t = '&', pt='=';
  char * cursor = params_str;
  int * positions = (int * ) malloc(sizeof(int));
  char ** params_strs = (char **) malloc(sizeof(char *));
  positions[0] = 0;
  cursor= strchr( cursor, t);
  while( cursor != NULL ){
    i++;
    params[i-1] = (url_params_t *) malloc(sizeof(url_params_t));
    int position = cursor - params_str;
    positions[i] = position + 1;
    int substr_len = positions[i]-positions[i-1];
    params_strs[i-1] = (char *)malloc(substr_len);
    memcpy( params_strs[i-1], &params_str[positions[i-1]], substr_len-1 );
    params_strs[i-1][substr_len -1] = '\0';
    //printf("params_strs[%d]=%s\n",(i-1),params_strs[i-1]);
    char * params_cursor = params_strs[i-1];
    params_cursor = strchr(params_cursor, pt);
    if( params_cursor != NULL) {
      position = params_cursor - params_strs[i-1];
      substr_len = position + 1;
      params[i-1]->key = (char*) malloc(substr_len);
      memcpy( params[i-1]->key, &params_strs[i-1][0], substr_len-1);
      params[i-1]->key[substr_len-1] = '\0';
      //printf("params[%d].key=%s\n",i-1,params[i-1]->key);
      params_cursor++; //skip the = char
      params[i-1]->value= (char *) malloc(strlen(params_cursor)+1);
      strcpy( params[i-1]->value, params_cursor );
      //printf("params[%d].value=%s\n",i-1,params[i-1]->value);
    }
    cursor = strchr(cursor + 1, t);
  }
  return params;
}

void url_params_free( url_params_t ** params, int len ){
  int i = 0;
  for( i = 0; i < len; i++ ){
    free( params[i]->key );
    free( params[i]->value );
    free( params[i] );
  }
  free (params);
}

referer_url_t * referer_url_init( int internal, char * hostname, char * path_str, char * params_str ){
  referer_url_t * ref = (referer_url_t *) malloc(sizeof(referer_url_t));
  ref->is_internal = internal;
  ref->hostname = (char *) malloc( strlen(hostname) + 1);
  strcpy( ref->hostname, hostname );
  ref->path_str = (char *) malloc( strlen(path_str) + 1);
  strcpy( ref->path_str, path_str );
  ref->params_str =(char *) malloc(strlen(params_str) + 1);
  strcpy( ref->params_str, params_str );
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
    //printf("referer_str: %s\n",referer_str);
    for(i=0; i<nih; i++){
      if( ( internal_host_path = strstr(
              internal_host_path, internal_hostnames[i] ) ) !=NULL){
        //printf("internal_host_path:%s\n",internal_host_path);
        internal_host_path += strlen(internal_hostnames[i])+1;
        //printf("internal_host_path:%s\n",internal_host_path);
        internal_match = i+1; // >0 in case of first match
        hostname = (char *) malloc( strlen( internal_hostnames[i] + 1));
        strcpy(hostname,internal_hostnames[i]);
        //printf("hostname:%s\n",hostname);
        break;
      }
    }
    if( !internal_match ) {
      for(i =0; i<nsh; i++){
        if((search_host_path= strstr(search_host_path,search_hostnames[i])) !=NULL){
          //printf("search_host_path:%s\n",search_host_path);
          search_host_path+=strlen(search_hostnames[i])+1;
          //printf("search_host_path:%s\n",search_host_path);
          search_match = i+1; // to be > 0
          hostname = (char *) malloc( strlen( search_hostnames[i] + 1));
          strcpy(hostname,search_hostnames[i]);
          //printf("hostname:%s\n",hostname);
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
        //printf("external_host_cursor:%d\n",external_host_cursor);
        i++;
        if( i == 2 ){
          external_match = external_host_cursor - referer_str + 1;
          //printf("external_match:%d\n",external_match);
        }
        if( i == 3 ){
          external_host_end = external_host_cursor - referer_str;
          //printf("external_host_end1:%d\n",external_host_end);
          break;
        }
        external_host_cursor = strchr(external_host_cursor+1, slash);
      }
      if( external_match ){
        if(!external_host_end){
          external_host_end = strlen(referer_str);
          //printf("external_host_end2:%d\n",external_host_end);
        }
        int hlen = external_host_end - external_match + 1;
        //printf("hlen:%d\n",hlen);
        hostname = (char *) malloc( hlen );
        memcpy( hostname, &referer_str[external_match], hlen - 1 );
        hostname[hlen-1] = '\0';
        //printf("hostname:%s\n",hostname);
        int hplen = strlen(referer_str) - hlen + 1;
        external_host_path = (char*) malloc( hplen );
        memcpy(external_host_path, &referer_str[external_match+hlen], hplen-1);
        external_host_path[hplen-1] = '\0';
        //printf("external_host_path:%s\n",external_host_path);
      }
    }
    //printf("internal_match:%d search_match:%d external_match:%d\n", internal_match, search_match, external_match);
    if(internal_match) {
      ref = referer_url_init( URL_INTERNAL, hostname, (char*) internal_host_path, (char*) params_str);
    }
    else if(search_match) {
      ref = referer_url_init( URL_SEARCH, hostname, (char*)search_host_path, (char*)params_str);
    }
    else if(external_match) {
      ref = referer_url_init( URL_INTERNAL, hostname, (char*)external_host_path, (char*)params_str);
    }
  }
  return ref;
}
