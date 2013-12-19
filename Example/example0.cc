#include "web++.hh"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::runtime_error;

struct CommandArgs { uint port; string location, homepage; };

CommandArgs getCommandArgs(int argc, char** argv)
{
  if(argc != 4) 
  {
    throw runtime_error{"Usage webxx <port> <location> <homepage>"};
  }

  CommandArgs result;
  string port_s = string{argv[1]};
  result.location = string{argv[2]},
  result.homepage = string{argv[3]};

  try { result.port = std::stoi(port_s); } 
  catch(...) {
    throw runtime_error{"[" + port_s + "] is an invalid port number"};
  }

  return result;
}

int main(int argc, char** argv)
{
  try {
    CommandArgs args = getCommandArgs(argc, argv);
    webxx::Server server(args.port, args.location, args.homepage);
    server.start();
  }
  catch(std::exception &e) { cerr << e.what() << endl; }

  return EXIT_SUCCESS;
}
