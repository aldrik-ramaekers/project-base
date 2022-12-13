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

	u8* complete_buffer = mem_alloc(50000);
	memset(complete_buffer, 0, 50000);
	u32 complete_buffer_cursor = 0;

	//int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

    do {
        iResult = recv(args->ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
			if (complete_buffer_cursor+iResult < 50000) memcpy(complete_buffer+complete_buffer_cursor, recvbuf, iResult);
			complete_buffer_cursor += iResult;
			
			u32 overflow = 0;
			do
			{
				u32 message_length = ((u32*)complete_buffer)[0];
				if (complete_buffer_cursor >= message_length) {
					overflow = complete_buffer_cursor - message_length;
					if (args->server->on_message) args->server->on_message(complete_buffer, complete_buffer_cursor, (network_client){args->ClientSocket, true});

					if (overflow > 0) {
						memcpy(complete_buffer, complete_buffer+message_length, overflow);
					}
					complete_buffer_cursor = overflow;
					//log_info("Received server message");
				}
				else {
					break;
				}	
			} while (overflow > 0);		
        }
        else if (iResult == 0) {
            log_info("Connection closing");
		}
        else  {
            log_info("recv failed with error");
            goto cleanup;
        }
    } while (args->server->is_open);

	cleanup:
	mem_free(args);
	mem_free(complete_buffer);
	log_info("Server stopped listening to client");
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
		
			if (server->is_open) {
				network_client client = (network_client){ClientSocket, true, 0};
				array_push(&server->clients, (u8*)&client);

				on_connect_args* args = mem_alloc(sizeof(on_connect_args));
				args->server = server;
				args->ClientSocket = ClientSocket;

				thread t = thread_start(server_start_receiving_data, (void*)args);
				thread_detach(&t);
			}
		}
	}
	for (int i = 0; i < server->clients.length; i++) {
		network_client* client = (network_client*)array_at(&server->clients, i);
		closesocket(client->ConnectSocket);
	}
	array_destroy(&server->clients);
	log_info("Server stopped");
	return 0;
}

void networking_destroy_server(network_server *server) {
	server->is_open = false;
	closesocket(server->ListenSocket);
}

network_server* networking_create_server() {
	network_server *server = mem_alloc(sizeof(network_server));
	server->is_open = false;
	server->on_message = 0;
	server->clients = array_create(sizeof(network_client));
	array_reserve(&server->clients, 50);

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
        return server;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        log_info("socket failed with error");
        freeaddrinfo(result);
        return server;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        log_info("bind failed with error");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return server;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        log_info("listen failed with error");
        closesocket(ListenSocket);
        return server;
    }

	server->ListenSocket = ListenSocket;
	server->is_open = true;

	thread t = thread_start(server_listen_for_clients_thread, (void*)server);
	thread_detach(&t);

	return server;
}

// Insert length of message into front of data buffer.
network_message network_create_message(u8* data, u32 length, u32 buffer_size) {
	network_message message;
	message.length = length+4;
	memmove(data+4, data, length);
	u32* datat = (u32*)data;
	datat[0] = message.length;
	message.data = data;
	return message;
}

void network_client_send(network_client* client, network_message message) {
	if (!client->is_connected) return;
	int iResult = send(client->ConnectSocket, (char*)message.data, message.length, 0);
    if (iResult == SOCKET_ERROR) {
        log_info("send failed with error");
        return;
    }
}

static void* network_client_receive_thread(void* args) {
	network_client* client = (network_client*)args;

	u8* complete_buffer = mem_alloc(50000);
	memset(complete_buffer, 0, 50000);
	u32 complete_buffer_cursor = 0;

	char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	while (client->is_connected) {
		do {
			iResult = recv(client->ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				if (complete_buffer_cursor+iResult < 50000) memcpy(complete_buffer+complete_buffer_cursor, recvbuf, iResult);
				complete_buffer_cursor += iResult;

				u32 overflow = 0;
				do
				{
					u32 message_length = ((u32*)complete_buffer)[0];
					if (complete_buffer_cursor >= message_length) {
						overflow = complete_buffer_cursor - message_length;
						if (client->on_message) client->on_message(complete_buffer, message_length);

						if (overflow > 0) {
							memcpy(complete_buffer, complete_buffer+message_length, overflow);
						}
						complete_buffer_cursor = overflow;
						//log_info("Received server message");
					}
					else {
						break;
					}	
				} while (overflow > 0);		
			}		
			else if ( iResult == 0 ) {
				log_info("Connection closed");
			}
			else {
				log_info("recv failed with error");
				printf("Error: %d\n", WSAGetLastError());
			}

		} while(client->is_connected);
	}

	mem_free(client);
	mem_free(complete_buffer);
	log_info("Client stopped");
	return 0;
}

network_client* network_connect_to_server(char* ip, char* port) {
	network_client* client = mem_alloc(sizeof(network_client));
	client->is_connected = false;
	client->on_message = 0;
	client->ConnectSocket = INVALID_SOCKET;

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
	if (client->is_connected) {
		client->is_connected = false;
		closesocket(client->ConnectSocket);
	}
}