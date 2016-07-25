/* Renata Junges e Daiane Andrade */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "hash.h"

//estrutura para os mutex que serão utilizados
//para a sincronização @param_mutex é um vetor com
// 10% do tamanho da hash
typedef struct synchronized{
  pthread_mutex_t * mutex;
  pthread_cond_t  * conds;
  int indicesTotais; //pra evitar ter que fazer (int) ceil...
}synchronized;

hash_t * ht_init(unsigned long tam){
	hash_t *newer;

	if(tam == 0){
		return NULL;
	}
	newer = malloc(sizeof(hash_t));

	newer->numero_elementos = tam;
	newer->armazenamento = malloc(sizeof(entrada_hash_t) * tam);
	
	for (int i = 0; i < tam; i++) {
		newer->armazenamento[i].chave = i;
		newer->armazenamento[i].conteudo = malloc(sizeof(conteudo_t));
		newer->armazenamento[i].conteudo = NULL;
		newer->armazenamento[i].proximo = NULL;		
	}
	//alocação da estrutura
	synchronized * sinc = malloc(sizeof(synchronized));
	//alocação de 10% do tamanho da hash na forma de mutex
	sinc->mutex = malloc(sizeof (pthread_mutex_t) * ceil(tam * 0.1));
	sinc->conds = malloc(sizeof (pthread_cond_t) * ceil(tam * 0.1));
	sinc->indicesTotais = (int) ceil(tam * 0.1);
	for (int i = 0; i < (int)ceil(tam*0.1); ++i)
	{
		//criando os mutex
		if(pthread_mutex_init(&sinc->mutex[i],NULL)){
			return NULL; //Não conseguiu inicializar o mutex
		}
		if (pthread_cond_init(&sinc->conds[i],NULL)){
			return NULL;
		}
	}
	//passando o que foi alocado pra estrutura da hash
	newer->sincronizacao = sinc;
	return newer;
}


void ht_remove(hash_t * ht){
	entrada_hash_t *auxiliar, *anterior;
	synchronized * s = (synchronized *)ht->sincronizacao;
	for (int i = 0 ; i < s->indicesTotais ; ++i){
		if ((pthread_mutex_trylock(&s->mutex[i]) != 0)){
			pthread_cond_wait(&s->conds[i],&s->mutex[i]);
		}
	}
	pthread_mutex_destroy(s->mutex);
	pthread_cond_destroy(s->conds);
	free(ht->armazenamento);
	free(ht);
}


chave_t ht_hash(hash_t *ht, conteudo_t * conteudo){
	unsigned long int chave;
	if(conteudo == NULL){
		return (chave_t)NULL;//erro
	}
	chave = (*conteudo * 6709) % ht->numero_elementos;
	return chave;
}


entrada_hash_t * ht_busca_chave(hash_t * ht, chave_t chave){
	synchronized * s = (synchronized *)ht->sincronizacao;
	unsigned long int posicao_mutex_livre = (int)floor(chave*0.1);
	if ((pthread_mutex_trylock(&s->mutex[posicao_mutex_livre]) == 0)){
		pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
	}
	if(ht->armazenamento[chave].conteudo!=NULL){
		return &ht->armazenamento[chave];
	} 

	return NULL;

}
entrada_hash_t * ht_busca_conteudo(hash_t * ht, conteudo_t * conteudo){
	chave_t chave = ht_hash(ht, conteudo);
	entrada_hash_t *auxiliar = &ht->armazenamento[chave];
	
	synchronized * s = (synchronized *)ht->sincronizacao;
	unsigned long int posicao_mutex_livre = (int)floor(chave*0.1);
	if ((pthread_mutex_trylock(&s->mutex[posicao_mutex_livre]) == 0)){
		pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
	}
	//Se nao tiver nenhum elemento na posição, na entra no if
	if(auxiliar->conteudo != NULL){
		//Caso o conteudo esteja encadeado, entra nesse caso
		while(auxiliar->proximo != NULL){
			if(*auxiliar->conteudo == *conteudo){
				return auxiliar;
			}
			else{
				auxiliar = auxiliar->proximo;
			}
		} 
		//Caso esteja na primeira posição do vetor,ou ultima, entra nesse caso
		if(*auxiliar->conteudo == *conteudo){
			return auxiliar;
		}
	}
	return NULL;
}
int ht_insere_conteudo(hash_t * ht, conteudo_t * conteudo){
	chave_t chave = ht_hash(ht, conteudo);
	entrada_hash_t *testeConteudo = malloc(sizeof(entrada_hash_t));
	testeConteudo = ht_busca_conteudo(ht,conteudo);
	synchronized * s = (synchronized *)ht->sincronizacao;
	unsigned long int posicao_mutex_livre = (int)floor(chave*0.1);
	if((pthread_mutex_trylock(&s->mutex[posicao_mutex_livre]) != 0)){
		pthread_cond_wait(&s->conds[posicao_mutex_livre],&s->mutex[posicao_mutex_livre]);
	}
	if(testeConteudo == NULL){
		//Armazena, pois não existe esse conteudo na hash
		if(ht->armazenamento[chave].conteudo == NULL){ 
			//Se é NULL então não existe nenhum conteúdo ainda na tabela 
			ht->armazenamento[chave].conteudo = conteudo;
			pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
			pthread_cond_signal(&s->conds[posicao_mutex_livre]);
			return 1;
		} 
		else {		//Já existe pelo menos um conteudo na tabela
			entrada_hash_t *nova = malloc (sizeof(entrada_hash_t));
			nova->chave = chave;
			nova->conteudo = conteudo;
			nova->proximo = NULL;
			if(ht->armazenamento[chave].proximo == NULL){	//Tem só 1 elemento na hash
				ht->armazenamento[chave].proximo = nova;
				pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
				pthread_cond_signal(&s->conds[posicao_mutex_livre]);
				return 1; 
			}
			else{  	//A hash tem mais de 1 elemento
				entrada_hash_t *auxiliar = ht->armazenamento[chave].proximo;
				entrada_hash_t *anterior;
				while(auxiliar != NULL){
					anterior = auxiliar;
					auxiliar = auxiliar->proximo;
				}
				//auxiliar->proximo == NULL, anterior ao auxiliar é o anterior
				anterior->proximo = nova;
				pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
				pthread_cond_signal(&s->conds[posicao_mutex_livre]);
				return 1; 
			}
			free(nova); // se der algo errado, desaloca
			pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
			pthread_cond_signal(&s->conds[posicao_mutex_livre]);
			return 0; // Se chegar aqui deu erro
		}	
	}
	//Se tentar duplicar elemento, dá erro
	pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
	return 0; // erro	
}

