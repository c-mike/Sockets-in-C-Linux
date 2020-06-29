#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read, write, close l k sta
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>
#include <sys/select.h>

#define STDIN 0

// conversa
void inicioConversa(int socketFD);
bool menuConversa(int socketFD);
bool login(int socketFD);
bool cadastro(int socketFD);
void manda_msg(int socketFd, char *msg, char *fichero);

// funcções de apoio
void limpaCache();
char *nm_caminho_fich(char *cltOrig, char *cltDest);
char *nome_fichero(char *origem, char *destino);
void mas_um_cliente_on(char *nm);
void erro(const char *msg);
