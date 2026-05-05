
## Midterm HTTP Server Project
A lightweight, multi-client HTTP server for Windows built in C. This project was developed using the Winsock2 API and utilizes non-blocking I/O (via select) to handle concurrent client connections.
## Features

* Concurrent Connections: Manages multiple active clients using a linked-list state machine.
* Static Resource Serving: Serves files from the local filesystem with automatic MIME-type detection (e.g., HTML, CSS, JS, Images).
* Robust Error Handling: Standard HTTP 400 (Bad Request) and 404 (Not Found) response generation.
* Debug Suite: Integrated logging macros for tracking socket events and Winsock errors in real-time.

## Project Structure

* http_server.c: The application entry point and main server loop.
* network_utils.h / .c: Various network function, for things like socket creation, handling http responses, etc.
* debug_utils.h: Debugging helpers.
* winhders.h: Boilerplate for Winsocket includes.

## Configuration
Key constants are defined in http_server.h and network_utils.h:

* Default Port: 8080
* Buffer Size: 4096 bytes for file transfers.
* Max Request Size: 2047 bytes.
* Winsock Version: 2.2.

## Compilation & Setup
This project is designed for Windows and requires linking with the Winsock library (Ws2_32.lib).
## Prerequisites

* A C compiler (GCC/MinGW, MSVC, or Clang).
* Windows OS.

## Building with GCC (MinGW)

gcc http_server.c network_utils.c -o server.exe -lws2_32

## Enabling Debug Mode
To see detailed logs in your terminal during development, uncomment the following line in debug_utils.h:

#define DEBUG

## Data Structures
The server tracks state using the client_info struct:

struct client_info {
    SOCKET socket;               
    struct sockaddr_storage address; 
    char address_buffer[128];    
    char request[2048];          
    int received;                
    struct client_info *next;    
};

------------------------------
Documentation generated via Doxygen 1.17.0
------------------------------

