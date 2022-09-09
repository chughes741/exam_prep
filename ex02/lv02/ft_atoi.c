
int	ft_atoi(const char *str) {
	int rtn = 0;
	int sign = 1;

	while (str[0] == ' ' || (str[0] >= 9 && str[0] <= 13)) str++;
	if (str[0] == '-') sign = -1;
	while (str[0] == '-' || str[0] == '+') str++;
	for (int i = 0; str[i]; ++i) {
		rtn *= 10;
		rtn += str[i] - '0';
	}
	rtn *= sign;
	return (rtn);
}
