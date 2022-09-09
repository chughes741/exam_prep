
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 4) return (write(1, "\n", 1));

	for (int i = 0; argv[1][i]; ++i) {
		if (argv[1][i] == argv[2][0])
			write(1, &argv[3][0], 1);
		else
			write(1, &argv[1][i], 1);
	}
	return (write(1, "\n", 1));
}
