#ifndef __HTLOG_CONTAINERS__
#include "htlog_containers.hpp"
#endif
// KVC
KeyValueContainer::KeyValueContainer( std::string k, std::string v) {
  key = k;
  value = v;
}
std::string KeyValueContainer::getKey() const{
  return key;
}
std::string KeyValueContainer::getValue() const {
  return value;
}
int KeyValueContainer::operator <( const KeyValueContainer &rhs) const {
  if( key != rhs.getKey() ) 
    return key < rhs.getKey();
  if(value != rhs.getValue()) 
    return value < rhs.getValue();
  return 0;
}
int KeyValueContainer::operator >( const KeyValueContainer &rhs) const {
  if( key != rhs.getKey() ) 
    return key > rhs.getKey();
  if(value != rhs.getValue()) 
    return value > rhs.getValue();
  return 0;
}
int KeyValueContainer::operator ==( const KeyValueContainer &rhs) const {
  return key == rhs.getKey() && value == rhs.getValue();
}
KeyValueContainer::~KeyValueContainer() {
}
std::ostream &operator<<(std::ostream &os, KeyValueContainer const &m) { 
  return os << m.getKey()<<" : "<<m.getValue();
}
//PC
ParamsContainer::ParamsContainer( int pageType, std::string host, std::string pagePath, std::string k, std::string v) : KeyValueContainer(k,v){
  page_type = pageType;
  hostname = host;
  page_path = pagePath;
}
std::string ParamsContainer::getHost() const{
  return hostname;
}
std::string ParamsContainer::getPage() const{
  return page_path;
}
int ParamsContainer::getPageType() const{
  return page_type;
}
int ParamsContainer::operator <( const ParamsContainer &rhs) const{
  if( hostname != rhs.getHost() )
    return hostname < rhs.getHost();
  if( page_path != rhs.getPage() )
    return page_path < rhs.getPage();
  if( getKey() != rhs.getKey() )
    return getKey() < rhs.getKey();
  if( getValue() != rhs.getValue() )
    return getValue() < rhs.getValue();
  return 0;
}
int ParamsContainer::operator >( const ParamsContainer &rhs) const{
  if( hostname != rhs.getHost() )
    return hostname > rhs.getHost();
  if( page_path != rhs.getPage() )
    return page_path > rhs.getPage();
  if( getKey() != rhs.getKey() )
    return getKey() > rhs.getKey();
  if( getValue()!= rhs.getValue() )
    return getValue() > rhs.getValue();
  return 0;
}
int ParamsContainer::operator ==( const ParamsContainer &rhs) const{
  return hostname == rhs.getHost() && page_path == rhs.getPage() && getKey() == rhs.getKey() && getValue() == rhs.getValue();
}
ParamsContainer::~ParamsContainer(){
}

