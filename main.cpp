#include <iostream>

#include "mgServer.hpp"

using namespace std;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char** argv) {
  char* httpPort = "8000";
  char* rootPath = "./web_root";
  callbackMap_t callbackMap;

  mgServer server(httpPort, rootPath, callbackMap, NULL);
  server.launch();

  while(server.running) {}
  
  return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
