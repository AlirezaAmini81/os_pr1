#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#define clear() printf("\033[H\033[J");
#define MAX_COM 100 // max length of a command
#define MAX_FILE_NAME 100
#define MAX_LET 1000 // max number of letters in each line of file

//biult-in functions
int lsh_fwl(char **args);
int lsh_mr(char **args);
int lsh_rs(char **args);
int lsh_ucl(char **args);
int lsh_lc(char **args);
int lsh_ftl(char **args);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// List of builtin commands
char *builtin_str[] = {
    "fwl",
    "mr",
    "rs",
    "ucl",
    "lc",
    "ftl",
    "cd",
    "help",
    "exit"
};

// List of builtin functions
int (*builtin_func[]) (char **) = {
    &lsh_fwl,
    &lsh_mr,
    &lsh_rs,
    &lsh_ucl,
    &lsh_lc,
    &lsh_ftl,
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  begin builtin function implementations
*/

// A
int lsh_fwl(char **args) {
    return 1;
}

// B
int lsh_mr(char **args) {
    return 1;
}

// C
int lsh_rs(char **args) {
    return 1;
}

// D
int lsh_ucl(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"ucl\"\n");
        return 1;
    }

    FILE *fp;
    char c;
    char line[MAX_LET];
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp  == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("Uncommented Lines of file %s:\n", filename);
    while(fgets(line, MAX_LET, fp) != NULL)
        if (line[0] != '#')
            printf("%s", line);

    fclose(fp);

    return 1;
}

// F
int lsh_lc(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"lc\"\n");
        return 1;
    }

    FILE *fp;
    char c;
    int count = 0;
    char *filename = args[1];
 
    fp = fopen(filename, "r");
 
    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }
 
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count++;

    fclose(fp);

    printf("File %s has %d lines.\n ", filename, count);

    return 1;
}

// G
int lsh_ftl(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"ftl\"\n");
        return 1;
    }

    FILE *fp;
    int count = 0;
    char line[MAX_LET];
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp  == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("First Ten Lines of file %s:\n", filename);

    while(fgets(line, MAX_LET, fp) != NULL && count++ < 10)
        printf("%s", line);

    fclose(fp);

    return 1;
}

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"cd\"\n");
        return 1;
    }

    if (chdir(args[1]) != 0)
        fprintf(stderr, "cd error\n");

    return 1;
}

int lsh_help(char **args) {
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

/*
  end builtin function implementations
*/

// lunch system command
int sys_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid < 0){
        // error forking
        perror("\nFailed forking child..");
    } else if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Could not execute command... ");
        }
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// execute command
int cmnd_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // an empty command was entered
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return sys_launch(args);
}

// parse space line, put result in arg
void parse_space(char* line, char** arg) {
	int i;

	for (i = 0; i < MAX_COM; i++) {
		arg[i] = strsep(&line, " ");

		if (arg[i] == NULL)
			break;
		if (strlen(arg[i]) == 0)
			i--;
	}
}

// print current directory
void printDir() {
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("\n~%s", cwd);
}

// handle ctrl+c signal
void signal_handler(int sig){
    printDir();
    printf("\n> ");
}

// command loop
void shell_program(void) {
    char *line;
    char* args[MAX_COM];
    int status;

    signal(SIGINT, signal_handler);

    do {
        printDir();
        line = readline("\n> ");
        if (line && *line)
            add_history(line);
        parse_space(line, args);
        status = cmnd_execute(args);

    } while (status);
}

// greeting shell during startup
void init_shell() {
	clear();
	printf("\n\t\t\t****WELCOME TO MY SHELL****\n");
	sleep(1);
	clear();
}

int main(int argc, char **argv) {
    // Load config files, if any.
    init_shell();
    // Run command loop.
    shell_program();
    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}


/*
gcc main.c -lreadline
./a.out 
*/

