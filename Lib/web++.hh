#ifndef WEBXX_HH
#define WEBXX_HH

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

namespace webxx {
  
  static constexpr size_t data_chunk_size{1024};

  std::string rcv_msg(boost::asio::ip::tcp::socket &socket);
  
  class Server
  {
    public:
      Server(unsigned short port, std::string root);

      unsigned short port();
      std::string root();

      void start();

      static bool isGet(const std::string & request);


    private:
      unsigned short m_port;
      std::string m_root;
      boost::asio::io_service m_io_service;
      boost::asio::ip::tcp::acceptor m_acceptor;

      void handleGet(boost::asio::ip::tcp::socket &,
          const std::string & request);
  };

}

#endif
