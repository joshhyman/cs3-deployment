#include "server.hh"

static int getPort(const NginxConfig &config) {
  for (const auto& statement : config.statements_) {
    bool kl = true;
    for (const std::string& token : statement->tokens_) {
      if (!kl) {
        try { return stoi(token); } catch (...) {}
      }
      kl = (token != "port");
    }
  }
  return -1;
}

Server::Server(const NginxConfig &config) : hl_{new handlerList(config)} {
  port_ = getPort(config);
  if (port_ < 1) {
    std::cerr << "Couldn't find a port" << std::endl;
    return;
  }
  io_service_.reset(new boost::asio::io_service);
  endpoint_.reset(new tcp::endpoint(tcp::v4(), port_));
  acceptor_.reset(new tcp::acceptor(*io_service_, *endpoint_));
  work_.reset(new boost::asio::io_service::work(*io_service_));
}

Server::~Server() {
  for (auto &t : threads_) {
    if (t && t->joinable()) {
      t->detach();
    }
  }
}

void Server::shutdown() {
  work_.reset(nullptr);
  io_service_->stop();
}

void Server::handle(tcp::socket *sock_ptr,
                    const boost::system::error_code &ec) {
  if (ec) {
    std::cerr << "Error accepting connection" << std::endl;
    std::cerr << ec.message() << std::endl;
    shutdown();
    return;
  } else {
    socketWrapper sw(sock_ptr);
    hl_->handleRequest(sw);
  }
  accept();
}

void Server::accept() {
  if (!(*this && work_)) {
    shutdown();
    return;
  }
  tcp::socket *sock_ptr = new tcp::socket(*io_service_);
  acceptor_->async_accept(*sock_ptr,
    [this, sock_ptr](const boost::system::error_code &ec) {
      handle(sock_ptr, ec); });
}

void Server::start_thread() {
  if (!*this) {
    shutdown();
    return;
  }
  // C++14 has a cleaner way to do this - we want to capture this->io_service_
  // by value, so we need to make a local copy and then copy it again
  shared_ptr<boost::asio::io_service> io_service(io_service_);
  threads_.push_back(unique_ptr<std::thread>(
    new std::thread([io_service]() {io_service->run();})));
}

void Server::run(int extraThreads) {
  if (!*this) {
    shutdown();
    return;
  }
  accept();
  for (int i = 0; i < extraThreads; ++i) {
    start_thread();
  }
  // TODO: maybe it's cleaner to start threads and then wait on a condition
  // variable?
  io_service_->run();
}

Server::operator bool() const {
  return (port_ > 0 && hl_ && io_service_ && endpoint_ && acceptor_);
}
