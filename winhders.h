/**
 * @file winhderss.h
 * @author David A. Sowles
 * @brief Winsocket header include boilerplate.
 * @version Midterm
*/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <direct.h>


// For use with microsoft IDE/Tools.
#pragma comment(lib, "Ws2_32.lib")

