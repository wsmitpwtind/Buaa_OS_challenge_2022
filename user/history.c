#include "lib.h"

void
umain(int argc, char** argv) {
	if (argc != 1) {
		fwritef(1, "usage: history\n");
	}
	int fdnum = open(".history", O_RDONLY);
	
	if (fdnum < 0) {
		fwritef(1, "open .history failed\n");
		return;
	}
	char buf;
	int r;
	int cnt = 0;
	int newline = 1;
	while ((r = read(fdnum, &buf, 1)) != 0) {
		if (newline) {
			//fwritef(1, "^^^^^^^^^^");
            fwritef(1, "\033[32mHistory \033[m");
			fwritef(1, "\033[34m%d \033[m", cnt);
            fwritef(1, "\033[32m: \033[m");
            fwritef(1, "%c", buf);
			cnt++;
			newline = 0;
		} else {
			//fwritef(1, "&&&&&&&&&&&");
			fwritef(1, "%c", buf);
		}
		if (buf == '\n') {
			//fwritef(1, "***********");
			newline = 1;
		}
	}
}
