/*Renata Junges e Yan Soares */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "crypfs.h"

//Leitura = 0
//Escrita = 1
//Leitura e Escrita = 2
//Vazio = -1
#define INDICES (int)((BLOCO/sizeof(int))-1) // pode ter 1023 blocos de dados dentro de um bloco de índice

typedef struct bloco{ //estrutura descritora de cada bloco
	int tipo; //-1 = vazio 0 = descritor 1 = bloco de indice 2 = bloco de dados
	int primeiro; //aponta para o início do bloco (primeiro byte)
	int ultimo; //aponta para o primeiro byte da ultima estrutura alocada nesse bloco (pode ser um descritor, dado ou índice)
	int limite; //limite de 4k de cada bloco
} bloco_t;

typedef struct blocovazio{ //estrutura que representa um bloco vazio, para ocupar o espaço livre restante do sistema de arquivos ao inicializar
	char n[4096];
} blocovazio_t;

int nblocos; //número de blocos totais do sistema de arquivo
int blocoslivres; //número de blocos livres do sistema de arquivo
bloco_t *bloco; //ponteiros para as estruturas de descritor dos blocos e descritor do sistema
cry_desc_t *pdescritor;

void limpaBloco(int blocos, FILE *hospedeiro);
void inicializaBlocos(int inicial, int final);
void limpaDescritor(descritor_t *descritor);
int procuraBlocoLivre();
int procuraDescritorLivre();
int procuraIndiceLivreTA(); //indice livre na tabela de abertos
int indiceNaTA(char *nome);
int indiceNosDescritores(char *nome);
void escreveBuffer(char buffer);
char criptografa(char byte, char chave);
char descriptografa(char byte, char chave);
int podeEscrever(int tamanho, int blocoDados, int blocoIndices); //retorna 1 se o tamanho da escrita for suportado (de acordo com o número de blocos livres), ou 0 caso não houver espaço suficiente

int initfs(char * arquivo, int blocos){
	int i, minblocos = (int)((sizeof(descritor_t)*256)/BLOCO); //número mínimo de blocos para conter pelo menos os descritores
	if(arquivo == "" || arquivo == "\0") return FALHA; //O arquivo precisa ter nome
	if(blocos >= minblocos){ //confere se tem o número mínimo de blocos para conter pelo menos os descritores
		FILE *arquivohospedeiro;
		descritor_t *descritor;
		int i, j, k;
		bloco = (bloco_t*) malloc(sizeof(bloco_t)*blocos);
		inicializaBlocos(0, blocos);

		if ((arquivohospedeiro = fopen(arquivo, "r")) == NULL){ //testa se arquivo já existe, se não existir:
			arquivohospedeiro = fopen(arquivo, "w+"); //cria um arquivo binário para leitura e escrita (vai ser o arquivo hospedeiro, com todos os blocos)

			if(!arquivohospedeiro)	return FALHA; //caso não tenha sido possível criar o arquivo, retorna falha
			pdescritor = (cry_desc_t*) malloc(sizeof(cry_desc_t));

			for(i = 0; i < minblocos; i++) bloco[i].tipo = 0; //altera os primeiros blocos para tipo descritores

			fseek(arquivohospedeiro, 0, SEEK_SET); //aponta para o início do arquivo para preencher com os 256 descritores

			descritor = (descritor_t*) malloc(sizeof(descritor_t));
			limpaDescritor(descritor);
			for(j = 0; j < 256; j++){
				//!!!!!!!CONFERIR DEPOIS SE NAO PRECISA DE CONVERSAO
				fwrite(&descritor->nome, sizeof(char)*256, 1, arquivohospedeiro);
				fwrite(&descritor->tempo, sizeof(time_t), 1, arquivohospedeiro);
				fwrite(&descritor->indices, sizeof(uint32_t), 1, arquivohospedeiro);
				fwrite(&descritor->tamanho, sizeof(uint32_t), 1, arquivohospedeiro);
			}
			free(descritor);
			limpaBloco((blocos-i), arquivohospedeiro); //preenche o sistema de arquivo com os blocos vazios que virão a ser de índices/dados
			fclose(arquivohospedeiro);
			nblocos = blocos;
			blocoslivres = nblocos-minblocos;
			return SUCCESSO;
		}
		return FALHA; //arquivo já existe, retorna falha (0)
	}
	return FALHA; //caso não tenha o número mínimo de blocos, retorna falha (0)
}

