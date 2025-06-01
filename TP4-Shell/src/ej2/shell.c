#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

int is_exit_command(const char *cmd) {
    return !strcmp(cmd, "exit")|| !strcmp(cmd, "q");
}

void perror_exit(char* message){
    perror(message);
    exit(1);
}

char *trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

char **parse_args(char *input_buffer) {
    char **arg_list = malloc((MAX_ARGS + 1) * sizeof(char*));
    int i = 0;
    char *p = trim_whitespace(input_buffer);
    char quote;

    while (*p && i < MAX_ARGS + 1) {

        while (*p && isspace(*p)) p++;
        if (*p == '\0') break;

        char *token_start = p;

        if (*p == '"' || *p == '\'') {
            quote = *p;
            p++;
            token_start = p;


            while (*p && *p != quote) p++;

            if (*p != quote) {
                for (int j = 0; j < i; j++)
                    free(arg_list[j]);

                free(arg_list);
                return NULL;
            }


            int len = p - token_start;
            arg_list[i] = malloc(len + 1);
            strncpy(arg_list[i], token_start, len);
            arg_list[i][len] = '\0';

            p++;
        } else {
            while (*p && !isspace(*p)) p++;

            int len = p - token_start;
            arg_list[i] = malloc(len + 1);
            strncpy(arg_list[i], token_start, len);
            arg_list[i][len] = '\0';
        }

        i++;
    }


    if (i > MAX_ARGS) {
        for (int j = 0; j < i; j++)
            free(arg_list[j]);

        free(arg_list);
        return NULL;
    }

    arg_list[i] = NULL;
    return arg_list;
}

int validate_pipeline(char *commands[], int command_count) {

    if (command_count == 0) return 0;

    for (int i = 0; i < command_count; i++) {
        char *trimmed = trim_whitespace(commands[i]);
        if (strlen(trimmed) == 0)
            return 0;
    }

    return 1;
}

int split_pipeline(char *line, char *commands[], int max_commands) {
    int count = 0;
    char *p = line;
    int in_quote = 0;
    char quote_char = '\0';
    char *start = p;

    while (*p) {
        if (!in_quote && (*p == '"' || *p == '\'')) {
            in_quote = 1;
            quote_char = *p;
        } else if (in_quote && *p == quote_char)
            in_quote = 0;
        else if (!in_quote && *p == '|') {
            *p = '\0';
            commands[count++] = trim_whitespace(start);
            if (count >= max_commands) return -1;
            start = p + 1;
        }
        p++;
    }

    commands[count++] = trim_whitespace(start);
    return count;
}


int main() {
    char input_buffer[4096];
    char *commands[MAX_COMMANDS];

    while (1) {
        if (isatty(STDIN_FILENO))
            printf("Shell> ");
        fflush(stdout);

        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) break;
        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        char *trimmed_command = trim_whitespace(input_buffer);

        if (strlen(trimmed_command) == 0) continue;

        if (is_exit_command(trimmed_command)) break;

        if (trimmed_command[0] == '|' ||
            trimmed_command[strlen(trimmed_command) - 1] == '|' ||
            strstr(trimmed_command, "||") != NULL ||
            strstr(trimmed_command, "| |") != NULL) {
            fprintf(stderr, "Syntax error: invalid pipe usage\n");
            continue;
        }

        int command_count = split_pipeline(trimmed_command, commands, MAX_COMMANDS);
        if (command_count == -1) {
            fprintf(stderr, "Error: too many commands in pipeline\n");
            continue;
        }

        if (!validate_pipeline(commands, command_count)) {
            fprintf(stderr, "Syntax error: invalid pipeline\n");
            continue;
        }

        if (command_count == 1) {
            char **args = parse_args(commands[0]);
            if (args == NULL) {
                if (strstr(commands[0], "\"") && !strstr(commands[0], "\" "))
                    fprintf(stderr, "Syntax error: unclosed quotes\n");
                else
                    fprintf(stderr, "Error: too many arguments\n");
                continue;
            }

            if (args[0] == NULL) {
                free(args);
                continue;
            }

            pid_t pid = fork();
            if (pid == -1)
                perror_exit("fork error");

            if (pid == 0) {
                execvp(args[0], args);
                perror_exit("execvp error");
            } else {
                int status;
                wait(&status);
            }

            for (int j = 0; args[j] != NULL; j++)
                free(args[j]);

            free(args);
            continue;
        }

        int pipes[command_count - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1)
                perror_exit("pipe error");
        }

        pid_t pids[command_count];
        int valid_pipeline = 1;

        for (int i = 0; i < command_count && valid_pipeline; i++) {
            char **args = parse_args(commands[i]);
            if (args == NULL) {
                fprintf(stderr, "Error parsing command: %s\n", commands[i]);
                valid_pipeline = 0;
                break;
            }

            if (args[0] == NULL) {
                fprintf(stderr, "Empty command in pipeline\n");
                free(args);
                valid_pipeline = 0;
                break;
            }

            pids[i] = fork();
            if (pids[i] == -1)
                perror_exit("fork error");

            if (pids[i] == 0) {
                if (i > 0)
                    dup2(pipes[i - 1][0], STDIN_FILENO);

                if (i < command_count - 1)
                    dup2(pipes[i][1], STDOUT_FILENO);

                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execvp(args[0], args);
                perror_exit("fork execvp");
            }

            for (int j = 0; args[j] != NULL; j++)
                free(args[j]);

            free(args);
        }

        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        if (valid_pipeline) {
            for (int i = 0; i < command_count; i++) {
                int status;
                waitpid(pids[i], &status, 0);
            }
        }
    }

    return 0;
}