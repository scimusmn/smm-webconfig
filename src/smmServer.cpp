#include "smmServer.hpp"

void printHttpOpts(struct mg_serve_http_opts opts);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

smmServer::smmServer(std::string port,
                     std::string path,
                     callbackMap_t callbackMap,
                     void* userData) :
  running(false),
  callbackMap(callbackMap),
  userData(userData),
  httpServerThread{} {
  // set up http port
  httpPort = (char*) malloc(256*sizeof(char));
  strcpy(httpPort, port.c_str());
  
  // set server options
  httpServerOptions.document_root = path.c_str();
  httpServerOptions.index_files = NULL;              
  httpServerOptions.per_directory_auth_file = NULL;  
  httpServerOptions.auth_domain = NULL;              
  httpServerOptions.global_auth_file = NULL;         
  httpServerOptions.enable_directory_listing = NULL; 
  httpServerOptions.ssi_pattern = NULL;              
  httpServerOptions.ip_acl = NULL;                   
  httpServerOptions.url_rewrites = NULL;             
  httpServerOptions.dav_document_root = NULL;        
  httpServerOptions.dav_auth_file = NULL;            
  httpServerOptions.hidden_file_pattern = NULL;      
  httpServerOptions.cgi_file_pattern = NULL;         
  httpServerOptions.cgi_interpreter = NULL;            
  httpServerOptions.custom_mime_types = NULL;        
  httpServerOptions.extra_headers = NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

smmServer::~smmServer() {
  shutdown();
  free(httpPort);
  mg_mgr_free(&eventManager);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool smmServer::launch() {
  running = true;
  httpServerThread = std::thread{&smmServer::beginServer, this};
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool smmServer::shutdown() {
  running = false;
  if (httpServerThread.joinable()) {
    httpServerThread.join();
  }
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
bool smmServer::beginServer() {
  mg_mgr_init(&eventManager, this);
  connection = mg_bind(&eventManager, httpPort, handleEvent);
  if (connection == NULL) {
    std::cerr << "FATAL: mg_bind() failed!\n";
    running = false;
    return false;
  }
  
  mg_set_protocol_http_websocket(connection);

  while(running) {
    mg_mgr_poll(&eventManager,1000);
  }
  return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handleEvent(struct mg_connection* connection,
                 int event,
                 void* eventData) {
  smmServer* server = (smmServer*) connection->mgr->user_data;
  struct http_message* message = (struct http_message*) eventData;

  server->access.lock();

  switch(event) {
  case MG_EV_HTTP_REQUEST:
    if (mg_vcmp(&message->uri, "/control") == 0) {
      char callbackKey[256];
      mg_get_http_var(&message->body, "callback", callbackKey, sizeof(callbackKey));
      try {
        callback_t callback = server->callbackMap.at(callbackKey);
        callback(connection, message, server->userData);
      }
      catch (std::out_of_range err) {
        std::cerr << "error: could not find callback with key '" << callbackKey <<"'" << std::endl;
        mg_http_send_error(connection, 422, "Invalid callback key");
      }
    }
    else {
      mg_serve_http(connection, message, server->httpServerOptions);
    }
    break;
  default:
    // do nothing
    break;
  }

  server->access.unlock();
}
  
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
