/**
 * @file http_server.h
 * @author David A. Sowles
 * @brief An HTTP Server
 * @version Midterm
*/

#pragma once

#include "winhders.h"
#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

/// @brief The default port the server listens on if none is specified.
#define DEFAULT_PORT "8080"

/// @brief Size of the temporary buffer for reading/writing file chunks.
#define BUFFER_SIZE 4096

/// @brief The Winsock version preferred by the server app.
WORD REQUIRED_WINSOCK_VERSION = MAKEWORD(2,2);

//-----------------------------------------------------------------------------