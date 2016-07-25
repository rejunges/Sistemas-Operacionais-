#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOESCRITA1 1000*BLOCO
#define TAMANHOESCRITA2 2300*BLOCO

void abreArquivos(cry_desc_t* descritor, int numero);
int escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void deletaTodosArquivos(cry_desc_t* descritor);
void leTodosOsDescritores(cry_desc_t * descritor);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);

int main(){
	char *nome = "arquivo4";
	int i, quantidade;
	char c;
	indice_arquivo_t retorno;
	cry_desc_t *descritor;

	retorno = initfs(nome, 10); //1 - Cria um arquivo hospedeiro sem o tamanho mínimo para os metadados
	assert(retorno == FALHA);
	printf("\nErro na inicialização do arquivo hospedeiro sem tamanho mínimo: OK\n");

	retorno = initfs(nome, 3000); //2 - Criar um arquivo hospedeiro relaticamente pequeno - 3000 blocos
	assert(retorno != FALHA);
	printf("Inicialização do arquivo hospedeiro com 3000 blocos: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");

	abreArquivos(descritor, 200); //3 - Adicionar 200 arquivos
	printf("Abertura de 200 arquivos: OK\n");

	retorno = escreve(50, TAMANHOESCRITA1); //4 - Escrever 1000 blocos de dados em um arquivo
	assert(retorno != FALHA);
	printf("Escrita de 1000 blocos de dados: OK\n");

	abreArquivos(descritor, 50); //5 - Adicionar mais 50 arquivos
	printf("Abertura de mais 50 arquivos: OK\n");

	retorno = escreve(10, TAMANHOESCRITA2); //6 - Tentar escrever mais blocos que o possível
	assert(retorno != SUCCESSO);
	printf("Erro ao tentar escrever mais blocos que o possível: OK\n");

	retorno = cry_delete(50); //7 - Deletar o arquivo que foi escrito e tentar escrever novamente o número de blocos da instrução 6
	assert(retorno == SUCCESSO);
	escreve(10, TAMANHOESCRITA2);
	printf("Exclusão do que foi escrito previamente e segunda tentativa de escrita: OK\n");

	deletaTodosArquivos(descritor); //8 - Deletar todos os arquivos existentes no sistema
	leTodosOsDescritores(descritor); //Ve quais são os arquivos ainda no sistema(nenhum)
	leTodosOsArquivosAbertos(descritor); //Ve quais arquivos estão abertos(nenhum)
	printf("Exclusão de todos os arquivos restantes: OK\n");

	return 0;
}

void abreArquivos(cry_desc_t* descritor, int numero){
	char nomeArquivo[150], deslocamento; //arquivos com nomes de 150 caracteres
	int i, j;
	indice_arquivo_t indiceArquivo;

	for(i = 0; i < numero; i++){
		for(j = 0; j < 149; j++){
			nomeArquivo[j] = 65 + (rand() % 26);
		}
		nomeArquivo[j] = '\0';
		deslocamento = rand() % 127;
		indiceArquivo = cry_open(descritor, nomeArquivo, LEITURAESCRITA, deslocamento);
		assert(indiceArquivo != FALHA);
	}
}

int escreve(indice_arquivo_t arquivo, uint32_t tamanho){
	char *bufferEscrita;
	int i, retorno;
	char c;

	printf("Escrevendo...\n");

	bufferEscrita = malloc(tamanho);
	for(i = 0; i < tamanho; i++){
		c = 65 + (rand() % 26);
		bufferEscrita[i] = c;
	}
	bufferEscrita[i]='\0';
	retorno = cry_write(arquivo, tamanho, bufferEscrita);
	free(bufferEscrita);
	return retorno;
}

void deletaTodosArquivos(cry_desc_t* descritor){
	int i, retorno;

	for(i = 0; i < 256; i++){
		if(descritor->abertos[i].arquivo != NULL){
			retorno = cry_delete(i + 1);
			assert(retorno != FALHA);
		}
	}
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
			printf("\nDescritor %d ocupado\n", i + 1);
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