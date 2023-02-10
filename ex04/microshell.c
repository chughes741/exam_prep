#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

int global_fd;

int ft_strlen(char *str) {
	int len = 0;
	while(str && str[len]) ++len;
	return len;
}

void putstr(char *str) {
	write(STDERR_FILENO, str, ft_strlen(str));
}

void error(char *msg1, char *msg2) {
	putstr("error: ");
	putstr(msg1);
	putstr(msg2);
	putstr("\n");
}

void exit_error(char *msg) {
	error(msg, NULL);
	exit(EXIT_FAILURE);
}

void cd(char **argv) {
	if (argv[2] && strcmp(argv[2], ";") != 0)
		error("cd: bad arguments", NULL);
	if (chdir(argv[1]) == -1)
		error("cd: cannot change directory to ", argv[1]);
}

void exe(char **argv, char **envp, int i) {

	bool next = argv[i] && strcmp(argv[i], "|") ? true : false;

	int fd[2];
	if (pipe(fd) == -1)
		exit_error("fatal");

	int pid = fork();
	if (pid == -1)
		exit_error("fatal");

	if (pid == 0) { // Child
		dup2(global_fd, STDIN_FILENO);
		if (global_fd != STDIN_FILENO) close(global_fd);
		if (next) dup2(fd[1], STDOUT_FILENO);

		argv[i] = NULL;
		if (execve(*argv, argv, envp) == -1)
			error("cannot execute ", argv[0]);

	} else { // Parent
		close(fd[1]);
		if (global_fd != STDIN_FILENO) close(global_fd);
		if (next) dup2(fd[0], global_fd);
		close(fd[0]);

		int status;
		waitpid(pid, &status, 0);
	}
}

int main(int argc, char **argv, char **envp) {
	if (argc < 2) return 0;
	argv[argc] = NULL;

	for (int i = 1; argv[i - 1] && argv[i]; ++i) {
		argv += i;
		i = 0;

		while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";"))
			++i;

		if (strcmp(argv[0], "cd") == 0) {
			cd(argv);
		} else {
			exe(argv, envp, i);
		}
	}
	return EXIT_SUCCESS;
}
