
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 2) return (write(1, "\n", 1));

	char c;
	for (int i = 0; argv[1][i]; ++i) {
		if (argv[1][i] >= 'a' && argv[1][i] <= 'z')
			c =  'z' - argv[1][i] + 'a';
		else if (argv[1][i] >= 'A' && argv[1][i] <= 'Z')
			c =  'Z' - argv[1][i] + 'A';
		else
			c = argv[1][i];
		write(1, &c, 1);
	}
	return (write(1, "\n", 1));
}
