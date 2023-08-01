#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>


void error(char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
    exit(EXIT_FAILURE);
}

int extract_message(char **buf, char **msg)
{
	*msg = 0;
	if (*buf == NULL) {
		return 0;
    }

    for (int i = 0; (*buf)[i]; i++) {
		if ((*buf)[i] == '\n') {
			char *newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0) {
				return -1;
            }
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return 1;
		}
	}
	return 0;
}

char *str_join(char *buf, char *add)
{
	int		len = buf == 0 ? 0 : strlen(buf);

	char *newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == NULL) {
		return NULL;
    }
	newbuf[0] = 0;
	if (buf != 0) {
		strcat(newbuf, buf);
    }

	free(buf);
	strcat(newbuf, add);
	return newbuf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("Wrong number of arguments");
    }

	int socket_fd, connection_fd, len;

	// socket create and verification
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		error("Fatal error");
	}

	struct sockaddr_in servaddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(2130706433), //127.0.0.1
        .sin_port = htons(atoi(argv[1]))
    };

	// Binding newly created socket to given IP and verification
	if ((bind(socket_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
		error("Fatal error");
	}
	else {
		printf("Socket successfully bound...\n");
    }

	if (listen(socket_fd, 10) != 0) {
		error("Fatal error");
	} else {
        printf("Server listening on ...\n");
    }

    struct sockaddr_in client;
	len = sizeof(client);
	connection_fd = accept(socket_fd, (struct sockaddr *)&client, &len);
	if (connection_fd < 0) {
        error("Fatal error");
    }
    else {
        printf("Server acccept the client...\n");
    }
}
