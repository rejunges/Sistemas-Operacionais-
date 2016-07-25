#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOLEITURA1 3000*BLOCO
#define TAMANHOLEITURA2 BLOCO/2
#define TAMANHOESCRITA1 500*BLOCO
#define TAMANHOESCRITA2 (3000*BLOCO)+(BLOCO/4) //Pegar um bloco 'quebrado'
#define TAMANHOESCRITA3 2000*BLOCO
#define TAMANHOESCRITA4 3*BLOCO + (BLOCO/3)

void escreveSemPermissao(indice_arquivo_t arquivo, uint32_t tamanho);
void escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);
void leTodosOsDescritores(cry_desc_t * descritor);
void abreArquivos(cry_desc_t* descritor, int numero, int acesso);

int main(){
	char *nome = "arquivo2", *bufferLeitura;
	cry_desc_t* descritor;
	uint32_t bytesLidos;
	int i, retorno;

	retorno = initfs(nome, 105000);
	assert(retorno != FALHA);
	printf("\nInicialização do arquivo hospedeiro: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");

	abreArquivos(descritor, 10, LEITURAESCRITA); //1 - Usar inicialmente uns 10 descritores
	printf("Abertura de 10 arquivos: OK\n");

	printf("Escrevendo 500 blocos no arquivo 5...\n");
	escreve(5, TAMANHOESCRITA1); //2 - Criar um arquivo de leitura e escrita, escrever uns 500 blocos de dados
	printf("Escrever 500 blocos em um arquivo de leitura e escrita: OK\n");

	abreArquivos(descritor, 75, LEITURAESCRITA); //3 - Abrir mais 75 arquivos
	printf("Abertura de mais 75 arquivos: OK\n");

	bufferLeitura = malloc(TAMANHOLEITURA1+1);
	bytesLidos = cry_read(5, TAMANHOLEITURA1, bufferLeitura); //3 - L ler o arquivo de leitura e escrita
	assert(bytesLidos != TAMANHOLEITURA1);
	printf("Tentar ler mais bytes do que o tamanho do arquivo e ler efetivamente o tamanho do arquivo: OK\n"); //Ele le o tamanho do arquivo
	printf("Se quiser visualizar o Buffer de leitura descomente a linha 51\n"); // 5-  Ler um dos arquivos adicionados que possui mais de 2 blocos de indices e mostrar buffer
	//printf("Buffer:\n%s\n", bufferLeitura);

	printf("Escrevendo mais 3000 blocos no arquivo 5...\n");
	escreve(5, TAMANHOESCRITA2); //3-Escrever mais 3000 blocos de dados
	printf("Escrever mais 3000 blocos em um arquivo de leitura e escrita: OK\n"); //3 - Abrir mais 75 arquivos, ler o arquivo de leitura e escrita e escrever mais 3000 blocos de dados

	abreArquivos(descritor, 30, ESCRITA); //4 - Criar arquivo de escrita
	printf("Abertura de mais 30 arquivos: OK\n");
	printf("Escrevendo 2000 blocos no arquivo 95...\n");
	escreve(95, TAMANHOESCRITA3);
	printf("Escrever 2000 blocos em um arquivo de escrita: OK\n");//4 - Escrever 2000 blocos de dados em um novo arquivo de escrita
	for(i = 0; i < 10; i++){
		escreve(5, TAMANHOESCRITA4);
		escreve(95, TAMANHOESCRITA4);
	}
	printf("Escrever mais alguns blocos no arquivo 5 intercalando com escrita de alguns blocos do arquivo 95: OK\n");//5 - Escrever no arquivo de leitura e escrita  e escrita intercaladamente
	bytesLidos = cry_read(5, TAMANHOLEITURA2, bufferLeitura); //6 - Ler
	assert(bytesLidos == TAMANHOLEITURA2);
	printf("Ler menos bytes do que o tamanho do arquivo: OK\n");
	printf("Se quiser visualizar o Buffer de leitura descomente a linha 71\n");
	//printf("Buffer:\n%s\n", bufferLeitura);

	retorno = cry_delete(5);
	assert(retorno == SUCCESSO);
	printf ("Deletar o arquivo 5: OK \n");//6 - Ler e depois deletar o arquivo de leitura e escrita.

	bytesLidos = cry_read(95, TAMANHOLEITURA2, bufferLeitura); //7 - Tentar ler o arquivo de escrita
	assert(bytesLidos == FALHA);
	printf("Erro ao tentar ler um arquivo de escrita: OK\n");

	free(bufferLeitura);
	//leTodosOsDescritores(descritor); //Ve quais são os arquivos ainda no sistema
	//leTodosOsArquivosAbertos(descritor); //Ve quais arquivos estão abertos
	return 0;
}

