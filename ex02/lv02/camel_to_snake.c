
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 2) return (write(1, "\n", 1));

	char c;
	for (int i = 0; argv[1][i]; ++i) {
		if (argv[1][i] >= 'A' && argv[1][i] <= 'Z') {
			write(1, "_", 1);
			c = argv[1][i] - 'A' + 'a';
		}
		else
			c = argv[1][i];
		write(1, &c, 1);
	}
	return (write(1, "\n", 1));
}
