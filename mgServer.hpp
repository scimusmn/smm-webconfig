#ifndef MG_SERVER_HPP
#define MG_SERVER_HPP

#include <unordered_map>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include "mg/mongoose.h"

using namespace std;

typedef void (*callback_t)(struct http_message*, void*);
typedef unordered_map<char*, callback_t> callbackMap_t;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class mgServer {
public:
  char* httpPort;
  struct mg_serve_http_opts httpServerOptions;
  struct mg_connection* connection;
  struct mg_mgr eventManager;

  mutex access;
  void* userData;
  callbackMap_t callbackMap;
  
  thread httpServerThread;
  atomic<bool> running;

  bool beginServer();
  
  mgServer(char* port,
           char* path,
           callbackMap_t callbackMap,
           void* userData);
  ~mgServer();

  bool launch();
  bool shutdown();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleEvent(struct mg_connection* connection, int event, void* event_data);


#endif
