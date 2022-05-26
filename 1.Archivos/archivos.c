#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

enum{KB = 1000, MB = 1000000};
int SIZE = 100*MB;

int tiempo = 0;
int tiempo2 = 0;

struct typeDatoKB
{
    char valor[KB];
};


void crearArchivo()
{
    FILE *archivo;
    archivo=fopen("datos.dat","wb");
    if (archivo==NULL){
        perror("Error en el archivo");
        exit(-1);
    }
    fclose(archivo);
}
void guardarKB(struct typeDatoKB dato){
    FILE *archivo;
    archivo=fopen("datos.dat","ab");
    if (archivo==NULL){
        perror("Error en el archivo");
        exit(-1);
    }
    fwrite(&dato, sizeof(dato), 1, archivo);
    fclose(archivo);
}



void guardarDatos(){
        struct typeDatoKB dato;
        for(int i = 0; i < KB-1; i++)
            dato.valor[i] = 'a';
        dato.valor[KB] = '\0';
        
        int n = SIZE/sizeof(dato);
        for(int i = 0; i < n; i++){
            //printf("%lf\n",(double)i*100/n);
            guardarKB(dato);
        }
      
}

void escrito(int pid){
    //printf("SE HA TERMINADO DE ESCRIBIR\n");
    usleep(1000);
    kill(pid,SIGIO);
}

void leido(int pid){
    //printf("SE HA TERMINADO DE LEER\n");
    kill(pid,SIGIO);
    kill(getpid(),SIGKILL);
    exit(EXIT_SUCCESS);
}

void finalEjecucion(){
    clock_t time3 = clock();
    //printf("TIEMPO 1: %d------------------------------\n",tiempo2 - tiempo);
    printf("El tiempo final fue de: %lf\n",(double)((int)time3 - tiempo)/CLOCKS_PER_SEC);
    exit(EXIT_SUCCESS);
}

void listado()
{
    FILE *archivo;
    archivo=fopen("datos.dat","rb");

    if (archivo==NULL){
        perror("Error en el archivo");
        exit(-1);
    }

    struct typeDatoKB dato;
    fseek(archivo,0L,SEEK_END);
    int tam = (double)ftell(archivo);
    if(tam >= MB)
        printf("Tamaño de lectura: %d MB\n",tam/1000000);
    else
        printf("Tamaño de lectura: %d KB\n",tam/1000);
    fseek(archivo,0L,SEEK_SET);
    fread(&dato, sizeof(struct typeDatoKB), 1, archivo);
    while(!feof(archivo))
    {
        //printf("Dato %ld: %s\n", ftell(archivo)/sizeof(dato), dato.valor);
        fread(&dato, sizeof(struct typeDatoKB), 1, archivo);
    }
    fclose(archivo);
    leido(getppid());
}

void ejecucion(){
    printf("INICIO DE LA EJECUCIÓN:\n-----------------------------------\n");
    int cpid = fork();
    if(cpid < 0){
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if(cpid == 0){
        signal(SIGIO,&listado);
        int i = 0;
        while(i<1){
            sleep(1);
        }
    }else{
        signal(SIGIO,&finalEjecucion);
        clock_t time1 = clock();
        crearArchivo();
        guardarDatos();
        clock_t time2 = clock();
        tiempo = time1;
        tiempo2 = time2;
        escrito(cpid);
        int j=0;
        while(j<1){
            sleep(1);
        }
    }
    exit(EXIT_SUCCESS);
}

void asignarTamanio(int tamanio){
    SIZE = tamanio;
}

void main(){
    printf("BIENVENIDO. **COMUNICACIÓN MEDIANTE ARCHIVOS**\n--------------------------------------------\n");
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