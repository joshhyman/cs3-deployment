#include "hwHandler.hh"

static const char kHWRespHead[] =
  "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
  "Content-Length: 48\r\n\r\n";
static const char kHWRespBody[] = 
  "<html><body><h1>Hello, world!</h1></body></html>";

void hwHandler::init(const NginxConfig& config) {
  using namespace std;
  if (!FindLocation(config, &loc)) {
    return;
  }
  for (const auto &statement : config.statements_) {
    if (statement->tokens_.size() > 1) {
      if (statement->tokens_[0] != "location") {
        cerr << "Warning: unrecognized option for hello world handler: " 
             << statement->tokens_[0] << endl;
      }
    }
  }
  if (loc == "") {
    cerr << "Error: Empty location for echo handler" << endl;
    return;
  }
  good_ = true;
}

bool hwHandler::handleRequest(const char *buf, int len, socketWrapper &sw) {
  if (!RequestHasPrefix(buf, loc))
    return false;
  sw.sync_send(kHWRespHead, sizeof(kHWRespHead));
  if (memcmp("HEAD", buf, 4)) {
    sw.sync_send(kHWRespBody, sizeof(kHWRespBody));
  }
  return true;
}
