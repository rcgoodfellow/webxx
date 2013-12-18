#include "web++.hh"

using namespace webxx;
using namespace boost::asio;
using boost::asio::ip::tcp;
using std::string;

Server::Server(unsigned short port, string root)
  :m_port{port}, 
   m_root{root},
   m_acceptor{m_io_service, tcp::endpoint{tcp::v4(), m_port}}
{}

unsigned short
Server::port()
{ return m_port; }

string
Server::root()
{ return m_root; }

bool
Server::isGet(const string & request)
{
  return request.substr(0, 3) == "GET";
}

void
Server::handleGet(tcp::socket & socket, const string & request)
{
  size_t arg_end = request.find("HTTP");
  string arg = request.substr(4, arg_end - 4 - 1);
  string full_path = m_root + arg;
  boost::filesystem::path p{full_path};
  boost::system::error_code ec;

  if(boost::filesystem::exists(p, ec))
  {
    std::cout 
      << "SERVING STATIC FILE" 
      << std::endl;
    std::cout << full_path << std::endl << std::endl;

    std::ifstream is(full_path.c_str(), std::ios::in);
    char buf[1024];
    string fs;
    while(is.read(buf, sizeof(buf)).gcount() > 0)
    {
      fs = fs + string(buf, is.gcount());
    }

    boost::asio::write(socket,
        boost::asio::buffer(fs.c_str(), fs.length()));

    std::cout
      << "================================================================"
      << std::endl << std::endl;
  }
  else
  {
    std::cout << "EC : " << ec.value() << std::endl;
    std::cout << "ROUTING GET REQUEST" << std::endl;
    std::cout << m_root+arg << std::endl;
    
    string response{"rotten muffin :(\r\n\r\n"};
    boost::asio::write(socket, 
        boost::asio::buffer(response.c_str(), response.length()));
  }
}

void 
Server::start()
{
  for(;;)
  {
    tcp::socket socket{m_io_service};
    m_acceptor.accept(socket);

    string request = rcv_msg(socket);

    std::cout << "REQUEST:" << std::endl;
    std::cout
      << "================================================================"
      << std::endl << std::endl;
    std::cout << request << std::endl;

    if(isGet(request)) { handleGet(socket, request); }
    

    
  }
}

string 
webxx::rcv_msg(ip::tcp::socket &socket)
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