cry_desc_t * cry_openfs(char * arquivo){
	FILE *arquivohospedeiro;
	int i;
	pdescritor = (cry_desc_t*) malloc(sizeof(cry_desc_t));

	if((arquivohospedeiro = fopen(arquivo, "r+")) == NULL) return NULL;

	pdescritor->arquivo_host = arquivohospedeiro; //linka a estrutura "descritora" do sistema com o arquivo hospedeiro

	for(i=0; i<256; i++){
		limpaDescritor(&pdescritor->descritores[i]);
		pdescritor->abertos[i].arquivo = NULL; //inicializa a tabela de arquivos abertos do descritor relativo àquele sistema
		pdescritor->abertos[i].acesso = -1;
		pdescritor->abertos[i].chave = '\0'; //ainda não tem a chave (passada na função cry_open para cada arquivo aberto)
	}

	return pdescritor;
}

/*tem que retornar -1 no caso de falha, pois o indice_arquivo pode ser 0 (que é FALHA) E NULL dá warning (mesmo com cast)*/
indice_arquivo_t cry_open(cry_desc_t *cry_desc, char * nome, int acesso, char deslocamento){
	int blocoLivreIndice, blocoLivreDados, descritorLivre, indiceTabelaAbertos, k;
	time_t tempo;

	if(cry_desc == NULL || nome == "" || nome == "\0") return FALHA;
	if(acesso == LEITURA || acesso == ESCRITA || acesso == LEITURAESCRITA){
		indiceTabelaAbertos = indiceNaTA(nome); //Verifica se o arquivo já está aberto
		if(indiceTabelaAbertos != -1){
			pdescritor->abertos[indiceTabelaAbertos].acesso = acesso;
			return (indiceTabelaAbertos + 1); //O arquivo já está na tabela de abertos
		}

		//Verifica se o arquivo existe mas foi fechado, e quer ser aberto de novo
		descritorLivre = indiceNosDescritores(nome);
		if(descritorLivre != -1){ // O arquivo existe no sistema de arquivos, porém foi fechado em outro momento
			indiceTabelaAbertos = procuraIndiceLivreTA();
			if(indiceTabelaAbertos == -1)	return FALHA; //não há como abrir mais arquivos no sistema
			pdescritor->abertos[indiceTabelaAbertos].arquivo = &cry_desc->descritores[descritorLivre];
			pdescritor->abertos[indiceTabelaAbertos].acesso = acesso;
			pdescritor->abertos[indiceTabelaAbertos].chave = deslocamento;
			return (indiceTabelaAbertos + 1);
		}
		if(acesso == LEITURA){
			return FALHA; // Se ele não está nos descritores e nem na tabela de abertos, eu nao posso criar um arquivo que seja pra leitura
		}
		blocoLivreIndice = procuraBlocoLivre();
		if(blocoLivreIndice == -1)	return FALHA; //não há mais blocos livres
		bloco[blocoLivreIndice].tipo = 1; //se torna um bloco de índices

		blocoLivreDados = procuraBlocoLivre();
		if(blocoLivreDados == -1){	//há espaço para o bloco de índices, mas nenhum de dados
			bloco[blocoLivreIndice].tipo = -1; //desaloca o bloco de índices
			return FALHA;
		}
		bloco[blocoLivreDados].tipo = 2; //aloca como bloco de dados

		descritorLivre = procuraDescritorLivre();
		if(descritorLivre == -1){ //não há descritores livres para receber o arquivo
			bloco[blocoLivreIndice].tipo = -1; //desaloca o bloco de índices
			bloco[blocoLivreDados].tipo = -1; // dasaloca o bloco de dados
			return FALHA;
		}
		cry_desc->descritores[descritorLivre].indices = blocoLivreIndice; //linka o descritor do arquivo com seu bloco de índices

		indiceTabelaAbertos = procuraIndiceLivreTA();
		if(indiceTabelaAbertos == -1){ //não há espaço livre na tabela de arquivos abertos para alocar esse arquivo
			bloco[blocoLivreIndice].tipo = -1; //desaloca o bloco de índices
			bloco[blocoLivreDados].tipo = -1; // dasaloca o bloco de dados
			cry_desc->descritores[descritorLivre].indices = 0; //desaloca o descritor livre
			return FALHA;
		}
		//Se não retornou até o momento, entao pode gravar as informações agora
		fseek(cry_desc->arquivo_host, bloco[blocoLivreIndice].primeiro, SEEK_SET);
		fwrite (&blocoLivreDados, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
		fflush(pdescritor->arquivo_host);
		bloco[blocoLivreIndice].ultimo+= sizeof(indice_arquivo_t); //O ultimo aponta para o lugar que vai ser inserido o proximo indice
		strcpy(cry_desc->descritores[descritorLivre].nome, nome);
		pdescritor->abertos[indiceTabelaAbertos].arquivo = &cry_desc->descritores[descritorLivre];
		pdescritor->abertos[indiceTabelaAbertos].acesso = acesso; //insere o tipo de acesso do arquivo (leitura, escrita ou leitura/escrita)
		pdescritor->abertos[indiceTabelaAbertos].chave = deslocamento;
		tempo = time(NULL);
		cry_desc->descritores[descritorLivre].tempo = tempo;
		blocoslivres-= 2; //Alocou bloco de indices e bloco de dados, então perdeu 2 blocos livres
		return (indiceTabelaAbertos + 1);
	}
	return FALHA;
}

int cry_close(indice_arquivo_t arquivo){ //como só fecha o arquivo, não o apaga, só precisa tirar as referências que há a ele na tabela de arquivos abertos
	arquivo--;
	if(arquivo < 256 && arquivo >= 0 && pdescritor->abertos[arquivo].arquivo != NULL){ //índice de arquivo passado existe de fato
		pdescritor->abertos[arquivo].arquivo = NULL;
		pdescritor->abertos[arquivo].acesso = -1;
		pdescritor->abertos[arquivo].chave = '\0';
		return SUCCESSO;
	}

	return FALHA; //o arquivo que está querendo fechar não está aberto na tabela de arquivos abertos
}

uint32_t cry_read(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){
 	int contIndices, retorno, blocoDados, blocoIndices, j = 0, k = 0, tamanhoAuxiliar;
 	uint32_t bytesLidos = 0;
	char byteLido;
 	descritor_t *descritor;
 	arquivo--;

 	if(arquivo > 255 || arquivo < 0 || pdescritor->abertos[arquivo].arquivo == NULL) return bytesLidos;

 	tamanhoAuxiliar = tamanho;
 	descritor = pdescritor->abertos[arquivo].arquivo;
 	if(descritor->tamanho == 0)	return bytesLidos; //Nao vai conseguir ler nenhum, retorna 0
	if(pdescritor->abertos[arquivo].acesso != ESCRITA){ //caso tenha permissão para leitura
	 	blocoIndices = descritor->indices; //Qual o primeiro bloco de indices que vai começar a ler
	 	if(tamanho > descritor->tamanho){ //caso seja passado um tamanho de leitura maior que o tamanho do próprio arquivo, lerá até o final do mesmo
	 		tamanhoAuxiliar = descritor->tamanho;
	 	}

	 	while(bytesLidos < tamanhoAuxiliar){ //a variável "j" é responsável pelo deslocamento dentro do bloco
	 		if((bloco[blocoIndices].primeiro + j) == (bloco[blocoIndices].limite - sizeof(indice_arquivo_t))){
	 			//Vai para o proximo bloco de indices
	 			fseek(pdescritor->arquivo_host, (bloco[blocoIndices].ultimo - sizeof(indice_arquivo_t)), SEEK_SET);
	 			retorno = fread(&blocoIndices, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);//pega qual o novo bloco de índices (apontado pelo índice final do bloco atual)
	 			j = 0;
	 		}
	 		fseek(pdescritor->arquivo_host, bloco[blocoIndices].primeiro + j, SEEK_SET);
	 		retorno = fread(&blocoDados, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
	 		while((bloco[blocoDados].primeiro + k) < bloco[blocoDados].limite && bytesLidos < tamanhoAuxiliar){ //"k" é o deslocamento da leitura dentro de cada bloco de dados
	 			fseek(pdescritor->arquivo_host, bloco[blocoDados].primeiro + k, SEEK_SET);
				retorno = fread(&byteLido, sizeof(char), 1, pdescritor->arquivo_host);
				byteLido = descriptografa(byteLido, pdescritor->abertos[arquivo].chave);
				buffer[bytesLidos] = byteLido;
	 			bytesLidos++;
	 			k++;
	 		}
	 		k = 0;
	 		j+= sizeof(indice_arquivo_t);
	 	}
	 	buffer[bytesLidos] = '\0';
	}
	return bytesLidos; //Se não tem permissão pra leitura, retorna 0, senão retorna o número de bytes lidos mesmo

}

int cry_write(indice_arquivo_t arquivo, uint32_t tamanho, char *buffer){ //se não houver mais espaço livre, escreve até acabar o espaço (não apaga o que já foi escrito)
	int contBlocoIndices, blocoDados, blocoIndices, retorno;
	uint32_t bytesEscritos = 0;
	char byteEscrito;
	time_t tempo;

	arquivo--;
	descritor_t *descritor = pdescritor->abertos[arquivo].arquivo;
	if(arquivo > 255 || arquivo < 0 || descritor == NULL) return FALHA;

	if(pdescritor->abertos[arquivo].acesso != LEITURA){ //caso tenha permissão para escrita
		contBlocoIndices = ceil((double)descritor->tamanho/(BLOCO*INDICES)); //Descobre quantos blocos de indices tem
		if(descritor->tamanho == 0)	contBlocoIndices = 1;
		blocoIndices = descritor->indices; //aponta para o primeiro bloco de índices daquele arquivo

		while(contBlocoIndices > 1){ //Caso houver diversos blocos de índices ele precisa encontrar o ultimo para fazer a função APPEND
			fseek(pdescritor->arquivo_host, bloco[blocoIndices].ultimo - sizeof(indice_arquivo_t), SEEK_SET); //Posiciona para ler qual é o ultimo bloco de indices disponivel
			retorno = fread(&blocoIndices, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
			contBlocoIndices--;
		}

		//Vai ler exatamente o indice que vai levar ao bloco de dados
		fseek(pdescritor->arquivo_host, (bloco[blocoIndices].ultimo - sizeof(indice_arquivo_t)), SEEK_SET);
		retorno = fread(&blocoDados, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);

		if(podeEscrever(tamanho, blocoDados, blocoIndices)){
			//Agora ele vai escrever no fim do bloco de dados que está em uso
			fseek(pdescritor->arquivo_host, bloco[blocoDados].ultimo, SEEK_SET); //função APPEND -> começa a escrever ao final do bloco de dados

			while(bytesEscritos < tamanho){
				if(bloco[blocoDados].ultimo < bloco[blocoDados].limite){ //enquanto houver espaço no bloco de dados atual para escrever
					fseek(pdescritor->arquivo_host, bloco[blocoDados].ultimo, SEEK_SET);
					byteEscrito = criptografa(buffer[bytesEscritos], pdescritor->abertos[arquivo].chave);
					fwrite(&byteEscrito, sizeof(char), 1, pdescritor->arquivo_host);
					bytesEscritos++;
					bloco[blocoDados].ultimo++; //char é 1 byte, então só incrementa
				}
				else{ //senão, procura um bloco livre e adiciona no bloco de índices
					if(bloco[blocoIndices].ultimo == (bloco[blocoIndices].limite - sizeof(indice_arquivo_t))){
						//Significa que a ultima referencia desse bloco de indices deve ser para um novo bloco de indices
						fseek(pdescritor->arquivo_host, bloco[blocoIndices].ultimo, SEEK_SET);
						bloco[blocoIndices].ultimo+= sizeof(indice_arquivo_t); //Agora é igual ao limite
						blocoIndices = procuraBlocoLivre();
						fwrite (&blocoIndices, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host); //Escreve no bloco de Indices a referencia para o novo bloco de indices
						fflush(pdescritor->arquivo_host);
						bloco[blocoIndices].tipo = 1;
						blocoslivres--;
					}
					blocoDados = procuraBlocoLivre();
					//O novo bloco de dados é sempre escrito no ultimo bloco de indices
					fseek(pdescritor->arquivo_host, bloco[blocoIndices].ultimo, SEEK_SET);
					fwrite (&blocoDados, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
					fflush(pdescritor->arquivo_host);
					bloco[blocoIndices].ultimo+= sizeof(indice_arquivo_t);
					bloco[blocoDados].tipo = 2; //tem que transformar o novo bloco que vai ser escrito em bloco de dados
					blocoslivres--;
				}
			}
			descritor->tamanho += tamanho; //tamanho total do arquivo

			tempo = time(NULL);
			descritor->tempo = tempo;

			return SUCCESSO;
		}
		return FALHA; //Escrita grande demais, não há blocos suficientes
	}
	return FALHA; //Você não tem permissão para escrever nesse arquivo

}

int cry_delete(indice_arquivo_t arquivo){
	int contBlocoIndices, contIndices, retorno, descritor, j, indicesLidos, blocoDados = 0, blocoIndices; //Conta quantos blocos de indices tem, quantos indices utilizados dentro do bloco de indices, e quantos blocos de dados
	arquivo--;
	descritor_t *remover = pdescritor->abertos[arquivo].arquivo;
	if(arquivo > 255 || arquivo < 0 || pdescritor->abertos[arquivo].arquivo == NULL) return FALHA;

	if(remover != NULL){
		contBlocoIndices = ceil((double)remover->tamanho/(BLOCO*INDICES));
		contIndices = ceil((double)remover->tamanho/BLOCO);

		if(remover->tamanho == 0){
			contBlocoIndices = 1;
			contIndices = 1;
		}
		descritor = indiceNosDescritores(remover->nome);
		if (descritor == -1) return FALHA;

		//Nesse while ele LIMPA todas as referencias, mas nao zera o bloco (se for zerar provavelmente o processamento vai demorar muito)
		while(contBlocoIndices){
			indicesLidos = 0;
			j = 0;
			while(indicesLidos < INDICES && contIndices){
				fseek(pdescritor->arquivo_host, bloco[remover->indices].primeiro + j, SEEK_SET); //Aponta para onde precisa ler
				retorno = fread(&blocoDados, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
				j+= sizeof(indice_arquivo_t);
				indicesLidos++;
				contIndices--;
				inicializaBlocos(blocoDados, (blocoDados + 1)); //Limpa bloco, mas nao zera o bloco (PRECISA ZERAR ?)
				blocoslivres++;
			}
			if (indicesLidos == INDICES && contIndices){
				//Se tiver mais de um contBlocoIndices ele entra nesse caso
				fseek(pdescritor->arquivo_host, bloco[remover->indices].ultimo - sizeof(indice_arquivo_t), SEEK_SET);//Aponta para a o lugar que aponta para o novo bloco de indices
				retorno = fread(&blocoIndices, sizeof(indice_arquivo_t), 1, pdescritor->arquivo_host);
				inicializaBlocos(remover->indices, remover->indices + 1);
				remover->indices = blocoIndices;
				blocoslivres++; //Libera um bloco de indices
			}
			contBlocoIndices--;
		}
		//Limpa o último bloco de indices
		inicializaBlocos(remover->indices, remover->indices + 1);
		blocoslivres++; //Libera um bloco de indices
		limpaDescritor(&pdescritor->descritores[descritor]); //Limpa todo o descritor
		return cry_close(arquivo + 1); //Tira da tabela de abertos
	}

	return FALHA; //Se o arquivo que está tentnado deletar não existe
}

/*Funções auxiliares*/
void limpaBloco(int blocos, FILE *hospedeiro){
	int j;
	char *blocoVazio;

	while(blocos){ //cria blocos vazios (que virão a ser blocos de indices ou blocos de dados de arquivos), preenchendo o arquivo com '0's
		blocovazio_t *blocov;
		blocov = (blocovazio_t*) malloc(sizeof(blocovazio_t));
		for(j = 0; j < BLOCO; j++){
			blocov->n[j] = '0';
		}
		fwrite(blocov, sizeof(blocovazio_t), 1, hospedeiro);
		blocos--;
		free(blocov);
	}
}

void inicializaBlocos(int inicial, int final){
	int i;
	for(i = inicial; i < final; i++){
		bloco[i].tipo = -1; //não tem nada no bloco ainda
		bloco[i].primeiro = BLOCO*i;
		bloco[i].ultimo = bloco[i].primeiro; //enquanto não tem nada escrito, o ultimo é o primeiro
		bloco[i].limite = BLOCO*(i+1);//primeiro do proximo bloco
	}
}

void limpaDescritor(descritor_t *descritor){
	int i;
	for (i = 0; i < 256; i++){
		descritor->nome[i] = '0';
	}
	descritor->nome[i] = '\0';
	descritor->tempo = 0;
	descritor->indices = 0;
	descritor->tamanho = 0;
}

int procuraBlocoLivre(){
	int i;

	for(i = 0; i < nblocos; i++){
		if(bloco[i].tipo == -1) return i;
	}

	return -1;
}

int procuraDescritorLivre(){
	int i;

	for(i = 0; i < 256; i++){
		if(pdescritor->descritores[i].indices == 0) return i;
	}

	return -1;
}

int procuraIndiceLivreTA(){
	int i;

	for(i = 0; i < 256; i++){
		if (pdescritor->abertos[i].arquivo == NULL) return i;
	}

	return -1;
}

int indiceNaTA(char *nome){
	int i;

	for(i = 0; i < 256; i++){
		if(pdescritor->abertos[i].arquivo != NULL && strcmp(pdescritor->abertos[i].arquivo->nome, nome) == 0) return i;
	}

	return -1;
}

int indiceNosDescritores(char *nome){
	int i;

	for (i = 0; i < 256; i++){
		if(strcmp(pdescritor->descritores[i].nome, nome) == 0)	return i;
	}

	return -1;
}

int podeEscrever(int tamanho, int blocoDados, int blocoIndices){
	int bytesEscritos = bloco[blocoDados].ultimo - bloco[blocoDados].primeiro; // Quantos bytes estão escritos até o momento no bloco de dados
	int blocosNecessarios = ceil((double)(tamanho + bytesEscritos)/BLOCO) - 1; //Menos 1 porque ele continua escrevendo no bloco já alocado
	int indicesNoBlocoIndices = (bloco[blocoIndices].ultimo - bloco[blocoIndices].primeiro)/sizeof(indice_arquivo_t); //informa quantos indices existe no bloco de indices atual

	if(blocosNecessarios > INDICES) blocosNecessarios+= floor(blocosNecessarios/(INDICES - indicesNoBlocoIndices)); //se necessitar de mais de um bloco de índices, adiciona o número extra de blocos de índices
	if(blocosNecessarios > blocoslivres) return FALHA;
	else return SUCCESSO;

}

char criptografa(char byte, char chave){
	if((byte + chave) > 127) return (byte + chave - 127); //caso ultrapasse o último caractere da tabela ASCII, "volta" para os primeiros
	else return (byte + chave);
}

char descriptografa(char byte, char chave){
	if((byte-chave) < 0) return (byte - chave + 127); //caso ultrapasse o primeiro caractere da tabela ASCII, "volta" para os últimos
	else return (byte - chave);
}
