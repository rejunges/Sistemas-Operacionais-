#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
/*
int main(int argc, char * agrv[]){
	hash_t *hash = ht_init(3);
	int i;
	conteudo_t *valor = malloc(sizeof(conteudo_t)*5);
	for(i=0; i<5; i++){
		scanf("%lu", &valor[i]);
		ht_insere_conteudo(hash, &valor[i]);
	}
	entrada_hash_t *busca1 = malloc(sizeof(entrada_hash_t));
	printf("Buscas:\n");
	for (i=0; i<5; i++){
		busca1 = ht_busca_conteudo(hash, &valor[i]);
		printf("%lu\n", *busca1->conteudo);
	}
	busca1 =  ht_busca_conteudo(hash, &valor[1]);
	i = ht_remove_elemento(hash, busca1);
	busca1 = ht_busca_conteudo(hash,&valor[1]);
	if(busca1 == NULL){
		printf("apagou o valor[1]\n");
	}
	busca1 = ht_busca_conteudo(hash, &valor[2]);
	printf("%lu\n", *busca1->conteudo);
	entrada_hash_t *testeNULL = malloc(sizeof(entrada_hash_t));
	*testeNULL->conteudo = 6;
	testeNULL->proximo = NULL;
	testeNULL->chave = 2;
	if(ht_remove_elemento(hash, testeNULL) == 0){
		printf("nao removeu, nao tinha\n");
	}
	ht_remove(hash);
	//descomente essa linha para testar o ht_remove
	//printf("%lu", *hash->armazenamento[0].conteudo);


}*/

// Testada!
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

	return newer;
}

//Não testada
void ht_remove(hash_t * ht){
	entrada_hash_t *auxiliar, *anterior;

	for(int i=0; i<ht->numero_elementos; i++){
		auxiliar = &ht->armazenamento[i];
		while(auxiliar != NULL){
			anterior = auxiliar;
			auxiliar = auxiliar->proximo;
			ht_remove_elemento(ht, anterior);
		}
	}
	free(ht->armazenamento);
	free(ht);
}

// Testada!
chave_t ht_hash(hash_t *ht, conteudo_t * conteudo){
	unsigned long int chave;
	if(conteudo == NULL){
		return NULL;//erro
	}
	chave = (*conteudo * 6709) % ht->numero_elementos;
	return chave;
}

// Testada!
entrada_hash_t * ht_busca_chave(hash_t * ht, chave_t chave){
	if(ht->armazenamento[chave].conteudo!=NULL){
		return &ht->armazenamento[chave];
	} 

	return NULL;
}

//Possibilidade de erro, conferir 
entrada_hash_t * ht_busca_conteudo(hash_t * ht, conteudo_t * conteudo){
	chave_t chave = ht_hash(ht, conteudo);
	entrada_hash_t *auxiliar = &ht->armazenamento[chave];
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

// Não Testada Completamente
int ht_insere_conteudo(hash_t * ht, conteudo_t * conteudo){
	chave_t chave = ht_hash(ht, conteudo);
	entrada_hash_t *testeConteudo = malloc(sizeof(entrada_hash_t));
	testeConteudo = ht_busca_conteudo(ht,conteudo);
	
	if(testeConteudo == NULL){
		//Armazena, pois não existe esse conteudo na hash
		if(ht->armazenamento[chave].conteudo == NULL){ 
			//Se é NULL então não existe nenhum conteúdo ainda na tabela 
			ht->armazenamento[chave].conteudo = conteudo; 
			return 1;
		} 
		else {		//Já existe pelo menos um conteudo na tabela
			entrada_hash_t *nova = malloc (sizeof(entrada_hash_t));
			nova->chave = chave;
			nova->conteudo = conteudo;
			nova->proximo = NULL;
			if(ht->armazenamento[chave].proximo == NULL){	//Tem só 1 elemento na hash
				ht->armazenamento[chave].proximo = nova;
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
				return 1; 
			}
			free(nova); // se der algo errado, desaloca
			return 0; // Se chegar aqui deu erro
		}	
	}
	//Se tentar duplicar elemento, dá erro
	return 0; // erro	
}

//Não testada completamente
int ht_remove_elemento(hash_t * ht, entrada_hash_t * entrada){
	chave_t chave = ht_hash(ht, entrada->conteudo);

	if(ht_busca_conteudo(ht, entrada->conteudo) == NULL){
		//Nao existe na hash o conteudo que deseja remover
		return 0; 
	}
	if(ht->armazenamento[chave].conteudo==NULL){
		//Nenhum conteúdo na chave
		return 0;
	}

	if(ht->armazenamento[chave].conteudo == entrada->conteudo && ht->armazenamento[chave].proximo == entrada->proximo) {
		/*Então é o primeiro elemento que precisa de free. 
		 * precisamos descobrir se ele é o unico elemento ou se existe outros depois desse primeiro*/
		if(ht->armazenamento[chave].proximo == NULL){
			/*Se a entrada é a única entrada então nao precisa dar free, 
			 *pois nao foi dado um malloc na criação.
			 *Apenas é necessário "limpar o conteudo"*/
			ht->armazenamento[chave].conteudo = NULL;
			return 1;
		}
		else{
			/*Existem elementos depois do primeiro elemento,
			 *então traz o segundo elemento para o primeiro elemento e apaga a célula do segundo*/
			entrada_hash_t *auxiliar = ht->armazenamento[chave].proximo;
			ht->armazenamento[chave].conteudo = auxiliar->conteudo;
			ht->armazenamento[chave].proximo = auxiliar->proximo;
			free(auxiliar);
			return 1;
		}
	} 
	//Arrumar Aqui, fazer condição pra quando nao tem nenhum elemento
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
		return 1;
	}
	
	return 0;
}

elemento_lista_t * ht_lista(hash_t * ht){
  elemento_lista_t * returnE = malloc(sizeof(elemento_lista_t));
  returnE->elemento = ht->armazenamento;
  returnE->proximo = NULL;
  return returnE;
}