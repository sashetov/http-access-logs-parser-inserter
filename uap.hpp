#ifndef __UAP__
#include <string>
struct Generic {
  std::string family;
};
struct Device : Generic {
  std::string model;
  std::string brand;
};
struct Agent : Generic {
  std::string major;
  std::string minor;
  std::string patch;
  std::string patch_minor;
  std::string toString() const { return family + " " + toVersionString(); }
  std::string toVersionString() const {
    return (major.empty() ? "0" : major) + "." + (minor.empty() ? "0" : minor) + "." + (patch.empty() ? "0" : patch);
  }
};
struct UserAgent {
  Device device;
  Agent os;
  Agent browser;
  std::string toFullString() const { return browser.toString() + "/" + os.toString(); }
  bool isSpider() const { return device.family == "Spider"; }
};
class UserAgentParser {
 public:
  explicit UserAgentParser(const std::string& regexes_file_path);
  UserAgent parse(const std::string&) const;
  ~UserAgentParser();
 private:
  const std::string regexes_file_path_;
  const void* ua_store_;
};
extern "C" typedef struct ua_agent {
  char * family;
  char * major;
  char * minor;
  char * patch;
  char * patch_minor;
} ua_agent_t;
extern "C" typedef struct ua_device {
  char * family;
  char * model;
  char * brand;
} ua_device_t;
extern "C" typedef struct ua {
  ua_device_t * device;
  ua_agent_t * os;
  ua_agent_t * browser;
} ua_t;
void to_cstr( std::string from, char * &to );
ua_agent_t * convert_to_cagent( Agent agent );
ua_device_t * convert_to_cdevice( Device device );
extern "C" ua_t * parse_to_c_ua( char * uastr );
#endif
