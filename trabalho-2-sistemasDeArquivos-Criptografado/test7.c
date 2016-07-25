#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOLEITURA1 20*BLOCO
#define TAMANHOESCRITA1 2*BLOCO
#define TAMANHOESCRITA2 BLOCO

void escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);
void leTodosOsDescritores(cry_desc_t * descritor);
void abreArquivos(cry_desc_t* descritor, int numero, int acesso);

int main(){
	char *nome = "arquivo7", *bufferLeitura;
	cry_desc_t* descritor;
	uint32_t bytesLidos;
	int i, retorno;

	retorno = initfs(nome, 200);
	assert(retorno != FALHA);
	printf("\nInicialização do arquivo hospedeiro: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");

	abreArquivos(descritor, 10, LEITURAESCRITA);
	printf("Abertura de 10 arquivos: OK\n");

	for(i = 0; i < 10; i++){
		escreve(1, TAMANHOESCRITA2); //escreve 1bloco
		escreve(2, TAMANHOESCRITA1); //escreve 2blocos
	}
	printf("Escrita intercalada entre arquivo 1(1 bloco por vez) e arquivo 2(2 blocos por vez): OK\n");
	bufferLeitura = malloc(TAMANHOLEITURA1+1);
	bytesLidos = cry_read(1, TAMANHOLEITURA1, bufferLeitura); 
	assert(bytesLidos == TAMANHOLEITURA1/2);
	printf("Ler 10 blocos do arquivo 1: OK\nDescomente a linha 44 para visualizar o buffer dessa leitura\n");
	//printf("Buffer arquivo 1:\n%s\n", bufferLeitura);

	bytesLidos = cry_read(2, TAMANHOLEITURA1, bufferLeitura); 
	assert(bytesLidos == TAMANHOLEITURA1);
	printf("Ler 20 blocos do arquivo 2: OK\nDescomente a linha 49 para visualizar o buffer dessa leitura\n");
	//printf("Buffer arquivo 2:\n%s\n", bufferLeitura);
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
		deslocamento = rand() % 127;
		indiceArquivo = cry_open(descritor, nomeArquivo, acesso, deslocamento);
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