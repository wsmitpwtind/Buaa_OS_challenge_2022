#include "lib.h"

void
umain(int argc, char **argv) {
	if (argc == 2) {
		if (create_dir(argv[1]) < 0) {
			fwritef(1, "create dir failed!\n");
		} else {
            fwritef(1, "\033[32mSuccess mkdir\033[m\n");
        }
	} else {
		fwritef(1, "usage: mkdir [dirname]\n");
	}
}
