//for lab6-challenge-env-value

int
create_shell_id(void);

int
declare_env_value(char* name, int value, int shell_id, int type);

int
unset_env_value(char* name, int shell_id);

int
get_env_value(char* name, int type, int shell_id);

