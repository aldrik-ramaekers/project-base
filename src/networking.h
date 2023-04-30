/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_NETWORKING
#define INCLUDE_NETWORKING

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define NETWORK_PACKET_OVERHEAD (12)

#ifdef OS_WIN
typedef struct t_network_client {
	SOCKET ConnectSocket;
	bool is_connected;
	void (*on_message)(u8* data, u32 length);
	char ip[INET6_ADDRSTRLEN];
} network_client;

typedef struct t_network_server {
	SOCKET ListenSocket;
	bool is_open;
	void (*on_message)(u8* data, u32 length, u64 timestamp, network_client client);
	array clients;
	void (*on_client_disconnect)(network_client client);
} network_server;

typedef struct t_on_connect_args {
	network_server* server;
	network_client client;
} on_connect_args;
#endif

typedef struct t_network_message {
	u32 length;
	u64 timestamp;
	u8* data;
} network_message;

network_server* networking_create_server();
network_client* network_connect_to_server(char* ip, char* port);
void network_client_send(network_client* client, network_message message);
void network_client_close(network_client* client);
network_message network_create_message(u8* data, u32 length, u32 buffer_size);

#endif