#ifndef SERVER_HH_
#define SERVER_HH_

#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "handlerList.hh"

using std::shared_ptr;
using std::unique_ptr;
using boost::asio::ip::tcp;

class Server final {
 private:
  unique_ptr<handlerList> hl_;
  shared_ptr<boost::asio::io_service> io_service_;
  unique_ptr<tcp::endpoint> endpoint_;
  unique_ptr<tcp::acceptor> acceptor_;
  unique_ptr<boost::asio::io_service::work> work_;
  vector<unique_ptr<std::thread>> threads_;
  int port_;

  // implementation details for run()
  void handle(tcp::socket *sock_ptr, const boost::system::error_code &ec);
  void accept();
 public:
  Server(const NginxConfig &config);

  // ***DETACHES*** threads
  ~Server();

  // Deletes the work and stops the io_service
  void shutdown();

  // Starts a new thread to run async tasks
  void start_thread();

  // Accepts connections and passes them off to handler list - if it returns,
  // the Server object is probably not very useful anymore
  void run(int extraThreads = 0);

  operator bool() const;
};

#endif  // SERVER_HH_
