/*! @file
 * Defines the smmServer class and some auxillary classes to make working
 * with the underlying Mongoose HTTP server a bit cleaner.
 */

#ifndef MG_SERVER_HPP
#define MG_SERVER_HPP

#include <unordered_map>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include "mg/mongoose.h"

/*! @typedef
 * @brief Helper typedef to make working with callback function pointers easier.
 */
typedef void (*callback_t)(struct mg_connection*, struct http_message*, void*);

/*! @typdef
 * @brief Helper typedef to make working with callback maps easier.
 */
typedef std::unordered_map<std::string, callback_t> callbackMap_t;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! @class
 * @brief The main server class.
 */
class smmServer {
private:
  static void handleEvent(struct mg_connection* connection, int event, void* event_data);

  bool beginServer();
  
  std::thread httpServerThread;
  std::atomic<bool> running;

  struct mg_connection* connection;
  struct mg_mgr eventManager;
  
public:
  /*! @brief The port to serve HTTP content over. */
  char* httpPort;
  struct mg_serve_http_opts httpServerOptions;

  /*! @brief Mutex for thread-safe manipulation of server data.
   *
   * If you are modifying @ref userData in both callback and non-callback functions, this
   * should be locked for all changes to prevent a race condition.
   */
  std::mutex access;
  /*! @brief A pointer to user data that is passed to callback functions. */
  void* userData;
  /*! @brief An unordered map of callback functions. */
  callbackMap_t callbackMap;

  /*! @brief smmServer constructor.
   *
   * @var port The port to serve HTTP content over.
   * @var path The path to the root directory of the web server.
   * @var callbackMap A map of POST callbacks.
   * @var userData A pointer to user-constructed data. This is passed to all callbacks.
   */
  smmServer(std::string port,
            std::string path,
            callbackMap_t callbackMap,
            void* userData);

  /*! @brief smmServer destructor.
   *
   * The destructor safely shuts down the server and deallocates all resources.
   */
  ~smmServer();

  /*! @brief Start the server. */
  void launch();

  /*! @brief Stop the server */
  void shutdown();

  void sendCode(struct mg_connection* connection, int code, const char* message);

  /*! @brief Returns the current state.
   *
   * @returns @c True if the server is running; @c False otherwise.
   */
  bool isRunning();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif
