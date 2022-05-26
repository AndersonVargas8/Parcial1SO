#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

enum{KB = 1000, MB = 1000000};
int SIZE = 100*MB;

struct typeDato
{
    char valor[KB];
};



void ejecucion(){
    int cpid = fork();

    if(cpid < 0){
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if(cpid == 0){
        key_t key;
        int shmId;
        struct typeDato *dato;

        key = ftok(".",'c');

        shmId = shmget(key,SIZE,0);

        if(shmId < 0){
            perror("Error en shmget");
            exit(EXIT_FAILURE);
        }

        dato = shmat(shmId,0,0);

        if(dato < 0){
            perror("Error en shmmat");
            exit(EXIT_FAILURE);
        }
        //printf("Tamaño de lectura: %ld\n%s\n",strlen((dato+0)->valor),(dato+0)->valor);

        int n = SIZE/KB;
        long recibido = 0;

        for(int i = 0; i < n; i++){
            recibido += sizeof((dato+i)->valor);
            //printf("Tamaño de lectura: %ld\n%s\n",strlen((dato+i)->valor),(dato+i)->valor);
        }
        if(SIZE < MB){
            printf("Tamaño de lectura: %ld KB\n",recibido/KB);
        }else{
            printf("Tamaño de lectura: %ld MB\n",recibido/MB);
        }
        exit(EXIT_SUCCESS);
    }else{
        key_t key;
        int shmId,status;
        struct typeDato *dato;

        key = ftok(".",'c');

        shmId = shmget(key,SIZE,0666 |IPC_CREAT);

        if(shmId < 0){
            perror("Error en shmget");
            exit(EXIT_FAILURE);
        }

        dato = shmat(shmId,0,0);

        if(dato < 0){
            perror("Error en shmat");
            exit(EXIT_FAILURE);
        }

        char data[KB];
        for(int i = 0; i < sizeof(struct typeDato)-1; i++){
            data[i] = 'a';
        }
        data[sizeof(struct typeDato)] = '\0';

        int n = SIZE/KB;

        clock_t tiempo1 = clock();

        for(int i = 0; i < n; i++){
            strcpy((dato+i)->valor,data);
        }

        if(wait(&status) == cpid){
        };

        clock_t tiempo2 = clock();
        float tiempo = (float)(tiempo2 - tiempo1)/CLOCKS_PER_SEC;
        if(WEXITSTATUS(status) == 0){
            printf("Tiempo de ejecución: %lf segundos\n",tiempo);
            
            shmdt(dato);
            shmctl(shmId, IPC_RMID, 0);
        }
    }

    
    exit(EXIT_SUCCESS);

    clock_t tiempo2 = clock();


}

void asignarTamanio(int tamanio){
    SIZE = tamanio;
}

void main(){
    printf("BIENVENIDO. **COMUNICACIÓN MEDIANTE MEMORIA COMPARTIDA**\n--------------------------------------------\n");
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