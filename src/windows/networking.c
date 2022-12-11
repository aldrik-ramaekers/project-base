#include "../networking.h"
#include "../thread.h"
#include "../logging.h"

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

static void* server_start_receiving_data(void *arg) {
	on_connect_args* args = (on_connect_args*)arg;

	int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

    do {
        iResult = recv(args->ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Server Bytes received: %d\n", iResult);

            iSendResult = send(args->ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                log_info("send failed with error");
                goto cleanup;
            }
            printf("Server Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0) {
            log_info("Connection closing");
		}
        else  {
            log_info("recv failed with error");
            goto cleanup;
        }
    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(args->ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        log_info("shutdown failed with error");
    }

	cleanup:
    closesocket(args->ClientSocket);
	mem_free(args);
	return 0;
}

static void* server_listen_for_clients_thread(void* args) {
	network_server* server = (network_server*)args;
	while (server->is_open) {
		SOCKET ClientSocket = accept(server->ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			log_info("accept failed with error");
		}
		else {
			log_info("New client connected");
		}

		on_connect_args* args = mem_alloc(sizeof(on_connect_args));
		args->server = server;
		args->ClientSocket = ClientSocket;

		thread t = thread_start(server_start_receiving_data, (void*)args);
		thread_detach(&t);
	}
	closesocket(server->ListenSocket);
	mem_free(server);
	log_info("Server stopped");
	return 0;
}

void networking_destroy_server(network_server *server) {
	server->is_open = false;
	WSACleanup();
}

network_server* networking_create_server() {
	network_server *server = mem_alloc(sizeof(network_server));
	server->is_open = false;
	WSADATA wsaData;

    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        log_info("WSAStartup failed with error");
        return server;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        log_info("getaddrinfo failed with error");
        WSACleanup();
        return server;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        log_info("socket failed with error");
        freeaddrinfo(result);
        WSACleanup();
        return server;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        log_info("bind failed with error");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return server;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        log_info("listen failed with error");
        closesocket(ListenSocket);
        WSACleanup();
        return server;
    }

	server->ListenSocket = ListenSocket;
	server->is_open = true;

	thread t = thread_start(server_listen_for_clients_thread, (void*)server);
	thread_detach(&t);

	return server;
}

void network_client_send(network_client* client, char* data) {
	if (!client->is_connected) return;

	int iResult = send(client->ConnectSocket, data, (int)strlen(data), 0);
    if (iResult == SOCKET_ERROR) {
        log_info("send failed with error");
		client->is_connected = false;
        closesocket(client->ConnectSocket);
        return;
    }

    printf("Client Bytes Sent: %d\n", iResult);
}

static void* network_client_receive_thread(void* args) {
	network_client* client = (network_client*)args;
	char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	while (client->is_connected) {
		do {
			iResult = recv(client->ConnectSocket, recvbuf, recvbuflen, 0);
			if ( iResult > 0 )
				printf("Client Bytes received: %d\n", iResult);
			else if ( iResult == 0 ) {
				log_info("Connection closed");
			}
			else {
				log_info("recv failed with error");
			}

		} while( iResult > 0 );
	}
	iResult = shutdown(client->ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        log_info("shutdown failed with error");
    }
	closesocket(client->ConnectSocket);
	mem_free(client);
	log_info("Client stopped");
	return 0;
}

network_client* network_connect_to_server(char* ip, char* port) {
	network_client* client = mem_alloc(sizeof(client));
	client->is_connected = false;

	WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        log_info("WSAStartup failed with error");
        return client;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, port, &hints, &result);
    if ( iResult != 0 ) {
        log_info("getaddrinfo failed with error");
        return client;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            log_info("socket failed with error");
            return client;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        log_info("Unable to connect to server");
        return client;
    }

	client->ConnectSocket = ConnectSocket;
	client->is_connected = true;

	thread t = thread_start(network_client_receive_thread, client);
	thread_detach(&t);
    return client;
}

void network_client_close(network_client* client) {
	client->is_connected = false;
}