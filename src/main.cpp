#include <iostream>

#include "smmServer.hpp"

void alpha(struct mg_connection* conn,
           struct http_message* mess,
           void* data) {
  std::cout << "alpha" << std::endl;
  mg_http_send_error(conn, 200, "alpha");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char** argv) {
  std::string httpPort = "8000";
  std::string rootPath = "./web_root";
  callbackMap_t callbackMap;
  callbackMap["alpha"] = &alpha;

  smmServer server(httpPort, rootPath, callbackMap, NULL);
  server.launch();

  while(server.running) {}
  
  return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
