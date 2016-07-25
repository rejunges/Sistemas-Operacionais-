#Casos de teste
Aqui vai uma descrição dos casos de teste que devem ser feitos no `main.c` tanto para sequencial quanto para paralelo.
O que deve ser impresso na tela é apenas se algo não for possível para fins de melhor desempenho por tempo.

###Coleta do tempo
Para cada caso de teste deve ser coletado o tempo total do caso de teste (recomendo `time ./main`).

###Caso Leve
- Inserção:
  - Inserção de 1k elementos e com metade dele como tamanho da tabela.
- Busca:
  - Buscar 20% dos elementos inseridos na tabela.
- Deleção:
  - Remover 20% dos elementos inseridos na tabela e deleção da tabela.

###Casos Intermediários
- Inserção:
  - Inserção de 50k elementos e com metade dele como tamanho da tabela.
  - Inserção de 100k elementos e com metade dele como tamanho da tabela.
- Busca:
  - Buscar 50% dos elementos inseridos na tabela para cada quantidade de inserção.
- Deleção:
  - Remover 50% dos elementos inseridos na tabela e deleção da tabela.

###Caso Ahead-of-the-Curve
- Inserção:
   - Inserção de 1m elementos e metade desse valor como tamanho da tabela.
- Busca
  - Buscas 90% dos elementos inseridos na tabela.
- Deleção
  - Remover 90% dos elementos inseridos na tabela e deleção da tabela.

###Informações extra:
- Fazer os casos de teste sequecial primeiro.
- Para a versão paralela deve-se usar no máximo o número de `cores` como número de threads.
- As threads devem tentar fazer várias buscas(para o teste de busca) ao mesmo tempo, várias inserções (para o teste de inserções) ao mesmo tempo e várias remoções de elementos (para o teste de deleção) ao mesmo tempo.
- Descrever qual o hardware que foi utilizado para o caso de testes.
