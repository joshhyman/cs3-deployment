#include "socketWrapper.hh"
#include <iostream>

size_t socketWrapper::sync_send(const char *buf, int len) {
  if (!sock_) {
    return 0;
  }
  return boost::asio::write(*sock_, boost::asio::buffer(buf, len));
}

size_t socketWrapper::sync_recv(char *buf, int len) {
  if (!sock_) {
    return 0;
  }
  return sock_->receive(boost::asio::buffer(buf, len));
}

socketWrapper::~socketWrapper() {
  boost::system::error_code ec;
  sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  sock_->close();
}
