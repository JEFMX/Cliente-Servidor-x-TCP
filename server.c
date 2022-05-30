#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX 1000
#define PORT 22
#define SA struct sockaddr
   
// Función encargada de la interacción entre el cliente y servidor
void intereccion(int connfd)
{
    char buff[MAX];
    int n;
    const char* filename = "comandos.txt";
    //Ciclo para mantener la terminal operativa (infinito hasta encontrar la ruptura)
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff)); //lectura del mensaje enviado por el cliente
        
        char *cmd = buff;
        char buf[BUFSIZ];
        FILE *ptr, *file;

        //se abre el archivo que envio el cliente hacia el servidor y se hace la lectura del comando para su ejecución
        file = fopen("comandos.txt", "w");
        if (!file) abort();
           if ((ptr = popen(cmd, "r")) != NULL) {
                   while (fgets(buf, BUFSIZ, ptr) != NULL)
                           fprintf(file, "%s", buf);
                   pclose(ptr);
           }
        fclose(file); //se cierra el archivo

        //errores de lectura del archivo
        FILE* input_file = fopen(filename, "r");
        if (!input_file)
            exit(EXIT_FAILURE);

        struct stat sb;
        if (stat(filename, &sb) == -1) {
            perror("stat fallo");
            exit(EXIT_FAILURE);
        }

        char* file_contents = malloc(sb.st_size);
        fread(file_contents, sb.st_size, 1, input_file);

        printf("%s\n", file_contents);

        write(connfd, file_contents, sizeof(buff));

        fclose(input_file);
        free(file_contents);
   
        //Se termina la conexion debido al comando recibido
        if (strncmp("exit", buff, 4) == 0) {
            printf("Conexión cliente-servidor terminada\n");
            break;
        }
    }
}
   

int main()
{
    int sockfd, connfd, len;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    //Error de que la conexion con el servidor fallo
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Coneccion con el servidor fallo\n");
        exit(0);
    }
    else //Notificación de que la conexión con el servidor fue correcta
        printf("Socket successfully binded..\n");
   
    // Error de la escucha 
    if ((listen(sockfd, 5)) != 0) {
        printf("En escucha fallo\n");
        exit(0);
    }
    else //Servidor en escucha o espera de una conexion
        printf("Servidor en escucha...\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("La aceptacion del servidor fallo...\n");
        exit(0);
    }
    else
        printf("El servidor acepto la conexion\n");
   
    //Llamado a la funcion de la interacción entre el cliente y el servidor
    intereccion(connfd);
   
    //Se cierra el socket
    close(sockfd);
}