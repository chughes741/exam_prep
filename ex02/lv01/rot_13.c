
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 2)
		return (write(1, "\n", 1));

	char c = '\0';
	for (int i = 0; argv[1][i]; ++i)
	{
		if (argv[1][i] >= 'a' && argv[1][i] <= 'z')
			c = ((argv[1][i] + 13 - 'a') % 26) + 'a';
		else if (argv[1][i] >= 'A' && argv[1][i] <= 'Z')
			c = ((argv[1][i] + 13 - 'A') % 26) + 'A';
		else
			c = argv[1][i];
		write(1, &c, 1);
	}
	return (write(1, "\n", 1));
}