int ht_remove_elemento(hash_t * ht, entrada_hash_t * entrada){
	chave_t chave = ht_hash(ht, entrada->conteudo);
	synchronized * s = (synchronized *)ht->sincronizacao;
	unsigned long int posicao_mutex_livre = (int)floor(chave*0.1);
	if(ht_busca_conteudo(ht, entrada->conteudo) == NULL){
		//Nao existe na hash o conteudo que deseja remover
		return 0; 
	}

	if(ht->armazenamento[chave].conteudo==NULL){
		//Nenhum conteúdo na chave
		return 0;
	}

	//estruta de sincronização a partir daqui 
	/*Se a chave nao tiver ocupada*/
	//if(!chaveOcupada(s->ocupado, 1, s->indicesTotais))

	if ((pthread_mutex_trylock(&s->mutex[posicao_mutex_livre]) != 0)){
		pthread_cond_wait(&s->conds[posicao_mutex_livre],&s->mutex[posicao_mutex_livre]);
	}
	if(ht->armazenamento[chave].conteudo == entrada->conteudo && ht->armazenamento[chave].proximo == entrada->proximo) {
		/*Então é o primeiro elemento que precisa de free. 
		 * precisamos descobrir se ele é o unico elemento ou se existe outros depois desse primeiro*/
		if(ht->armazenamento[chave].proximo == NULL){
			/*Se a entrada é a única entrada então nao precisa dar free, 
			 *pois nao foi dado um malloc na criação.
			 *Apenas é necessário "limpar o conteudo"*/
			ht->armazenamento[chave].conteudo = NULL;
			pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
			pthread_cond_signal(&s->conds[posicao_mutex_livre]);
			return 1;
		}
		else{
			/*Existem elementos depois do primeiro elemento,
			 *então traz o segundo elemento para o primeiro elemento e apaga a célula do segundo*/
			entrada_hash_t *auxiliar = ht->armazenamento[chave].proximo;
			ht->armazenamento[chave].conteudo = auxiliar->conteudo;
			ht->armazenamento[chave].proximo = auxiliar->proximo;
			free(auxiliar);
			pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
			pthread_cond_signal(&s->conds[posicao_mutex_livre]);
			return 1;
		}
	} 
	
	else{
		//Hash tem mais de 1 elemento, é preciso procurar na lista encadeada e dar free nele
		entrada_hash_t *auxiliar = &ht->armazenamento[chave];
		entrada_hash_t *anterior = malloc(sizeof(entrada_hash_t));
		while(auxiliar->conteudo != entrada->conteudo){
			anterior = auxiliar;
			auxiliar = auxiliar->proximo;
		}
		//
		anterior->proximo = auxiliar->proximo;
		free(auxiliar);
		pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
		pthread_cond_signal(&s->conds[posicao_mutex_livre]);
		return 1;
	}
	pthread_mutex_unlock(&s->mutex[posicao_mutex_livre]);
	return 0;
}

elemento_lista_t * ht_lista(hash_t * ht){
	elemento_lista_t * returnE = malloc(sizeof(elemento_lista_t));
	returnE->elemento = ht->armazenamento;
	returnE->proximo = NULL;
	return returnE;
}
