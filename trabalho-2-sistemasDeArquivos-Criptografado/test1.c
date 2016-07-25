#include <stdio.h>
#include <stdlib.h>
#include "crypfs.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHOLEITURA1 3000*BLOCO
#define TAMANHOESCRITA1 1500*BLOCO
#define TAMANHOESCRITA2 5125*BLOCO
#define TAMANHOESCRITA3 55*BLOCO

void abreArquivos(cry_desc_t* descritor, int numero);
void escreveSemBlocoLivre(indice_arquivo_t arquivo, uint32_t tamanho);
void escreve(indice_arquivo_t arquivo, uint32_t tamanho);
void leTodosOsArquivosAbertos(cry_desc_t * descritor);
void leTodosOsDescritores(cry_desc_t * descritor);

/*Inicialmente:
- 17 blocos de descritores
- 512 blocos ocupados quando todos os descritores estão sendo usados
- 1500 blocos para arquivo1(vai ter 2 blocos de índices)
- 5125 blocos para arquivo2(vai ter 6 blocos de índices)
- 2915 blocos para os demais 53 arquivos ( pois cada um dos arquivos vão ter 55 blocos de dados)
------------------
 10069 - 55 blocos de dados + 6 blocos de indices = 10020 blocos totalizados*/

int main(){
	char *nome = "arquivo1", *bufferLeitura;
	cry_desc_t* descritor;
	uint32_t bytesLidos;
	int i, retorno;

	retorno = initfs(nome, 10020);
	assert(retorno != FALHA);
	printf("\nInicialização do arquivo hospedeiro: OK\n");

	descritor = cry_openfs(nome);
	assert(descritor != NULL);
	printf("Abertura do arquivo hospedeiro: OK\n");
	
	leTodosOsDescritores(descritor);
	leTodosOsArquivosAbertos(descritor);

	abreArquivos(descritor, 256); //1 - Usa todos os descritores
	printf("Abertura de 256 arquivos: OK\n");

	printf("Escrevendo 1500 blocos...\n");
	escreve(150, TAMANHOESCRITA1); //Escreve os 1500 blocos para o arquivo 150
	printf("Escrever 1500 blocos: OK\n");

	printf("Escrevendo 5125 blocos...\n");
	escreve(101, TAMANHOESCRITA2); //Escreve os 5125 blocos para o arquivo 100
	printf("Escrever 5125 blocos: OK\n");

	printf("Escrevendo 55 blocos no arquivos 1 até o arquivo 53...\n");
	for(i = 1; i < 54; i++){
		escreve(i, TAMANHOESCRITA3); //Escreve os 55 blocos para o cada arquivo ( ao todo são 53 )
	}
	printf("Escrever 55 blocos de dados em 53 arquivos: OK\n"); // 1- Usado todos os blocos de memória com arquivos diferentes

	escreveSemBlocoLivre(80, BLOCO+1); // Escreve sem espaço livre, retorna falha
	printf("Não escreve quando não existe bloco livre para que a escrita seja realizada: OK\n"); //2- Tentar alocar mais um bloco de dados em algum arquivo sem ter memória

	retorno = cry_close(101); // 3- Fechar um arquivo que já foi escrito
	assert(retorno == SUCCESSO);
	//leTodosOsDescritores(descritor); //Nesse ponto ele mostra que o 100está nos descritores
	//leTodosOsArquivosAbertos(descritor); //Nesse ponto ele mostra que o 100 nao está nos arquivos abertos
	
	retorno = cry_open(descritor, "arquivo100", LEITURAESCRITA, 'R'); // 3- Abri-lo novamente
	assert(retorno == 101);
	printf("Fechar um arquivo que já foi escrito e abri-lo novamente: OK\n");

	retorno = cry_delete(150); //4 - Deletar um arquivo
	assert(retorno == SUCCESSO);
	abreArquivos(descritor, 1); //4 - Adicionar outro arquivo de tamanho menor
	escreve(150, TAMANHOESCRITA3);
	printf ("Deletar um arquivo e adicionar outro de tamanho menor: OK \n");

	bufferLeitura = malloc(TAMANHOLEITURA1+1);
	bytesLidos = cry_read(101, TAMANHOLEITURA1, bufferLeitura); //5 - Lê 3000 blocos do arquivo de 5125
	assert(bytesLidos == TAMANHOLEITURA1);
	printf("Ler 3000 blocos: OK\n");

	printf("Se quiser visualizar o Buffer de leitura descomente a linha 87\n"); // 5-  Ler um dos arquivos adicionados que possui mais de 2 blocos de indices e mostrar buffer
	//printf("Buffer:\n%s\n", bufferLeitura);
	free(bufferLeitura);

	retorno = cry_open(descritor, "arquivo256", ESCRITA, 'A');
	assert(retorno == FALHA);
	printf ("Erro ao tentar criar um novo arquivo: OK\n");

	return 0;
}

void abreArquivos(cry_desc_t* descritor, int numero){
	char nomeArquivo[25], deslocamento; //arquivos com nomes de 25 caracteres
	int i, j;
	indice_arquivo_t indiceArquivo;

	for(i = 0; i < numero; i++){
		for(j = 0; j < 24; j++){
			nomeArquivo[j] = 65 + (rand() % 26);
		}
		nomeArquivo[j] = '\0';
		if(i == 100){
			indiceArquivo = cry_open(descritor, "arquivo100", LEITURAESCRITA, 'R');
		}
		else{
			deslocamento = rand() % 127;
			indiceArquivo = cry_open(descritor, nomeArquivo, LEITURAESCRITA, deslocamento);
		}
		assert(indiceArquivo != FALHA);
	}
}

void escreveSemBlocoLivre(indice_arquivo_t arquivo, uint32_t tamanho){
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
