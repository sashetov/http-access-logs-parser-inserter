#ifndef __HTLOG_CONTAINERS__
#include <iostream>
#include <string>
#include <time.h>

class KeyValueContainer{
  public:
    KeyValueContainer( );
    KeyValueContainer( std::string , std::string );
    std::string getKey() const;
    std::string getValue() const;
    int operator <( const KeyValueContainer & ) const;
    int operator >( const KeyValueContainer & ) const;
    int operator ==( const KeyValueContainer & ) const;
    std::string toString() const;
    ~KeyValueContainer();
  private:
    std::string key;
    std::string value;
};
std::ostream &operator<<(std::ostream &, KeyValueContainer const &);
class ParamsContainer : public KeyValueContainer {
  public:
    ParamsContainer( );
    ParamsContainer(int, std::string, std::string, std::string, std::string);
    std::string getHost() const;
    std::string getFullPagePath() const;
    int getPageType() const;
    int operator <( const ParamsContainer & ) const;
    int operator >( const ParamsContainer & ) const;
    int operator ==( const ParamsContainer & ) const;
    std::string toString() const;
    ~ParamsContainer();
  private:
    int page_type;
    std::string hostname;
    std::string page_path;
    std::string page_path_full;
    std::string key;
    std::string value;
};
class TVectorContainer {
  public:
    TVectorContainer();
    TVectorContainer(bool, std::string, std::string, std::string);
    bool vectorIsInner() const;
    std::string getExternalDomain() const;
    std::string getPagePathA() const;
    std::string getPagePathB() const;
    int operator <( const TVectorContainer & ) const;
    int operator >( const TVectorContainer & ) const;
    int operator ==( const TVectorContainer & ) const;
    std::string toString() const;
    ~TVectorContainer();
  protected:
    bool isInnerVector; // internal referer to page : true, external referer to page : false
    std::string referer_domain;
    std::string page_path_a;
    std::string page_path_b;
};
class HourlyHitsContainer {
  public:
    HourlyHitsContainer();
    HourlyHitsContainer( unsigned long, time_t );
    time_t getHourlyTs() const;
    std::string getTsHour( );
    std::string getTsMysql( );
    unsigned long getDomainId() const;
    int operator <( const HourlyHitsContainer &) const;
    int operator >( const HourlyHitsContainer &) const;
    int operator ==( const HourlyHitsContainer &) const;
    ~HourlyHitsContainer();
  protected:
    time_t roundTsToHour(time_t *);
    time_t hour_ts;
    unsigned long domain_id;
};
class HourlyVisitsContainer : public HourlyHitsContainer {
  public:
    HourlyVisitsContainer();
    HourlyVisitsContainer( unsigned long, time_t, unsigned long);
    unsigned long getIp() const;
    int operator <( const HourlyVisitsContainer &) const;
    int operator >( const HourlyVisitsContainer &) const;
    int operator ==( const HourlyVisitsContainer &) const;
    ~HourlyVisitsContainer();
  protected:
    unsigned long client_ip;
};
class HourlyPageviewsContainer : public HourlyVisitsContainer {
  public:
    HourlyPageviewsContainer();
    HourlyPageviewsContainer( unsigned long, time_t, unsigned long, std::string );
    std::string getPagePath() const;
    int operator <( const HourlyPageviewsContainer&) const;
    int operator >( const HourlyPageviewsContainer&) const;
    int operator ==( const HourlyPageviewsContainer&) const;
    ~HourlyPageviewsContainer();
  protected:
    std::string page_path;
};
class HourlyReferersContainer : public HourlyHitsContainer {
  public:
    HourlyReferersContainer();
    HourlyReferersContainer( unsigned long, std::string, std::string, time_t, bool );
    std::string getRefererPathFull() const;
    std::string getRefererDomain() const;
    bool isInnerReferer() const;
    int operator <( const HourlyReferersContainer &) const;
    int operator >( const HourlyReferersContainer &) const;
    int operator ==( const HourlyReferersContainer &) const;
    ~HourlyReferersContainer();
  protected:
    bool inner;
    std::string referer_domain;
    std::string referer_path_full;
};
class HourlySearchTermsContainer  : public HourlyHitsContainer {
  public:
    HourlySearchTermsContainer();
    HourlySearchTermsContainer( unsigned long, std::string, std::string, std::string, time_t);
    std::string getSearchTerm() const;
    std::string getSearchEngineDomain() const;
    std::string getPagePathFull() const;
    int operator <( const HourlySearchTermsContainer &) const;
    int operator >( const HourlySearchTermsContainer &) const;
    int operator ==( const HourlySearchTermsContainer &) const;
    ~HourlySearchTermsContainer();
  protected:
    std::string search_engine_domain;
    std::string search_term;
    std::string page_path_full;
};
class HourlyUserAgentEntityContainer: public HourlyHitsContainer {
  public:
    HourlyUserAgentEntityContainer();
    HourlyUserAgentEntityContainer( unsigned long, std::string, std::string, time_t );
    std::string getUserAgentEntityName() const;
    std::string getUserAgentEntityType() const;
    int operator <(  const HourlyUserAgentEntityContainer&) const;
    int operator >(  const HourlyUserAgentEntityContainer&) const;
    int operator ==( const HourlyUserAgentEntityContainer&) const;
    ~HourlyUserAgentEntityContainer();
  protected:
    std::string name;
    std::string type;
};
class HourlyBandwidthContainer : public HourlyHitsContainer {
  public:
    HourlyBandwidthContainer();
    HourlyBandwidthContainer( unsigned long, time_t, unsigned long, std::string );
    unsigned long getSizeInKb() const;
    std::string getPagePath() const;
    int operator < ( const HourlyBandwidthContainer & ) const;
    int operator >( const HourlyBandwidthContainer &) const;
    int operator ==( const HourlyBandwidthContainer &) const;
    ~HourlyBandwidthContainer();
  protected:
    unsigned long size_kb;
    std::string page_path;
};
class HourlyTVContainer : public HourlyHitsContainer, public TVectorContainer {
  public:
    HourlyTVContainer();
    HourlyTVContainer( bool, std::string, std::string, std::string, unsigned long, time_t );
    int operator <( const HourlyTVContainer&) const;
    int operator >( const HourlyTVContainer&) const;
    int operator ==( const HourlyTVContainer&) const;
    ~HourlyTVContainer();
  protected:
};
class HourlyLocationsContainer : public HourlyHitsContainer {
  public:
    HourlyLocationsContainer();
    HourlyLocationsContainer( unsigned long, std::string, time_t );
    std::string getCountryCode() const;
    int operator <( const HourlyLocationsContainer&) const;
    int operator >( const HourlyLocationsContainer&) const;
    int operator ==( const HourlyLocationsContainer&) const;
    ~HourlyLocationsContainer();
  protected:
    std::string country_code;
};
#define __HTLOG_CONTAINERS__
#endif
