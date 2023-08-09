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

// If error happens, print error message and exit
void fatal_error() {
	write(STDERR_FILENO, "Fatal error\n", 12);
	exit(EXIT_FAILURE);
}

// Allocate memory and exit if allocation fails
void *xalloc(size_t nmemb, size_t size) {
	void *ptr = calloc(nmemb, size);
	if (ptr == NULL) {
		fatal_error();
	}
	return ptr;
}

// Reallocate memory and exit if reallocation fails
void *xrealloc(void *ptr, size_t nmemb, size_t size) {
	ptr = realloc(ptr, nmemb * size);
	if (ptr == NULL) {
		fatal_error();
	}
	return ptr;
}

// Copy n characters from src to dest
void xstrncpy(char *dest, const char *src, size_t n) {
	size_t i = 0;
	while (i < n && src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	while (i < n) {
		dest[i] = '\0';
		i++;
	}
}

// Send a message to all clients except the sender
void send_message_(int *clients, const char *message, int client_id, bool from_server) {
	// Create prefix message
	char server_message[64];
	if (from_server) {
		sprintf(server_message, "server: client %d ", client_id - 1);
	} else {
		sprintf(server_message, "client %d: ", client_id - 1);
	}

	// Create buffer to store the message to be sent
	char *buffer = xalloc(strlen(server_message) + strlen(message) + 2, sizeof(char));

	// Copy the message to the buffer
	sprintf(buffer, "%s%s\n", server_message, message);

	// Send the message to all clients
	for (int i = 1; i <= clients[0]; i++) {
		if (i == client_id) continue;
		if (send(clients[i], buffer, strlen(buffer), 0) == -1) fatal_error();
	}

	free(buffer);
}

void send_message(int *clients, char *message, int client_id, bool from_server) {
	// If the message contains no newline characters, send the message as-is.
	if (from_server || strstr(message, "\n") == NULL) {
		send_message_(clients, message, client_id, from_server);
	} else {
		// Otherwise, send each part of the message separately.
		char *head = (char *)message;
		char *tail = strstr(message, "\n");

		// Keep sending messages until there are no more newline characters in the message.
		while (tail != NULL) {
			// Copy the part of the message before the newline into a new buffer.
			char *buffer = xalloc(tail - head + 1, sizeof(char));
			xstrncpy(buffer, head, tail - head);

			// Send the part of the message before the newline.
			send_message_(clients, buffer, client_id, from_server);

			// Free the buffer and move on to the next part of the message.
			free(buffer);
			head = tail + 1;
			tail = strstr(head, "\n");
		}
		if (head[0] != '\0') {
			send_message_(clients, head, client_id, from_server);
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		write(STDERR_FILENO, "Wrong number of arguments\n", 26);
		exit(1);
	}

	// Create socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		fatal_error();
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,					// IPv4
		.sin_addr.s_addr = htonl(2130706433),	// 127.0.0.1
		.sin_port = htons(atoi(argv[1]))		// Port
	};

	// Bind socket to port
	if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		fatal_error();
	}

	// Listen for connections
	if (listen(server_fd, 10) != 0) {
		fatal_error();
	}

	// clients[0] is the len of clients
	int* clients = xalloc(1, sizeof(int));
	fd_set read_fds;
	int max_fd = server_fd;

	size_t buffer_size = 256;
	char *buffer = xalloc(buffer_size, sizeof(char));

	while (1) {
		// Reset read_fds
		FD_ZERO(&read_fds);
		FD_SET(server_fd, &read_fds);
		for (int i = 1; i <= clients[0]; i++) {
			FD_SET(clients[i], &read_fds);
		}

		// Wait for activity
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			fatal_error();
		}

		// Accept new connection
		if (FD_ISSET(server_fd, &read_fds)) {
			int client_fd = accept(server_fd, NULL, NULL);

			max_fd = client_fd > max_fd ? client_fd : max_fd;
			clients[0] += 1;
			clients = xrealloc(clients, clients[0] + 1, sizeof(int));
			clients[clients[0]] = client_fd;
			send_message(clients, "just arrived", clients[0], true);

		} else {
			// Read message from client
			for (int i = 1; i <= clients[0]; i++) {
				if (FD_ISSET(clients[i], &read_fds)) {
					int bytes_recieved;
					int total_bytes_recieved = 0;

					while (1) {
						bytes_recieved = recv(clients[i], buffer, buffer_size - 1, 0);

						if (bytes_recieved <= 0) {
							break;
						}

						total_bytes_recieved += bytes_recieved;

						if (total_bytes_recieved == buffer_size - 1) {
							buffer_size *= 2;
							buffer = xrealloc(buffer, buffer_size, sizeof(char));
						} else {
							break;
						}
					}
					buffer[total_bytes_recieved] = '\0';


					if (bytes_recieved > 0) {
						send_message(clients, buffer, i, false);
					} else {
						send_message(clients, "just left", i, true);

						clients[0] -= 1;
						for (int j = i; j <= clients[0]; j++) {
							clients[j] = clients[j + 1];
						}
						clients = xrealloc(clients, clients[0] + 1, sizeof(int));
					}
				}
			}
		}
	}
	free(buffer);
	free(clients);
	return EXIT_SUCCESS;
}
