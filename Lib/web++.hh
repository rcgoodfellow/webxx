#ifndef WEBXX_HH
#define WEBXX_HH

#define BOOST_ALL_DYN_LINK

#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace webxx {
  
  static constexpr size_t data_chunk_size{1024};

  std::string rcv_msg(boost::asio::ip::tcp::socket &socket);
  std::string requestHost(const std::string & request);

  enum LogLevel { Normal, Warning, Error, Critical };
  BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogLevel);
  BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int);
  BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
  BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", boost::log::attributes::current_thread_id::value_type)

  inline
  std::ostream &
  operator<<(std::ostream & o, LogLevel ll)
  {
    static const char* const str[] = 
    {"Normal", "Warning", "Error", "Critical"};
    if(static_cast<std::size_t>(ll) < 4) { o << str[ll]; }
    else { o << "?"; };
    return o;
  }

  inline
  boost::log::formatting_ostream& 
  operator<<(boost::log::formatting_ostream& o,
    boost::log::to_log_manip<LogLevel, tag::severity> const& manip)
  {
    static const char* const str[] = 
    {"Normal", "Warning", "Error", "Critical"};
    LogLevel ll = manip.get();
    if(static_cast<std::size_t>(ll) < 4) { o << str[ll]; }
    else { o << "?"; };
    return o;
  }

  class Handler
  {
    public:
      using RouteHandler = std::function<std::string(std::string)>;

      Handler(std::string route, RouteHandler impl);

      const std::string & route() const;
      std::string operator()(std::string) const;

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
  };

}

#endif
