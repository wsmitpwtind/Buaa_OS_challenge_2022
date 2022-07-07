#include "lib.h"

#define MAXDEPTH 50

void
dfs(char *path, int depth, int finalFile[]) {
	struct Fd *fd;
	struct Filefd* fileFd;
	int r = open(path, O_RDONLY);
	if (r < 0) {
		fwritef(1, "shit\n");
		return;
	}
	
	//print
	int i;
	for (i = 0; i < depth; i++) {
		if (i == depth - 1) {
			if (finalFile[i]) {
				fwritef(1, "`-- ");
			} else {
				fwritef(1, "|-- ");
			}
		} else {
			if (finalFile[i]) {
				fwritef(1, "    ");
			} else {
				fwritef(1, "|   ");
			}
		}
	}
	fd = (struct Fd*)num2fd(r);
	fileFd = (struct FileFd*)fd;
	if (fileFd->f_file.f_type == 1) {
        fwritef(1, "\033[34m%s\033[m\n", fileFd->f_file.f_name);
    } else {
        fwritef(1, "%s\n", fileFd->f_file.f_name);
    }
	if (fileFd->f_file.f_type == FTYPE_REG) {
		return;
	}

	u_int size = fileFd->f_file.f_size;
	u_int num = ROUND(size, sizeof(struct File)) / sizeof(struct File);
	struct File *file = (struct File *)fd2data(fd);

	u_int real_num = 0;
	for (i = 0; i < num; i++) {
		if (file[i].f_name[0] != '\0') {
			real_num++;
		}
	}
	
	u_int j = 0;
	for (i = 0; i < num; i++) {
		if (file[i].f_name[0] == '\0') {
			continue;
		}
		char newpath[MAXPATHLEN] = {'\0'};
		strcpy(newpath, path);
		int len = strlen(newpath);
		if (newpath[len - 1] != '/') {
			newpath[len] = '/';
			len++;
		}
		strcpy(newpath+len, file[i].f_name);
		if (j == real_num - 1) {
			finalFile[depth] = 1;
		}
		//fwritef(1, "!!!!!!!!!!!!!!!next: %s\n", newpath);
		dfs(newpath, depth+1, finalFile);
		j++;
	}
	finalFile[depth] = 0;
}


void
umain(int argc, char **argv) {
	if (argc > 2) {
		fwritef(1, "u\033[31msage: tree [dirname]\033[m\n");
		return;
	}
	int finalFile[MAXDEPTH] = {0};	//0:ok, 1:finalfile
	//fwritef(1, "\033[32mSuccese tree\033[m\n");
    if (argc == 1) {
		dfs("/", 0, finalFile);
	} else {
		dfs(argv[1], 0, finalFile);
	}
    fwritef(1, "\033[32mSuccess tree\033[m\n");
}
