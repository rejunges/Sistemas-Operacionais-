# Trabalho 1 - Sistemas Operacionais

## Descrição

Neste trabalho, os alunos irão desenvolver uma tabela hash concorrente em C e casos de teste usando POSIX Threads. A interface a ser implementada está disponível no arquivo *hash.h*, que **NÃO** deve ser modificado em hipótese alguma.

A sincronização deve ser toda realizada dentro da implementação da estrutura de dados, de forma transparente ao programador. Podem ser usados:
* Mutex
* Semáforos
* Variáveis de condição

A avaliação de desempenho deverá ser realizada de acordo com princípios de reproducibilidade e com validade estatística. Prestem especial atenção ao _número de repetições_.

O trabalho deve ser compilável e executável em plataforma GNU/Linux.

## Produtos

* Implementação da tabela hash concorrente sequencial no arquivo hash_s.c
* Implementação da tabela hash concorrente no arquivo hash.c
* Casos de teste documentados
* Análise de desempenho, comparando a versão sequencial com a versão *multithread*
  * Metodologia de teste
  * Tempo de execução
  * Speedup

## Cronograma

* 05 e 06/04/2016 - aulas reservadas para o trabalho
* 11/04/2016 - versão sequencial (2,0 pontos)
* 02/05/2016 - entrega da versão final (6,0 pontos) com relatório (2,0 pontos)
* 03/05/2016 - correção dos trabalhos

## Referências

* [POSIX Threads Programming] https://computing.llnl.gov/tutorials/pthreads/
* [POSIX thread (pthread) libraries] http://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
