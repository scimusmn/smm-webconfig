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

  std::unordered_map<std::string, callback_t> postCallbackMap;
  std::unordered_map<std::string, callback_t>  getCallbackMap;
  
public:
  /*! @brief The port to serve HTTP content over. */
  char* httpPort;
  struct mg_serve_http_opts httpServerOptions;

  /*! @brief A pointer to user data that is passed to callback functions. 
   *
   * This server is multithreaded, so modifying the data pointed to by userData 
   * both within a callback and in another thread is not safe, and may cause a 
   * race condition. For this reason, it is recommended that you use a mutex
   * or other guard to control access to your userData object.
   */
  void* userData;

  /*! @brief smmServer constructor.
   *
   * @var port The port to serve HTTP content over.
   * @var path The path to the root directory of the web server.
   * @var callbackMap A map of POST callbacks.
   * @var userData A pointer to user-constructed data. This is passed to all callbacks.
   */
  smmServer(std::string port,
            std::string path,
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

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  /*! @brief Add a callback to a POST request.
   *
   * Adds a callback which can be invoked by sending a POST
   * request to @c /post with the HTTP variable @c callback set to @c name.
   *
   * @param name The string key to invoke the callback later.
   * @param callback Function pointer to the callback itself.
   */
  void addPostCallback(std::string name, callback_t callback);

  /*! @brief Retrieves a POST callback.
   *
   * @param name The string key of the callback to retrieve.
   *
   * @returns The callback stored with key @c name if it exists, and @c NULL otherwise.
   */
  callback_t retrievePostCallback(std::string name);

  /*! @brief Removes a POST callback.
   *
   * @param name The string key of the callback to remove.
   */
  void removePostCallback(std::string name);

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  /*! @brief Add a callback to a GET request.
   *
   * Adds a callback which can be invoked by sending a GET
   * request to @c /get/[name].
   *
   * @param name Final URI string to invoke the callback.
   * @param callback Function pointer to the callback itself.
   */
  void addGetCallback(std::string name, callback_t callback);

  /*! @brief Retrieves a GET callback.
   *
   * @param name The string key of the callback to retrieve.
   * 
   * @returns The callback stored with key @c name if it exists, and @c NULL otherwise.
   */  
  callback_t retrieveGetCallback(std::string name);

    /*! @brief Removes a GET callback.
   *
   * @param name The string key of the callback to remove.
   */
  void removeGetCallback(std::string name);
  
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif
