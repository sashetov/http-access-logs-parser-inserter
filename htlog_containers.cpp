#include "htlog_containers.hpp"
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
  if( key != rhs.getKey() ) return key < rhs.getKey();
  if(value != rhs.getValue()) return value < rhs.getValue();
  return 0;
}
int KeyValueContainer::operator >( const KeyValueContainer &rhs) const {
  if( key != rhs.getKey() ) return key > rhs.getKey();
  if(value != rhs.getValue()) return value > rhs.getValue();
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
  if( hostname != rhs.getHost() ) return hostname < rhs.getHost();
  if( page_path != rhs.getPage() ) return page_path < rhs.getPage();
  if( page_path_full != rhs.getFullPagePath() ) return page_path_full < rhs.getFullPagePath();
  if( getKey() != rhs.getKey() ) return getKey() < rhs.getKey();
  if( getValue() != rhs.getValue() ) return getValue() < rhs.getValue();
  return 0;
}
int ParamsContainer::operator >( const ParamsContainer &rhs) const{
  if( hostname != rhs.getHost() ) return hostname > rhs.getHost();
  if( page_path != rhs.getPage() ) return page_path > rhs.getPage();
  if( page_path_full != rhs.getFullPagePath() ) return page_path_full > rhs.getFullPagePath();
  if( getKey() != rhs.getKey() ) return getKey() > rhs.getKey();
  if( getValue()!= rhs.getValue() ) return getValue() > rhs.getValue();
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
  if( isInnerVector != rhs.vectorIsInner() ) return isInnerVector < rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() ) return referer_domain < rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() ) return page_path_a < rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() ) return page_path_b < rhs.getPagePathB();
  return 0;
}
int TVectorContainer::operator >( const TVectorContainer & rhs ) const {
  if( isInnerVector != rhs.vectorIsInner() ) return isInnerVector > rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() ) return referer_domain > rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() ) return page_path_a > rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() ) return page_path_b > rhs.getPagePathB();
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
HourlyHitsContainer::HourlyHitsContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyHitsContainer::HourlyHitsContainer( unsigned long did, time_t ts_full ){
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
unsigned long HourlyHitsContainer::getDomainId() const{
  return domain_id;
}
int HourlyHitsContainer::operator <( const HourlyHitsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts < rhs.getHourlyTs();
  return 0;
}
int HourlyHitsContainer::operator >( const HourlyHitsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts > rhs.getHourlyTs();
  return 0;
}
int HourlyHitsContainer::operator ==( const HourlyHitsContainer & rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs();
}
HourlyHitsContainer::~HourlyHitsContainer(){
}
HourlyVisitsContainer::HourlyVisitsContainer() : HourlyHitsContainer() {
  client_ip = 0;
}
HourlyVisitsContainer::HourlyVisitsContainer(unsigned long did, time_t ts_full, unsigned long ip) : HourlyHitsContainer( did, ts_full ) {
  client_ip = ip;
}
unsigned long HourlyVisitsContainer::getIp() const{
  return client_ip;
}
int HourlyVisitsContainer::operator <(const HourlyVisitsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts < rhs.getHourlyTs();
  if( client_ip != rhs.getIp() ) return client_ip < rhs.getIp();
  return 0;
}
int HourlyVisitsContainer::operator >(const HourlyVisitsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts > rhs.getHourlyTs();
  if( client_ip != rhs.getIp() ) return client_ip > rhs.getIp();
  return 0;

}
int HourlyVisitsContainer::operator ==(const HourlyVisitsContainer & rhs) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && client_ip == rhs.getIp();
}
HourlyVisitsContainer::~HourlyVisitsContainer(){
}
HourlyPageviewsContainer::HourlyPageviewsContainer() : HourlyVisitsContainer() {
  page_path = "";
}
HourlyPageviewsContainer::HourlyPageviewsContainer(unsigned long did, time_t ts_full, unsigned long ip, std::string full_path) : HourlyVisitsContainer( did, ts_full, ip ) {
  page_path = full_path;
}
std::string HourlyPageviewsContainer::getPagePath() const{
  return page_path;
}
int HourlyPageviewsContainer::operator <(const HourlyPageviewsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts < rhs.getHourlyTs();
  if( client_ip != rhs.getIp() ) return client_ip < rhs.getIp();
  if( page_path != rhs.getPagePath() ) return page_path < rhs.getPagePath();
  return 0;
}
int HourlyPageviewsContainer::operator >(const HourlyPageviewsContainer & rhs) const {
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts > rhs.getHourlyTs();
  if( client_ip != rhs.getIp() ) return client_ip > rhs.getIp();
  if( page_path != rhs.getPagePath() ) return page_path > rhs.getPagePath();
  return 0;
}
int HourlyPageviewsContainer::operator ==(const HourlyPageviewsContainer & rhs) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && client_ip == rhs.getIp() && page_path == rhs.getPagePath();
}
HourlyPageviewsContainer::~HourlyPageviewsContainer(){
}
HourlyReferersContainer::HourlyReferersContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyReferersContainer::HourlyReferersContainer( unsigned long did, std::string referer_hostname, std::string referer_path, time_t ts_full, bool is_inner_hostname ){
  domain_id = did;
  hour_ts = roundTsToHour(&ts_full);
  referer_domain = referer_hostname;
  referer_path_full = referer_path;
  inner= is_inner_hostname;
}
std::string HourlyReferersContainer::getRefererPathFull() const {
  return referer_path_full;
}
std::string HourlyReferersContainer::getRefererDomain() const {
  return referer_domain;
}
bool HourlyReferersContainer::isInnerReferer() const {
  return inner;
}
int HourlyReferersContainer::operator <( const HourlyReferersContainer & rhs ) const {
  if( inner !=rhs.isInnerReferer() ) return inner < rhs.isInnerReferer();
  if( referer_path_full != rhs.getRefererPathFull()) return referer_path_full < rhs.getRefererPathFull();
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )   return hour_ts < rhs.getHourlyTs();
  if( referer_domain != rhs.getRefererDomain() ) return referer_domain < rhs.getRefererDomain();
  return 0;
}
int HourlyReferersContainer::operator >( const HourlyReferersContainer & rhs ) const {
  if( inner !=rhs.isInnerReferer() ) return inner > rhs.isInnerReferer();
  if( referer_path_full != rhs.getRefererPathFull()) return referer_path_full > rhs.getRefererPathFull();
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts > rhs.getHourlyTs();
  if( referer_domain != rhs.getRefererDomain() ) return referer_domain > rhs.getRefererDomain();
  return 0;
}
int HourlyReferersContainer::operator ==( const HourlyReferersContainer & rhs ) const {
  return inner == rhs.isInnerReferer() && referer_path_full == rhs.getRefererPathFull() && domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && referer_domain == rhs.getRefererDomain();
}
HourlyReferersContainer::~HourlyReferersContainer(){
}
HourlySearchTermsContainer::HourlySearchTermsContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlySearchTermsContainer::HourlySearchTermsContainer( unsigned long did, std::string search_engine_hostname, std::string search_terms, std::string page_path, time_t ts_full ){
  domain_id = did;
  hour_ts = roundTsToHour(&ts_full);
  search_term = search_terms;
  search_engine_domain = search_engine_hostname;
  page_path_full = page_path;
}
std::string HourlySearchTermsContainer::getPagePathFull() const{
  return page_path_full;
}
std::string HourlySearchTermsContainer::getSearchEngineDomain() const{
  return search_engine_domain;
}
std::string HourlySearchTermsContainer::getSearchTerm() const{
  return search_term;
}
int HourlySearchTermsContainer::operator <( const HourlySearchTermsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( search_engine_domain!= rhs.getSearchEngineDomain() ) return search_engine_domain< rhs.getSearchEngineDomain();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts < rhs.getHourlyTs();
  if( search_term!= rhs.getSearchTerm() ) return search_term < rhs.getSearchTerm();
  if( page_path_full!= rhs.getPagePathFull() ) return page_path_full < rhs.getPagePathFull();
  return 0;
}
int HourlySearchTermsContainer::operator >( const HourlySearchTermsContainer & rhs ) const {
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( search_engine_domain!= rhs.getSearchEngineDomain() ) return search_engine_domain> rhs.getSearchEngineDomain();
  if( hour_ts != rhs.getHourlyTs() ) return hour_ts > rhs.getHourlyTs();
  if( search_term!= rhs.getSearchTerm() ) return search_term > rhs.getSearchTerm();
  if( page_path_full!= rhs.getPagePathFull() ) return page_path_full > rhs.getPagePathFull();
  return 0;
}
int HourlySearchTermsContainer::operator ==( const HourlySearchTermsContainer & rhs ) const {
    return domain_id == rhs.getDomainId() && search_engine_domain== rhs.getSearchEngineDomain() && hour_ts == rhs.getHourlyTs() && search_term == rhs.getSearchTerm() && page_path_full == rhs.getPagePathFull();
}
HourlySearchTermsContainer::~HourlySearchTermsContainer(){
}
HourlyUserAgentEntityContainer::HourlyUserAgentEntityContainer(){
  hour_ts = 0;
  domain_id = 0;
}
HourlyUserAgentEntityContainer::HourlyUserAgentEntityContainer( unsigned long real_did, std::string uae_name, std::string uae_type, time_t ts_full){
  domain_id = real_did;
  hour_ts = roundTsToHour(&ts_full);
  name = uae_name;
  type = uae_type;
}
std::string HourlyUserAgentEntityContainer::getUserAgentEntityName() const {
  return name;
}
std::string HourlyUserAgentEntityContainer::getUserAgentEntityType() const {
  return type;
}
int HourlyUserAgentEntityContainer::operator <( const HourlyUserAgentEntityContainer& rhs ) const {
  if( domain_id != rhs.getDomainId() )      return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )        return hour_ts < rhs.getHourlyTs();
  if( name!= rhs.getUserAgentEntityName() ) return name < rhs.getUserAgentEntityName();
  if( type!= rhs.getUserAgentEntityType() ) return type < rhs.getUserAgentEntityType();
  return 0;
}
int HourlyUserAgentEntityContainer::operator >( const HourlyUserAgentEntityContainer& rhs ) const {
  if( domain_id != rhs.getDomainId() )      return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )        return hour_ts > rhs.getHourlyTs();
  if( name!= rhs.getUserAgentEntityName() ) return name > rhs.getUserAgentEntityName();
  if( type!= rhs.getUserAgentEntityType() ) return type > rhs.getUserAgentEntityType();
  return 0;
}
int HourlyUserAgentEntityContainer::operator ==( const HourlyUserAgentEntityContainer& rhs ) const {
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && name == rhs.getUserAgentEntityName() && type == rhs.getUserAgentEntityType();
}
HourlyUserAgentEntityContainer::~HourlyUserAgentEntityContainer(){
}
HourlyBandwidthContainer::HourlyBandwidthContainer(){
  domain_id= 0;
  hour_ts =0;
  size_kb =0;
}
HourlyBandwidthContainer::HourlyBandwidthContainer( unsigned long real_did, time_t timestamp, unsigned long sizeInKb, std::string path ){
  domain_id= real_did;
  hour_ts = timestamp;
  size_kb = sizeInKb;
  page_path = path;
}
unsigned long HourlyBandwidthContainer::getSizeInKb() const{
  return size_kb;
}
std::string HourlyBandwidthContainer::getPagePath() const{
  return page_path;
}
int HourlyBandwidthContainer::operator < ( const HourlyBandwidthContainer & rhs) const{
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )    return hour_ts < rhs.getHourlyTs();
  if( size_kb!= rhs.getSizeInKb() )     return size_kb< rhs.getSizeInKb();
  return 0;
}
int HourlyBandwidthContainer::operator > (const HourlyBandwidthContainer & rhs) const{
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )    return hour_ts > rhs.getHourlyTs();
  if( size_kb!= rhs.getSizeInKb() )     return size_kb> rhs.getSizeInKb();
  return 0;
}
int HourlyBandwidthContainer::operator == (const HourlyBandwidthContainer & rhs) const{
  return domain_id == rhs.getDomainId()&& hour_ts == rhs.getHourlyTs() && size_kb== rhs.getSizeInKb();
}
HourlyBandwidthContainer::~HourlyBandwidthContainer(){
}
HourlyLocationsContainer::HourlyLocationsContainer( unsigned long real_did, std::string country, time_t timestamp ){
  domain_id= real_did;
  hour_ts =timestamp;
  country_code =country;
}
std::string HourlyLocationsContainer::getCountryCode() const{
  return country_code;
}
int HourlyLocationsContainer::operator < (const HourlyLocationsContainer& rhs) const{
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )   return hour_ts < rhs.getHourlyTs();
  if( country_code!= rhs.getCountryCode() )    return country_code< rhs.getCountryCode();
  return 0;
}
int HourlyLocationsContainer::operator > ( const HourlyLocationsContainer & rhs) const{
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )    return hour_ts > rhs.getHourlyTs();
  if( country_code!= rhs.getCountryCode() )     return country_code> rhs.getCountryCode();
  return 0;
}
int HourlyLocationsContainer::operator == ( const HourlyLocationsContainer & rhs) const{
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && country_code== rhs.getCountryCode();
}
HourlyLocationsContainer::~HourlyLocationsContainer(){
}
HourlyTVContainer::HourlyTVContainer(){
  domain_id= 0;
  hour_ts =0;
}
HourlyTVContainer::HourlyTVContainer( bool is_inner_vector, std::string external_hostname, std::string full_path_a, std::string full_path_b, unsigned long real_did, time_t timestamp ) : HourlyHitsContainer(real_did, timestamp), TVectorContainer(is_inner_vector,external_hostname, full_path_a, full_path_b  ) {
}
int HourlyTVContainer::operator < ( const HourlyTVContainer& rhs) const{
  if( domain_id != rhs.getDomainId() ) return domain_id < rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )   return hour_ts < rhs.getHourlyTs();
  if( isInnerVector != rhs.vectorIsInner() ) return isInnerVector < rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() ) return referer_domain < rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() ) return page_path_a < rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() ) return page_path_b < rhs.getPagePathB();
  return 0;
}
int HourlyTVContainer::operator > ( const HourlyTVContainer & rhs) const {
  if( domain_id != rhs.getDomainId() ) return domain_id > rhs.getDomainId();
  if( hour_ts != rhs.getHourlyTs() )   return hour_ts > rhs.getHourlyTs();
  if( isInnerVector != rhs.vectorIsInner() ) return isInnerVector > rhs.vectorIsInner();
  if( referer_domain != rhs.getExternalDomain() ) return referer_domain > rhs.getExternalDomain();
  if( page_path_a != rhs.getPagePathA() ) return page_path_a > rhs.getPagePathA();
  if( page_path_b != rhs.getPagePathB() ) return page_path_b > rhs.getPagePathB();
  return 0;
}
int HourlyTVContainer::operator == ( const HourlyTVContainer & rhs) const{
  return domain_id == rhs.getDomainId() && hour_ts == rhs.getHourlyTs() && isInnerVector == rhs.vectorIsInner() && referer_domain == rhs.getExternalDomain()  && page_path_a == rhs.getPagePathA()  && page_path_b == rhs.getPagePathB();
}
HourlyTVContainer::~HourlyTVContainer(){
}
