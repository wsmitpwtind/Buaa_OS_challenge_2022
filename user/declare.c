#include "lib.h"

//attention: always put shell_id to the last argument

void
umain(int argc, char **argv) {
	if (argc > 5) {
		fwritef(1, "detect too many argument! usage: declare [-xr] [NAME [=VALUE]]\n");
		return;
	}

	int i = 0;
	int shell_id = 0;
	while(argv[argc-1][i] != '\0') {
		shell_id = shell_id * 10 + argv[argc-1][i] - '0';
		i++;
	}

	if (argc == 2) {
		syscall_get_env_value(NULL, 1, shell_id);
		return;
	}
	char name[32] = {'\0'};
	int rdonly = 0;
	int value = 0;
	int has_xr = 0;
	if (argc > 3 && argv[1][0] == '-') {
		has_xr = 1;
		if (argv[1][1] == 'x') {
			shell_id = 0;
			if (argv[1][2] == 'r') {
				rdonly = 1;
			}
		} else if (argv[1][1] == 'r') {
			rdonly = 1;
		}
	}

	if (has_xr && argc == 3) {
		fwritef(1, "value name not detected! usage: declare [-xr] [NAME [=VALUE]]\n");
		return;
	}

	i = 0;
	while(argv[has_xr + 1][i] != '\0') {
		name[i] = argv[has_xr + 1][i];
		i++;
	}
	name[i] = '\0';

	if (argc == has_xr + 4) {	//has =VALUE
		if (argv[has_xr + 2][0] != '=') {
			fwritef(1, "value should follow '='! usage: declare [-xr] [NAME [=VALUE]]\n");
			return;
		}
		i = 1;
		while(argv[has_xr + 2][i] != '\0') {
			value = value * 10 + argv[has_xr + 2][i] - '0';
			i++;
		}
	}
	syscall_declare_env_value(name, value, shell_id, rdonly);
}
