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

void send_message(int *clients, const char *message, int client_id, bool from_server) {
	char *server_message = calloc(1, sizeof(char));
	if (from_server) {
		server_message = realloc(server_message, 1 + strlen("server: ") * sizeof(char));
		strcpy(server_message, "server: ")
	}

	char *buffer = calloc(strlen(server_message) + strlen(message) + 4, sizeof(char));

	sprintf(buffer, "%sclient: %d %s", server_message, client_id + 1, message);
	send(clients[client_id], buffer, strlen(buffer), 0);

	free(buffer);
	free(server_message);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		write(STDERR_FILENO, "Wrong number of arguments\n", 26);
		exit(1);
	}

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		fatal_error("Fatal error");
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(2130706433), //127.0.0.1
		.sin_port = htons(atoi(argv[1]))
	};

	if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		fatal_error("Fatal error");
	} 

	if (listen(server_fd, 10) != 0) {
		fatal_error("Fatal error");
	}

	// clients[0] is the len of clients
	int* clients = calloc(1, sizeof(int));

	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(server_fd, &read_fds);
	int max_fd = server_fd;

	char *buffer = calloc(128, sizeof(char));

	while (1) {
		if (select(max_fd, &read_fds, NULL, NULL, NULL) == -1) {
			fatal_error("Fatal error");
		}

		if (FD_ISSET(server_fd, &read_fds)) {
			int client_fd = accept(server_fd, NULL, NULL);
			if (client_fd == -1) {
				fatal_error("Fatal error");
			}

			clients[clients[0]] = client_fd;
			FD_SET(client_fd, &read_fds);
			max_fd = client_fd > max_fd ? client_fd : max_fd;

			send_message(clients, "just arrived\n", clients[0], true);
			clients[0]++;
			clients = realloc(clients, sizeof(int *) * clients[0]);
		}

		for (int i = 0; i < clients[0]; i++) {
			if (FD_ISSET(clients[i], &read_fds)) {
				bzero(buffer, 128);
				int total_bytes = 0;

				char *message = calloc(1, sizeof(char));

				int bytes_recieved = recv(clients[i], buffer, 127, 0);
				total_bytes += bytes_recieved;
				while (bytes_recieved > 0) {
					message = realloc(message, strlen(message) + strlen(buffer) + 1);
					strcat(message, buffer);
					bzero(buffer, 128);
					bytes_recieved = recv(clients[i], buffer, 127, 0);
					total_bytes += bytes_recieved;
				}

				if (total_bytes > 0) {
					send_message(clients, message, i, false);
				} else if (close(clients[i]) == -1) {
					fatal_error("Fatal error");
				} else {
					send_message(clients, "just left\n", i, true);

					clients[0]--;
					for (int j = i; j < clients[0]; j++) {
						clients[j] = clients[j + 1];
					}
					clients = realloc(clients, sizeof(int *) * clients[0]);
				}
				free(message);
			}
		}
	}
	free(clients);
	return 0;
}
