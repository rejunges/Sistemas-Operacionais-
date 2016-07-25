#include <stdio.h>

/* Mude estas definicoes de tipo caso queira fazer outras tabelas hash (nao para este
 * trabalho). */
typedef unsigned long int chave_t;
typedef unsigned long int conteudo_t;

typedef struct entrada_hash{
  chave_t chave;
  conteudo_t * conteudo;
  struct entrada_hash * proximo;
} entrada_hash_t;

typedef struct elemento_lista{
  entrada_hash_t * elemento;
  struct elemento_lista * proximo;
} elemento_lista_t;


typedef struct tabela_hash{
  unsigned long int numero_elementos;
  entrada_hash_t * armazenamento; /* Entradas da tabela contendo chaves e
                      * conteudo */
  void * sincronizacao; /* Aqui vai uma struct com todas as variaveis inicializadas
               * para sincronizacao. Use a imaginacao. */

} hash_t;


/** Inicializa uma tabela hash, sua sincronizacao e retorna um ponteiro para seu descritor
  * @param tam numero de entradas
  * @return ponteiro para descritor, ou NULL
  */
hash_t * ht_init(unsigned long tam);

/** Desaloca uma tabela hash, respeitando sincronizacao
  * @param ht ponteiro para descritor da tabela
  */
void ht_remove(hash_t * ht);

/** Calcula a chave para um conteudo
  * @param ht ponteiro para descritor da tabela
  * @param conteudo conteudo a calcular chave
  * @return chave
  */
chave_t ht_hash(hash_t *ht, conteudo_t * conteudo);

/** Consulta por uma chave na tabela hash
  * @param ht ponteiro para descritor da tabela
  * @param chave a chave a ser consultada
  * @return ponteiro para item se encontrado, ou NULL
  */
entrada_hash_t * ht_busca_chave(hash_t * ht, chave_t chave);


/** Consulta um conteudo na tabela hash
  * @param ht ponteiro para descritor da tabela
  * @param ponteiro para conteudo  a ser consultada
  * @return ponteiro para item se encontrado, ou NULL
  */
entrada_hash_t * ht_busca_conteudo(hash_t * ht, conteudo_t * conteudo);

/** Insere um elemento na tabela hash
  * @param ht ponteiro para descritor da tabela
  * @param conteudo ponteiro para conteudo a ser inserido
  * @return 1 se inserido, 0 se erro
  */
int ht_insere_conteudo(hash_t * ht, conteudo_t * conteudo);

/** Remove um elemento da tabela (deve ter sido buscado primeiro)
  * @param ht ponteiro para descritor da tabela
  * @param entrada ponteiro para elemento
  * @return 1 se removido, 0 se erro
  */
int ht_remove_elemento(hash_t * ht, entrada_hash_t * entrada);

/** Retorna lista com todos os elementos da tabela
  * @param ht ponteiro para descritor da tabela
  * @return ponteiro para primeiro elemento da lista
  */
elemento_lista_t * ht_lista(hash_t * ht);