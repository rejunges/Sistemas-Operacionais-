#include "hash.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]){
	hash_t *ht ;
	/* CASO LEVE */
	/*ht = ht_init(500);
	entrada_hash_t *entrada;
	int i;
	//inserção de 1k 
	for(i=0; i<1000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_insere_conteudo(ht, conteudo)!=1){
			printf("Erro ao inserir o conteudo %lu.\n", *conteudo);
		}
	}

	//busca por 20% dos elementos
	for(i=0; i<200; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_busca_conteudo(ht, conteudo) == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
	}

	//remover 20% dos elementos da tabela 
	for(i=0; i<200; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		entrada = ht_busca_conteudo(ht, conteudo);
		if(entrada == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
		if(ht_remove_elemento(ht,entrada)==0){
			printf("Erro na remoção do conteudo %lu", *entrada->conteudo);
		}
	}
	*/

	/*CASOS INTERMEDIÁRIOS*/
	//CASO 1: 50k
	/*ht = ht_init(25000);
	entrada_hash_t *entrada;
	int i;
	//inserção de 50k 
	for(i=0; i<50000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_insere_conteudo(ht, conteudo)!=1){
			printf("Erro ao inserir o conteudo %lu.\n", *conteudo);
		}
	}

	//busca por 50% dos elementos
	for(i=0; i<25000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_busca_conteudo(ht, conteudo) == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
	}

	//remover 20% dos elementos da tabela 
	for(i=0; i<25000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		entrada = ht_busca_conteudo(ht, conteudo); 
		if(entrada == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
		if(ht_remove_elemento(ht,entrada)==0){
			printf("Erro na remoção do conteudo %lu", *entrada->conteudo);
		}
	}*/

	//CASO 2: 100k
	/*ht = ht_init(50000);
	entrada_hash_t *entrada;
	int i;
	//inserção de 50k 
	for(i=0; i<100000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_insere_conteudo(ht, conteudo)!=1){
			printf("Erro ao inserir o conteudo %lu.\n", *conteudo);
		}
	}

	//busca por 50% dos elementos
	for(i=0; i<50000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_busca_conteudo(ht, conteudo) == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
	}

	//remover 20% dos elementos da tabela 
	for(i=0; i<50000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		entrada = ht_busca_conteudo(ht, conteudo) ;
		if(entrada == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
		if(ht_remove_elemento(ht,entrada)==0){
			printf("Erro na remoção do conteudo %lu", *entrada->conteudo);
		}
	}*/
	/*CASOS AHEAD-OF-THE-CURVE*/
	/*ht = ht_init(500000);
	entrada_hash_t *entrada;
	int i;
	//inserção de 1m 
	for(i=0; i<1000000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_insere_conteudo(ht, conteudo)!=1){
			printf("Erro ao inserir o conteudo %lu.\n", *conteudo);
		}
	}

	//busca por 90% dos elementos
	for(i=0; i<900000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_busca_conteudo(ht, conteudo) == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
	}

	//remover 90% dos elementos da tabela 
	for(i=0; i<900000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		entrada = ht_busca_conteudo(ht, conteudo) ;
		if(entrada == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
		if(ht_remove_elemento(ht,entrada)==0){
			printf("Erro na remoção do conteudo %lu", *entrada->conteudo);
		}
	}*/
	/*hash_t *ht ;
	/*CASOS AHEAD-OF-THE-CURVE*/
	/*ht = ht_init(100);
	entrada_hash_t *entrada;
	int i;
	//inserção de 1m 
	for(i=0; i<50000; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_insere_conteudo(ht, conteudo)!=1){
			printf("Erro ao inserir o conteudo %lu.\n", *conteudo);
		}
	}

	//busca por 90% dos elementos
	for(i=0; i<100; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		if(ht_busca_conteudo(ht, conteudo) == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
	}

	//remover 90% dos elementos da tabela 
	for(i=0; i<100; i++){
		conteudo_t * conteudo = (conteudo_t *) calloc(1, sizeof(conteudo_t));
		*conteudo = i;
		entrada = ht_busca_conteudo(ht, conteudo) ;
		if(entrada == NULL){
			printf("Erro ao buscar o conteudo %lu.\n", *conteudo);
		}
		if(ht_remove_elemento(ht,entrada)==0){
			printf("Erro na remoção do conteudo %lu", *entrada->conteudo);
		}
	}
	*/

	ht_remove(ht);
	return 0;
}