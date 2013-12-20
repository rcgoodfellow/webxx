#include "session.hh"

using namespace webxx;
using namespace webxx::ws;
using namespace boost::asio;
using std::string;

const string Session::handshake_key_suffix
  = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

Session::Session(std::string handshake, ip::tcp::socket && sock)
    : m_socket{std::move(sock)},
      m_receiver{m_socket}
{
  this->handshake(handshake);
  //start();
}

Session::~Session()
{
  m_receiver_thread.join();
}


unsigned short Session::port() { return m_port; }

void Session::start()
{
  m_receiver_thread = std::thread(&Receiver::start, &m_receiver);
  //m_receiver.start();
}

bool Session::isHandshake(const string & msg)
{
  return !(msg.find("Upgrade: websocket") == std::string::npos);
}

void Session::handshake(const string & msg)
{
  string hsk{msg};
  size_t kb = hsk.find("Key:"),
         ke = hsk.find("\r\n", kb);
  kb += 4;
  string key = hsk.substr(kb, ke - kb);
  boost::erase_all(key, " ");

  string combined_key = key + handshake_key_suffix;
  boost::uuids::detail::sha1 s;
  s.process_bytes(combined_key.c_str(), combined_key.size());

  unsigned char hash[20];
  unsigned int digest[5];
  s.get_digest(digest);
  for(int i = 0; i < 5; ++i) {
    const char *tmp = reinterpret_cast<char *>(digest);
    hash[i * 4] = tmp[i * 4 + 3];
    hash[i * 4 + 1] = tmp[i * 4 + 2];
    hash[i * 4 + 2] = tmp[i * 4 + 1];
    hash[i * 4 + 3] = tmp[i * 4];
  }

  std::string b64Encoded = ::base64_encode(hash, 20);


  std::string response = 
    "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
    "Upgrade: WebSocket\r\n"
    "Connection: Upgrade\r\n"
    "Sec-WebSocket-Accept: "
    + b64Encoded + "\r\n\r\n";

  boost::asio::write(m_socket, 
      boost::asio::buffer(response.c_str(), response.length()));

}

void Session::onMessage(Receiver::Callback cb)
{
  m_receiver.onMessage(cb);
}

void Session::onNextMessage(Receiver::Callback cb)
{
  m_receiver.onNextMessage(cb);
}

void Session::onNextMessageExclusive(Receiver::Callback cb)
{
  m_receiver.onNextMessageExclusive(cb);
}
