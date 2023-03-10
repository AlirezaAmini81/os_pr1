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

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  begin builtin function implementations
*/

// A
int lsh_fwl(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"fwl\"\n");
        return 1;
    }

    FILE *fp;
    char line[MAX_LET];
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("First Word of Lines of file %s:\n", filename);
    while (fgets(line, MAX_LET, fp)) {
        int i = 0;
        for(; line[i] == ' ' && i < MAX_LET; i++);
        for(; i < MAX_LET; i++) {
            if(line[i] == '\n' || line[i] == ' ' ) {
                printf("\n");
                break;
            } else {
                printf("%c", line[i]);
            }
        }
    }

    fclose(fp);

    return 1;
}

// B
int lsh_mr(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"mr\"\n");
        return 1;
    }

    FILE *fp;
    char ch, *line;
    size_t len =0;
    char words[1000][1000], word[20];
    int i = 0, j, k, maxCount = 0, count;
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    while(getline(&line, &len, fp) != -1) {
        for(k = 0; line[k] != '\0'; k++) {
            if(line[k] != ' ' && line[k] != '\n' && line[k] != ',' && line[k] != '.') {
                words[i][j++] = tolower(line[k]);
            } else {
                words[i][j] = '\0';
                i++;
                j = 0;
            }
        }
    }

    int length = i;
    for(i = 0; i < length; i++) {
        count = 1;
        for( j = i + 1; j < length; j++)
            if(strcmp(words[i], words[j]) == 0 && (strcmp(words[i], " ") != 0))
                count++;

        if(count > maxCount) {
            maxCount = count;
            strcpy(word, words[i]);
        }
    }

    fclose(fp);

    printf(" Most repeated word: %s", word);

    return 1;
}

// C
int lsh_rs(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"rs\"\n");
        return 1;
    }

    FILE *fp;
    char line[MAX_LET];
    char out[MAX_LET * MAX_LET];
    int j = 0;
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("Remove Space of file %s:\n", filename);
    while (fgets(line, MAX_LET, fp)) {
        for(int i = 0; i < MAX_LET; i++) {
            if(line[i] == '\n') {
                break;
            } else if(line[i] != ' ') {
                out[j] = line[i];
                j++;
            }
        }
    }

    printf("%s\n", out);

    fclose(fp);

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

    if (fp  == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("UnCommented Lines of file %s:\n", filename);
    while(fgets(line, MAX_LET, fp)) {
        int i = 0;
        for(; line[i] == ' ' && i < MAX_LET; i++);
        if (line[i] != '#')
            printf("%s", line);
    }

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
    char line[MAX_LET];
    int count = 0;
    char *filename = args[1];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    while(fgets(line, MAX_LET, fp))
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

    if (fp  == NULL) {
        fprintf(stderr, "could not open file %s\n", filename);
        return 1;
    }

    printf("First Ten Lines of file %s:\n", filename);

    while(fgets(line, MAX_LET, fp) && count++ < 10)
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
    printf("The following commands are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++)
        printf("  %s\n", builtin_str[i]);

    return 1;
}

/*
  end builtin function implementations
*/

// List of builtin functions
int (*builtin_func[]) (char **) = {
        &lsh_fwl,
        &lsh_mr,
        &lsh_rs,
        &lsh_ucl,
        &lsh_lc,
        &lsh_ftl,
        &lsh_cd,
        &lsh_help
};

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

// lunch builtin command
int builtin_launch(int i,char **args) {
    pid_t pid, wpid;
    int status;

    if (strcmp(args[0],"exit") == 0){
        return 0;
    } else if (strcmp(args[0],"cd") == 0) {
        return ((*builtin_func[6])(args));
    }

    pid = fork();

    if (pid < 0){
        // error forking
        perror("\nFailed forking child..");
    } else if (pid == 0) {
        // Child process
        if ((*builtin_func[i])(args) < 0) {
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

    if (args[0] == NULL)
        // an empty command was entered
        return 1;

    for (i = 0; i < lsh_num_builtins(); i++)
        if (strcmp(args[0], builtin_str[i]) == 0)
            return builtin_launch(i, args);

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
    init_shell();

    // Run command loop.
    shell_program();

    return EXIT_SUCCESS;
}


/*
gcc main.c -lreadline
./a.out
*/