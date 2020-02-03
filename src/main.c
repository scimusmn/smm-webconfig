#include <stdio.h>
#include "smm_server.h"

void hello(struct mg_connection* c,
           struct http_message* m,
           void* data) {
  printf("hello\n");
}

int main(int argc, char** argv) {
  printf("begin\n");
  callback_map_t map;
  create_callback_map(&map, 8);
  add_callback(map, "hello", (callback_t) &hello);

  printf("initialize server\n");

  server_t server;
  setup_server(&server, "8000", &map, NULL);

  launch_server(server);

  free_server(server);
  free_callback_map(map);
  return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


