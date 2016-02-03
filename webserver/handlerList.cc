#include "handlerList.hh"
#include "hwHandler.hh"
#include "sfHandler.hh"

static const char resp_431_head[] = 
  "HTTP/1.1 431 Request Header Fields Too Large\r\n"
  "Content-Type: text/html\r\n\r\n";
static const char resp_431_body[] =
  "<html><body><h1>431 Request Header Fields Too Large</h1></body></html>";

static const char resp_501_head[] =
  "HTTP/1.1 501 Not Implemented\r\n"
  "Content-Type: text/html\r\n\r\n";
static const char resp_501_body[] =
  "<html><body><h1>501 Not Implemented</h1></body></html>";

static const char resp_404_head[] = 
  "HTTP/1.1 404 Not Found\r\n"
  "Content-Type: text/html\r\n\r\n";

static const char resp_404_body[] =
  "<html><body><h1>404 Not Found</h1></body></html>";

static const int kFirstReadLength = 4096;

using std::string;

class handlerList::errorHandler final : public Handler {
 public:
  void init(const NginxConfig &config) override {
    good_ = true;
  }
  bool handleRequest(const char *buf, int len, socketWrapper &sw) override {
    if (buf[len] != '\0') {
      using namespace std;
      // uh oh
      cerr << "Buffer was not null-terminated! (memory corruption?)" << endl;
      exit(-1);
    }
    // Check that the end of the header is inside the buffer
    // I suppose this will also reject headers that contain a null byte; I
    // consider this to be a feature.
    const char *head_end = strstr(buf, "\r\n\r\n");
    if (head_end == nullptr) {
      sw.sync_send(resp_431_head, sizeof(resp_431_head));
      if (memcmp("HEAD", buf, 4)) {
        sw.sync_send(resp_431_body, sizeof(resp_431_head));
      }
      return true;
    }
    // Check the first token to see whether it's a real request type
    const char *tok_end = strchr(buf, ' ');
    // TODO: refactor, perhaps - this looks like it will be reused
    string method = string(buf, tok_end - buf);
    // To be expanded; maybe I will implement a handler that supports POST?
    if (method == "GET" || method == "HEAD") {
      return false;  // OK
    }
    sw.sync_send(resp_501_head, sizeof(resp_501_head));
    sw.sync_send(resp_501_body, sizeof(resp_501_body));
    return true;
  }
};

class handlerList::nfHandler final : public Handler {
 public:
  void init(const NginxConfig& config) override {
    good_ = true;
  }
  bool handleRequest(const char *buf, int len, socketWrapper &sw) override {
    sw.sync_send(resp_404_head, sizeof(resp_404_head));
    sw.sync_send(resp_404_body, sizeof(resp_404_body));
    return true;
  }
};

static Handler *buildHandler
(const std::shared_ptr<NginxConfigStatement> &statement) {
  using namespace std;
  Handler *tmp;
  if (statement->tokens_[0] == "handler") {
    string htype = statement->tokens_[1];
    if (htype == "hwHandler") {
      tmp = new hwHandler;
      if (statement->child_block_) {
        tmp->init(*statement->child_block_);
        if (*tmp)
          return tmp;
      }
      delete tmp;
      cerr << "Warning: could not initialize Hello World handler" << endl;
    }
    else if (htype == "sfHandler") {
      tmp = new sfHandler;
      if (statement->child_block_) {
        tmp->init(*statement->child_block_);
        if (*tmp)
          return tmp;
      }
      delete tmp;
      cerr << "Warning: could not initialize static file handler" << endl;
    }
    else {
      cerr << "Warning: unrecognized handler type" << endl;
    }
  }
  return NULL;
}

static void setupHandlers(const NginxConfig &config, handlerList *hl) {
  Handler *tmp;
  // Parse the config file, looking for handler blocks
  for (const auto &statement : config.statements_) {
    if (statement->tokens_.size() > 1) {
      tmp = buildHandler(statement);
      if (tmp)
        hl->addHandler(tmp);
    }
  }
}

handlerList::handlerList() : firstHandler_(new errorHandler),
                             lastHandler_(new nfHandler) {
  // The standard is confused about what this should do, but on the compilers
  // I've tested, indirection through a null pointer is not an error until
  // you try to do lvalue-to-rvalue conversion. See C++ Defect Report 232
  // ("Is indirection through a null pointer undefined behavior?")
  // (http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#232)
  // Since the methods called here don't even bother to bind the value to a
  // name, I think we're okay.
  firstHandler_->init(*(const NginxConfig *)nullptr);
  lastHandler_->init(*(const NginxConfig *)nullptr);
}

handlerList::handlerList(const NginxConfig &config) :
  firstHandler_(new errorHandler),
  lastHandler_(new nfHandler) {
  firstHandler_->init(*(const NginxConfig *)nullptr);
  lastHandler_->init(*(const NginxConfig *)nullptr);
  setupHandlers(config, this);
}

void handlerList::addHandler(Handler *h) {
  handlers_.push_back(unique_ptr<Handler>(h));
}

void handlerList::handleRequest(socketWrapper &sw) {
  char buf[kFirstReadLength+1];
  int len = sw.sync_recv(buf, kFirstReadLength);
  buf[len] = '\0';
  if (firstHandler_->handleRequest(buf, len, sw)) {
    return;
  }
  for (auto &h : handlers_) {
    if (h->handleRequest(buf, len, sw)) {
      return;
    }
  }
  lastHandler_->handleRequest(buf, len, sw);
}
