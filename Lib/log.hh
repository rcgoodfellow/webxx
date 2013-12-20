#ifndef WEBXX_LOG
#define WEBXX_LOG

#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace webxx { 
  
  enum LogLevel { Normal, Warning, Error, Critical };
  BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogLevel);
  BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int);
  BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", 
      boost::posix_time::ptime)
  BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", 
      boost::log::attributes::current_thread_id::value_type)
  BOOST_LOG_ATTRIBUTE_KEYWORD(process_id, "ProcessID",
      boost::log::attributes::current_process_id::value_type);
  BOOST_LOG_ATTRIBUTE_KEYWORD(component, "Component", std::string);
  
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

}

#endif
