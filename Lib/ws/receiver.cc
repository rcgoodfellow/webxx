#include "receiver.hh"

using namespace webxx;
using namespace webxx::ws;
using std::string;
using std::vector;
using std::function;
using namespace boost::asio;

Receiver::Receiver(boost::asio::ip::tcp::socket &socket)
  : mp_socket{&socket} {}

void Receiver::onMessage(Callback cb)
{
  m_callbacks.push_back(cb);
}

void Receiver::onNextMessage(Callback cb)
{
  m_next_callbacks.push_back(cb);
}

void Receiver::onNextMessageExclusive(Callback cb)
{
  m_exclusive_callbacks.push_back(cb);
}
    
bool Receiver::valid(MessageMetadata &m, const std::string &msg)
{
  return m.valid == true && msg.length() > m.data_begin; 
}

void Receiver::start()
{
  for(;;)
  {
    string msg = rcv(*mp_socket);
    auto metadata = getMetadata(msg);
    if(!valid(metadata, msg)) { return; }
    
    auto decoded = decodeMessage(msg, metadata);
    size_t decoded_size = decoded.size();
    
    if(decoded_size < metadata.payload_size)
    {
      metadata.data_begin = 0;
      while(decoded_size < metadata.payload_size)
      {
        msg = rcv(*mp_socket);
        decoded += decodeMessage(msg, metadata);
        decoded_size = decoded.size();
      }
    }

    if(m_exclusive_callbacks.size() > 0)
    {
      for(auto & cb : m_exclusive_callbacks)
      {
        cb(decoded);
      }
      m_exclusive_callbacks.clear();
      continue;
    }

    //ghetto STM
    auto cbs = m_callbacks,
         ncbs = m_next_callbacks;

    for(auto & cb : ncbs) 
    { 
      cb(decoded); 
    }
    m_next_callbacks.clear();
    
    for(auto & cb : cbs) 
    { 
      cb(decoded); 
    }
  }
}

MessageMetadata bad_metadata()
{
  return MessageMetadata(0, 0, std::array<char,4>{{0,0,0,0}}, 0, 0, false);
}

MessageMetadata Receiver::getMetadata(const std::string & msg)
{
  //check validity THIS DOES NOT WORK
  uint8_t check = (uint8_t)msg[1];
  if( (check & 0b01110000) != 0 ) 
  { 
    //return bad_metadata(); 
  }
  char b = msg[1];
  size_t msg_size = b & (char)127;
  size_t mask_begin = 2;
  std::array<char, 4> mask;
  
  bool valid = true;
  if(msg_size == 126) 
  { 
    mask_begin = 4; 
    //extract the message size
    char sz_a[2] = {msg[2], msg[3]};
    uint16_t sz = *(uint16_t*)sz_a;
    msg_size = ntohs(sz);
  }
  else if(msg_size == 127) 
  { 
    mask_begin = 10; 
    //extract the message size
    char sz_a[8] = 
    {msg[2], msg[3], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9]};

    uint64_t sz = *(uint64_t*)sz_a;
    msg_size = be64toh(sz);
  }
  else if(msg_size > 127) { valid = false; }
  mask[0] = msg[mask_begin];
  mask[1] = msg[mask_begin+1];
  mask[2] = msg[mask_begin+2];
  mask[3] = msg[mask_begin+3];

  size_t data_begin = mask_begin + 4;
  size_t payload_size = msg_size;//msg_size - data_begin;

  return MessageMetadata(msg_size, payload_size, mask, mask_begin, data_begin, valid);
}

std::string Receiver::decodeMessage(const std::string &msg)
{
  return decodeMessage(msg, getMetadata(msg));
}

std::string Receiver::decodeMessage(
    const std::string & msg, MessageMetadata metadata)
{
  size_t decoded_size = msg.length() - metadata.data_begin;
  char *decoded = new char[decoded_size];

  for(uint i=metadata.data_begin, j=0; i<msg.length(); ++i, ++j)
  {
    decoded[j] = msg[i] ^ metadata.mask[j % 4];
  }

  auto res = std::string(decoded, decoded_size);
  delete(decoded);
  return res;
}

MessageMetadata::MessageMetadata(
    size_t msg_size, size_t payload_size, std::array<char, 4> mask, 
    size_t mask_begin, size_t data_begin, bool valid)
  : msg_size{msg_size}, payload_size{payload_size}, mask(mask), 
    mask_begin{mask_begin}, data_begin{data_begin}, valid{valid}
{}
