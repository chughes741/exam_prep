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

#define BUFFER_SIZE 120000

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

void send_message(int *clients, int n_client, const char *message) {
    for (int i = 1; i <= clients[0]; i++) {
        if (i == n_client) continue;
        send(clients[i], message, strlen(message), 0);
    }

}

void extract_message(char *buffer, char *msg) {
    for (int i = 0; buffer && buffer[i] && i < BUFFER_SIZE; ++i) {
        if (buffer[i] == '\n') {
            xstrncpy(msg, buffer, i);
            xstrncpy(buffer, buffer + i + 1, strlen(buffer) - i - 1);
            return;
        }
    }
}

char *str_join(char *buf, char *add) {
    int len = buf == 0 ? 0 : strlen(buf);

    char *newbuf = xalloc(sizeof(*newbuf), (len + strlen(add) + 1));
    if (buf != 0) strcat(newbuf, buf);
    free(buf);
    strcat(newbuf, add);
    return (newbuf);
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
    char **client_buf = xalloc(1, sizeof(char *));
    char *buffer = xalloc(BUFFER_SIZE, sizeof(char));
    fd_set read_fds;
    int max_fd = server_fd;

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
            client_buf = xrealloc(client_buf, clients[0] + 1, sizeof(char *));
            client_buf[clients[0]] = xalloc(BUFFER_SIZE, sizeof(char));

            memset(buffer, 0, BUFFER_SIZE);
            sprintf(buffer, "server: client %d just arrived", clients[0] - 1);
            send_message(clients, clients[0], buffer);

        } else {
            for (int i = 1; i <= clients[0]; i++) {
                if (FD_ISSET(clients[i], &read_fds)) {
                    memset(buffer, 0, BUFFER_SIZE);
                    ssize_t bytes_recieved = recv(clients[i], buffer, BUFFER_SIZE - 1, 0);
                    client_buf[i] = str_join(client_buf[i], buffer);

                    if (strlen(clients[i]) > 0) {
                        memset(buffer, 0, BUFFER_SIZE);
                        extract_message(client_buf[i], buffer);
                        sprintf(buffer, "client %d: %s", i - 1, buffer);
                        send_message(clients, i - 1, buffer);
                    } else {
                        memset(buffer, 0, BUFFER_SIZE);
                        sprintf(buffer, "server: client %d just left", i - 1);
                        send_message(clients, i - 1, clients[i]);

                        clients[0] -= 1;
                        for (int j = i; j <= clients[0]; j++) {
                            clients[j] = clients[j + 1];
                        }
                        clients = xrealloc(clients, clients[0] + 1, sizeof(int));

                        free(client_buf[i]);
                        for (int j = i; j <= clients[0]; j++) {
                            client_buf[j] = client_buf[j + 1];
                        }
                        client_buf = xrealloc(client_buf, clients[0] + 1, sizeof(char *));
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
