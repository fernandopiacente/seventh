#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server_functions.h"

int socket_servidor, socket_cliente = -1;
struct sockaddr_in servidor_addr, cliente_addr;
pthread_t thread_receber, thread_enviar;

void *receber_mensagens() {
        char mensagem[1000];
        while (1) {
                int bytes_recebidos = recv(socket_cliente, mensagem, sizeof(mensagem), 0);
                if (bytes_recebidos <= 0) {
                        printf("\nConexão encerrada pelo cliente.\n");
                        close(socket_cliente);
                        printf("Aguardando conexão...\n");
                        break;
                }
                mensagem[bytes_recebidos] = '\0';
                printf("\nMensagem do cliente: %s", mensagem);
                printf("Digite uma mensagem: ");
                fflush(stdout);
        }
        return NULL;
}

void *enviar_mensagens() {
        char mensagem[1000];
        while (1) {
                printf("Digite uma mensagem: ");
                fgets(mensagem, sizeof(mensagem), stdin);
                if (strcmp(mensagem, "#\n") == 0) {
                        printf("Encerrando conexão.\n");
                        send(socket_cliente, mensagem, strlen(mensagem), 0);
                        close(socket_cliente);
                        exit(0);
                }
                send(socket_cliente, mensagem, strlen(mensagem), 0);
        }
        return NULL;
}

void iniciar_servidor(int porta) {
        socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_servidor == -1) {
                perror("Erro ao criar socket");
                exit(1);
        }

        // Reusar porta para evitar erro de endereço em uso
        int reuse = 1;
        setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

        servidor_addr.sin_family = AF_INET;
        servidor_addr.sin_port = htons(porta);
        servidor_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(socket_servidor, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1) {
                perror("Erro ao fazer bind");
                exit(1);
        }

        if (listen(socket_servidor, 1) == -1) {
                perror("Erro ao escutar");
                exit(1);
        }

        printf("Aguardando conexão.\n");

        while (1) {
                socklen_t cliente_len = sizeof(cliente_addr);
                if (socket_cliente == -1) { // Se não houver cliente conectado
                        socket_cliente =
                            accept(socket_servidor, (struct sockaddr *)&cliente_addr, &cliente_len);
                        if (socket_cliente == -1) {
                                perror("Erro ao aceitar conexão");
                                exit(1);
                        }
                        printf("Conexão aceita.\n");
                        pthread_create(&thread_receber, NULL, receber_mensagens, NULL);
                        pthread_create(&thread_enviar, NULL, enviar_mensagens, NULL);
                }
                pthread_join(thread_receber, NULL);
                pthread_cancel(thread_enviar);
                close(socket_cliente);
                socket_cliente = -1;
        }

        close(socket_servidor);
}
