#include "cabesalho.h"

int main(int argc, char *argv[])
{
    system("clear");
    if(argc < 2) // ta verefica s dado porta de conecson
    {   fprintf(stderr, "ca dado porta de conecson\nPowerOff\n"); 
        exit(1);
    }
    int sockFd, novoSockFd, numPorta; // sockFd => socket file description
    
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);// 0 e padron de tcp
    if(sockFd < 0)
        erro("erro na abre socket_1.\n");
    else
        printf("socket criado cu valor %d...\n", sockFd);    

    bzero((char *) &serv_addr, sizeof(serv_addr)); // ta paga qualquer texto/lixo, deacordo cu nos refencia
    numPorta = atoi(argv[1]); // na posison 0=> file name, na 1=> porta

    serv_addr.sin_family = AF_INET; // significa me protoloco de internet
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(numPorta); // host to networt shot

    if(bind(sockFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        erro("falha na binding.\n");
    else
        printf("bind concluido...\n");
    
    limpaCache();

    listen(sockFd, 8); // 8 => maximo d cliente k pd sta ligado
    clilen = sizeof(cli_addr);// tamanho estrutura
    
    printf("servidor abertu na porta %s...\n",argv[1]);
    int op;
    while (1) // loop infinito pa pode trab cu fork
    {
        novoSockFd = accept(sockFd, (struct sockaddr *) &cli_addr, &clilen);
        if(novoSockFd < 0)
            erro("erro na aceitason.\n");
        else
            printf("cliente conectado...\n");       
        
        switch (fork())
        {
        // caso e ka cria processo
            case -1:
            {
                erro("erro na cria processo...\n");
                close(novoSockFd);
            }
            break;
        // processo criado cu sucesso
            case 0:
            {
                close(sockFd);
                do{
                    if (read(novoSockFd, &op, sizeof(int)) < 0) 
                        erro("erro na le opson...\n");
                    
                    switch(op)
                    {
                    // login
                        case 1:{
                            if (login(novoSockFd)) inicioConversa(novoSockFd);
                            else op = 0;
                        } break;
                    // cadastro
                        case 2: {
                            if (cadastro( novoSockFd)) inicioConversa(novoSockFd);
                            else op = 0;
                        } break;
                    }
                } while(op != 0);
            } break;
        }
        close(novoSockFd);
    }
    return 0;
}
