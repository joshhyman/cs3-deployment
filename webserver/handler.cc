#include "handler.hh"
#include <memory>

using std::string;

/* static */
bool Handler::FindLocation(const NginxConfig &config,
                           string *location) {
  using namespace std;
  for (const auto &statement : config.statements_) {
    if (statement->tokens_.size() > 1) {
      if (statement->tokens_[0] == "location") {
        if (*location != "")
          cerr << "Warning: multiple locations for handler given;" << endl
               << "using last one" << endl;
        *location = statement->tokens_[1];
      }
    }
  }
  if (*location == "") {
    cerr << "Error: Empty location for handler" << endl;
    return false;
  }
  return true;
}

/* static */
bool Handler::RequestHasPrefix(const string &request,
                               const string &location) {
  using namespace std;
  const size_t firstspace = request.find(' ');
  if (firstspace == string::npos) {
    cerr << "Warning: Invalid request passed to handler" << endl;
    return false;
  }

  const string method = request.substr(0, firstspace);
  const string fs = request.substr(firstspace + 1,
                                   request.find(' ', firstspace + 1) -
                                   (firstspace + 1));
  
  // Check whether the prefix of the request matches, returning false if not
  if (fs.substr(0, location.length()) != location) {
    return false;
  }

  return true;
}
