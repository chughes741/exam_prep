
int    ft_strcmp(char *s1, char *s2) {
	int i;
	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i]; ++i);
	return (s1[i] - s2[i]);
}
