#include "mgServer.hpp"

void printHttpOpts(struct mg_serve_http_opts opts);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

mgServer::mgServer(char* port,
                   char* path,
                   callbackMap_t callbackMap,
                   void* userData) :
                   httpPort(port),
                   running(false),
                   callbackMap(callbackMap),
                   userData(userData),
                   httpServerThread{} {
  httpServerOptions.document_root = path;
  httpServerOptions.auth_domain = "example.com";
  httpServerOptions.ip_acl = NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

mgServer::~mgServer() {
  shutdown();
  mg_mgr_free(&eventManager);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool mgServer::launch() {
  running = true;
  cout << "launching..." << endl;
  httpServerThread = thread{&mgServer::beginServer, this};
  cout << "launched server at path " << httpServerOptions.document_root << endl;
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool mgServer::shutdown() {
  running = false;
  if (httpServerThread.joinable()) {
    httpServerThread.join();
  }
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
bool mgServer::beginServer() {
  cout << "a" << endl;
  mg_mgr_init(&eventManager, this);
  connection = mg_bind(&eventManager, httpPort, handleEvent);
  mg_set_protocol_http_websocket(connection);

  cout << "b" << endl;

  while(running) {
    mg_mgr_poll(&eventManager,1000);
  }
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleEvent(struct mg_connection* connection,
                 int event,
                 void* eventData) {
  cout << "handleEvent start" << endl;
  mgServer* server = (mgServer*) connection->mgr->user_data;
  struct http_message* message = (struct http_message*) eventData;

  server->access.lock();

  switch(event) {
  case MG_EV_HTTP_REQUEST:
    if (mg_vcmp(&message->uri, "/control") == 0) {
      char callbackKey[256];
      mg_get_http_var(&message->body, "callback", callbackKey, sizeof(callbackKey));
      try {
        callback_t callback = server->callbackMap.at(callbackKey);
        callback(message, server->userData);
      }
      catch (out_of_range err) {
        cerr << "error: could not find callback with key '" << callbackKey <<"'" << endl;
        mg_printf(connection,
                  "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nConnection: close\r\nContent-Length: 9\r\n\r\nNot Found");
      }
    }
    else {
      cout << "serving http..." << endl;
      //printHttpOpts(server->httpServerOptions);
      mg_serve_http(connection, message, server->httpServerOptions);
      cout << "done" << endl;
    }
    break;
  default:
    // do nothing
    break;
  }

  server->access.unlock();

  cout << "handleEvent exit" << endl;
}
  
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void printHttpOpts(struct mg_serve_http_opts opts) {
  cout << "Document root: " << opts.document_root << endl;
  cout << "List of index files: " << opts.index_files << endl;
  cout << "Per-directory auth files: " << opts.per_directory_auth_file << endl;
  cout << "Authorization domain: " << opts.auth_domain << endl;
  cout << "Global auth file: " << opts.global_auth_file << endl;
  cout << "Directory listing enabled: " << opts.enable_directory_listing << endl;
  cout << "SSI Patter: " << opts.ssi_pattern << endl;
  cout << "IP ACL: " << opts.ip_acl << endl;
  cout << "URL Rewrites: " << opts.url_rewrites << endl;
}
