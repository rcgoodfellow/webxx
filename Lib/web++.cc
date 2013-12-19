#include "web++.hh"

using namespace webxx;
using namespace boost::asio;
using boost::asio::ip::tcp;
using std::string;

// Free Functions -------------------------------------------------------------
string 
webxx::rcv_msg(ip::tcp::socket &socket)
{
  char data[data_chunk_size];
  boost::system::error_code err;

  size_t read_len = socket.read_some(buffer(data, data_chunk_size), err);
  string msg = string(data, read_len);


  while(read_len == data_chunk_size)
  {
    read_len = socket.read_some(buffer(data, data_chunk_size), err);
    msg += string(data, read_len);
  }
  
  /*
  std::cout << "====MSG====" << std::endl;
  std::cout << msg;
  std::cout << "===/MSG/===" << std::endl;
  */

  return msg;
}

string 
webxx::requestHost(const string & request)
{
  size_t host_begin = request.find("Host") + 6,
         host_end = request.find("\r", host_begin);

  return request.substr(host_begin, host_end - host_begin);
}

string
webxx::extractContent(const string & request)
{
  size_t content_begin = request.find("\r\n\r\n") +4;
  size_t content_end = request.find("\r\n\r\n", content_begin);
  return request.substr(content_begin, content_end - content_begin);
}

string
webxx::http200(const string & content)
{
  return
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/xml; charset=utf-8\r\n"
      "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n"
      + content + "\r\n\r\n";
}

string
webxx::http404()
{
  return "HTTP/1.1 404 Not Found\r\n\r\n";
}

// Server ---------------------------------------------------------------------
Server::Server(unsigned short port, string root, string homepage)
  :m_port{port}, 
   m_root{root},
   m_homepage{homepage},
   m_acceptor{m_io_service, tcp::endpoint{tcp::v4(), m_port}}
{

  boost::log::add_console_log
  (
    std::cout,
    boost::log::keywords::format = //"%TimeStamp% [%ThreadID%] <%Level%> %Message%"
    (
      boost::log::expressions::stream 
        << "[" << timestamp << "] "
        << "[" << thread_id << "] "
        << "<" << severity << ">\t"
        << boost::log::expressions::smessage
    )
  );

  boost::log::add_common_attributes();
}

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

bool
Server::isPost(const string & request)
{
  return request.substr(0, 4) == "POST";
}

void
Server::addHandler(Handler h)
{
  m_handlers.push_back(h);
}

string
Server::getRoute(const string & request) const
{
  return request.substr(4, request.find("HTTP") - 4 - 1);
}
      
string 
Server::globRequestRoute(string route) const
{
  if(route == "/") { route += m_homepage; }
  return m_root + route;
}

void
Server::logRoute(string route)
{
  BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Route: " << route;
}

void 
Server::logRequestor(tcp::socket & socket)
{
  string from = socket.remote_endpoint().address().to_string();
  BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "From: " << from;
}

void
Server::handleGet(tcp::socket & socket, const string & request)
{
  BOOST_LOG_SEV(m_logger, Normal) << "GET";
  string route = getRoute(request);
  string abs_path = globRequestRoute(route);
  logRoute(route);
  logRequestor(socket);

  boost::filesystem::path p{abs_path};
  boost::system::error_code ec;

  if(boost::filesystem::exists(p, ec))
  {
    BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Serving static file: " << abs_path;
    serveStaticContent(socket, abs_path);
  }
  else
  {
    BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Executing handler";
  
    handleOr(socket, request, getStaticContent(m_root+"/404.html"));
  }
}

void
Server::handlePost(tcp::socket & socket, const string & request)
{
  BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "POST";

  handleOr(socket, request, http404());
}

void
Server::handleOr(tcp::socket & socket, const string & request, string orElse)
{
  string route = getRoute(request).erase(0,1);
  logRoute(route);
  logRequestor(socket);

  auto handle_iter = 
    std::find_if(m_handlers.begin(), m_handlers.end(),
      [&route](const Handler &h) { return route == h.route(); });

  string response_content{""};

  if(handle_iter != m_handlers.end())
  {
    BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Handler Found";
  
    string request_content = extractContent(request);
    BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Request Content: " << request_content;

    response_content = (*handle_iter)(request_content);
    BOOST_LOG_SEV(m_logger, LogLevel::Normal) << "Response Content: " << response_content;

    string response = http200(response_content);
    boost::asio::write(socket, 
        boost::asio::buffer(response.c_str(), response.length()));
  }
  else
  {
    BOOST_LOG_SEV(m_logger, LogLevel::Warning) << "No Handler Found";
    string response = orElse;
    boost::asio::write(socket,
        boost::asio::buffer(response.c_str(), response.length()));
  }
}

string
Server::getStaticContent(string path)
{
  std::ifstream is(path.c_str(), std::ios::in);
  char buf[1024];
  string fs;
  while(is.read(buf, sizeof(buf)).gcount() > 0)
  {
    fs = fs + string(buf, is.gcount());
  }
  return fs;
}
void
Server::serveStaticContent(boost::asio::ip::tcp::socket & socket, 
    string path)
{
  string fs = getStaticContent(path);
  boost::asio::write(socket,
      boost::asio::buffer(fs.c_str(), fs.length()));
}

void 
Server::start()
{
  for(;;)
  {
    tcp::socket socket{m_io_service};
    m_acceptor.accept(socket);

    string request = rcv_msg(socket);

    if(isGet(request)) { handleGet(socket, request); }
    else if(isPost(request)) { handlePost(socket, request); }
  }
}

// Handler --------------------------------------------------------------------
Handler::Handler(string route, RouteHandler impl)
  :m_route{route},
   m_impl{impl}
{}

const string &
Handler::route() const { return m_route; }

string
Handler::operator()(string request_content) const { return m_impl(request_content); }
