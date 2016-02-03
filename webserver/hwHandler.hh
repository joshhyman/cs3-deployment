#ifndef HWHANDLER_HH_
#define HWHANDLER_HH_
#include "handler.hh"
#include <string>

class hwHandler final : public Handler {
 private:
  std::string loc;
 public:
  void init(const NginxConfig& config) override;
  bool handleRequest(const char *buf, int len, socketWrapper &sw) override;
};
#endif  // HWHANDLER_HH_
