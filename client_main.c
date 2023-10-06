#include <stdio.h>
#include <stdlib.h>

#include "client_functions.h"

int main(int argc, char *argv[]) {
        if (argc != 3) {
                fprintf(stderr, "Uso: %s <IP> <PORTA>\n", argv[0]);
                exit(1);
        }

        char *ip = argv[1];
        int porta = atoi(argv[2]);

        iniciar_cliente(ip, porta);

        return 0;
}
