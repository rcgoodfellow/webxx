#include "web++.hh"
#include <cstdlib>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

int main(int argc, char** argv)
{
  if(argc != 4)
  {
    cerr << "Usage webxx <port> <location> <homepage>" << endl;
    return EXIT_FAILURE;
  }
  string port_s = string{argv[1]},
         location = string{argv[2]},
         homepage = string{argv[3]};
  unsigned short port;
  try {
    port = std::stoi(port_s);
  } catch(...) {
    cerr << "[" << port_s << "] is an invalid port number" << endl;
    return EXIT_FAILURE;
  }

  webxx::Server server(port, location, homepage);
  server.start();
  return EXIT_SUCCESS;
}
