#ifndef __HTLOG_CONTAINERS__
#include <iostream>
#include <string>
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
#define __HTLOG_CONTAINERS__
#endif
