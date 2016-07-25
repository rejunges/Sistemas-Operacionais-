#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define NUM 50000
#define PORC 100

void * insere500 (void * auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	conteudo_t *valor;
	int i;

	for(i=0; i<NUM/2; i++){
		//adiciona do 1 ao 10 na hash
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		if(ht_insere_conteudo(hashTable, valor) == 0){
			printf("Elemento %lu não inserido.\n", *valor);
		}
	}
}
void * insere1000 (void * auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	conteudo_t *valor;
	int i;

	for(i=NUM/2; i<NUM; i++){
		//adiciona do 1 ao 10 na hash
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		if(ht_insere_conteudo(hashTable, valor) == 0){
			printf("Elemento %lu não inserido.\n", *valor);
		}
	}
}

void * removeElemento1(void * auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	entrada_hash_t * entrada;
	conteudo_t *valor;
	int i;
	//apaga do 
	for(i=0; i < PORC/2; i++){
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		entrada = ht_busca_conteudo(hashTable, valor);
		if(entrada == NULL){
			printf("Elemento %lu não encontrado na hash\n", *valor);
		}
		else{
			//Remove o elemento
			if(ht_remove_elemento(hashTable, entrada) == 0){
				printf("Remoção do elemento %lu falhou.\n", *valor);
			}
		}
	}
}


void * removeElemento2(void * auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	entrada_hash_t * entrada;
	conteudo_t *valor;
	int i;
	//apaga do 
	for(i=PORC/2; i < PORC; i++){
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		entrada = ht_busca_conteudo(hashTable, valor);
		if(entrada == NULL){
			printf("Elemento %lu não encontrado na hash\n", *valor);
		}
		else{
			//Remove o elemento
			if(ht_remove_elemento(hashTable, entrada) == 0){
				printf("Remoção do elemento %lu falhou.\n", *valor);
			}
		}
	}
}
void * buscaConteudo1( void *auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	int i;
	entrada_hash_t *entrada;
	conteudo_t *valor;
	for (i=0; i< PORC/2;i++){
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		entrada = ht_busca_conteudo(hashTable, valor);
		if(entrada == NULL){
			printf("Elemento %lu não encontrado na hash\n", *valor);
		}
	}
}
void * buscaConteudo2( void *auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	int i;
	entrada_hash_t *entrada;
	conteudo_t *valor;
	for (i=PORC/2; i< PORC;i++){
		valor = malloc(sizeof(conteudo_t));
		*valor = i;
		entrada = ht_busca_conteudo(hashTable, valor);
		if(entrada == NULL){
			printf("Elemento %lu não encontrado na hash\n", *valor);
		}
	}
}

void * deletaTabela (void * auxiliar){
	hash_t *hashTable = (hash_t*) auxiliar;
	ht_remove(hashTable);
}
void imprimeLista(hash_t * ht){
	int i, vez=0;
	entrada_hash_t *aux;
		
	for(i=0; i < ht->numero_elementos; i++){
		aux = &ht->armazenamento[i];
		if(aux->conteudo != NULL){
			if(aux->proximo == NULL){
				//primeira posição da memoria
				printf("Chave: %d | Conteudo: %lu\n", i, *aux->conteudo);
			}
			else{
				while(aux->proximo != NULL){
					printf("Chave: %d | Conteudo: %lu\n", i, *aux->conteudo);
					aux = aux->proximo;
				}
				//imprime ultimo elemento
				printf("Chave: %d | Conteudo: %lu\n", i, *aux->conteudo);
			}

		}

	}
}


int main(int argc, char * argv[]){
	pthread_t thread[7];
	hash_t *hash = ht_init(100);
	 
	if(pthread_create(&thread[0], NULL, &insere500, (void *)hash)){
		return 1;
	}

	if(pthread_create(&thread[1], NULL, &insere1000, (void *)hash)){
		return 1; //aborta programa pq a thread não foi criadaif
	}
	pthread_join(thread[0],NULL);
	pthread_join(thread[1],NULL);

	if(pthread_create(&thread[2], NULL, &buscaConteudo1, (void *)hash)){
		return 1;
	}

	if(pthread_create(&thread[3], NULL, &buscaConteudo2, (void *)hash)){
		return 1; //aborta programa pq a thread não foi criada
	}
	pthread_join(thread[2],NULL);
	pthread_join(thread[3],NULL);

	if(pthread_create(&thread[4], NULL, &removeElemento1, (void *)hash)){
		return 1;
	}

	if(pthread_create(&thread[5], NULL, &removeElemento2, (void *)hash)){
		return 1; //aborta programa pq a thread não foi criadaif
	}
	pthread_join(thread[4],NULL);
	pthread_join(thread[5],NULL);

	if(pthread_create(&thread[6], NULL, &deletaTabela, (void *)hash)){
		return 1;
	}
	pthread_join(thread[6],NULL);
	
	return 0;
}

