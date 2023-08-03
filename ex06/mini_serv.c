#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void fatal_error() {
	write(STDERR_FILENO, "Fatal error\n", 12);
	exit(1);
}

void send_message_(int *clients, const char *message, int client_id, bool from_server) {
	char server_message[64];
	if (from_server) {
		sprintf(server_message, "server: client %d ", client_id - 1);
	} else {
		sprintf(server_message, "client %d: ", client_id - 1);
	}

	char *buffer = calloc(strlen(server_message) + strlen(message) + 1, sizeof(char));
	if (buffer == NULL) fatal_error();

	sprintf(buffer, "%s%s\n", server_message, message);
	for (int i = 1; i <= clients[0]; i++) {
		if (i == client_id) continue;
		if (send(clients[i], buffer, strlen(buffer), 0) == -1) fatal_error();
	}

	printf("%s", buffer);
	free(buffer);
}

void send_message(int *clients, const char *message, int client_id, bool from_server) {
	if (from_server || strstr(message, "\n") == NULL) {
		send_message_(clients, message, client_id, from_server);
	} else {
		char *head = (char *)message;
		char *tail = strstr(message, "\n");
		while (tail != NULL) {
			char *buffer = calloc(tail - head + 1, sizeof(char));
			if (buffer == NULL) fatal_error();
			strncpy(buffer, head, tail - head);
			send_message_(clients, buffer, client_id, from_server);
			free(buffer);
			head = tail + 1;
			tail = strstr(head, "\n");
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		write(STDERR_FILENO, "Wrong number of arguments\n", 26);
		exit(1);
	}

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) fatal_error();

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(2130706433), //127.0.0.1
		.sin_port = htons(atoi(argv[1]))
	};

	if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) fatal_error();

	if (listen(server_fd, 10) != 0) fatal_error();

	// clients[0] is the len of clients
	int* clients = calloc(1, sizeof(int));
	if (clients == NULL) fatal_error();
	fd_set read_fds;
	int max_fd = server_fd;

	while (1) {
		// Reset read_fds
		FD_ZERO(&read_fds);
		FD_SET(server_fd, &read_fds);
		for (int i = 1; i <= clients[0]; i++) {
			FD_SET(clients[i], &read_fds);
		}

		// Wait for activity
		printf("\nWaiting for activity\n");
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) fatal_error();

		// Accept new connection
		if (FD_ISSET(server_fd, &read_fds)) {
			printf("Accepting new connection\n");
			int client_fd = accept(server_fd, NULL, NULL);
			if (client_fd == -1) fatal_error();

			max_fd = client_fd > max_fd ? client_fd : max_fd;
			clients[0] += 1;
			clients = realloc(clients, (clients[0] + 1) * sizeof(int));
			clients[clients[0]] = client_fd;
			if (clients == NULL) fatal_error();
			send_message(clients, "just arrived", clients[0], true);
		} else {
			// Read message from client
			for (int i = 1; i <= clients[0]; i++) {
				printf("Checking client %d\n", i);
				if (FD_ISSET(clients[i], &read_fds)) {
					printf("Reading message from client\n");
					char buffer[256];
					bzero(buffer, 256);
					int bytes_recieved = recv(clients[i], buffer, 255, 0);

					if (bytes_recieved > 0) {
						send_message(clients, buffer, i, false);
					} else {
						send_message(clients, "just left", i, true);

						clients[0] -= 1;
						for (int j = i; j <= clients[0]; j++) {
							clients[j] = clients[j + 1];
						}
						clients = realloc(clients, (clients[0] + 1) * sizeof(int));
						if (clients == NULL) fatal_error();
					}
				}
			}
		}
	}
	free(clients);
	return 0;
}
