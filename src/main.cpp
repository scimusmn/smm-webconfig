#include <iostream>

#include "smmServer.hpp"

using namespace std;

void alpha(struct mg_connection* conn,
           struct http_message* mess,
           void* data) {
  cout << "alpha" << endl;
  mg_http_send_error(conn, 200, "alpha");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char** argv) {
  char* httpPort = "8000";
  char* rootPath = ".\web_root";
  callbackMap_t callbackMap;
  callbackMap["alpha"] = &alpha;

  smmServer server(httpPort, rootPath, callbackMap, NULL);
  server.launch();

  while(server.running) {}
  
  return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
