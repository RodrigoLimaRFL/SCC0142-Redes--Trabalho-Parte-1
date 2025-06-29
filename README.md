# SCC0142-Redes--Trabalho-Parte-1
Projeto feito para a disciplina SCC0142 - Redes de Computadores (2025) administrada pela professora Kalinka Regina Lucas Jaquie Castelo Branco.
## Integrantes
- Henrique Drago
- Henrique Sekido
- Rodrigo Lima
## Passo-a-Passo para Executar o código
### 1. Clonar o Repositório
Rode no terminal: 
```
git clone https://github.com/RodrigoLimaRFL/SCC0142-Redes--Trabalho-Parte-1.git
```
### 2. Executar o Programa
Abra a pasta do repositório no temrinal, pode-se utilizar o seguinte comando caso continue imediatamente do passo anterior:
```
cd SCC0142-Redes--Trabalho-Parte-1
```
Execute os seguintes comandos para iniciar a execução do programa:
```
make all
make run
```
## Utilização
### 1. Conexão
Ao executar o programa, a conexão será estabelecida imediatamente.

Os detalhes dos pacotes enviados e recebidos serão exibidos na tela, assim como intruções para o prosseguimento da utilização do programa.
### 2. Envio
Digitar "enviar" no terminal irá iniciar o procedimento de envio.

O programa irá imediatamente pedir que o usuário insira os dados. Após essa inserção, os dados serão enviados e os pacotes enviados e recebidos serão impressos na tela.

Caso necessário, os dados serão divididos em multiplos pacotes antes de serem enviados. Devido ao uso de threads, a impressão dos pacotes slow no terminal pode sair fragmentada nesse caso, porém o envio ocorre normalmente.

Enviar dados após o fim do tempo de sessão resultará em um erro, finalizando o programa.
### 3. Desconexão
Digitar "desconectar" no terminal irá iniciar o procedimento de desconexão.

Esse procedimento irá enviar um pedido de desconexão para o servidor, com ambos os pacotes enviado e recebido sendo imprimidos na tela.

Após a desconexão, será possível realizar um revive ou finalizar o programa.
### 4. Revive
Digitar "enviar" no terminal após uma desconexão irá iniciar o procedimento de revive.

Similar ao procedimento de envio padrão, o programa irá imediatamente pedir que o usuário insira os dados. Após a inserção, o procedimento ocorrerá normalmente assim como no procedimento de envio, exceto que dessa vez o pacote enviado terá flags diferentes sinalizando um revive.

Enviar dados após o fim do tempo de sessão ou o recebimento de uma rejeição (ao revive) do servidor resultará em um erro, finalizando o programa.
### 5. Finalização
Digitar "finalizar" no terminal após uma desconexão irá iniciar o procedimento de finalização.

Esse procedimento irá encerrar as threads, fechar a conexão do socket UDP e finalizar o programa.
