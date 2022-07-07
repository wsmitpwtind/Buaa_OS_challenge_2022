#include "lib.h"

//lab6-challenge
//always put shell id to the last argument

void
umain(int argc, char **argv)
{
    int i, nflag;

    nflag = 0;
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        nflag = 1;
        argc--;
        argv++;
    }   

	int shell_id = 0;
	for (i = 0; argv[argc-1][i] != '\0'; i++) {
		shell_id = shell_id * 10 + argv[argc-1][i] - '0';
	}

//    for (i = 1; i < argc; i++) {
	for (i = 1; i < argc - 1; i++) {
        if (i > 1)
            write(1, " ", 1);
		//lab6-challenge-final
		if (argv[i][0] == '$') {
			int value = syscall_get_env_value(&argv[i][1], 0, shell_id);
		    fwritef(1, "\033[36mName : \033[m\033[32m%s\033[m ", &argv[i][1]);
            fwritef(1, "\033[36mValue : \033[m\033[32m%d\033[m ", value);
            //fwritef(1, "%d", value);
		} else {
        	write(1, argv[i], strlen(argv[i]));
		} 
    }   
    if (!nflag)
        write(1, "\n", 1); 
}
