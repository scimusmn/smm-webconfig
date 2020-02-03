/*!
 * @file
 * Contains the typedefs, structs, and functions needed to create
 * and launch a configuration server.
 */

#ifndef INCLUDE_MG_SERVER_H
#define INCLUDE_MG_SERVER_H

#include <stdlib.h>
#include <string.h>

#include "mg/mongoose.h"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*!
 * @defgroup CALLBACK_RETVALS Callback map function retvals
 * 
 * @{
 */

/*! Function completed successfully */
#define CALLBACK_SUCCESS 0
/*! The requested key was not found in the map. */
#define CALLBACK_NOT_FOUND_ERROR 1
/*! There was an error allocating memory needed for success */
#define CALLBACK_ALLOCATE_ERROR 2
/*! The map is full and a new callback cannot be added */
#define CALLBACK_FULL_ERROR 3
/*! @} */

/*!
 * @typedef callback_t
 * @brief A helper typedef to make working with callbacks easier.
 *
 * An example callback:
 * @code
 * void callback(struct mg_connection* connection,
 *               struct http_message* message,
 *               void* user_data) {
 *   printf("callback triggered!\n");
 * }
 * @endcode
 *
 * @param connection The Mongoose server's HTTP connection with
 *                   a client.
 * @param message The Mongoose HTTP message sent from the client 
 *                that triggered the callback.
 * @param user_data A void pointer to the user data pased when 
 *                  the server was created.
 */
typedef void (*callback_t)(struct mg_connection* connection,
                           struct http_message* message,
                           void* user_data);

/*!
 * @brief A very simple associative array for callback functions,
 * with strings as keys.
 * 
 * Note that this is not a very efficient implementation, and so
 * use with significant numbers of callbacks is discouraged.
 *
 * Usage example:
 * @code
 * #include "smm_server.h"
 * 
 * // a callback function
 * void hello(struct mg_connection* c,
 *            struct http_message* m,
 *            void* data) {
 *   printf("hello\n");
 * }
 * 
 * int main(int argc, char** argv) {
 *   // create callback map
 *   callback_map_t map;
 *   create_callback_map(&map, 8);
 *
 *   // add a callback
 *   add_callback(map, "hello", (callback_t) &hello);
 * 
 *   // retrieve a callback
 *   callback_t cb;
 *   get_callback(map, "hello", &cb);
 *   (*cb)(NULL, NULL, NULL);
 * 
 *   // attempt to retrieve nonexistent callback
 *   int result = get_callback(map, "goodbye", &cb);
 *   if (result == CALLBACK_NOT_FOUND_ERROR) {
 *     printf("could not find callback with key '%s'\n", "goodbye");
 *   }
 *   
 *   // free map when done
 *   free_callback_map(map);
 *   return 0;
 * }
 * @endcode
 */
typedef struct {
  /*! The number of callbacks the map can hold */
  unsigned int size;
  /*! The string keys for callbacks */
  char** names;
  /*! Array containing the callbacks themselves */
  callback_t* callbacks;
} callback_map_t;

/*! @brief Creates a callback map.
 * 
 * All needed memory is allocated by this function.
 *
 * @param map Pointer to the map structure to create.
 * @param size The desired number of elements the map can contain
 */
int create_callback_map(callback_map_t* map, unsigned int size);

/*! @brief Adds a new callback to an existing map.
 * 
 * @param map The map to add the callback to.
 * @param name The string key to associate with the callback.
 * @param callback A function pointer to the callback itself.
 */
int add_callback(callback_map_t map, char* name, callback_t callback);

/*! @brief Get a callback from a map.
 *
 * @param map The map to retrieve the callback from.
 * @param name The string key of the desired callback.
 * @param callback A pointer to a callback_t pointer to fill 
                   with the requested callback.
*/
int get_callback(callback_map_t map, char* name, callback_t* callback);

/*! @brief Delete a key/value pair from a callback map.
 * 
 * @param map The map to remove the pair from.
 * @param name The key to remove from the map.
 */
int remove_callback(callback_map_t map, char* name);

/*! @brief Deallocate the memory for a callback map.
 *
 * You should call this function upon exiting the program.
 * @param map The map to deallocated
 */
int free_callback_map(callback_map_t map);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! Server function completed normally */
#define SERVER_SUCCESS 0
/*! Server function encountered an error allocating memeory */
#define SERVER_ALLOCATE_ERROR 1

/*! @struct server_t
 * @brief The primary server structure.
 */
typedef struct {
  /*! HTTP port to serve content. */
  char* http_port;
  /*! Mongoose server options */
  struct mg_serve_http_opts http_server_options;
  /*! Mongoose connection */
  struct mg_connection* connection;
  /*! Mongoose event manager */
  struct mg_mgr* event_manager;
  /*! Pointer to callback map */
  callback_map_t* callback_map;
  /*! User data accessible to callbacks */
  void* user_data;
  /*! Flag indicating server status */
  unsigned int running;
} server_t;

/*! @brief Configure a server structure.
 *
 * @param server A pointer to the server structure to initialize.
 * @param port The port to serve content on.
 * @param callback_map A pointer to a callback_map_t containing 
 *                     user-defined callback functions.
 * @param user_data A void pointer to some user data structure.
 *                  This is accessible to callback functions.
 */
int setup_server(server_t* server,
                 char* port,
                 callback_map_t* callback_map,
                 void* user_data);

/*! @brief Begin the server loop.
 *
 * It is recommended to launch this function in a separate thread.
 *
 * @param server The server struct to launch.
 */
int launch_server(server_t server);

/*! @brief Free the server's resources.
 *
 * This function should be called on shutdown.
 *
 * @param server The server structure to free.
 */
int free_server(server_t server);

#endif
