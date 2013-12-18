#include "web++.hh"
#include <cstdlib>

int main()
{
  webxx::Server server(80, "/home/ry/GridSpace/www", "index.html");
  server.start();
  return EXIT_SUCCESS;
}
