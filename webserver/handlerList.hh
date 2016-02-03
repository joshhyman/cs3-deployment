#ifndef HANDLERLIST_HH_
#define HANDLERLIST_HH_

#include "handler.hh"
#include <memory>
#include <vector>
#include "socketWrapper.hh"

using std::unique_ptr;
using std::vector;

// Manages the list of handlers that the server has
// For now, the order that handlers are added is the order in which they will
// be tried, so put more specific domains before less specific ones unless you
// have a good reason (if you think you do, you probably don't)
class handlerList final {
 private:
  class errorHandler;
  class nfHandler;
  unique_ptr<Handler> firstHandler_;
  vector<unique_ptr<Handler>> handlers_;
  unique_ptr<Handler> lastHandler_;
 public:
  // Initializes the handler with just the error and 404 handlers
  handlerList();

  // Constructs handlers from config file
  handlerList(const NginxConfig &config);

  ~handlerList() = default;

  // Takes ownership of provided handler; it should be initialized beforehand.
  void addHandler(Handler *h);

  // See handler.hh for requirements.
  void handleRequest(socketWrapper &sw);
};

#endif  // HANDLERLIST_HH_
