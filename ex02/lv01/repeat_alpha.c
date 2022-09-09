
#include <unistd.h>

int main(int argc, char **argv) {
	int rep = 0;
	if (argc != 2)
		return (write(1, "\n", 1));
	for (int i = 0; argv[1][i]; ++i) {
		if (argv[1][i] >= 65 && argv[1][i] <= 90) rep = argv[1][i] - 65;
		if (argv[1][i] >= 97 && argv[1][i] <= 122) rep = argv[1][i] - 97;
		while (rep--) write(1, &argv[1][i], 1);
		write(1, &argv[1][i], 1);
	}
	return 0;
}
