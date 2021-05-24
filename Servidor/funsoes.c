#include "cabesalho.h"

// fç pa paga cache de execuson anterior
void limpaCache(){
    FILE *f;
    if ((f=fopen(".sistema/clientesON.txt", "r")) == NULL)
        printf("fichero ca existe...\n");
    else
    {
        system("rm .sistema/clientesON.txt");
        fclose(f);
        printf("cache de execuson anterior apagado...\n");
    }
    sleep(1);
}

// cria o abre fichero de msg
char *nm_caminho_fich(char *cltOrig, char *cltDest){
    char nome_1[80], nome_2[80], *fich;
    FILE *f;

    strcpy(nome_1, nome_fichero(cltOrig, cltDest));
    strcpy(nome_2, nome_fichero(cltDest, cltOrig));
    
    if ( (f=fopen(nome_1, "r")) != NULL)            // caso fichero dxa exist de primera...
    {
        fich = nome_1;
        printf("fichero [%s] aberto...1\n", fich);
    }
    else if ( (f=fopen(nome_2, "r")) != NULL)       // caso fichero existe cu nm aucontrario
    {
        fich = nome_2;
        printf("fichero [%s] aberto...2\n", fich);
    }
    else                                            // caso fichero ca existe nta crial
    {
        if ( (f=fopen(nome_1, "w")) != NULL)
        {
            fich = nome_1;
            printf("fichero criado - [%s]...\n", fich);
        }
    }
    fclose(f);
    return (fich);
}

// ta cria fichero cu nome introduzido na pasta ".mensagens"
char *nome_fichero(char *origem, char *destino){
    char nome[80] = { }, *nm;
    strcat(nome, ".mensagens/");
    strcat(nome, origem);
    strcat(nome, destino);
    strcat(nome, ".txt");

    nm = nome;

                    // // pa tra espaso na nome
                    // char text[100], nmFinal[100];
                    // int c=0, d=0;
                    // while (nm[c] != '\0')
                    // {
                    //     if (!(nm[c] == ' ' && nm[c+1] == ' ')) {
                    //         nmFinal[d] = nm[c];
                    //         d++;
                    //     }
                    //     c++;
                    // }
                    
                    // nmFinal[d] = '\0';
                    // printf("nome final: %s\n", nmFinal);
    
    return (nm);
}

// add cliente na fichero
void mas_um_cliente_on(char *nm){
    printf("add cliente na fichero tmp...\n");

    FILE *f;
    char nmArmazenado[32];

    if ((f=(fopen(".sistema/clientesON.txt","a+"))) != NULL)
    {
        bool s_n = false;
        while (fscanf(f,"%s",nmArmazenado) != -1) // ta procura kel nm s ka dxa sta aramazenado
        {
            if (strcmp(nm,nmArmazenado) == 0)
                s_n = true;            
        }
        if (!s_n) fprintf(f,"%s\n",nm);
        fclose(f);
    }
}

void erro(const char *msg) // mensagem de erro 
{
    perror(msg); // e ta interpreta erro(k ta ser numero), e ta mostra discrison
    exit(1);
}