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

#define MAX_BUFFER 512
#define MAX_CLIENTS 256

void fatal_error() {
	write(STDERR_FILENO, "Fatal error\n", 12);
	exit(1);
}

void send_message(int *clients, const char *message, int client_id, bool from_server) {
	char server_message[9] = "server: \0";
	if (!from_server) {
		server_message[0] = '\0';
	}

	char buffer[MAX_BUFFER + 64];
	sprintf(buffer, "%sclient: %d %s", server_message, client_id + 1, message);
	send(clients[client_id], buffer, strlen(buffer), 0);
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

	int clients[MAX_CLIENTS];
	int num_clients = 0;

	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(server_fd, &read_fds);
	int max_fd = server_fd;

	while (1) {
		if (select(max_fd, &read_fds, NULL, NULL, NULL) == -1) {
			fatal_error("Fatal error");
		}

		if (FD_ISSET(server_fd, &read_fds)) {
			int client_fd = accept(server_fd, NULL, NULL);
			if (client_fd == -1) {
				fatal_error("Fatal error");
			}

			clients[num_clients] = client_fd;
			FD_SET(client_fd, &read_fds);
			max_fd = client_fd > max_fd ? client_fd : max_fd;

			send_message(clients, "just arrived\n", num_clients, true);
			num_clients++;
		}

		for (int i = 0; i < num_clients; i++) {
			if (FD_ISSET(clients[i], &read_fds)) {
				char buffer[MAX_BUFFER];
				int bytes_recieved = recv(clients[i], buffer, MAX_BUFFER - 1, 0);

				if (bytes_recieved > 0) {
					buffer[bytes_recieved] = '\0';
					send_message(clients, buffer, i, false);
				} else if (close(clients[i]) == -1) {
					fatal_error("Fatal error");
				} else {
					send_message(clients, "just left\n", i, true);

					num_clients--;
					for (int j = i; j < num_clients; j++) {
						clients[j] = clients[j + 1];
					}
				}
			}
		}
	}
	return 0;
}
