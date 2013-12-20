#ifndef WEBXX_WS_RECEIVER
#define WEBXX_WS_RECEIVER

#include "../utility.hh"
#include <boost/asio.hpp>
#include <functional>
#include <string>
#include <vector>


namespace webxx { namespace ws {

struct MessageMetadata
{
  MessageMetadata(size_t msg_size, size_t payload_size, std::array<char, 4> mask, 
      size_t mask_begin, size_t data_begin, bool valid);

  size_t msg_size;
  size_t payload_size;
  std::array<char, 4> mask;
  size_t mask_begin;
  size_t data_begin;
  bool valid;
};

class Receiver
{
  public:
    using Callback = std::function<void(std::string)>;
    Receiver(const Receiver &) = delete;
    Receiver & operator =(const Receiver &) = delete;
    Receiver(Receiver &&) = delete;
    Receiver & operator =(Receiver &&) = delete;

    Receiver(boost::asio::ip::tcp::socket &socket);
    void onMessage(Callback);
    void onNextMessage(Callback);
    void onNextMessageExclusive(Callback);
    void start();

    static constexpr size_t data_chunk_size{1024};

  private:
    std::vector<Callback> m_callbacks;
    std::vector<Callback> m_next_callbacks;
    std::vector<Callback> m_exclusive_callbacks;
    boost::asio::ip::tcp::socket *mp_socket; 

    friend class Session;

    std::string rcvMessage();
    MessageMetadata getMetadata(const std::string &msg);
    std::string decodeMessage(const std::string & msg);
    std::string decodeMessage(const std::string & msg, MessageMetadata metadata);
    bool valid(MessageMetadata &, const std::string &);
};

}}//namespace webxx::ws
#endif
