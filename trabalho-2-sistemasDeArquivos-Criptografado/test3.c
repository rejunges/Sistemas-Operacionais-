#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOLEITURA1 50*BLOCO
#define TAMANHOLEITURA2 140*BLOCO
#define TAMANHOESCRITA1 1300*BLOCO
#define TAMANHOESCRITA2 130*BLOCO

void abre50Arquivos(cry_desc_t* descritor);
void escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void leTodosOsDescritores(cry_desc_t * descritor);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);

int main(){
	char *nome = "arquivo3", *bufferLeitura, *bufferLeitura2, *nomeArquivo, chaveArquivo;
	indice_arquivo_t indiceArquivo;
	cry_desc_t* descritor;
	uint32_t bytesLidos;
	int i, retorno, acessoArquivo;

	retorno = initfs(nome, 15000);
	assert(retorno != FALHA);
	printf("\nInicialização do arquivo hospedeiro: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");

	abre50Arquivos(descritor); //1 - Abre 50 arquivos
	printf("Abertura de 50 arquivos: OK\n");

	retorno = cry_close(100); //2 - Tenta fechar um arquivo que não existe
	assert(retorno != SUCCESSO);
	printf("Erro ao tentar fechar arquivo que não existe: OK\n");

	retorno = cry_delete(100); //3 - Tenta excluir um arquivo que não existe
	assert(retorno != SUCCESSO);
	printf("Erro ao tentar deletar arquivo que não existe: OK\n");

	for(i = 0; i < 10; i++){
		printf("%dª escrita sendo executada...\n", i+1);
		escreve(5, TAMANHOESCRITA1); //4 - Escreve 1300 blocos num arquivo 10 vezes seguidas
	}
	printf("Escrever 1300 blocos 10 vezes: OK\n");

	bufferLeitura = malloc(TAMANHOLEITURA1+1);
	bytesLidos = cry_read(5, TAMANHOLEITURA1, bufferLeitura); //5 - Lê 50 blocos do arquivo acima
	assert(bytesLidos == TAMANHOLEITURA1);
	printf("Ler 50 blocos: OK\n");
	printf("Se quiser visualizar o buffer descomente a linha 56\n");
	//printf("Buffer:\n%s\n", bufferLeitura);
	free(bufferLeitura);

	escreve(25, TAMANHOESCRITA2);	//6 - Escreve 130 blocos de dados em um segundo arquivo
	printf("Escrever 130 blocos: OK\n");

	bufferLeitura2 = malloc(TAMANHOLEITURA2+1);
	bytesLidos = cry_read(25, TAMANHOLEITURA2, bufferLeitura2); //7 - Tenta ler 140 blocos de dados do segundo arquivo
	assert(bytesLidos == TAMANHOESCRITA2); //lê até o final do arquivo
	printf("Tentar ler 140 blocos e ler efetivamente 130 pois é o tamanho do arquivo: OK\n");
	printf("Se quiser visualizar o buffer descomente a linha 67\n");
	//printf("Buffer:\n%s\n", bufferLeitura2);
	free(bufferLeitura2);

	nomeArquivo = malloc(sizeof(descritor->abertos[5].arquivo->nome)); //salva os atributos do arquivo antes de fechá-lo para poder abrí-lo novamente na instrução 10
	nomeArquivo = descritor->abertos[5].arquivo->nome;
	acessoArquivo = descritor->abertos[5].acesso;
	chaveArquivo = descritor->abertos[5].chave;
	retorno = cry_close(5); //8 - Fecha o primeiro arquivo
	assert(retorno != FALHA);
	printf("Fechar o primeiro arquivo: OK\n");

	abre50Arquivos(descritor); //9 - Cria mais 50 arquivos
	printf("Abertura de mair 50 arquivos: OK\n");

	indiceArquivo = cry_open(descritor, nomeArquivo, acessoArquivo, chaveArquivo); //10 - Abre o primeiro arquivo novamente
	assert(indiceArquivo != FALHA);
	printf("Abrir novamente o primeiro arquivo: OK\n");

	//leTodosOsDescritores(descritor); //Ve quais são os arquivos ainda no sistema
	//leTodosOsArquivosAbertos(descritor); //Ve quais arquivos estão abertos
	return 0;
}

void abre50Arquivos(cry_desc_t* descritor){
	char nomeArquivo[25], deslocamento; //arquivos com nomes de 25 caracteres
	int i, j;
	indice_arquivo_t indiceArquivo;

	for(i = 0; i < 50; i++){
		for(j = 0; j < 24; j++){
			nomeArquivo[j] = 65 + (rand() % 26);
		}
		nomeArquivo[j]='\0';
		deslocamento = rand() % 127;
		indiceArquivo = cry_open(descritor, nomeArquivo, LEITURAESCRITA, deslocamento);
		assert(indiceArquivo != FALHA);
	}
}

void escreve(indice_arquivo_t arquivo, uint32_t tamanho){
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
			printf("\nDescritor %d ocupado\n", i );
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
