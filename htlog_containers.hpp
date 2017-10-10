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
    ParamsContainer(int, std::string, std::string, std::string, std::string, std::string);
    //using KeyValueContainer::getKey;
    //using KeyValueContainer::getValue;
    std::string getHost() const;
    std::string getPage() const;
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
  private:
    bool isInnerVector; // internal referer to page : true, external referer to page : false
    std::string referer_domain;
    std::string page_path_a;
    std::string page_path_b;
};
class HourlyHitsContainer {
  public:
    HourlyHitsContainer();
    HourlyHitsContainer( int, time_t );
    time_t getHourlyTs() const;
    std::string getTsHour( );
    std::string getTsMysql( );
    int getDomainId() const;
    int operator <( const HourlyHitsContainer &) const;
    int operator >( const HourlyHitsContainer &) const;
    int operator ==( const HourlyHitsContainer &) const;
    ~HourlyHitsContainer();
  protected:
    time_t roundTsToHour(time_t *);
    time_t hour_ts;
    int domain_id;
};
class HourlyVisitsContainer : public HourlyHitsContainer {
  public:
    HourlyVisitsContainer();
    HourlyVisitsContainer( int, time_t, unsigned long);
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
    HourlyPageviewsContainer( int, time_t, unsigned long, std::string );
    std::string getPagePath() const;
    int operator <( const HourlyPageviewsContainer&) const;
    int operator >( const HourlyPageviewsContainer&) const;
    int operator ==( const HourlyPageviewsContainer&) const;
    ~HourlyPageviewsContainer();
  protected:
    std::string page_path;
};
#define __HTLOG_CONTAINERS__
#endif
