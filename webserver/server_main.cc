#include <fstream>
#include <iostream>
#include <memory>
// #include <boost/asio.hpp>

//#include "handlerList.hh"
//#include "hwHandler.hh"
//#include "socketWrapper.hh"
#include "config_parser.h"
#include "server.hh"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " config_file" << std::endl;
    return -1;
  }
  NginxConfigParser config_parser;
  NginxConfig config;
  if (!config_parser.Parse(argv[1], &config)) {
    return -1;
  }
  Server s{config};
  if (!s) {
    return -1;
  }
  s.run();
  return 0;
}
