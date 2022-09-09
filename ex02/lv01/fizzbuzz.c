
#include <unistd.h>

void putnum(int nbr) {
	char c = 48 + (nbr / 10);
	if (nbr >= 10)
		write(1, &c, 1);
	c = 48 + (nbr % 10);
	write(1, &c, 1);		
}

int main() {
	for (int i = 1; i <= 100; ++i) {
		if (i % 5 == 0 && i % 3 == 0)
			write(1, "fizzbuzz", 8);
		else if (i % 3 == 0)
			write(1, "fizz", 4);
		else if (i % 5 == 0)
			write(1, "buzz", 4);
		else
			putnum(i);
		write(1, "\n", 1);
	}
	return 0;
}
