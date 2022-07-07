#include "env_value.h"
#include <printf.h>

#define MAX_VALUE_NUM	128
#define MAX_NAME_LEN	32
#define MAX_GET_RET_LEN	256

struct ENV_VALUE{
	char name[MAX_NAME_LEN];
	int value;
	int shell_id;	//0:global
	int rdonly;	//1:readonlyi
	int alive;	//0 means unset
} env_value[MAX_VALUE_NUM];
static int envNow = 0;

static int idNow = 1;

int create_shell_id() {
	int ret = idNow;
	++idNow;
	return ret;
}

int isSameName(char *name1, char *name2) {
	int i;
 	for (i = 0; name1[i] != '\0' && name2[i] != '\0'; i++) {
		if (name1[i] != name2[i]) {
			return 0;
		}
	}
	if (name1[i] != '\0' || name2[i] != '\0') {
		return 0;
	}
	return 1;
}

int declare_env_value(char *name, int value, int shell_id, int rdonly) {
	//check reonly
	int i;
	for (i = 0; i < envNow; i++) {
		if (!env_value[i].alive) {
			continue;
		}
		if (env_value[i].shell_id == shell_id || env_value[i].shell_id == 0) {
			if (isSameName(env_value[i].name, name)) {
				if (env_value[i].rdonly == 1) {
					printf("\033[31mWARNING : Redeclare readonly value\033[m\n");
					return 0;
				} else if (env_value[i].shell_id == shell_id) {
					env_value[i].value = value;
					env_value[i].rdonly = rdonly;
					printf("\033[32mSuccess redeclare value\033[m\n");
					return 0;
				}
			}
		}
	}
	for (i = 0; name[i] != '\0'; i++) {
		env_value[envNow].name[i] = name[i];
	}
	env_value[envNow].name[i] = '\0';
	env_value[envNow].value = value;
	env_value[envNow].shell_id = shell_id;
	env_value[envNow].rdonly = rdonly;
	env_value[envNow].alive = 1;
	++envNow;
	printf("\033[32mSuccess create value\033[m\n");
	return 0;
}

int unset_env_value(char *name, int shell_id) {
	int i;
	for (i = 0; i < envNow; i++) {
		if (!env_value[i].alive) {
			continue;
		}
		if (env_value[i].shell_id != shell_id && env_value[i].shell_id != 0) {
			continue;
		}
		if (isSameName(env_value[i].name, name)) {
			if (env_value[i].rdonly == 1) {
                printf("\033[31mWARNING : Unset readonly value\033[m\n");
				return 0;
			}
			env_value[i].alive = 0;
			printf("\033[32mSuccess unset value\033[m\n");
		}
	} 
	return 0;
}

//type: 0 means find env value named name, 1 means find all env value
//type 0: ret: value
//type 0: first consider part to part, then consider part to global
//type 1: print
int get_env_value(char *name, int type, int shell_id) {
	int i;
	if (type == 0) {
		for (i = 0; i < envNow; i++) {
			if (!env_value[i].alive) {
				continue;
			}
			if (env_value[i].shell_id != shell_id) {
				continue;
			}
			if (isSameName(name, env_value[i].name)) {
				return env_value[i].value;
			}
		}
		for (i = 0; i < envNow; i++) {
			if (!env_value[i].alive) {
				continue;
			}
			if (env_value[i].shell_id != 0) {
				continue;
			}
			if (isSameName(name, env_value[i].name)) {
				return env_value[i].value;
			}
		}
		return -214748;
	} else if (type == 1) {
		for (i = 0; i < envNow; i++) {
			if (!env_value[i].alive) {
				continue;
			}
			if (env_value[i].shell_id != shell_id && env_value[i].shell_id != 0) {
				continue;
			}
			printf("\033[36mName : \033[m\033[32m%s\033[m ", env_value[i].name);
			printf("\033[36mValue : \033[m\033[32m%d\033[m ", env_value[i].value);
			if (env_value[i].rdonly) {
				printf("\033[36mReadOnly : \033[m\033[32mYES\033[m ");
			} else {
				printf("\033[36mReadOnly : \033[m\033[31mNo\033[m  ");
			}
			if (env_value[i].shell_id) {
				printf("\033[36mVisibility : \033[m\033[32mLOCAL\033[m \n");
			} else {
				printf("\033[36mVisibility : \033[m\033[31mGLOBAL\033[m\n");
			}
		}
	}
	return 0;
}


