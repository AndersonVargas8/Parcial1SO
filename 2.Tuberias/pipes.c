#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

enum{KB = 1000, MB = 1000000};
int SIZE = 100*MB;

struct typeDato{
    char valor[KB];
};

int ejecucion()
{
    char buffer[KB];
    int pipefd[2];
    int pipeTiempo[2];
    int r;
    pid_t cpid;
    double timeHijo, timePadre;
    double tiempoTotal;

    r = pipe(pipefd);
    if (r < 0)
    {
        perror("Error en tuberia");
        exit(-1);
    }
    r = pipe(pipeTiempo);
    if (r < 0)
    {
        perror("Error en tuberia de tiempo");
        exit(-1);
    }

    cpid = fork();
    if (cpid < 0)
    {
        perror("Error en fork");
        exit(-1);
    }

    if (cpid == 0)
    {
        close(pipefd[0]);
        char buffer[sizeof(struct typeDato)];

        struct typeDato dato;
        for (int i = 0; i < sizeof(struct typeDato); i++)
        {
            buffer[i] = 'a';
        }

        strcpy(dato.valor,buffer);
        clock_t time1 = clock();

        int n = SIZE/sizeof(struct typeDato);
        for (int i = 0; i < n; i++)
        {
            r = write(pipefd[1], dato.valor, sizeof(struct typeDato));
            if (r != sizeof(struct typeDato))
            {
                perror("Error enviando los datos");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd[1]);
        clock_t time2 = clock();

        timeHijo = (double)(time2 - time1) / CLOCKS_PER_SEC;

   
        close(pipeTiempo[0]);
        r = write(pipeTiempo[1], &timeHijo, sizeof(double));
        if (r != sizeof(double))
        {
            perror("Error en envio de tiempo");
            exit(EXIT_FAILURE);
        }
        close(pipeTiempo[1]);
    }
    else
    {
        close(pipefd[1]);
        clock_t time1 = clock();

        struct typeDato dato;
        int n = SIZE/sizeof(struct typeDato);
        long recibido = 0;
        for (int i = 0; i < n; i++)
        {
            r = read(pipefd[0], dato.valor, sizeof(struct typeDato));

            recibido += strlen(dato.valor);
            //printf("%s\n\n",dato.valor);
            if (r != sizeof(struct typeDato))
            {
                perror("Error en la lectura");
                exit(EXIT_FAILURE);
            }
        }

        clock_t time2 = clock();
        timePadre = (double)(time2 - time1) / CLOCKS_PER_SEC;

        buffer[sizeof(struct typeDato)] = 0;
        close(pipefd[0]);

        if(SIZE < MB){
            printf("Tamaño de lectura: %ld KB\n", recibido/KB);
        }else{
            printf("Tamaño de lectura: %ld MB\n", recibido/MB);
        }

        //////////////////////////////////
        close(pipeTiempo[1]);
        r = read(pipeTiempo[0], &tiempoTotal, sizeof(double));
        if (r < 8)
        {
            perror("Error en tiempo de envio");
            exit(-1);
        }
        close(pipeTiempo[0]);
        ///////////////////////////////////

        tiempoTotal += timePadre;
        printf("Tiempo de ejecución:%lf Segundos \n", tiempoTotal);
    }

    exit(EXIT_FAILURE);
}

void asignarTamanio(int tamanio){
    SIZE = tamanio;
}

void main(){
    printf("BIENVENIDO. **COMUNICACIÓN MEDIANTE TUBERÍAS**\n--------------------------------------------\n");
    int opc = 0;

    while(opc != 7){
        printf("Seleccione el tamaño de datos que quiere usar\n1. 1KB\n2. 10KB\n3. 100KB\n4. 1MB\n5. 10MB\n6. 100MB\n7. Salir\n=> ");
        scanf("%d",&opc);

        switch(opc){
            case 1:
                asignarTamanio(1*KB);
                break;
            
            case 2:
                asignarTamanio(10*KB);
                break;
            
            case 3:
                asignarTamanio(100*KB);
                break;

            case 4:
                asignarTamanio(1*MB);
                break;
            
            case 5:
                asignarTamanio(10*MB);
                break;
            
            case 6:
                asignarTamanio(100*MB);
                break;
            case 7:
                exit(EXIT_SUCCESS);
        }

        ejecucion();
    }
}
