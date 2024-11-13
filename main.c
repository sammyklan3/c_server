#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Broadcast message to all clients
void broadcast_message(char *message, int sender_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0 && client_sockets[i] == sender_socket)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Handle communicaton with a single client
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
        broadcast_message(buffer, client_socket); // Send message to all cleints
    }

    // Remove client when they disconnect
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_sockets[i] == client_socket)
        {
            client_sockets[i] = 0;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    printf("Clients disconnected\n");
    return NULL;
}

int main()
{
    int server_socket, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize client sockets to 0
    memset(client_sockets, 0, sizeof(client_sockets));

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Main loop to accept clients
    while ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        // Add new client to the list
        pthread_mutex_lock(&clients_mutex);
        int i;
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                client_sockets[i] = new_socket;
                break;
            }
        }

        pthread_mutex_unlock(&clients_mutex);

        if (i < MAX_CLIENTS)
        {
            //    Create new thread for the client
            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, &new_socket);
            pthread_detach(tid); // Auto-reap thread when it finishes.
            printf("New client connected\n");
        }
        else
        {
            printf("Max clients reached. Connection rejected\n");
            close(new_socket);
        }
    }

    return 0;
}