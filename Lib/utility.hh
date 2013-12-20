#ifndef WEBXX_UTILITY
#define WEBXX_UTILITY

#include <boost/asio.hpp>

namespace webxx {

  static constexpr size_t data_chunk_size{1024};

  std::string rcv(boost::asio::ip::tcp::socket &socket);

}//namespace webxx

#endif
