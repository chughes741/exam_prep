
// malloc, free, write, va_start, va_arg, va_copy, va_end
#include <stdlib.h>
#include <stdarg.h>

int write_str(va_list args) {
	char	*str;
	int		count = 0;

	str = va_arg(args, char *);
	for (int i = 0; str[i], ++i)
		count += write(1, &str, 1);
	return count;
}

int	write_int(va_list agrs) {
	return 0;
}

int write_hex(va_list args) {
	return 0;
}

int ft_printf(const char *str, ... ) {
	int count = 0;
	va_list	arg;
	
	va_start(arg, str);
	while (str) {
		if (str == '%') {
			str++;
			if (str == 's')
				count += write_str(arg);
			else if (str == 'd')
				count += write_int(arg);
			else if (str == 'x')
				count += write_hex(arg);
		}
		else
			count += write(1, &str, 1);
		str++;
	}
	return count;
}
