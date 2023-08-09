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
    exit(EXIT_FAILURE);
}

void *xalloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fatal_error();
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t nmemb, size_t size) {
    ptr = realloc(ptr, nmemb * size);
    if (ptr == NULL) {
        fatal_error();
    }
    return ptr;
}

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

void send_message_(int *clients, const char *message, int client_id, bool from_server) {
    char server_message[64];
    if (from_server) {
        sprintf(server_message, "server: client %d ", client_id - 1);
    } else {
        sprintf(server_message, "client %d: ", client_id - 1);
    }

    char *buffer = xalloc(strlen(server_message) + strlen(message) + 2, sizeof(char));

    sprintf(buffer, "%s%s\n", server_message, message);

    for (int i = 1; i <= clients[0]; i++) {
        if (i == client_id) continue;
        if (send(clients[i], buffer, strlen(buffer), 0) == -1) fatal_error();
    }

    free(buffer);
}

void send_message(int *clients, char *message, int client_id, bool from_server) {
    if (from_server || strstr(message, "\n") == NULL) {
        send_message_(clients, message, client_id, from_server);
    } else {
        char *head = (char *)message;
        char *tail = strstr(message, "\n");

        while (tail != NULL) {
            if (head == tail) {
                send_message_(clients, "", client_id, from_server);
                head++;

            } else {
                char *buffer = xalloc(tail - head + 1, sizeof(char));
                xstrncpy(buffer, head, tail - head);

                send_message_(clients, buffer, client_id, from_server);

                free(buffer);
                head = tail + 1;
            }
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

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fatal_error();
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,					// IPv4
        .sin_addr.s_addr = htonl(2130706433),	// 127.0.0.1
        .sin_port = htons(atoi(argv[1]))		// Port
    };

    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        fatal_error();
    }

    if (listen(server_fd, 10) != 0) {
        fatal_error();
    }

    int* clients = xalloc(1, sizeof(int));
    fd_set read_fds;
    int max_fd = server_fd;

    size_t buffer_size = 256;
    char *buffer = xalloc(buffer_size, sizeof(char));

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        for (int i = 1; i <= clients[0]; i++) {
            FD_SET(clients[i], &read_fds);
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            fatal_error();
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            int client_fd = accept(server_fd, NULL, NULL);

            max_fd = client_fd > max_fd ? client_fd : max_fd;
            clients[0] += 1;
            clients = xrealloc(clients, clients[0] + 1, sizeof(int));
            clients[clients[0]] = client_fd;
            send_message(clients, "just arrived", clients[0], true);

        } else {
            for (int i = 1; i <= clients[0]; i++) {
                if (FD_ISSET(clients[i], &read_fds)) {
                    ssize_t bytes_recieved;
                    size_t total_bytes_recieved = 0;

                    while (1) {
                        bytes_recieved = recv(clients[i], buffer, buffer_size - 1, 0);

                        if (bytes_recieved <= 0) {
                            break;
                        }

                        total_bytes_recieved += bytes_recieved;

                        if (total_bytes_recieved >= buffer_size - 1) {
                            buffer_size *= 2;
                            buffer = xrealloc(buffer, buffer_size, sizeof(char));
                        } else {
                            break;
                        }
                    }
                    buffer[total_bytes_recieved] = '\0';


                    if (total_bytes_recieved > 0) {
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
    return EXIT_SUCCESS;
}
