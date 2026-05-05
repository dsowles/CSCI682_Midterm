/**
 * @file debug_utils.h
 * @author David A. Sowles
 * @brief Debugging Helpers.
 * @version Midterm
*/

#pragma once

#include "winhders.h"
#include <stdio.h>

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

/****************************************************
 * MAKE SURE TO COMMENT THIS OUT FOR RELEASE BUILDS *
 ****************************************************/
//#define DEBUG
/************************************************** */

#ifdef DEBUG
    /// @brief Prints a formatted debug message to stderr with file and line info.
    /// @param fmt The format string.
    /// @param ... Additional arguments for the format string.
    #define DEBUG_LOG(fmt, ...) \
    fprintf(stderr, "DEBUG [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    /// @brief Prints the last Winsocket error to stderr.
    #define LAST_SOCKET_ERROR() \
    fprintf(stderr, "DEBUG [%s:%d]: Last Winsock error: %d\n", __FILE__, __LINE__, WSAGetLastError())

#else
    // Debug logging is disabled in release builds
    #define DEBUG_LOG(fmt, ...) // Does nothing in release builds

    #define LAST_SOCKET_ERROR()
#endif

//-----------------------------------------------------------------------------