#include "smm_server.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int create_callback_map(callback_map_t* map, unsigned int size) {
  map->size = size;
  map->names = calloc(size, sizeof(char*));
  if (map->names == NULL) {
    return CALLBACK_ALLOCATE_ERROR;
  }
  map->callbacks = calloc(size, sizeof(callback_t));
  if (map->callbacks == NULL) {
    return CALLBACK_ALLOCATE_ERROR;
  }

  return CALLBACK_SUCCESS;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int add_callback(callback_map_t map, char* name, callback_t callback) {
  for (int i=0; i<map.size; i++) {
    if (map.names[i] == NULL && map.callbacks[i] == NULL) {
      map.names[i] = name;
      map.callbacks[i] = callback;
      return CALLBACK_SUCCESS;
    }
  }

  return CALLBACK_FULL_ERROR;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int get_callback(callback_map_t map, char* name, callback_t* callback) {
  for (int i=0; i<map.size; i++) {
    if ( map.names[i] != NULL &&
         strcmp(name, map.names[i]) == 0 ) {
      *callback = map.callbacks[i];
      return CALLBACK_SUCCESS;
    }
  }
  return CALLBACK_NOT_FOUND_ERROR;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int remove_callback(callback_map_t map, char* name) {
  for (int i=0; i<map.size; i++) {
    if ( map.names[i] != NULL &&
         strcmp(name, map.names[i]) == 0 ) {
      map.names[i] = NULL;
      map.callbacks[i] = NULL;
      return CALLBACK_SUCCESS;
    }
  }
  return CALLBACK_NOT_FOUND_ERROR;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int free_callback_map(callback_map_t map) {
  free(map.names);
  free(map.callbacks);
}  

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
