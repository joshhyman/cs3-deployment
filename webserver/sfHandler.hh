#ifndef SFHANDLER_HH_
#define SFHANDLER_HH_
#include "handler.hh"
#include <string>

// semi-asynchronous file handler
class sfHandler final : public Handler {
 private:
  std::string loc, dir;
 public:
  void init(const NginxConfig& config) override;
  bool handleRequest(const char *buf, int len, socketWrapper &sw) override;
};
#endif  // SFHANDLER_HH_
