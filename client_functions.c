#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_functions.h"

int socket_cliente;
struct sockaddr_in servidor_addr;
pthread_t thread_receber, thread_enviar;

void *receber_mensagens() {
        char mensagem[1000];
        while (1) {
                int bytes_recebidos = recv(socket_cliente, mensagem, sizeof(mensagem), 0);
                if (bytes_recebidos <= 0) {
                        printf("Conexão encerrada pelo servidor.\n");
                        close(socket_cliente);
                        printf("Tentando reconectar em 2 segundos...\n");
                        break;
                }
                mensagem[bytes_recebidos] = '\0';
                printf("\nMensagem do servidor: %s", mensagem);
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
                        printf("Encerrando programa...\n");
                        close(socket_cliente);
                        exit(0);
                }
                send(socket_cliente, mensagem, strlen(mensagem), 0);
        }
        return NULL;
}

void iniciar_cliente(char *ip, int porta) {
        while (1) {
                socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
                if (socket_cliente == -1) {
                        perror("Erro ao criar socket");
                        exit(1);
                }

                servidor_addr.sin_family = AF_INET;
                servidor_addr.sin_port = htons(porta);
                servidor_addr.sin_addr.s_addr = inet_addr(ip);

                if (connect(socket_cliente, (struct sockaddr *)&servidor_addr,
                            sizeof(servidor_addr)) == -1) {
                        perror("Erro ao conectar");
                        close(socket_cliente);
                        printf("Tentando reconectar em 2 segundos...\n");
                        sleep(2); // Espera 2 segundos antes de tentar novamente
                        continue; // Tenta novamente
                }

                pthread_create(&thread_receber, NULL, receber_mensagens, NULL);
                pthread_create(&thread_enviar, NULL, enviar_mensagens, NULL);

                pthread_join(thread_receber, NULL);
                pthread_cancel(thread_enviar);

                close(socket_cliente);
        }
}
