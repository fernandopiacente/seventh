#include <stdio.h>
#include <stdlib.h>

#include "server_functions.h"

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Uso: %s <PORTA>\n", argv[0]);
                exit(1);
        }

        int porta = atoi(argv[1]);
        
        iniciar_servidor(porta);

        return 0;
}
