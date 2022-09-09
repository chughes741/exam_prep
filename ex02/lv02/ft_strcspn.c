
size_t	ft_strcspn(const char *s, const char *reject) {
	size_t count = 0;
	for (int i = 0; s[i]; ++i) {
		for (int j = 0; reject[j]; ++j) {
			if (s[i] == reject[j]) return count;
		}
		count++;
	}
	return count;
}
