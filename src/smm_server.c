#include "smm_server.h"

static void event_handler(struct mg_connection* connection,
                          int event,
                          void* event_data);

void print_settings(server_t server) {
  if( server.http_server_options.document_root != NULL)            { printf("document_root: %s\n", server.http_server_options.document_root); }                       else { printf("document_root: NULL\n"); }            
  if( server.http_server_options.index_files != NULL)              { printf("index_files: %s\n", server.http_server_options.index_files); }                           else { printf("index_files: NULL\n"); }              
  if( server.http_server_options.per_directory_auth_file != NULL)  { printf("per_directory_auth_file: %s\n", server.http_server_options.per_directory_auth_file); }   else { printf("per_directory_auth_file: NULL\n"); }  
  if( server.http_server_options.auth_domain != NULL)              { printf("auth_domain: %s\n", server.http_server_options.auth_domain); }                           else { printf("auth_domain: NULL\n"); }              
  if( server.http_server_options.global_auth_file != NULL)         { printf("global_auth_file: %s\n",server.http_server_options.global_auth_file); }                  else { printf("global_auth_file: NULL\n"); }         
  if( server.http_server_options.enable_directory_listing != NULL) { printf("enable_directory_listing: %s\n",server.http_server_options.enable_directory_listing); }  else { printf("enable_directory_listing: NULL\n"); } 
  if( server.http_server_options.ssi_pattern != NULL)              { printf("ssi_pattern: %s\n", server.http_server_options.ssi_pattern); }                           else { printf("ssi_pattern: NULL\n"); }              
  if( server.http_server_options.ip_acl != NULL)                   { printf("ip_acl: %s\n",server.http_server_options.ip_acl); }                                      else { printf("ip_acl: NULL\n"); }                   
  if( server.http_server_options.url_rewrites != NULL)             { printf("url_rewrites: %s\n",server.http_server_options.url_rewrites); }                          else { printf("url_rewrites: NULL\n"); }             
  if( server.http_server_options.dav_document_root != NULL)        { printf("dav_document_root: %s\n", server.http_server_options.dav_document_root); }               else { printf("dav_document_root: NULL\n"); }        
  if( server.http_server_options.dav_auth_file != NULL)            { printf("dav_auth_file: %s\n", server.http_server_options.dav_auth_file); }                       else { printf("dav_auth_file: NULL\n"); }            
  if( server.http_server_options.hidden_file_pattern != NULL)      { printf("hidden_file_pattern: %s\n", server.http_server_options.hidden_file_pattern); }           else { printf("hidden_file_pattern: NULL\n"); }      
  if( server.http_server_options.cgi_file_pattern != NULL)         { printf("cgi_file_pattern: %s\n",server.http_server_options.cgi_file_pattern); }                  else { printf("cgi_file_pattern: NULL\n"); }         
  if( server.http_server_options.cgi_interpreter != NULL)          { printf("cgi_interpreter: %s\n", server.http_server_options.cgi_interpreter); }                   else { printf("cgi_interpreter: NULL\n"); }          
  if( server.http_server_options.custom_mime_types != NULL)        { printf("custom_mime_types: %s\n", server.http_server_options.custom_mime_types); }               else { printf("custom_mime_types: NULL\n"); }        
  if( server.http_server_options.extra_headers != NULL)            { printf("extra_headers: %s\n", server.http_server_options.extra_headers); }                       else { printf("extra_headers: NULL\n"); }            
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int setup_server(server_t* server,
                 char* port,
                 callback_map_t* callback_map,
                 void* user_data) {
  server->http_port = port;
  server->callback_map = callback_map;
  server->user_data = user_data;

  server->event_manager = calloc(1, sizeof(struct mg_mgr));
  if (server->event_manager == NULL) {
    return SERVER_ALLOCATE_ERROR;
  }

  mg_mgr_init(server->event_manager, (void*) server);
  server->connection = mg_bind(server->event_manager, server->http_port, event_handler);
  mg_set_protocol_http_websocket(server->connection);

  server->http_server_options.document_root = "../web_root";
  server->http_server_options.url_rewrites = NULL;
  server->http_server_options.index_files = "index.html,index.htm,index.shtml,index.cgi,index.php";
  server->http_server_options.per_directory_auth_file = NULL;
  server->http_server_options.global_auth_file = NULL;
  server->http_server_options.custom_mime_types = NULL;

  print_settings(*server);
  
  return SERVER_SUCCESS;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int launch_server(server_t server) {
  while(server.running != 0) {
    mg_mgr_poll(server.event_manager, 1000);
  }
  return SERVER_SUCCESS;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int free_server(server_t server) {
  mg_mgr_free(server.event_manager);
  return SERVER_SUCCESS;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void event_handler(struct mg_connection* connection,
                          int event,
                          void* event_data) {
  struct http_message* message = (struct http_message*) event_data;
  server_t* server = (server_t*) connection->mgr->user_data;

  switch(event) {
  case MG_EV_HTTP_REQUEST:
    if (mg_vcmp(&message->uri, "/control") == 0) {
      callback_t cb;
      char callback_key[256];
      mg_get_http_var(&message->body, "callback", callback_key, sizeof(callback_key));
      int result = get_callback(*(server->callback_map), callback_key, &cb);
      if (result == CALLBACK_SUCCESS) {
        cb(connection, message, server->user_data);
      }
      else if (result = CALLBACK_NOT_FOUND_ERROR) {
        fprintf(stderr, "Could not find callback with key \"%s\"", callback_key);
        mg_http_send_error(connection, 422, "Invalid callback key");
      }
      else {
        mg_http_send_error(connection, 500, "Error retrieving callback");
      }
    }
    else {
      mg_serve_http(connection, message, server->http_server_options);
    }
    break;
  default:
    // do nothing
    break;
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
