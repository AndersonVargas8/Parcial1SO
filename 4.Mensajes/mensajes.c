#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

/*struct sockaddr{
    unsigned short sa_family; // PF_*
    char sa_data[14] //Dirección del protocolo
};

struct in_addr{
    unsigned long s_addr; //4 bytes
};

struct sockaddr_in{
    short int sin_family; // PF_INET
    unsigned short sin_port; // Número del puerto
    struct in_addr sin_addr; // Dirección IP
    unsigned char sin_zero[8]; //Relleno
};*/


enum{KB = 1000, MB = 1000000};

int SIZE = 100*MB;

struct typeDato{
    char valor[KB];
};

void ejecucion(){

    int cpid = fork();

    if(cpid < 0){
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if(cpid == 0){
        struct typeDato dato;

        char buffer[KB];

        for(int i = 0; i < sizeof(struct typeDato)-1; i++){
            buffer[i] = 'a';
        }
        buffer[KB] = '\0';

        strcpy(dato.valor,buffer); 

        struct sockaddr_in server_addr;

        int sockfd = socket(PF_INET, SOCK_STREAM, 0);

        server_addr.sin_family = PF_INET;
        server_addr.sin_port = htons(1234);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if(sockfd < 0){
            perror("Error en el socket hijo");
        }   

        int addrlen = sizeof(struct sockaddr);

        connect(sockfd, (struct sockaddr *)&server_addr, addrlen);

        int n = SIZE/KB;

        int envio = 0;

        for(int i = 0; i < n; i++){
            envio += send(sockfd,dato.valor,strlen(dato.valor)+1,0);
        }

        if(SIZE >= MB)
            printf("Datos enviados: %iMB\n", envio/MB);
        else{
            printf("Datos enviados: %dKB\n", envio/KB);
        }

        close(sockfd);

    }else{
        clock_t tiempo1 = clock();
        struct typeDato dato;
        struct sockaddr_in server_addr;
        struct sockaddr_in remote_addr;

        int sockfd = socket(PF_INET, SOCK_STREAM, 0);

        server_addr.sin_family = PF_INET;
        server_addr.sin_port = htons(1234);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


        if(sockfd < 0){
            perror("Error en el socket padre");
        }

        int ret = bind(sockfd,(struct sockaddr *)&server_addr, sizeof(struct sockaddr));

        if(ret < 0){
            perror("Error en bind");
        }

        listen(sockfd,5);

        int addrlen = sizeof(struct sockaddr);

        int newSockfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addrlen);
        
        long recibido = recv(newSockfd,dato.valor, sizeof(struct typeDato), 0);

        long aux = recibido;
        while(aux > 0){
            aux = recv(newSockfd,dato.valor, sizeof(struct typeDato), 0); 
            recibido += aux;
        }
        //recibido += recv(newSockfd,dato.valor, SIZE, 0);

        clock_t tiempo2 = clock();

        double tiempo = (double)(tiempo2 - tiempo1)/CLOCKS_PER_SEC;
        if(SIZE >= MB)
            printf("Datos recibidos: %ldMB:  \n", recibido/MB);
        else{
            printf("Datos recibidos: %ldKB:  \n", recibido/KB);
        }

        printf("Tiempo de ejecución: %f segundos\n",tiempo);

        close(sockfd);
        close(newSockfd);


    }
    exit(EXIT_SUCCESS);
}

void asignarTamanio(int tamanio){
    SIZE = tamanio;
}

void main(){
    printf("BIENVENIDO. **COMUNICACIÓN MEDIANTE PASO DE MENSAJES**\n--------------------------------------------\n");
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