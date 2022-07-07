#include "lib.h"
#include <args.h>

int debug_ = 0;

//lab6-challenge-final
int shell_id = 0;

//
// get the next token from string s
// set *p1 to the beginning of the token and
// *p2 just past the token.
// return:
//	0 for end-of-string
//	> for >
//	| for |
//	w for a word
//
// eventually (once we parse the space where the nul will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int
_gettoken(char *s, char **p1, char **p2)
{
	int t;

	if (s == 0) {
		//if (debug_ > 1) writef("GETTOKEN NULL\n");
		return 0;
	}

	//if (debug_ > 1) writef("GETTOKEN: %s\n", s);

	*p1 = 0;
	*p2 = 0;

	while(strchr(WHITESPACE, *s))
		*s++ = 0;
	if(*s == 0) {
	//	if (debug_ > 1) writef("EOL\n");
		return 0;
	}

	//lab6-challenge
    
	if(*s == '"') {
		*s = 0;
		*p1 = ++s;
		while (s != 0 && *s != '"') {
			s++;
		}
		if (s == 0) {
			writef("\"don't match!!!\n");
			return 0;
		}
		*s = 0;
		*p2 = s;
		return 'w';
	}

	if(strchr(SYMBOLS, *s)){
		t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
//		if (debug_ > 1) writef("TOK %c\n", t);
		return t;
	}
	*p1 = s;
	while(*s && !strchr(WHITESPACE SYMBOLS, *s))
		s++;
	*p2 = s;
	if (debug_ > 1) {
		t = **p2;
		**p2 = 0;
//		writef("WORD: %s\n", *p1);
		**p2 = t;
	}

    /*
    if(*s == '"')
{
    *s = 0;
    s++;
    writef("s = %d\n",s);
    *p1 = s;
    while(s != 0 && *s != '"') 
    {
        s++;
    }
    if (s == 0) 
    {
        writef("don't match\n");
        return 0;
    }
    writef("s = %d\n",s);
    *s = 0;
    *p2 = s;
    return 'w';
}
*/
	return 'w';
}

