#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 1000
#define PORT 22 //puerto de SSH en realidad puede ser cualquier puerto
#define SA struct sockaddr

//Función encargada de la interacción entre el cliente y servidor
void intereccion(int sockfd)
{
    char buff[MAX]; //buffer que limita el tamaño de lo que se puede enviar al cliente 
    int n;
    for (;;) { //Ciclo para mantener la terminal operativa (infinito hasta encontrar la ruptura)
        bzero(buff, sizeof(buff)); //
        printf("Operación para el server : "); //Mensaje en espera del comando para el server
        n = 0;
        while ((buff[n++] = getchar()) != '\n'); //Captura de lo tecleado por el usuario
        
        write(sockfd, strcat(buff," > comandos.txt"), sizeof(buff));  //Se guarda el comando en el archivo comandos.txt y se envia al servidor para su ejecución
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff)); //se lee la respuesta provieniente del server
        printf("Respuesta desde el server : %s", buff); //Se imprime la respuesta del servidor

        //Logica para detener la conexion cliente-servidor
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Conexión cliente-servidor terminada\n");
            break;
        }
    }
}
   
int main(int argc, char *argv[])
{
    //Error mostrado en caso de no pasar argumentos para realizar la conexion
    if(argc != 3)
    {
        printf("\n Usa: %s <ip del server> <puerto> \n",argv[0]);
        return 1;
    } 

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    //Error creacion del socket fallo 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Creacion del socket fallo...\n");
        exit(0);
    }
    else //Notificación de que el socket se creo de forma correcta
        printf("El socket se creo correctamente..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    //Se establece la IP y el puerto
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));
   
    //Error de que la conexion con el servidor fallo
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Coneccion con el servidor fallo\n");
        exit(0);
    }
    else //Notificación de que la conexión con el servidor fue correcta
        printf("Conectado al servidor\n");
   
    //Llamado a la funcion de la interacción entre el cliente y el servidor
    intereccion(sockfd);
   
    //Se cierra el socket
    close(sockfd);
}