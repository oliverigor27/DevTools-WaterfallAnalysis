#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_SIZE_BUFFER 1024
#define PORT 8080

int main()
{
    // Define o file descriptor do client e do server
    int server_fd, client_fd;

    // Define o endereço do socket
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // Define o buffer da aplicação onde o servidor recebe as requests.
    // Data vai ser movida do buffer de receiver do kernel do SO para aqui.
    char buffer[MAX_SIZE_BUFFER] = {0};

    // Cria o socket de conexão no kernel
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed!");
        exit(EXIT_FAILURE);
    }

    // Bind o socket
    address.sin_family = AF_INET; // IPV4
    address.sin_addr.s_addr = INADDR_ANY; // listen 0.0.0.0 interfaces
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed!");
        exit(EXIT_FAILURE);
    }

    // Cria as queues ( Receive, Send, Accept, SYN )
    // Espera por clientes com 10 de backlog (Apenas 10 conexões na queue de Accept)
    if(listen(server_fd, 10) < 0)
    {
        perror("Listen failed!");
        exit(EXIT_FAILURE);
    }

    // Loop infinito. A primeira coisa que ele faz é chamar o 'accept()'
    while(1)
    {
        printf("\nWaiting for connections...\n");

        // Aceita uma conexão do cliente (client_fd)
        // Uma operação bloqueante
        if((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0)
        {
            perror("Accept failure");
            exit(EXIT_FAILURE);
        }

        // Lê os dados que vieram do buffer de receiver do OS para o buffer da aplicação
        // Essencialmente, ele lê as requisições HTTP.
        read(client_fd, buffer, MAX_SIZE_BUFFER);
        printf("%s\n", buffer);


        // Nós enviamos a resposta escrevendo no socket e enviando para o send buffer no OS
        char *http_response = "HTTP/1.1 200 OK\n"
            "Content-Type: text/plain\n"
            "Content-Length: 13\n\n"
            "Hello World!\n";

        //Escreve no socket
        // send queue OS
        write(client_fd, http_response, strlen(http_response));

        // fecha a conexão! (TCP)
        close(client_fd);
    }

    return 0;
}