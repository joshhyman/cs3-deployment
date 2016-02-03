#ifndef SOCKETWRAPPER_HH_
#define SOCKETWRAPPER_HH_

#include <memory>
#include <cstddef>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using std::size_t;

// Wraps a socket, allowing handlers to manage object lifetime and preventing
// them from calling arbitrary socket functions
// TODO: maybe allow error handlers, silently ignoring errors is awkward
// TODO: change this to allow a mock version (mocketWrapper?)
class socketWrapper {
 private:
  std::shared_ptr<tcp::socket> sock_;
 public:
  explicit socketWrapper(tcp::socket *sock) : sock_{sock} {}

  ~socketWrapper();

  // len is the number of bytes to send; returns number of bytes sent
  // (note that it is implemented using socket::write instead of socket::send,
  // so it will send the entire buffer unless there is an error)
  size_t sync_send(const char *buf, int len);

  // len is the maximum number of bytes to read; returns number of bytes
  // actually read.
  size_t sync_recv(char *buf, int len);

  // asynchronously writes a buffer to the socket connection; calls the
  // provided handler after completion.
  // handler should be copyable. it will be called if the operation succeeded;
  // failure will be silent.
  template<typename WriteHandler>
  void async_send(const char *buf, int len, WriteHandler handler,
                  bool callHandlerOnError = true);

  // asynchronously reads to a buffer; calls the provided handler after
  // completion. handler is analogous to above.
  template<typename ReadHandler>
  void async_recv(char *buf, int len, ReadHandler handler,
                  bool callHandlerOnError = true);

  // returns true if the wrapper is actually wrapping a socket
  explicit operator bool() const { return (bool)sock_; }
};

template<typename WriteHandler>
void socketWrapper::async_send(const char *buf, int len, WriteHandler handler,
                               bool callHandlerOnError) {
  if (!sock_) {
    return;
  }
  auto l = [handler, callHandlerOnError](const boost::system::error_code &ec,
                                         size_t s) {
    if (ec) {
      std::cerr << ec.message() << std::endl;
      if (!callHandlerOnError) {
        return;
      }
    }
    handler(s);
  };
  boost::asio::async_write(*sock_, boost::asio::buffer(buf, len), l);
}

template<typename ReadHandler>
void socketWrapper::async_recv(char *buf, int len, ReadHandler handler,
                               bool callHandlerOnError) {
  if (!sock_) {
    return;
  }
  auto l = [handler, callHandlerOnError](const boost::system::error_code &ec,
                                         size_t s) {
    if (ec) {
      std::cerr << ec.message() << std::endl;
      if (!callHandlerOnError) {
        return;
      }
    }
    handler(s);
  };
  sock_->async_receive(boost::asio::buffer(buf, len), l);
}

#endif  // SOCKETWRAPPER_HH_
