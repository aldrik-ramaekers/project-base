/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_NETWORKING
#define INCLUDE_NETWORKING

#ifdef OS_WIN
typedef struct t_network_server {
	SOCKET ListenSocket;
	bool is_open;
} network_server;

typedef struct t_on_connect_args {
	network_server* server;
	SOCKET ClientSocket;
} on_connect_args;

typedef struct t_network_client {
	SOCKET ConnectSocket;
	bool is_connected;
} network_client;
#endif

network_server* networking_create_server();
network_client* network_connect_to_server(char* ip, char* port);
void network_client_send(network_client* client, char* data);
void network_client_close(network_client* client);

#endif