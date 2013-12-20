#ifndef WEBXX_HTTP_SERVER
#define WEBXX_HTTP_SERVER

#define BOOST_ALL_DYN_LINK

#include "../log.hh"

#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace webxx { namespace http {
  
  static constexpr size_t data_chunk_size{1024};
  
  std::string rcv_msg(boost::asio::ip::tcp::socket &socket);
  std::string requestHost(const std::string & request);
  std::string extractContent(const std::string & request);
  std::string http200(const std::string & content);
  std::string http404();

  class Handler
  {
    public:
      using RouteHandler = std::function<std::string(std::string)>;
      enum class ReturnType { HTTP200, HTTP404, HTML };

      Handler(std::string route, RouteHandler impl);

      const std::string & route() const;
      std::string operator()(std::string) const;

      ReturnType return_type{ReturnType::HTTP200};

    private:
      std::string m_route;
      RouteHandler m_impl;
  };

  class Server
  {
    public:
      Server(unsigned short port, std::string root, std::string homepage);

      unsigned short port();
      std::string root();
      void start();
      static bool isGet(const std::string & request);
      static bool isPost(const std::string & request);
      void addHandler(Handler);
      std::string getStaticContent(std::string path);

    private:
      unsigned short m_port;
      std::string m_root;
      std::string m_homepage;
      boost::asio::io_service m_io_service;
      boost::asio::ip::tcp::acceptor m_acceptor;
      boost::log::sources::severity_logger<LogLevel> m_logger;
      std::vector<Handler> m_handlers;

      void handleGet(boost::asio::ip::tcp::socket &,
          const std::string & request);

      void handlePost(boost::asio::ip::tcp::socket &,
          const std::string & request);

      std::string getRoute(const std::string & request) const;
      std::string globRequestRoute(std::string route) const;
      void logRoute(std::string route);
      void logRequestor(boost::asio::ip::tcp::socket & socket);
      void handleOr(boost::asio::ip::tcp::socket & socket, 
          const std::string & request, std::string orElse);
      void serveStaticContent(boost::asio::ip::tcp::socket & socket,
          std::string path);
  };

}}//namespace webxx::http

#endif
