#include "network_utils.h"
/******************************************************************************
 * Project: Midterm
 * File: network_utils.c
 * Author: David A. Sowles
 * Description: A set of network utility functions for the server and client.
 ******************************************************************************/

#include <stdlib.h>
#include "network_utils.h"
#include "debug_utils.h"

void start_winsock(WORD wVersion, LPWSADATA lpWSADATA) {
    DEBUG_LOG("Starting WinSock.");

    int i_err = WSAStartup(wVersion, lpWSADATA);
    if (i_err) {
        DEBUG_LOG("Winsock startup failed.");
        STATUS_LOG("WSAStartup failed with exit code: %d", i_err);
        STATUS_LOG("Exiting...");
        exit(1);
    }

    DEBUG_LOG("WinSock successfully started.");
}

const char *get_content_type(const char *path)
{
    DEBUG_LOG("Getting path's content type.");

    const char *last_dot = strrchr(path, '.');
    if (last_dot) {
        DEBUG_LOG("Found last dot, matching extension.");
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return "application/javascript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
    }

    DEBUG_LOG("Failed to find extension, defaulting to raw bytes.");
    return "application/octet-stream";
}

SOCKET create_listening_socket(const char *host, const char *port)
{
    DEBUG_LOG("Creating socket to listen on.");
    STATUS_LOG("Constructing server socket.");


    //-------------------------------------------------------------------------
    // Retrieve Address Info About The Client
    //-------------------------------------------------------------------------
    
    int i_err = 0;

    // A pointer to a linked-list of address info about the host.
    struct addrinfo *result = NULL;

    // Info about the type of socket the caller wants.
    struct addrinfo hints;

    // Ensure that hints is clean memory before use.
    ZeroMemory(&hints, sizeof(hints));
    // Setup for IPv4 and TCP.
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;    // This value is for use with bound().

    // Retrieve address information about the host before socket creation.
    DEBUG_LOG("Getting address info on host.");
    i_err = getaddrinfo(host, port, &hints, &result);
    if(i_err) {
        STATUS_LOG("Address information for host could not be retrieved.");
        STATUS_LOG("Connection failed. [%d]", i_err);
        LAST_SOCKET_ERROR();
        DEBUG_LOG("Returning with invalid socket descriptor.");
        return INVALID_SOCKET;
    }
    DEBUG_LOG("Host address resolution successful.");
    
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Construct The Listening Socket
    //-------------------------------------------------------------------------

    DEBUG_LOG("Constructing the network socket.");

    // Construct the socket for the server.
    STATUS_LOG("Setting up socket for server.");
    SOCKET listen_sock = socket(result->ai_family, result->ai_socktype,
        result->ai_protocol);
    if(listen_sock == INVALID_SOCKET) {
        DEBUG_LOG("Bad socket creation.");
        STATUS_LOG("Connection failed, socket could not be setup.");
        freeaddrinfo(result);
        LAST_SOCKET_ERROR();
        DEBUG_LOG("Returning with invalid socket descriptor.");
        return listen_sock;
    }
    
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Bind The Socket to Local Address
    //-------------------------------------------------------------------------
    DEBUG_LOG("Binding the socket.");
    STATUS_LOG("Binding socket to local address...");
    if (bind(listen_sock, result->ai_addr, result->ai_addrlen)) {
        DEBUG_LOG("Could not bind socket to local address.");
        STATUS_LOG("Binding socket failed.");
        freeaddrinfo(result);
        closesocket(listen_sock);
        listen_sock = INVALID_SOCKET;
        LAST_SOCKET_ERROR();
        DEBUG_LOG("Returning with invalid socket descriptor.");
        return listen_sock;
    }
    DEBUG_LOG("Socket bind successful.");

    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Setup Socket for listening.
    //-------------------------------------------------------------------------

    if(listen(listen_sock, 10)) {
        DEBUG_LOG("Socket listen failed.");
        LAST_SOCKET_ERROR();
        DEBUG_LOG("Returning with invalid socket descriptor.");
        freeaddrinfo(result);
        closesocket(listen_sock);
        listen_sock = INVALID_SOCKET;
        return listen_sock;
    }
    DEBUG_LOG("Server's listening socket successfully constructed.");

    //-------------------------------------------------------------------------
    // Print connection information to console.
    //-------------------------------------------------------------------------

    char nameBuf[128];
    char servBuf[128];

    getnameinfo(result->ai_addr, result->ai_addrlen, nameBuf, sizeof(nameBuf),
        servBuf, sizeof(servBuf), (NI_NUMERICHOST | NI_NUMERICSERV));

    STATUS_LOG("Listening to %s, on port: %s", nameBuf, servBuf);


    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Return the successfully constructed socket.
    //-------------------------------------------------------------------------
    
    // Ensure result is released before leaving.
    freeaddrinfo(result);

    DEBUG_LOG("Returning good socket.");
    return listen_sock;

    //-------------------------------------------------------------------------
}