int
gettoken(char *s, char **p1)
{
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

//lab6-challenge-declare
//return 1 means cmd is declare or unset
int isDeclareOrUnset(char *name) {
	char *de = "declare.b";
	char *un = "unset.b";
	char *ec = "echo.b";
	int i;
	for (i = 0; name[i] != '\0' && de[i] != '\0'; i++) {
		if (name[i] != de[i]) {
			break;
		}
	}
	if (name[i] == '\0' && de[i] == '\0') {
		return 1;
	}
	for (i = 0; name[i] != '\0' && un[i] != '\0'; i++) {
		if (name[i] != un[i]) {
			break;
		}
	}
	if (name[i] == '\0' && un[i] == '\0') {
		return 1;
	}
	for (i = 0; name[i] != '\0' && ec[i] != '\0'; i++) {
		if (name[i] != ec[i]) {
			break;
		}
	}
	if (name[i] == '\0' && ec[i] == '\0') {
		return 1;
	}
	return 0;
}

#define MAXARGS 16
void
runcmd(char *s)
{
	char *argv[MAXARGS], *t;
	int argc, c, i, r, p[2], fd, rightpipe;
	int fdnum;
	int forktemp; //lab6-challenge
	int noWait = 0;	//lab6-challenge, cmd with '&' run without wait
	rightpipe = 0;
	gettoken(s, 0);
again:
	argc = 0;
	for(;;){
		c = gettoken(0, &t);
		switch(c){
		case 0:
			goto runit;
		case 'w':
			if(argc == MAXARGS){
				writef("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if(gettoken(0, &t) != 'w'){
				writef("syntax error: < not followed by word\n");
				exit();
			}
			// Your code here -- open t for reading,
			// dup it onto fd 0, and then close the fd you got.
			r = open(t, O_RDONLY);
			if (r < 0) {
				user_panic("< open file failed!");
			}
			fd = r;
			dup(fd, 0);
			close(fd);
			break;
		case '>':
			// Your code here -- open t for writing,
			// dup it onto fd 1, and then close the fd you got.
			if (gettoken(0, &t) != 'w') {
				writef("syntax error: > not followed by word\n");
				exit();
			}
			r = open(t, O_WRONLY);
			if (r < 0) {
				user_panic("> open file failed!");
			}
			fd = r;
			dup(fd, 1);
			close(fd);
			break;
		case '|':
			// Your code here.
			// 	First, allocate a pipe.
			//	Then fork.
			//	the child runs the right side of the pipe:
			//		dup the read end of the pipe onto 0
			//		close the read end of the pipe
			//		close the write end of the pipe
			//		goto again, to parse the rest of the command line
			//	the parent runs the left side of the pipe:
			//		dup the write end of the pipe onto 1
			//		close the write end of the pipe
			//		close the read end of the pipe
			//		set "rightpipe" to the child envid
			//		goto runit, to execute this piece of the pipeline
			//			and then wait for the right side to finish
			pipe(p);
			if ((rightpipe = fork()) == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				goto again;
			}
			else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				goto runit;
			}
			break;
		case ';':
			forktemp = fork();
			if (forktemp) {
				wait(forktemp);
				goto again;
			} else {
				goto runit;
			}
			break;
		case '&':
			noWait = 1;
			break;
		}
	}

runit:
	if(argc == 0) {
		if (debug_) writef("EMPTY COMMAND\n");
		return;
	}
	argv[argc] = 0;
	if (1) {
		writef("[%08x] SPAWN:", env->env_id);
		for (i=0; argv[i]; i++)
			writef(" %s", argv[i]);
		writef("\n");
	}
	
	//lab6-challenge-declare/unset
	if (isDeclareOrUnset(argv[0])) {
		int tmp = shell_id;
		char num[15] = {'\0'};
		int loc = 0;
		while (tmp != 0) {
			num[loc++] = tmp % 10 + '0';
			tmp = tmp / 10;
		}
		loc--;
		int loc2 = 0;
		char num2[15] = {'\0'};
		while(loc >= 0) {
			num2[loc2++] = num[loc];
			loc--;
		}
		num2[loc2] = '\0';
		argv[argc] = num2;
		argc++;
		argv[argc] = NULL;
	}

	if ((r = spawn(argv[0], argv)) < 0)
		writef("spawn %s: %e\n", argv[0], r);
	close_all();
	if (r >= 0) {
		if (debug_) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
		if (!noWait) wait(r);
	}
	if (rightpipe) {
		if (debug_) writef("[%08x] WAIT right-pipe %08x\n", env->env_id, rightpipe);
		wait(rightpipe);
	}

	exit();
}

//lab6-challenge
int newDirCmd = 1;	//if 1: dircmd next to dircmd, else 0
int offset;	//0:empty line, -1:last cmd, -2:...
int
solveDirCmd(char *buf, int type) {	//type: 0 means up, 1 means down
	if (newDirCmd == 1) {
		offset = 0;
	}
	if (type == 0) {
		offset--;
	} else if (offset < 0) {
		offset++;
	}
    int x = 0;
	if (offset == 0) {
		//buf[0] = '\0';
        while(buf[x] != '\0') {
        buf[x] = '\0';
        x ++;
    }
		return -1;
	}
    //fwritef(1, "offset = %d\n", offset);
	int fdnum = open(".history", O_RDONLY);
	if (fdnum < 0) {
		fwritef(1, "open .history failed in sloveDir!\n");
		return 0;
	}
	struct Fd *fd = num2fd(fdnum);
	char *c;
	char *begin = fd2data(fd);
	char *end = begin + ((struct Filefd*)fd)->f_file.f_size;
	c = end - 1;

	while(((*c) == '\n' || (*c) == 0) && (c > begin)) {
		c--;
	}

	if (c == begin) {	//no history cmd
		buf[0] = '\0';
		return 0;
	}

	c++;	//last \n or \0
	int i;
	for (i = 0; i > offset; i--) {
		while((*c) != '\n' && (*c) != '\0') {
			c--;
			if (c <= begin) {
				break;
			}
		}
		c--;
		if (c <= begin) {
			break;
		}
	}	
	offset = i;	//avoid offset too bigger than real cmd num
	if (c > begin) {
		while(c > begin && (*c) != '\n') {
			c--;
		}
		if ((*c) == '\n') {
			c++;
		}
	} else {
		c = begin;
	}
	int now = 0;
    while(buf[now] != '\0') {
        buf[now] = '\0';
        now ++;
    }
    now = 0;
	while ((*c) != '\n' && (*c) != '\0' && (*c) < end) {
		buf[now] = *c;
		now++;
		c++;
	}
	//buf[now] = '\0';
    //writef("QWQ %d\n", now - 1);
	return now-1;
}

void
readline(char *buf, u_int n)
{
	int i, r;

	r = 0;
	for(i=0; i<n; i++){
		if((r = read(0, buf+i, 1)) != 1){
			if(r < 0)
				writef("read error: %e", r);
			exit();
		}

		if(buf[i] == '\b'){
			if(i > 0)
				i -= 2;
			else
				i = 0;
		}
		if(buf[i] == '\r' || buf[i] == '\n'){
			buf[i] = 0;
			return;
		}

		//lab6-challenge
		if(buf[i] == 27) {
			char tmp;
			read(0, &tmp, 1);
            char tmp2;
			read(0, &tmp2, 1);
			if (tmp == 91 && tmp2 == 65) {
				fwritef(1, "\x1b[B");	//down to cmd line
				int j;
				for (j = 0; j < i; j++) {
					fwritef(1, "\x1b[D");	//left to line head
				}
				fwritef(1, "\x1b[K");	//clean line
				//fwritef(1, "prebuf=begin%send\n", buf);
                i = solveDirCmd(buf, 0);
                //fwritef(1, "i=%d\n", i);
				//buf[i] = '\0';
				fwritef(1, "%s", buf);
                //fwritef(1, "get up\n");
			} else if (tmp == 91 && tmp2 == 66) {
				int j;
				for (j = 0; j < i; j++) {
					fwritef(1, "\x1b[D");
				}
				fwritef(1, "\x1b[K");
				i = solveDirCmd(buf, 1);
				//buf[i] = '\0';
				fwritef(1, "%s", buf);
			}
			newDirCmd = 0;
		} else {
			newDirCmd = 1;
		}
	}
	writef("line too long\n");
	while((r = read(0, buf, 1)) == 1 && buf[0] != '\n')
		;
	buf[0] = 0;
}	

char buf[1024];

void
usage(void)
{
	writef("usage: sh [-dix] [command-file]\n");
	exit();
}

//lab6-challenge
void
save_cmd(char* cmd) {
	int r = open(".history", O_CREAT | O_WRONLY | O_APPEND);
	if (r < 0) {
		fwritef(1, "open .history failed! in save");
		return r;
	}
    //fwritef(1, "save success");
	write(r, cmd, strlen(cmd));
	write(r, "\n", 1);
	return 0;
}

void
umain(int argc, char **argv)
{
	int r, interactive, echocmds;
	interactive = '?';
	echocmds = 0;
    writef(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    writef("::  \033[31m__          __\033[m            _ _                 _   _           \033[31m_ \033[m ::\n");
    writef("::  \033[31m\\ \\        / /\033[m           (_) |               | | (_)         \033[31m| |\033[m ::\n");
    writef("::   \033[31m\\ \\  /\\  / /\033[m__ \033[31m_ __ ___\033[m  _| |_ \033[31m_ __\033[m__      _| |_ _ _ __  \033[31m __| |\033[m ::\n");
    writef("::    \033[31m\\ \\/  \\/ /\033[m __\033[31m| '_ ` _ \\\033[m| | __\033[31m| '_ \033[m\\ \\ /\\ / / __| | '_ \\\033[31m / _` |\033[m ::\n");
    writef("::     \033[31m\\  /\\  /\033[m\\__ \\ \033[31m| | | | \033[m| | |_\033[31m| |_) \033[m\\ V  V /| |_| | | | \033[31m| (_| |\033[m ::\n");
    writef("::      \033[31m\\/  \\/\033[m |___/\033[31m_| |_| |_\033[m|_|\\__\033[31m| .__/\033[m \\_/\\_/  \\__|_|_| |_|\033[31m\\__,_|\033[m ::\n");
    writef("::                                 \033[31m| |\033[m                               ::\n");
    writef("::                                 \033[31m|_|\033[m       github.com/wsmitpwtind  ::\n");
    writef(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

	shell_id = syscall_create_shell_id();
	writef("\033[34mThis shell's id :\033[m \033[32m%d\033[m\n", shell_id);

	ARGBEGIN{
	case 'd':
		debug_++;
		break;
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}ARGEND

	if(argc > 1)
		usage();
	if(argc == 1){
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0)
			user_panic("open %s: %e", r);
		user_assert(r==0);
	}
	if(interactive == '?')
		interactive = iscons(0);
	for(;;){
		if (interactive)
			fwritef(1, "\n$ ");
		readline(buf, sizeof buf);
	
		//lab6-challenge
		save_cmd(buf);
	
		if (buf[0] == '#')
			continue;
		if (echocmds)
			fwritef(1, "# %s\n", buf);
		if ((r = fork()) < 0)
			user_panic("fork: %e", r);
		if (r == 0) {
			runcmd(buf);
			exit();
			return;
		} else
			wait(r);
	}
}

