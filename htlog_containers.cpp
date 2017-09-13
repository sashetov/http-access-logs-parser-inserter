#ifndef __HTLOG_CONTAINERS__
#include "htlog_containers.hpp"
#endif
// KVC
KeyValueContainer::KeyValueContainer( ) {
  key = "";
  value = "";
}
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
std::string KeyValueContainer::toString() const {
  return key+","+value;
}
KeyValueContainer::~KeyValueContainer() {
}
std::ostream &operator<<(std::ostream &os, KeyValueContainer const &m) { 
  return os << m.getKey()<<" : "<<m.getValue();
}
//PC
ParamsContainer::ParamsContainer( ) : KeyValueContainer(){
  page_type = 0;
  hostname = "";
  page_path = "";
  page_path_full = "";
  key = "";
  value = "";
}
ParamsContainer::ParamsContainer( int pageType, std::string host, std::string pagePath, std::string pagePathFull, std::string k, std::string v) : KeyValueContainer(k,v){
  page_type = pageType;
  hostname = host;
  page_path = pagePath;
  page_path_full = pagePathFull;
  key = k;
  value = v;
}
std::string ParamsContainer::getHost() const{
  return hostname;
}
std::string ParamsContainer::getPage() const{
  return page_path;
}
std::string ParamsContainer::getFullPagePath() const{
  return page_path_full;
}
int ParamsContainer::getPageType() const{
  return page_type;
}
int ParamsContainer::operator <( const ParamsContainer &rhs) const{
  if( hostname != rhs.getHost() )
    return hostname < rhs.getHost();
  if( page_path != rhs.getPage() )
    return page_path < rhs.getPage();
  if( page_path_full != rhs.getFullPagePath() )
    return page_path_full < rhs.getFullPagePath();
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
  if( page_path_full != rhs.getFullPagePath() )
    return page_path_full > rhs.getFullPagePath();
  if( getKey() != rhs.getKey() )
    return getKey() > rhs.getKey();
  if( getValue()!= rhs.getValue() )
    return getValue() > rhs.getValue();
  return 0;
}
int ParamsContainer::operator ==( const ParamsContainer &rhs) const{
  return hostname == rhs.getHost() && page_path == rhs.getPage() && page_path_full == rhs.getFullPagePath() && getKey() == rhs.getKey() && getValue() == rhs.getValue();
}
std::string ParamsContainer::toString() const{
  return std::to_string(page_type)+","+hostname+","+page_path+","+page_path_full+","+key+","+value;
}
ParamsContainer::~ParamsContainer(){
}
//TVC
TVectorContainer::TVectorContainer(){
  isInnerVector = true;
  referer_domain = "";
  page_path_a = "";
  page_path_b = "";

}
TVectorContainer::TVectorContainer(bool inner_vector, std::string external_hostname, std::string full_path_a, std::string full_path_b ){
  isInnerVector = inner_vector;
  referer_domain = external_hostname ;
  page_path_a = full_path_a;
  page_path_b = full_path_b;
}
bool TVectorContainer::vectorIsInner() const {
  return isInnerVector;
}
std::string TVectorContainer::getExternalDomain() const {
  return referer_domain;
}
std::string TVectorContainer::getPagePathA() const {
  return page_path_a;
}
std::string TVectorContainer::getPagePathB() const{
  return page_path_b;
}
int TVectorContainer::operator <( const TVectorContainer & rhs ) const {
  if( isInnerVector != rhs.vectorIsInner() )
    return isInnerVector < rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() )
    return referer_domain < rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() )
    return page_path_a < rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() )
    return page_path_b < rhs.getPagePathB();
  return 0;
}
int TVectorContainer::operator >( const TVectorContainer & rhs ) const {
  if( isInnerVector != rhs.vectorIsInner() )
    return isInnerVector > rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() )
    return referer_domain > rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() )
    return page_path_a > rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() )
    return page_path_b > rhs.getPagePathB();
  return 0;
}
int TVectorContainer::operator ==( const TVectorContainer & rhs ) const {
  return isInnerVector == rhs.vectorIsInner() && referer_domain == rhs.getExternalDomain() && page_path_a == rhs.getPagePathA() && page_path_b == rhs.getPagePathB();
}
std::string TVectorContainer::toString() const{
  if(isInnerVector){
    return "inner("+page_path_a+" -> "+page_path_b+")";
  }
  else {
    return "incoming("+referer_domain+"/"+page_path_a+" -> "+page_path_b+")";
  }
}
TVectorContainer::~TVectorContainer(){
}
