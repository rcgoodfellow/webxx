#ifndef WEBXX_WS_SESSION
#define WEBXX_WS_SESSION

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/sha1.hpp>
#include "base64.hpp"
#include "receiver.hh"
#include <thread>


namespace webxx { namespace ws {

class Session
{
  public:
    Session(std::string handshake, boost::asio::ip::tcp::socket &&);
    Session(const Session &) = delete;
    Session & operator = (const Session &) = delete;
    ~Session();

    void start();
    unsigned short port();
    static bool isHandshake(const std::string &);

    static const std::string handshake_key_suffix;

    void onMessage(Receiver::Callback);
    void onNextMessage(Receiver::Callback);
    void onNextMessageExclusive(Receiver::Callback);

  private:
    boost::asio::ip::tcp::socket m_socket;
    Receiver m_receiver;
    std::thread m_receiver_thread;
    unsigned short m_port{4747};

    void handshake(const std::string &msg);

};

}}//namespace webxx::ws

#endif
