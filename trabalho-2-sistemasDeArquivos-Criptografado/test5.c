#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOESCRITA1 47000*BLOCO
#define TAMANHOESCRITA2  5000*BLOCO

void escreveSemBlocoLivre(indice_arquivo_t arquivo, uint32_t tamanho);
void escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);
void leTodosOsDescritores(cry_desc_t * descritor);

int main(){
	char *nome = "arquivo5", *bufferLeitura;
	cry_desc_t* descritor;
	uint32_t bytesLidos;
	int i, retorno;

	retorno = initfs(nome, -1);
	assert(retorno == FALHA);
	printf("\nErro na inicialização com tamanho de bloco negativo: OK\n");

	retorno = initfs(nome, 50000);
	assert(retorno != FALHA);
	printf("Inicialização do arquivo hospedeiro com 50000 blocos: OK\n");

	descritor = cry_openfs("arquivo");
	assert(descritor == NULL);
	printf("Erro ao abrir um arquivo hospedeiro inexistente: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");
	
	leTodosOsDescritores(descritor);
	leTodosOsArquivosAbertos(descritor);

	retorno = cry_open(descritor, "arquivo1", LEITURAESCRITA, 'a');
	assert(retorno != FALHA);
	printf("Abertura de 1 arquivo: OK\n");

	printf("Escrevendo 47000 blocos...\n");
	escreve(1, TAMANHOESCRITA1); 
	printf("Escrever 47000 blocos: OK\n");

	retorno = cry_close(1); 
	assert(retorno == SUCCESSO);
	leTodosOsDescritores(descritor); 
	leTodosOsArquivosAbertos(descritor); 
	
	//Tenta excluir um arquivo que não está aberto
	retorno = cry_delete(1);
	assert(retorno == FALHA);
	printf("Erro ao tentar excluir um arquivo que não está aberto: OK\n");

	retorno = cry_open(descritor, "arquivo1", LEITURAESCRITA, 'a');
	assert(retorno != FALHA);
	escreveSemBlocoLivre(1, TAMANHOESCRITA2);
	printf("Erro ao tentar escrever mais que blocos do que existe: OK\n");

	printf("Descritores e tabela de arquivos abertos do sistema atual:\n");
	leTodosOsDescritores(descritor);
	leTodosOsArquivosAbertos(descritor);

	return 0;
}

void escreveSemBlocoLivre(indice_arquivo_t arquivo, uint32_t tamanho){
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
	assert(retorno == FALHA);
	free(bufferEscrita);
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
