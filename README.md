# Socket-Em-C-Linux

  Um Socket full duplex em C Linux

# Compilação

  ## Servidor
    Dentro do diretorio(pasta) Servidor, no terminal execute:
  ### cc -Wall servidor.c conversa.c funsoes.c -o servidor

  ## Cliente
    Dentro do diretorio(pasta) Cliente, no terminal execute:
  ### cc cliente.c -o cliente

# Execução
  No terminal:

  ## Servidor
    O servidor recebe um parametro, numero da porta de conexão de preferencia maior que 5000:
  ### ./servidor 8888

  ## Cliente
    O cliente recebe dois parametros, numero de IP e o numero da porta, esta que deve ser igual com a do servidor:
  ### ./cliente 127.0.0.1 8888
    Isto é para o caso o cliente estiver na mesma maquina que o servidor.
