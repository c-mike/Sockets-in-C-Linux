#include "cabesalho.h"

char nmCliente[32], clt_Dest[32];
//******************************************* fç principal de conversa
void inicioConversa(int socketFD){
    printf("cliente logado...\n");

    bool volta;
MN: volta = menuConversa(socketFD);

    if (!volta) return;
    
    printf("cliente destino - %s\n",clt_Dest);
    if (clt_Dest != NULL)
    {
        char msg[255], fichero[80];
        FILE *f;
        strcpy(fichero, nm_caminho_fich(nmCliente, clt_Dest));

    // ta inicializa kes variavel de select
        fd_set copyFD, readFD;
        FD_ZERO(&copyFD);
        FD_ZERO(&readFD);
        FD_SET(STDIN, &readFD);
        FD_SET(socketFD, &readFD);
        struct timeval tm; //pan pd usa tmp

        printf("so na conversa...\n");
        while (1) {
            bzero(msg, 255);
            manda_msg(socketFD, msg, fichero);

            // inicializa select
            copyFD=readFD;
            tm.tv_sec= 4;
            tm.tv_usec = 4000000;
            
            // e ta retorna um valor se cliente manda msg ou 0 time-out
            // kel segundo parametro e de read, so el kin meste

            int slt = select(socketFD+1, &copyFD, NULL, NULL, &tm);
            if (slt == -1) // pa caso de da erro
                erro("erro na select...\n");
            
            else if (slt != 0) // caso cliente manda msg
            {
                bzero(msg, 255);
                // ta dxobe se valor de sockete sta na retorno de select
                if (FD_ISSET(socketFD, &copyFD)) // se soket e sta pronto
                {
                    // ta recebe msg
                    if (read(socketFD, msg, 255) < 0)
                        erro("erro na recebe msg de cliente...\n");
                        
                    if (strcmp(msg, "*") == 0)
                    {
                        printf("cliente dxa sai de conversa...\n");
                        goto MN;
                    } else if (strcmp(msg, ".") == 0)
                        return;

                    // ta guardal na fichero
                    if ((f=fopen(fichero,"a")) != NULL)
                        fprintf(f, "%s\n", msg);
                    else 
                        printf("erro na abre fichero pa guarda msg...\n");
                    
                    fclose(f);
                }
                // caso e sta vazio ta manda cliente msg
                //if (FD_ISSET(0, &copyFD)) // se enytrada/manda sta pronto
            }
        }
    }
}

//******************************************* fç pa manda msg armzenado na fichero
void manda_msg(int socketFD, char msg[], char *fichero){
    int i=0;
    FILE *f;

    // como servidor ca ta screve pa manda, e tem k ta abri fichero ntom:
    if ((f=fopen(fichero, "r")) != NULL)
    {
        while (fscanf(f, "%s", msg) != -1) i++;

        if (write(socketFD, &i, sizeof(i)) < 0)
            erro("erro na manda tamanho de fichero...1\n");

        if (i>0)
        {
            rewind(f); // 
            while (fscanf(f, "%s", msg) != -1){
                
                if ( write(socketFD, msg, 255) < 0)
                    erro("erro na manda msg k sta na fichero...\n");
            }
        }
    } else {
        if (write(socketFD, &i, sizeof(i)) < 0)
            erro("erro na manda tamanho de fichero...2\n");
        printf("erro na abri fichero pa le...\n");
    }
    fclose(f);
}

//******************************************* fç pa manda cliente lista de clientes
bool menuConversa(int socketFD){
    printf("so na manda menu...");

    int i=0;
    FILE *f;
    char nmClts[32];

    if ((f=fopen(".sistema/clientesON.txt","a+")) != NULL) // ta abre/cria kel fichero tmp li
    {
        while (fscanf(f,"%s",nmClts) != -1) i++;

        if (write(socketFD, &i, sizeof(i)) < 0)
            erro("ERRO!!!\n na manda tamanho de fichero tmp...\n");

        if (i>0)
        {
            rewind(f); // posiciona indicador de posison na inicio de arquivo pe torna le
            bzero(nmClts, 32);
            while (fscanf(f,"%s",nmClts) != -1)
            {
                if (write(socketFD, nmClts, 32) < 0)
                    erro("ERRO!!!\nna manda msg de fichero tmp...\n");
            }
            printf("Ok.\n");

            if (read(socketFD, &clt_Dest, 32) < 0)
                erro("ERRO!!!\nna recebe nm cliente dest...\n");

            if (strcmp(clt_Dest, ".") == 0)
                return false;
        }
    } else {
        if (write(socketFD, &i, i) < 0)
            erro("ERRO!!!\n na manda tamanho de fichero tmp_2...\n");
    }
    return true;
}

//******************************************* fç pa fz cadastro
bool cadastro(int socketFD){
    printf("so na cadastra cliente...\n");

    FILE *f;
    char user[70];
    bool s=false;

    if (read(socketFD, &user, 70) < 0){
        erro("erro na recebe info user na cadastro.\n");
    }
    if ((f=fopen(".sistema/login.txt","a+")) != NULL)
    {
        fprintf(f,"%s\n",user); // ta add cliente na fichero login.txt 
        s = true;
        
        strcpy(nmCliente, strtok(user,"-")); // copia de sugundo pa purmero
        mas_um_cliente_on(nmCliente);
        //insere(&cabesa, nmCliente);
    }
    fclose(f);
    if ( write(socketFD, &s, 1) < 0)
        erro("erro na confirma cadastro...\n");
        
    return s;
}

//******************************************* fç pa fz login
bool login(int socketFD){
    printf("so na faze login...\n");

    char user[70], usuario[50];
    int i=0;
    bool simNou = false;
    FILE *f;

M:  if (read(socketFD, &user, 70) < 0)
        erro("erro na recebe info user...\n");
    
    if ((f=fopen(".sistema/login.txt","r")) != NULL){
       
        while(fscanf(f,"%s",usuario) != -1) // ta faze um loop a procura de user 
        {
            if (strcmp(user,usuario) == 0){ // ta comparas a cd linha

                simNou = true;
                strcpy(nmCliente, strtok(user,"-")); // caso for vdd ta separal pa nu pd panha nm cliente
                mas_um_cliente_on(nmCliente);
            }
        }
    }
    if ( write(socketFD, &simNou, 1) < 0)
        erro("erro na confirma login...\n");
    
    if (!simNou) {
        if (i++ < 2)
        {
            bzero(user, 70);
            goto M; // caso sta erado e ta ripite mas 2 bes...
        }
    }
    return simNou;
}