void print_socket_info(const SOCKET sock) {

    STATUS_LOG("Socket Info:");

    if(sock == INVALID_SOCKET) {
        STATUS_LOG("Cannot print socket information, socket descriptor invalid.");
        return;
    }

    // local host name string buffer.
    char local_host[128 + 1];
    int local_host_len = sizeof(local_host);
    // Name of service used by local host.
    char local_service[128 + 1];
    int local_service_len = sizeof(local_service);

    // Name of service used by remote host.
    char remote_host[128 + 1];
    int remote_host_len = sizeof(remote_host);

    // Name of service used by local host.
    char remote_service[128 + 1];
    int remote_service_len = sizeof(local_service);

    SOCKADDR_STORAGE addrinfo;
    int addrlen = sizeof(addrinfo);
    if(getsockname(sock, (struct sockaddr*)&addrinfo, &addrlen)){
        DEBUG_LOG("Socket name retrieval failed.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Could not print socket information for local host.");
        return;
    }

    if(getnameinfo((struct sockaddr*)&addrinfo, addrlen, local_host,
        local_host_len, local_service, local_service_len, NI_NUMERICHOST)){

        DEBUG_LOG("Could not resolve local host and service information from socket name.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Could not print socket information for local host.");
        return;
    }
    STATUS_LOG("Local Host: %s, Service: %s", local_host, local_service);

    if(getpeername(sock, (struct sockaddr*)&addrinfo, &addrlen)) {
        DEBUG_LOG("Could not resolve remote host and service information from socket name.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Could not print socket information for remote host.");
        return;
    }

    if(getnameinfo((struct sockaddr*)&addrinfo, addrlen, remote_host, remote_host_len,
         remote_service, remote_service_len, NI_NUMERICHOST)){

        DEBUG_LOG("Could not resolve remote host and service information from socket name.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Could not print socket information for remote host.");
        return;
    }
    STATUS_LOG("Remote Host: %s, Service: %s", remote_host, remote_service);

    return;
}

struct client_info *get_client_info(struct client_info **client_info_list, SOCKET sock)
{
    DEBUG_LOG("Getting client info.");

    if(client_info_list == NULL) {
        DEBUG_LOG("Bad pointer to client_info list.");
        exit(1);
    }

    struct client_info *client_info_item = *client_info_list;

    DEBUG_LOG("Searching client info list for item with the needed socket.");
    while(client_info_item){
        if(client_info_item->socket == sock) break;
        client_info_item = client_info_item->next;
    }

    if (client_info_item) {
        DEBUG_LOG("Client info found.");
        DEBUG_LOG("Returning client info item.");
        return client_info_item;
    }
    DEBUG_LOG("Client info not found, preparing to build new info item.");
    
    struct client_info *n =
        (struct client_info*) calloc(1, sizeof(struct client_info));
    if(!n) {
        DEBUG_LOG("Memory for client info could not be allocated.");
        printf("Critical error, closing server.");
        exit(1);
    }
    
    DEBUG_LOG("Adding new empty item to client info list.");
    n->socket = INVALID_SOCKET;
    n->address_length = sizeof(n->address);
    n->next = *client_info_list;
    *client_info_list = n;
    DEBUG_LOG("Returning new client info item.");
    return n;
}


void drop_client(struct client_info **client_info_list, struct client_info *client_info)
{
    DEBUG_LOG("Preparing to drop client.");

    if(client_info_list == NULL || *client_info_list == NULL) {
        DEBUG_LOG("client_info_list is invalid.");
        STATUS_LOG("Critical Error, could not drop client.");
        exit(1);
    }

    if(client_info == NULL) {
        DEBUG_LOG("client_info is invalid.");
        STATUS_LOG("Critical Error, could not drop client.");
        exit(1);
    }

    if(client_info->socket != INVALID_SOCKET) {
        if(closesocket(client_info->socket)){
            DEBUG_LOG("Socket could not be closed.");
            LAST_SOCKET_ERROR();
            STATUS_LOG("Error, could not close socket.");
        }
        client_info->socket = INVALID_SOCKET;
    }
    else {
        DEBUG_LOG("Client socket closure skipped, already invalid.");
    }


    struct client_info **list_item = client_info_list;

    DEBUG_LOG("Searching for client to drop.");
    while(*list_item) {
        if (*list_item == client_info) {
            DEBUG_LOG("Client found, dropping from list.");
            STATUS_LOG("Dropping client.");
            *list_item = client_info->next;

            DEBUG_LOG("Freeing client's resources.");
            free(client_info);
            client_info = NULL;
            return;
        }
        list_item = &(*list_item)->next;
    }

    STATUS_LOG("Critical Error, client to drop could not be found.");
    exit(1);
}

