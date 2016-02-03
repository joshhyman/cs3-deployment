#include "sfHandler.hh"

#include <string>
#include <cstdio>
#include <sys/mman.h>

void sfHandler::init(const NginxConfig &config) {
  using namespace std;
  if (!FindLocation(config, &loc)) {
    return;
  }
  for (vector<shared_ptr<NginxConfigStatement>>::const_iterator it =
	 config.statements_.begin(); it != config.statements_.end(); ++it) {
    if (((*it)->tokens_.size() > 1)) {
      if ((*it)->tokens_[0] == "document_root") {
        if (dir != "")
          cerr << "Warning: multiple roots for file handler given;" << endl
               << "using last one" << endl;
        dir = (*it)->tokens_[1];
      }
      else if ((*it)->tokens_[0] != "location") {
        cerr << "Warning: unrecognized option for file handler: " 
             << (*it)->tokens_[0] << endl;
      }
    }
  }
  if (loc == "") {
    cerr << "Error: Empty location for echo handler" << endl;
    return;
  }
  good_ = true;
}

bool sfHandler::handleRequest(const char *buf, int len, socketWrapper &sw) {
  if (!RequestHasPrefix(buf, loc)) {
    return false;
  }
  using std::string;
  using std::cerr;
  using std::endl;
  const string request(buf, len); // TODO: replace string operations with
  // C string operations
  const size_t firstspace = request.find(' ');
  if (firstspace == string::npos) {
    // Should really never happen
    cerr << "Invalid request passed to static file handler" << endl;
    return false;
  }
  const string method = request.substr(0, firstspace);
  const string fs =
    request.substr(firstspace+1,
                   request.find(' ', firstspace+1) - (firstspace + 1));
  const string path = dir + fs.substr(loc.length());
  FILE *f = fopen(path.c_str(), "rb");
  if (f == nullptr) {
    return false;
  }

  // construct and write header
  static const string sf_head = "HTTP/1.1 200 OK\r\n"
    "Content-Length: ";
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (size == LONG_MAX) {
    fclose(f);
    return false;
  }
  rewind(f);
  string resp_head = sf_head + std::to_string(size) + "\r\n\r\n";
  // synchronously write header to avoid having to manage things - besides,
  // it's small enough
  sw.sync_send(resp_head.c_str(), resp_head.size());

  // TODO: In 64-bit mode we might as well mmap the entire file - in 32-bit mode
  // we'd probably have to do something more clever like mmapping 2MB at a time
  char *mm = (char *) mmap(nullptr, size, PROT_READ, MAP_SHARED, fileno(f), 0);

  if (mm == MAP_FAILED) {
    fclose(f);
    return false;
  }
  // TODO: This is pretty awful but I can't do better with this interface -
  // if i capture sw I get a reference and that doesn't do what I want.
  socketWrapper sw_copy(sw);
  sw.async_send(mm, size, [f, mm, size, sw_copy](size_t) {
      munmap(mm, size); fclose(f); });
  return true;
}
