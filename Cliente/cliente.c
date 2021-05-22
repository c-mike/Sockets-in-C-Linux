#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read, write, close l k sta
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> // e ta dinife de host e struct
#include <ctype.h>
#include <stdbool.h>
#include <locale.h>
#include <sys/select.h> 

#define STDIN 0

// nd no

//******************************************* cabeçalho de funsons
bool menuConversa(int sockFd);
void conversa(int socketFd);
int menu();
void erro(const char *msg);
bool login(int sockFd);
bool cadastro(int sockFd);
void recebe_msg(int socketFd);
void kebra_linha(char *msg);
bool stado_cliente(char *nome);
void ajuda();

char cliente[20];

int main(int argc, char *argv[])
{
    int sockFd, numPorta, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; // ta guarda info d un host tp hostname

    if (argc < 3)
    {
        fprintf(stderr, "usando %s hostname porta\n", argv[0]);
        exit(1);
    }
    numPorta = atoi(argv[2]); // ta passal porta
    sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockFd < 0)
        erro("erro na abre socket.\n");

    server = gethostbyname(argv[1]); // ta passal ip
    if(server == NULL)
    {   fprintf(stderr, "erro ca tem host"); }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; // add dados na estrutura

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); // transfere informason
    serv_addr.sin_port = htons(numPorta); // add porta
    if(connect(sockFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        erro("falha na conecta.\n");

    //configura biblioteca locale 
    setlocale(LC_ALL, "Portuguese");

    // inicializa lib curses
    /*
        initscr();
        raw();
        noecho();
        keypad(stdscr, TRUE);
    */

    int op;
    do{
        
        op = menu();
        if (write(sockFd, &op, sizeof(int)) < 0)
            erro("erro na manda opson de menu.\n");
        
        switch(op) {
        // efetua login.
            case 1:{
                if (login(sockFd)) 
                    conversa(sockFd);
                else
                    { op = 0; }
            } break;
        // novo cadastro.
            case 2:{
                if (cadastro(sockFd)) 
                    conversa(sockFd);
                else
                    { op = 0; }
            } break;
            case 3: { ajuda(); } break;
        } 
    } while(op != 0);

    close(sockFd);
    return 0;
}

//******************************************* fç principal de concersa
void conversa(int socketFd){
    char msg[225], msg_final[255]={ }, op;

    bool volta;
MN: volta = menuConversa(socketFd);
    if (!volta) return;    

    // variavel select
    fd_set copyFD, readFD;
    FD_ZERO(&copyFD);
    FD_ZERO(&readFD);
    FD_SET(STDIN, &readFD);
    FD_SET(socketFd, &readFD);
    struct timeval tm;

    while(1)
    {
        bzero(msg, 225);
        recebe_msg(socketFd);

        // inicializa select
        copyFD=readFD;
        tm.tv_sec = 4; // 2 segundo
        tm.tv_usec = 4000000; // ... microsugundo

        printf("%s: ", cliente);
        int slt = select(socketFd+1, &copyFD, NULL, NULL, &tm);
        if (slt == -1) // pa casoo de erro
            erro("erro na select...\n");
        
        else if (slt != 0) // pa caso de nao time-out
        {
            // caso cliente cre manda msg
            //if (FD_ISSET(socketFd, &copyFD)) // ta recebe msg de servidor
            bzero(msg_final, 255);
            bzero(msg, 225);

            if (FD_ISSET(0, &copyFD)) // pa manda msg
            {
                fgets(msg, 225, stdin);
                if (strcmp(msg, "*") == 0 || strcmp(msg, ".") == 0)
                {
                    strcat(msg_final, msg);

                    if (write(socketFd, msg_final, 255) < 0)
                        erro("erro na manda msg pa sai...\n");

                    goto MN;
                }
                strcpy(msg_final, cliente);
                strcat(msg_final, ": ");
                strcat(msg_final, msg);
                strcat(msg_final, "\\n");

                if ( write(socketFd, msg_final, 255) < 0)
                    erro("erro na manda msg...\n");
            }
        }
    }
}

//******************************************* fç pa mostra cliente comandos
void ajuda(){
    system("clear");
    printf("\n\n");
    printf("[*] - Para sair de uma conversa com um amigo.\n");
    printf("[.] - Para ir para tela de login/Cadastro.\n");
    printf("[?] - Para pedir ajuda quando estas no menu da conversa.\n");
    printf("pressione un caracter para voltar!\n");
    getchar();
}

//******************************************* fç pa analisa pa dxobe s cliente sta ON
bool stado_cliente(char *nome){
    FILE *f;
    if ((f = fopen(".sistema/clientesON.txt", "r")) != NULL)
    {
        char nm_fich[32];
        while ((fscanf(f, "%s", nm_fich)) != -1)
        {
            if ((strcmp(nm_fich, nome)) == 0)
            {
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

//******************************************* fç pa recebe msg de servidor
void recebe_msg(int socketFd){
    int i=0;
    char msg[255];
    system("clear");

    // recebe tamanho de fichero
    if (read(socketFd, &i, sizeof(i)) < 0)
        erro("erro na recebe tamanho de fichero...\n");
    
    if (i>0)
    {
        char *n, nm_direita[32]={ };
        bzero(msg, 255);
        while (i-- > 0){
            if (read(socketFd, &msg, 255) < 0)
                erro("erro na recebe msg k sta na kel fichero...\n");
            else
            {
                if (strchr(msg, '\\') != NULL) // pa da kebra de linha na msg k ta bbem de fich
                {
                    n = strtok(msg, "\\");
                    printf("%s\n",n);
                }
                else // pa pui nm de cliente mas pa direita!
                    printf("%s ", msg);
            }
        }
    }
}

//******************************************* fç pa recebe lista de cliente on de servidor
// kel funson li ta cria menu de conversa de um det. cliente
bool menuConversa(int socketFD){
    int i=0;
    ajuda();
    system("clear");

    if (read(socketFD, &i, sizeof(i)) < 0)
        erro("erro na recebe tamanho fichero tmp...\n");

    if (i>0) {
        char nome[32];
        while (i-- != 0) // ciclo pa recebe lista de clientes
        {
            if (read(socketFD, &nome, 32) < 0)
                erro("erro na recebe msg de fichero...\n");
            else
            {
                if (!(strcmp(nome, cliente) == 0)) // pa cliente ca odxa se nome na lista de clienteON
                {
                    printf("-----------------------------\n");
                    printf("|\t%s\n", nome);
                }
            }
        }
        printf("-----------------------------\n\n");
        printf("Entre com nome do cliente para conversar!!\n");
        
        fgets(nome, 32, stdin);
        //scanf("%s",nome);
                                    // if (strcmp(nome, ".") == 0){
                                    //     break;
                                    // }
        
                                        // while (1)
                                        // {
                                        //     fgets(nome, 32, stdin);
                                        //     //scanf("%s",nome);
                                        //     if (strcmp(nome, ".") == 0){
                                        //         break;
                                        //     }

                                        //     // if (stado_cliente(nome)) 
                                        //     //     break;
                                        //     // else
                                        //     //     printf("Não tem niguem con esse nome!\n");
                                        // }
        if (write(socketFD, nome, 32) < 0)
            erro("erro na manda nm de destino!\n");
        
        if (strcmp(nome, ".") == 0) return false;
    } 
    else printf("ca tem niguem ON...\n");

    return true;
}

//******************************************* fç pa fz cadastro
bool cadastro(int sockFd){
    char user[70]={ }, nm[50], senha[20], confSenha[20];
    printf("Entre com o Nome :\n");
    scanf("%s", nm);

    int i=1;
L:  printf("Entre com a Senha :\n");
    scanf("%s", senha);

    printf("Confirma sua Senha :\n");
    scanf("%s", confSenha);

    if (strcmp(senha, confSenha) != 0)
    {
        printf("As senhas sao diferentes.\n");
        if (i++ == 3) return false;
        goto L;
    }
    strcat(user,nm);
    strcat(user,"-");
    strcat(user,senha);

    if (write(sockFd, user, 70) < 0)
        erro("envia info usuario.\n");
    
    bool simNou = false;
    if (read(sockFd, &simNou, 1) < 0)
        erro("erro na recebe conf. de login.\n");
    
    if (simNou) strcpy(cliente, nm);
    
    return simNou;
}

//******************************************* fç pa fase login
bool login(int sockFd){
    char user[70]={ }, nm[50], senha[20];
    int i=0;

M:  printf("Entre com o nome e senha:\n");
    scanf("%s %s", nm, senha);

    strcat(user,nm);
    strcat(user,"-");
    strcat(user,senha);

    if (write(sockFd, user, 70) < 0)
        erro("erro na envia info usuario.\n");
    
    bool simNou = false;
    if (read(sockFd, &simNou, 1) < 0)
        erro("erro na recebe conf. de login.\n");

    if (simNou)
        strcpy(cliente,nm);
    else {
        printf("Nome ou senha incoretos!\n");
        if (i++ < 3)
        {
            bzero(user, 70);
            goto M;
        }       
    }
    return simNou;
}

//******************************************* fç pa mostra menu logo na inicio
int menu(){
    system("clear");
    printf("\n\n\t***************************************\n");
    printf("\t--------                    -----------\n");
    printf("\t\t   Seja bem vindO\n\n\n");
    printf("\t[1] => Login.\n\t[2] => Cadastro.\n\t[3] => Ajuda\n\t[0] => Sair.\n");
    int op;
    scanf("%d", &op);
    return op;
}

//******************************************* fç pa interpreta erro
void erro(const char *msg) // mensagen de erro 
{
    perror(msg); // e ta interpreta erro(k ta ser numero), e ta mostra discrison
    exit(1);
}