const char *get_client_address(struct client_info *info)
{
    DEBUG_LOG("Getting address name from client info.");

    if(info == NULL) {
        DEBUG_LOG("Client address could not be retrieved.");
        return NULL;
    }

    if(getnameinfo((struct sockaddr*)&info->address,
            info->address_length,
            info->address_buffer, sizeof(info->address_buffer), 0, 0,
            NI_NUMERICHOST)) {

        DEBUG_LOG("Could not resolve name from socket infomration.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Client address could not be retrieved.");
        return NULL;
    }

    DEBUG_LOG("Returning client address.");
    return info->address_buffer;
}

fd_set wait_for_network_event(struct client_info **client_info_list, SOCKET listen_sock)
{
    DEBUG_LOG("Waiting for network event.");
    STATUS_LOG("Waiting on clients...");

    if(client_info_list == NULL) {
        DEBUG_LOG("client_info_list is a bad pointer.");
        STATUS_LOG("Critical error, client info list could not be pointed to.");
        exit(1);
    }

    if(listen_sock == INVALID_SOCKET) {
        DEBUG_LOG("Socket arg is INVALID_SOCKET.");
        STATUS_LOG("Critical Error, bad socket descriptor given.");
        exit(1);
    }

    // Register the server's listen socket to the read list.
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(listen_sock, &reads);

    // REgister the client(s)'s sockets to the read list.
    struct client_info *current = *client_info_list;
    while(current) {
        if (current->socket != INVALID_SOCKET) {
            FD_SET(current->socket, &reads);
        }
        current = current->next;
    }

    if (select(0, &reads, 0,0,0) < 0) {
        DEBUG_LOG("select() failed.");
        STATUS_LOG("Critical error, wait for client failed.");
        exit(1);
    }

    return reads;
}

void send_400(struct client_info **client_info_list, struct client_info *client_info)
{
    DEBUG_LOG("Sending http 400 response.");

    if(client_info_list == NULL || client_info == NULL) {
        DEBUG_LOG("Could not send 400 response, bad args to function.");
        return;
    }

    const char *rsp_str = "HTTP/1.1 400 Bad Request\r\n"
        "Connection: close\r\n"
        "Content-Length: 17\r\n\r\n400 - Bad Request";
    
    int bytesSent;
    if( (bytesSent = send(client_info->socket, rsp_str, (int)strlen(rsp_str), 0)) < 0) {
        DEBUG_LOG("send() failed.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Http Response send failed.");
        return;
    }
    STATUS_LOG("Bytes sent to client: %d", bytesSent);
    drop_client(client_info_list, client_info);

    return;
}

void send_404(struct client_info **client_info_list, struct client_info *client_info)
{
    DEBUG_LOG("Sending http 404 response.");

    if(client_info_list == NULL || client_info == NULL) {
        DEBUG_LOG("Could not send 404 response, bad args to function.");
        return;
    }

    const char *rsp_str = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 15\r\n\r\n404 - Not Found";
    
    int bytesSent;
    if( (bytesSent = send(client_info->socket, rsp_str, (int)strlen(rsp_str), 0)) < 0) {
        DEBUG_LOG("send() failed.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Http Response send failed.");
        return;
    }
    STATUS_LOG("Bytes sent to client: %d", bytesSent);
    drop_client(client_info_list, client_info);

    return;
}

