#ifndef MG_SERVER_HPP
#define MG_SERVER_HPP

#include <unordered_map>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include "mg/mongoose.h"

typedef void (*callback_t)(struct mg_connection*, struct http_message*, void*);
typedef std::unordered_map<std::string, callback_t> callbackMap_t;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class smmServer {
public:
  char* httpPort;
  struct mg_serve_http_opts httpServerOptions;
  struct mg_connection* connection;
  struct mg_mgr eventManager;

  std::mutex access;
  void* userData;
  callbackMap_t callbackMap;
  
  std::thread httpServerThread;
  std::atomic<bool> running;

  bool beginServer();
  
  smmServer(std::string port,
            std::string path,
            callbackMap_t callbackMap,
            void* userData);
  ~smmServer();

  bool launch();
  bool shutdown();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleEvent(struct mg_connection* connection, int event, void* event_data);


#endif
