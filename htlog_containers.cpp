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
//HH
HourlyHitsContainer::HourlyHitsContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyHitsContainer::HourlyHitsContainer( int did, time_t ts_full ){
  hour_ts = roundTsToHour(&ts_full);
  domain_id = did;
}
time_t HourlyHitsContainer::roundTsToHour( time_t * ts_full ) {
  struct tm * timeinfo;
  timeinfo = localtime(ts_full);
  // null the minutes and seconds
  timeinfo->tm_sec = 0;
  timeinfo->tm_min = 0;
  time_t ts_hourly = mktime(timeinfo);
  return ts_hourly;
}
time_t HourlyHitsContainer::getHourlyTs() const{
  return hour_ts;
}
std::string HourlyHitsContainer::getTsHour( ){
  struct tm * tm_info;
  char buffer[26];
  tm_info = localtime(&hour_ts);
  strftime(buffer, 26, "%d-%h-%Y %H:%M:%S %Z", tm_info);
  return std::string(buffer);
}
std::string HourlyHitsContainer::getTsMysql( ){
  struct tm * tm_info;
  char buffer[26];
  tm_info = localtime(&hour_ts);
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  return std::string(buffer);
}
int HourlyHitsContainer::getDomainId() const{
  return domain_id;
}
int HourlyHitsContainer::operator <( const HourlyHitsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  return 0;
}
int HourlyHitsContainer::operator >( const HourlyHitsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  return 0;
}
int HourlyHitsContainer::operator ==( const HourlyHitsContainer & rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs();
}
HourlyHitsContainer::~HourlyHitsContainer(){
}
//HV
HourlyVisitsContainer::HourlyVisitsContainer() : HourlyHitsContainer() {
  client_ip = 0;
}
HourlyVisitsContainer::HourlyVisitsContainer(int did, time_t ts_full, unsigned long ip) : HourlyHitsContainer( did, ts_full ) {
  client_ip = ip;
}
unsigned long HourlyVisitsContainer::getIp() const{
  return client_ip;
}
int HourlyVisitsContainer::operator <(const HourlyVisitsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  if( client_ip != rhs.getIp() )
    return client_ip < rhs.getIp();
  return 0;
}
int HourlyVisitsContainer::operator >(const HourlyVisitsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  if( client_ip != rhs.getIp() )
    return client_ip > rhs.getIp();
  return 0;

}
int HourlyVisitsContainer::operator ==(const HourlyVisitsContainer & rhs) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && client_ip == rhs.getIp();
}
HourlyVisitsContainer::~HourlyVisitsContainer(){
}
//HPV
HourlyPageviewsContainer::HourlyPageviewsContainer() : HourlyVisitsContainer() {
  page_path = "";
}
HourlyPageviewsContainer::HourlyPageviewsContainer(int did, time_t ts_full, unsigned long ip, std::string full_path) : HourlyVisitsContainer( did, ts_full, ip ) {
  page_path = full_path;
}
std::string HourlyPageviewsContainer::getPagePath() const{
  return page_path;
}
int HourlyPageviewsContainer::operator <(const HourlyPageviewsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  if( client_ip != rhs.getIp() )
    return client_ip < rhs.getIp();
  if( page_path != rhs.getPagePath() )
    return page_path < rhs.getPagePath();
  return 0;
}
int HourlyPageviewsContainer::operator >(const HourlyPageviewsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  if( client_ip != rhs.getIp() )
    return client_ip > rhs.getIp();
  if( page_path != rhs.getPagePath() )
    return page_path > rhs.getPagePath();
  return 0;
}
int HourlyPageviewsContainer::operator ==(const HourlyPageviewsContainer & rhs) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && client_ip == rhs.getIp() && page_path == rhs.getPagePath();
}
HourlyPageviewsContainer::~HourlyPageviewsContainer(){
}
//HE
HourlyEntitiesContainer::HourlyEntitiesContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyEntitiesContainer::HourlyEntitiesContainer( int did, time_t ts_full ){
  hour_ts = roundTsToHour(&ts_full);
  domain_id = did;
}
int HourlyEntitiesContainer::getEntityId() const{
  return entity_id;
}
int HourlyEntitiesContainer::operator <( const HourlyEntitiesContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  return 0;
}
int HourlyEntitiesContainer::operator >( const HourlyEntitiesContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  return 0;
}
int HourlyEntitiesContainer::operator ==( const HourlyEntitiesContainer & rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs();
}
HourlyEntitiesContainer::~HourlyEntitiesContainer(){
}
//HR
HourlyReferersContainer::HourlyReferersContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyReferersContainer::HourlyReferersContainer( int did, std::string referer_hostname, std::string referer_path, time_t ts_full ){
  hour_ts = roundTsToHour(&ts_full);
  referer_domain = referer_hostname;
  referer_path_full = referer_path;
  domain_id = did;
}
std::string HourlyReferersContainer::getRefererPathFull() const {
  return referer_path_full;
}
std::string HourlyReferersContainer::getRefererDomain() const {
  return referer_domain;
}
int HourlyReferersContainer::operator <( const HourlyReferersContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  return 0;
}
int HourlyReferersContainer::operator >( const HourlyReferersContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  return 0;
}
int HourlyReferersContainer::operator ==( const HourlyReferersContainer & rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs();
}
HourlyReferersContainer::~HourlyReferersContainer(){
}
//HST
HourlySearchTermsContainer::HourlySearchTermsContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlySearchTermsContainer::HourlySearchTermsContainer( int did, std::string search_engine_hostname, std::string search_terms, std::string page_path, time_t ts_full ){
  hour_ts = roundTsToHour(&ts_full);
  domain_id = did;
  search_term = search_terms;
  search_engine_domain = search_engine_hostname;
  page_path_full = page_path;
}
std::string HourlySearchTermsContainer::getPagePathFull(){
  return page_path_full;
}
std::string HourlySearchTermsContainer::getSearchEngineDomain(){
  return search_engine_domain;
}
std::string HourlySearchTermsContainer::getSearchTerm(){
  return search_term;
}
int HourlySearchTermsContainer::operator <( const HourlySearchTermsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts < rhs.getHourlyTs();
  return 0;
}
int HourlySearchTermsContainer::operator >( const HourlySearchTermsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() )
    return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )
    return hour_ts > rhs.getHourlyTs();
  return 0;
}
int HourlySearchTermsContainer::operator ==( const HourlySearchTermsContainer & rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs();
}
HourlySearchTermsContainer::~HourlySearchTermsContainer(){
}