void serve_resource(struct client_info **client_info_list, struct client_info *client_info, const char *path)
{
    DEBUG_LOG("Preparing to send resource to client.");
    STATUS_LOG("Serving Resource to client.");

    // If bad path, return immediately.
    if(path == NULL) {
        DEBUG_LOG("path is invalid pointer.");
        STATUS_LOG("Bad path arg, could not send resource.");
        return;
    }

    // Prevent client from requesting files outside of public folder.
    if (strstr(path, "..") || strstr(path, "\\")) {
        DEBUG_LOG("Path contains forbidden characters.");
        DEBUG_LOG("Bad path given to serve_resource: %.100s", path);
        send_404(client_info_list, client_info);
        return;
    }

    
    // If root directory only is specified, send default file.
    if (strcmp(path, "/") == 0) path = "/index.html";

    // Reject excessively long URLs to prevent buffer overflow.
    if (strlen(path) > 100) {
        DEBUG_LOG("Bad path given to serve_resource.");
        send_400(client_info_list, client_info);
        return;
    }

    DEBUG_LOG("Raw path: %s", path);

    // Setup the full path from which to retrieve the requested file.
    char full_path[128];
    sprintf(full_path, "public%s", path);

    char *p = full_path;
    while (*p) {
        if (*p == '/') *p = '\\';
        ++p;
    }
    DEBUG_LOG("Translated path: %s", full_path);

    DEBUG_LOG("Attempting to open requested file.");
    // Retrieve the file.
    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        DEBUG_LOG("File could not be opened at the given path.");
        send_404(client_info_list, client_info);
        return;
    }

    // Obtain the file's size.
    fseek(fp, 0L, SEEK_END);
    size_t file_len = ftell(fp);
    rewind(fp);

    // Get the file type.
    const char *file_type = get_content_type(full_path);

    // Prepare the response message.
#define BSIZE 1024
    
    char buffer[BSIZE];

    // Lewis Van Winkle sends headers one by one, instead we
    // will send headers all at once.
    /*
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client_info->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client_info->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %zu\r\n", file_len);
    send(client_info->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", file_type);
    send(client_info->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client_info->socket, buffer, strlen(buffer), 0);
    */

    DEBUG_LOG("Constructing the response header.");
    // Send the response header.
    snprintf(buffer, BSIZE,
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: %s\r\n"
        "\r\n", 
        file_len, file_type);

    #ifdef DEBUG
        STATUS_LOG("\n\n");
        STATUS_LOG("HTTP Response Header:");
        STATUS_LOG("%s\n\n", buffer);
    #endif

    int n_bytes_sent = 0;
    if( (n_bytes_sent = send(client_info->socket, buffer, strlen(buffer), 0)) < 0){
        DEBUG_LOG("send() error.");
        LAST_SOCKET_ERROR();
        STATUS_LOG("Message send failed.");
        fclose(fp);
        drop_client(client_info_list, client_info);
        return;
    }
    STATUS_LOG("%d bytes sent to outgoing buffer.", n_bytes_sent);

    // Send the file.
    size_t r = fread(buffer, 1, BSIZE, fp);

    // Continue to send until all file contents are transmitted.
    while (r) {
        // Number of bytes for individual send.
        n_bytes_sent = 0;
        // Total number of bytes sent.
        int total_sent = 0;
        // Chunk count.
        int i = 0;

        // Continue to send until all of buffer is sent.
        while(total_sent < r) {

            // Send the chunk.
            if( (n_bytes_sent = send(client_info->socket, buffer + total_sent, r - total_sent, 0)) < 0) {
                DEBUG_LOG("send() error.");
                LAST_SOCKET_ERROR();
                STATUS_LOG("Message send failed.");
                fclose(fp);
                drop_client(client_info_list, client_info);
                return;
            }
            #ifdef DEBUG
                STATUS_LOG("\n\n");
                STATUS_LOG("Chunk Sent:");

                if (strcmp(file_type, "text/css") == 0  || 
                    strcmp(file_type, "text/csv") == 0  || 
                    strcmp(file_type, "text/html") == 0 || 
                    strcmp(file_type, "application/javascript") == 0 ||
                    strcmp(file_type, "application/json") == 0 ||
                    strcmp(file_type, "text/plain") == 0)  {
                    STATUS_LOG("%.*s\n\n", n_bytes_sent, buffer + total_sent);
                } else {
                    STATUS_LOG("\n[Binary Data: %s]\n\n", file_type);
                }

            #endif

            STATUS_LOG("Sent %d bytes for chunk %d", n_bytes_sent, i);
            total_sent += n_bytes_sent;
            i++;
        }
        STATUS_LOG("%d bytes sent to outgoing buffer.\n", total_sent);

        r = fread(buffer, 1, BSIZE, fp);
    }

    // Make sure to close the file before returning.
    fclose(fp);
    // Close the connection.
    drop_client(client_info_list, client_info);
}
