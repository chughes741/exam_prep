#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define MAX_CLIENTS 1024
#define MAX_BUFFER_SIZE 256

typedef struct {
    int fd;
    int id;
} Client;

void fatal_error(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
    exit(1);
}

void send_to_all_clients(Client *clients, int num_clients, int sender_id, const char *message) {
    char buffer[MAX_BUFFER_SIZE];
    sprintf(buffer, MAX_BUFFER_SIZE, "client %d: %s", sender_id, message);

    for (int i = 0; i < num_clients; i++) {
        if (clients[i].id != sender_id) {
            if (send(clients[i].fd, buffer, strlen(buffer), 0) == -1) {
                fatal_error("Fatal error");
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fatal_error("Wrong number of arguments");
    }

    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fatal_error("Fatal error");
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        fatal_error("Fatal error");
    }

    if (listen(server_fd, 5) != 0) {
        fatal_error("Fatal error");
    }

    Client clients[MAX_CLIENTS] = {0};
    int num_clients = 0;

	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(server_fd, &read_fds);
    int max_fd = server_fd;

    while (1) {
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            fatal_error("Fatal error");
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                fatal_error("Fatal error");
            } else {
                clients[num_clients].fd = client_fd;
                clients[num_clients].id = num_clients;
                num_clients++;
                FD_SET(client_fd, &read_fds);
                max_fd = client_fd > max_fd ? client_fd : max_fd;

                char welcome_message[MAX_BUFFER_SIZE];
                sprintf(welcome_message, MAX_BUFFER_SIZE, "server: client %d just arrived\n", num_clients - 1);
                send_to_all_clients(clients, num_clients, num_clients - 1, welcome_message);
            }
        }

        for (int i = 0; i < num_clients; i++) {
            if (FD_ISSET(clients[i].fd, &read_fds)) {
                char buffer[MAX_BUFFER_SIZE];
                int bytes_received = recv(clients[i].fd, buffer, MAX_BUFFER_SIZE - 1, 0);

                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';
                    send_to_all_clients(clients, num_clients, clients[i].id, buffer);

                } else {
                    close(clients[i].fd);

                    char message[MAX_BUFFER_SIZE];
                    sprintf(message, MAX_BUFFER_SIZE, "server: client %d just left\n", clients[i].id);
                    send_to_all_clients(clients, num_clients, clients[i].id, message);

                    for (int j = i; j < num_clients - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                }
            }
        }
    }

    return 0;
}
