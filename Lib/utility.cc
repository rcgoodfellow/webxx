#include "utility.hh"

using namespace webxx;
using namespace boost::asio;
using std::string;

string webxx::rcv(ip::tcp::socket &socket)
{
  char data[data_chunk_size];
  boost::system::error_code err;

  string msg;
  size_t read_len = socket.read_some(buffer(data, data_chunk_size), err);
  msg = string(data, read_len);

  while(read_len == data_chunk_size)
  {
    read_len = socket.read_some(buffer(data, data_chunk_size), err);
    msg += string(data, read_len);
  }

  return msg;
}
