/**
 * @file http_server.c
 * @author David A. Sowles
 * @brief The server code file for the server.
 * @version Midterm
*/

#include "debug_utils.h"
#include "http_server.h"
#include "network_utils.h"

int main() {

    //-------------------------------------------------------------------------
    // Server Setup
    //-------------------------------------------------------------------------

    STATUS_LOG("Server starting up...");

    WSADATA wsaData;
    start_winsock(REQUIRED_WINSOCK_VERSION, &wsaData);

    SOCKET listen_sock = INVALID_SOCKET;
    listen_sock = create_listening_socket(0, "8080");

    print_socket_info(listen_sock);

    struct client_info *client_info_list = NULL;

    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // Server App Main Loop
    //-------------------------------------------------------------------------
    DEBUG_LOG("Entering server app's main loop.");
    while(1) {

        STATUS_LOG("Waiting for activity.");
        fd_set reads;
        reads = wait_for_network_event(&client_info_list, listen_sock);

        // Handle listening socket.
        if (FD_ISSET(listen_sock, &reads)) {
            STATUS_LOG("Incoming connection.");

            struct client_info *client = get_client_info(&client_info_list, -1);

            client->socket = accept(listen_sock, (struct sockaddr*) &(client->address), 
                &(client->address_length));

            if(client->socket == INVALID_SOCKET) {
                DEBUG_LOG("accept() failed.");
                LAST_SOCKET_ERROR();
                STATUS_LOG("New connection could not be made.");
            }

            STATUS_LOG("New connection made successfully.");
            print_socket_info(client->socket);
            

        } // if (FD_ISSET(listen_sock, &reads))

        // Handle client sockets.
        struct client_info *client = client_info_list;
        while(client) {
            struct client_info *next = client->next;

            if(FD_ISSET(client->socket, &reads)) {

                // Handle client request that exceeds client_info buffer.
                if(MAX_REQUEST_SIZE == client->received) {
                    DEBUG_LOG("Client sent bad request, sending HTTP Response message.");
                    send_400(&client_info_list, client);
                    client = next;
                    continue;
                }

                // Receive the raw request message from the client;
                int r = recv(client->socket,
                        client->request + client->received,
                        MAX_REQUEST_SIZE - client->received, 0);

                if (r < 1) {
                    STATUS_LOG("Unexpected disconnect from %s.",
                            get_client_address(client));
                    drop_client(&client_info_list, client);
                } else {

                    #ifdef DEBUG
                        STATUS_LOG("\n\nRaw HTTP Request from Client:");
                        STATUS_LOG("%.*s", r, client->request + client->received);
                        STATUS_LOG("(%d bytes left in request buffer)\n\n", 
                            (int)(sizeof(client->request)) - (client->received + r));
                    #endif

                    STATUS_LOG("Received message from client, %d bytes\n\n", r);

                    client->received += r;
                    client->request[client->received] = 0;

                    // header_end is used to point to the end of the http request header.
                    char *header_end = strstr(client->request, "\r\n\r\n");
                    if (header_end){
                        // "Chop off" the header.
                        *header_end = 0;

                        if(strncmp("GET /", client->request, 5)) {
                            // If no match, send bad request.
                            DEBUG_LOG("Bad HTTP Request, header, sending HTTP 400.");
                            send_400(&client_info_list, client);
                            client = next;
                            continue;
                        } else {

                            // Obtain the path from the HTTP Request.
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if (!end_path) {
                                DEBUG_LOG("Bad Path in HTTP Request header, sending HTTP 400.");
                                send_400(&client_info_list, client);
                                client = next;
                                continue;
                            } else {
                                STATUS_LOG("Sending file to client.");
                                *end_path = 0;
                                serve_resource(&client_info_list, client, path);
                                client = next;
                                continue;
                            }

                        } // if(strncmp("GET /", client->request, 5)) { ... } else {

                    } // if (header_end)

                } // if (r < 1) { ... } else { 

            } // if(FD_ISSET(client->socket, &reads))


            client = next;
        } //while(client)

    } // while(1)


    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // Cleanup
    //-------------------------------------------------------------------------

    WSACleanup();
    closesocket(listen_sock);
    DEBUG_LOG("Cleanup finished in main(), closing app.");

    //-------------------------------------------------------------------------

    printf("Server shutting down.\n");
    return 0;
}