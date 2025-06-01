#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>

void perror_exit(char* mensaje){
    perror(mensaje);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 4)
        perror_exit("Uso: anillo <num_procesos> <valor_inicial> <proceso_inicio>");

    int num_procesos = atoi(argv[1]);
    int valor_inicial = atoi(argv[2]);
    int proceso_inicio = atoi(argv[3]);

    if (num_procesos <= 0)
        perror_exit("La cantidad de procesos debe ser positiva y no nula.");

    if (valor_inicial < INT_MIN || valor_inicial > INT_MAX - num_procesos)
        perror_exit("El valor no puede superar el máximo ni mínimo de INT.");

    if (proceso_inicio < 0 || proceso_inicio >= num_procesos)
        perror_exit("El proceso inicial debe estar entre 0 y num_procesos - 1.");

    int pipes[num_procesos][2];
    for (int i = 0; i < num_procesos; i++)
        if (pipe(pipes[i]) == -1)
            perror_exit("Error al crear pipe.");

    for (int i = 0; i < num_procesos; i++) {
        pid_t pid = fork();
        if (pid == -1)
            perror_exit("Error al hacer fork.");

        if (pid != 0) continue; // padre continúa

        int idx_anterior = (i + num_procesos - 1) % num_procesos;

        int fd_lectura = pipes[idx_anterior][0];
        int fd_escritura = pipes[i][1];

        for (int j = 0; j < num_procesos; j++) {
            if (j != i) close(pipes[j][1]);
            if (j != idx_anterior) close(pipes[j][0]);
        }

        int valor;

        if (i == proceso_inicio) {
            valor = valor_inicial;
            printf("Proceso %d inicia con valor: %d\n", i, valor);
            write(fd_escritura, &valor, sizeof(int));
        }

        read(fd_lectura, &valor, sizeof(int));
        printf("Proceso %d recibió: %d\n", i, valor);
        valor++;

        if (i == proceso_inicio)
            printf("Valor final: %d\n", valor);
        else
            write(fd_escritura, &valor, sizeof(int));

        close(fd_lectura);
        close(fd_escritura);
        exit(0);
    }

    for (int i = 0; i < num_procesos; i++)
        wait(NULL);

    printf("Valor final padre: %d\n", valor_inicial);

    return 0;
}
