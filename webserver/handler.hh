#ifndef HANDLER_HH_
#define HANDLER_HH_
// Handler API
#include "socketWrapper.hh"
#include "config_parser.h"

class Handler {
 protected:
  bool good_ = false;
  static bool FindLocation(const NginxConfig &config, std::string *location);
  static bool RequestHasPrefix(const std::string &request,
                               const std::string &location);
 public:
  // Initializes the handler according to the config; must set good_ to
  // true if the initialization succeeded.
  virtual void init(const NginxConfig& config) = 0;

  // Requirements for calling:
  // buf contains the results of the first read, which should contain at least
  // the entire http header (i.e. up to the first "\r\n\r\n"). It must be
  // null-terminated, but may contain internal null characters.
  // Requirements for implementation:
  // If handleRequest returns false, it must not use sw.
  // handleRequest may return true before the request is handled; if it does
  // so, it must manage sw's lifetime [by std::move or std::swapping it away],
  // e.g. by std::binding it to an asynchronous handler, because sw will go out
  // of scope on the calling side. buf will become invalid after return -
  // copy its contents if you still need them.
  virtual bool handleRequest(const char *buf, int len, socketWrapper &sw) = 0;

  explicit operator bool() const { return good_; }

  virtual ~Handler() = default;
};

#endif  // HANDLER_HH_
