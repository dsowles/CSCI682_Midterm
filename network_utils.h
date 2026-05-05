/**
 * @file network_utils.h
 * @author David A. Sowles
 * @brief A set of network utility functions for the server.
 * @version Midterm
*/

#pragma once

#include "winhders.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

/// @brief The maximum allowable size for an incoming HTTP request.
#define MAX_REQUEST_SIZE 2047

extern WORD winsock_version;

/// @brief Stores state and metadata for an active client connection.
struct client_info {
    int address_length;                 ///< The actual size of the stored address.
    struct sockaddr_storage address;    ///< The binary IP/Port address of the client.
    char address_buffer[128];           ///< A human-readable string of the client's IP.
    SOCKET socket;                      ///< The active socket descriptor for this client.
    char request[MAX_REQUEST_SIZE + 1]; ///< Buffer containing the incoming HTTP request.
    int received;                       ///< Current number of bytes stored in the request buffer.
    struct client_info *next;           ///< Pointer to the next client in the linked list.
};

/// @brief A basic macro function for printing status messages to stdout.
#define STATUS_LOG(fmt, ...) do { \
    fprintf(stdout, fmt "\n" __VA_OPT__(,) __VA_ARGS__); \
    fflush(stdout); \
} while(0)

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------------------

/// @brief Starts the Winsock API for the app.
/// @param wVersion The version of Winsock to be requested.
/// @param lpWSADATA A structure holding the Winsock module's information.
void start_winsock(WORD wVersion, LPWSADATA lpWSADATA);

/// @brief Returns the content-type value appropriate for the path arg's extension type.
/// @param path A valid path string to some file.
/// @return The appropriate content-header value for the path's extension.
const char *get_content_type(const char *path);



/// @brief Creates and binds a socket for a server to listen on.
/// @param host The local address to bind to (use NULL or 0 for any available interface).
/// @param port The port number or service name (e.g., "8080").
/// @return A bound socket ready for listening.
SOCKET create_listening_socket(const char *host, const char *port);

/// @brief Prints information about a network socket, given a bare socket as input.
/// @param sock The socket.
void print_socket_info(const SOCKET sock);

/// @brief Searches the linked list for a client associated with a specific socket.
/// @param client_list A pointer to the head of the client linked list.
/// @param sock The socket descriptor to search for.
/// @return A pointer to the client_info struct if found; NULL otherwise.
struct client_info *get_client_info(struct client_info **client_list, SOCKET sock);



/// @brief Removes the given client from the server's client list of connections.
///        Also frees memory used by client.
/// @param client_list A pointer to the head item in the client linked-list.
/// @param client The client connection to be removed and shutdown.
void drop_client(struct client_info **client_list, struct client_info *client);



/// @brief Retrieves the human-readable IP address string for a client.
/// @param info A pointer to the client_info struct.
/// @return A constant pointer to the client's IP address string.
const char *get_client_address(struct client_info *info);



/// @brief Waits until a client's socket is ready to be handled.
/// @param client_list A pointer to the head of the connected clients list.
/// @param server The listening server socket to monitor for new connections.
/// @return An fd_set containing all sockets that are ready for an I/O operation.
fd_set wait_for_network_event(struct client_info **client_list, SOCKET server);


/// @brief Sends an HTTP 400 Bad Request response and closes the connection.
/// @param client_list A pointer to the head pointer of the client list.
/// @param client The client that sent the malformed request.
void send_400(struct client_info **client_list, struct client_info *client);



/// @brief Sends an HTTP 404 Not Found response and closes the connection.
/// @param client_list A pointer to the head pointer of the client list.
/// @param client The client that requested a non-existent resource.
void send_404(struct client_info **client_list, struct client_info *client);



/// @brief Locates a requested file on disk and transmits it to the client.
/// @param client_list A pointer to the head pointer of the client list.
/// @param client The client requesting the resource.
/// @param path The local filesystem path to the requested file.
void serve_resource(struct client_info **client_list, 
    struct client_info *client, const char *path);


//-----------------------------------------------------------------------------