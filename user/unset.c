#include "lib.h"

//attention: always put shell id to the last argument

void
umain(int argc, char **argv) {
	if (argc != 3) {
		fwritef(1, "usage: unset NAME\n");
		return;
	}
	int shell_id = 0;
	int i = 0;
	while (argv[2][i] != '\0') {
		shell_id = shell_id * 10 + argv[2][i] - '0';
		i++;
	}
	char name[32] = {'\0'};
	i = 0;
	while (argv[1][i] != '\0') {
		name[i] = argv[1][i];
		i++;
	}
	name[i] = '\0';
	syscall_unset_env_value(name, shell_id);
}