void abreArquivos(cry_desc_t* descritor, int numero, int acesso){
	char nomeArquivo[25], deslocamento; //arquivos com nomes de 25 caracteres
	int i, j;
	indice_arquivo_t indiceArquivo;

	for(i = 0; i < numero; i++){
		for(j = 0; j < 24; j++){
			nomeArquivo[j] = 65 + (rand() % 26);
		}
		nomeArquivo[j] = '\0';
		if(i == 100){
			indiceArquivo = cry_open(descritor, "arquivo100", acesso, 'R');
		}
		else{
			deslocamento = rand() % 127;
			indiceArquivo = cry_open(descritor, nomeArquivo, acesso, deslocamento);
		}
		assert(indiceArquivo != FALHA);
	}
}

void escreve(indice_arquivo_t arquivo, uint32_t tamanho){
	char *bufferEscrita;
	int i, retorno;
	char c;

	bufferEscrita = malloc(tamanho + 1);
	for(i = 0; i < tamanho; i++){
		c = 65 + (rand() % 26);
		bufferEscrita[i] = c;
	}
	bufferEscrita[i] = '\0';
	retorno = cry_write(arquivo, tamanho, bufferEscrita);
	assert(retorno != FALHA);
	free(bufferEscrita);
}

void escreveSemPermissao(indice_arquivo_t arquivo, uint32_t tamanho){
	char *bufferEscrita;
	int i, retorno;
	char c;

	bufferEscrita = malloc(tamanho);
	for(i = 0; i < tamanho; i++){
		c = 65 + (rand() % 26);
		bufferEscrita[i] = c;
	}
	bufferEscrita[i] = '\0';
	retorno = cry_write(arquivo, tamanho, bufferEscrita);
	assert(retorno == FALHA);
	free(bufferEscrita);
}

void leTodosOsDescritores(cry_desc_t * descritor){
	int i;
	char nomeZerado[256];
	for (i = 0; i < 256; i++){
		nomeZerado[i] = '0';
	}
	nomeZerado[i] = '\0';
	for(i = 0; i < 256; i++){
		if(strcmp(descritor->descritores[i].nome, nomeZerado)!=0){
			int contBlocoIndices = ceil((double)descritor->descritores[i].tamanho/(BLOCO*1023));
			if (contBlocoIndices == 0) contBlocoIndices =1;
			printf("\nDescritor %d ocupado\n", i);
			printf("Informações do escritor:\n");
			printf("Nome do arquivo:%s\n", descritor->descritores[i].nome);
			printf("Tempo:%ld\n", descritor->descritores[i].tempo);
			printf("O programa possui %d bloco de índices\n", contBlocoIndices);
			printf("1º bloco de índices:%d\n", descritor->descritores[i].indices);
			printf("Tamanho do arquivo:%d bytes\n", descritor->descritores[i].tamanho);
		}
	}
}

void leTodosOsArquivosAbertos(cry_desc_t * descritor){
	int i;
	for(i = 0; i < 256; i++){
		if(descritor->abertos[i].arquivo!= NULL){
			int contBlocoIndices = ceil((double)descritor->abertos[i].arquivo->tamanho/(BLOCO*1023));
			if (contBlocoIndices == 0) contBlocoIndices =1;
			printf("\nPosição %d da tabela de arquivos abertos\n", i + 1);
			printf("Informações desse arquivo aberto:\n");
			printf("Nome do arquivo:%s\n", descritor->abertos[i].arquivo->nome);
			printf("Tempo:%ld\n", descritor->abertos[i].arquivo->tempo);
			printf("O programa possui %d bloco de índices\n", contBlocoIndices);
			printf("1º bloco de índices:%d\n", descritor->abertos[i].arquivo->indices);
			printf("Tamanho do arquivo:%d bytes\n", descritor->abertos[i].arquivo->tamanho);
			if(descritor->abertos[i].acesso == 0) printf("Tipo de acesso: LEITURA\n");
			if(descritor->abertos[i].acesso == 1) printf("Tipo de acesso: ESCRITA\n");
			if(descritor->abertos[i].acesso == 2) printf("Tipo de acesso: LEITURA E ESCRITA\n");
			printf("Chave de deslocamento: %c\n", descritor->abertos[i].chave);
		}
	}
}