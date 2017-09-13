#ifndef __HTLOG_CONTAINERS__
#include <iostream>
#include <string>
class KeyValueContainer{
  public:
    KeyValueContainer( std::string , std::string );
    std::string getKey() const;
    std::string getValue() const;
    int operator <( const KeyValueContainer & ) const;
    int operator >( const KeyValueContainer & ) const;
    int operator ==( const KeyValueContainer & ) const;
    ~KeyValueContainer();
  private:
    std::string key;
    std::string value;
};
class ParamsContainer : public KeyValueContainer {
  public:
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
#define __HTLOG_CONTAINERS__
#endif
