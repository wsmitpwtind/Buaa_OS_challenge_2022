#include "lib.h"

void
umain(int argc, char **argv) {
	if (argc == 2) {
		if (create_file(argv[1]) < 0) {
			fwritef(1, "create file failed!\n");
		} else {
            fwritef(1, "\033[32mSuccess touch\033[m\n");
        }
	} else {
		fwritef(1, "usage: touch [filename]\n");
	}	
}
