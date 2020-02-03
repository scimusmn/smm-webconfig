#include <stdio.h>
#include "smm_server.h"

void hello(struct mg_connection* c,
           struct http_message* m,
           void* data) {
  printf("hello\n");
}

int main(int argc, char** argv) {
  callback_map_t map;
  create_callback_map(&map, 8);
  add_callback(map, "hello", (callback_t) &hello);

  callback_t cb;
  get_callback(map, "hello", &cb);
  (*cb)(NULL, NULL, NULL);

  int result = get_callback(map, "goodbye", &cb);
  if (result == CALLBACK_NOT_FOUND_ERROR) {
    printf("could not find callback with key '%s'\n", "goodbye");
  }
  
  free_callback_map(map);
  return 0;
}
