###	Testes para TENTAR abordar todos os casos possíveis	###

############	test1
1 - Testar usando todos os descritores, e todos os blocos de memória com arquivos diferentes.
2 - Tentar alocar mais um bloco de dados em algum arquivo ( e retornar erro pois não é possível, memória cheia )
3 - Fechar um arquivo que já foi escrito e abrir abri-lo.
4 - Deletar um arquivo e adicionar outro arquivo de um tamanho menor.
5 - Ler um dos arquivos adicionados que possui mais de 2 blocos de indices e mostrar buffer
6 - Tentar criar um novo arquivo ( erro pois os todos os descritores estão sendo usados)

############	test2
1 - Usar inicialmente uns 10 descritores
2 - Criar um arquivo de leitura e escrita, escrever uns 50 blocos de dados
3 - Abrir mais 75 arquivos, ler o arquivo de leitura e escrita e escrever mais 1000 blocos de dados
4 - Escrever 2000 blocos de dados em um novo arquivo de escrita e voltar a escrever no arquivo de leitura e escrita até faltar alguns blocos para acabar os blocos livres
5 - Escrever o arquivo de escrita até acabar os blocos livres.
6 - Ler e depois deletar arquivo de leitura e escrita.
7 - Abrir um novo arquivo

########### 	test3
1 - Abre 50 arquivos
2 - Tenta fechar um arquivo que não existe
3 - Tenta deletar um arquivo que não existe
4 - Escreve 1300 blocos num arquivo 10 vezes seguidas
5 - Le 50 blocos do arquivo acima
6 - Escreve em 130 blocos de dados em um segundo arquivo
7 - Tenta ler 140 blocos de dados do segundo arquivo
8 - Fecha o primeiro arquivo
9 - Cria mais 50 arquivos.
10 - Abre o primeiro arquivo novamente.

##########	test4
1 - Cria um arquivo hospedeiro sem o tamanho mínimo para os metadados
2 - Criar um arquivo hospedeiro relativamente pequeno - 3000 blocos.
3 - Adicionar 200 arquivos
4 - Escrever 1000 blocos de dados em um arquivo
5 - Adicionar mais 50 arquivos
6 - Tentar escrever mais blocos que o possível.
7 - Deletar o arquivo que foi escrito e tentar escrever novamente o número de blocos da instrução 6
8 - Deletar todos os arquivos ainda existentes no sistema.
9 - Mosta os descritores e os arquivos abertos ( nenhum ).

##########	test5
1 - Tenta criar um arquivo hospedeiro com tamanho de blocos negativo.
2 - Cria um arquivo hospedeiro com 50000 blocos.
3 - Tenta abrir um arquivo hospedeiro que não existe.
4 - Abre o arquivo hospedeiro com os 50000 blocos.
5 - Mostra que não existe nenhum descritor sendo usado e nenhum arquivo aberto.
6 - Abre um arquivo e escreve um valor extremamente grande de blocos de dados nele.
7 - Fecha o arquivo.
8 - Mostra que o arquivo sai da tabela dos arquivos abertos mas continua salvo no sistema de arquivos.
9 - Tenta deletar um arquivo que não está aberto.
10 - Abre de novo esse arquivo e tenta escrever mais blocos do que tem disponível no sistema de arquivos.
11 - Mostra os descritores do arquivo e os arquivos abertos no sistema.

##########	test6
1 - Cria um arquivo hospedeiro com 19 blocos.
2 - Abre o sistema de arquivos e abre um arquivo.
3 - Escreve BLOCO/4, 4 vezes no arquivo. 
4 - Mostra os descritores e a tabela de arquivos abertos.

##########	test7
1 - Cria um sistema hospedeiro pequeno - 200 blocos.
2 - Abre o sistema hospedeiro e abre 10 arquivos.
3 - Escreve no arquivo 1 (1 bloco) e no arquivo 2(2 blocos) intercaladamente numa execução de 10 vezes.
4 - Le o buffer do arquivo 1 
5 - Le o buffer do arquivo 2 

################################################## SOBRE OS TESTES ##############################################################
Nos testes implementados mostrou-se que o sistema de arquivos funciona corretamente, sem bugs.
Foram implementados 7 testes que de uma maneira geral tentam pegar diferentes aspectos do que poderia causar algum erro no sistema de arquivos,
a seguir será explicado os objetivos de cada teste. 

###Objetivos test1:
- Mostrar que a gerência de memória do sistema de arquivos funciona de maneira precisa.
- Mostrar que a função close e a função delete funcionam de maneira adequada
- Mostrar que a função read funciona com arquivos com mais de 2 blocos de índice.
- Mostrar que a função delete funciona.
------------------------------------------------------------------------------------------
Detalhes e explicações do test1:	
O sistema de arquivos é inicializado com exatamente 10020 blocos, esses blocos servem para conter EXATAMENTE os 17 blocos dos descritores,
os 512 blocos que serão reservados aos 256 arquivos que serão abertos, aos 1500 blocos que serão para um arquivo ( 1500 blocos sendo que dois
deles são de índice, ou seja, um já está previamente alocado; e o 1499 novos blocos de dados serão criados, pois 1 bloco de dados já foi alocado 
na abertura do arquivo), aos 5125 blocos que serão alocados para um outro arquivo ( sendo que possuirá 6 blocos de índices, pois um já é previamente
alocado; e possuirá 5124 blocos de dados pois um já foi alocado na abertura do arquivo também), e 2915 blocos para outros 53 arquivos ( pois cada um 
conterá 55 blocos de dados; ou seja, 53 blocos de dados previamente alocados). Tudo isso totaliza exatamente 10020 blocos, mais detalhes de cálculo
podem ser encontrados nos comentários do test1.c.
Após preencher toda essa memória com os arquivos mencionados acima, e verificar que não ocorre nenhum erro, tenta-se escrever mais de um bloco 
em um arquivo que ainda não foi utilizado. Esse arquivo retorna erro, mostrando que o sistema de gerência de memória livre está funcionando da maneira desejada.
Depois o programa fecha um arquivo, caso a linha 69 seja descomentada é possível verificar que o arquivo realmente não se encontra mais na tabela de abertos. E depois ele reabre o mesmo arquivo, é possível conferir que esse open funcionanda pois logo depois é feita a leitura desse arquivo e o buffer( que estão comentado ) apresenta a leitura correta.
Por fim, tenta-se abrir mais um arquivo, retornando erro, pois o sistema de arquivos já está com o máximo de arquivos possíveis ( 256 ).

###Objetivos test2:
- Mostrar que os arquivos de LEITURA, ESCRITA e de LEITURA e ESCRITA estão funcionando de maneira correta, com suas respectivar restrições.
- Mostrar que mesmo intercalando blocos de dados de arquivos diferentes(vendo como uma memória sequencial), o sistema continua funcionando de maneira correta.
------------------------------------------------------------------------------------------
Detalhes e explicações do test2:	
É criado um sistema de arquivos gigante (105000 blocos) para mostrar que não dá erro. Os arquivos são abertos intercaladamente para mostrar que não faz diferença alguma. São criados arquivos de LEITURA e ESCRITA; ESCRITA; LEITURA, para mostrar que retorna erro caso tente ler um arquivo de ESCRITA, ou que tente escrever em um arquivo de LEITURA. Também é deletado um arquivo, mostrando que não influe em outros arquivos. 

###Objetivos test3:
- Mostrar que tentar manipular funções, como fechar e deletar, em arquivos que inexistentes, não funciona.
- Mostrar que podemos escrever mais de 1 bloco de índices repetidas vezes em um mesmo arquivo não causa erro.
- Mostrar que é possível ler menos blocos que o tamanho total de um arquivo.
- Mostrar que tentar ler mais blocos do que o tamanho real do arquivo, não causa erro, porém, a leitura é feita até o fim do arquivo.
------------------------------------------------------------------------------------------
Detalhes e explicações do test3:	
É criado um sistema de arquivos, que abre apenas 50 arquivos no começo, após isso, tenta-se fechar um arquivo que não existe, retornando falha. Depois tenta-se deletar esse mesmo arquivo ( que não existe ), e o algoritmo retorna falha, pois não é possível executar essa operação.
Depois escrevesse 10x uma escrita de 1300blocos, equivalendo a mais de um bloco de indices cada vez, mostrando que o sistema de arquivos funciona corretamente. Após isso, é feita uma leitura de apenas 50 blocos nesse arquivo, e depois tenta-se fazer uma leitura de 140 blocos num aquivo que só tem 130 blocos, e em ambas as situações o código se comporta da maneira esperada; a primeira vez lendo os 50 blocos, e a segunda vez lendo 130 blocos ( que é o tamanho limite do arquivo ).
Por fim, ele fecha o arquivo que tem 13000 blocos, abre mais 50 arquivos, e abre novamente o arquivo dos 13000blocos, é possível ver que isso funciona corretamente descomentando as ultimas 2 linhas do main, que são funções que leem os descritores existentes e leem os arquivos abertos no momento.

###Objetivos test4:
- Mostrar que não é possível criar um sistema de arquivos sem o mínimo de blocos necessários para os metadados.
- Mostrar que é funciona deletar um arquivo para liberar blocos na memória para escrever blocos em outro arquivo. 
- Mostrar que é funciona a deleção de todos os arquivos criados no sistema.
------------------------------------------------------------------------------------------
Detalhes e explicações do test4:	
O test4 mostra que quando tenta-se abrir um sistema de arquivos com menos blocos do que o necessário para armazenar os metadados retorna erro. Depois tenta-se escrever mais blocos do que os blocos livres no sistema de arquivos, retornando erro; para consertar esse erro, um arquivo que está ocupando 1000 blocos é excluido, após isso, tenta-se novamente escrever aquela mesma quantidade de Blocos no arquivo que havia dado erro anteriormente, e agora esses blocos são alocados com sucesso. Por fim, ocorre a exclusão de todos os arquivos restantes, essa exclusão é comprovada com as funções "leTodosOsDescritores" e "leTodosOsArquivosAbertos" pois não mostram nenhum arquivo.

###Objetivos test5:
- Mostrar vários casos de testes "bobos" que retornem erro
- Mostrar que um único arquivo com uma quantidade enorme de blocos(47000 Blocos) não dá erro.
- Mostrar que quando fechar um arquivo, ele continua salvo no sistema de arquivos.
------------------------------------------------------------------------------------------
Detalhes e explicações do test5:	
Basicamente, o código test5 mostra que coisas que não deveriam ser aceitas pelo sistema, realmente não o são. Primeiramente, inicia-se com um sistema de arquivos com números de blocos negativos, obviamente o algoritmo detecta isso e retorna erro, depois tenta-se abrir um sistema de arquivos que não existe, obviamente, também retorna erro. No final do código, tenta-se deletar um arquivo que não está aberto, também retorna erro, pois a função delete deve APAGAR e tirar da tabela de abertos o arquivo. Escreve-se um arquivo com 47000blocos, e depois tenta-se escrever mais 5000( o que não é possível pois essa quantidade de blocos é maior que o número de blocos livres no momento); no final é mostrado os descritores e os arquivos abertos, mostrando que o programa funciona corretamente.

###Objetivos test6:
- Mostrar que a gerência de memória funciona de maneira certa, para quando o bloco escrito não está completo
------------------------------------------------------------------------------------------
Detalhes e explicações do test6:	
O arquivo test6, mostra que a função podeEscrever() implementada no cry_write do trabalho, funciona de maneira correta, pois essa função indica quantos blocos são necessários para escrever determinada quantidade de bytes. O test6 cria um sistema de arquivos com 19 blocos, ou seja, 17 reservados para os metadados, e 2(1 bloco de índices e 1 bloco de dados) reservados para 1 arquivo. Por isso, 1/4 do Bloco é escrito 4x nesse arquivo, sem dar erro, pois não falta blocos para escrever, ele precisa exatamente de 1 bloco e esse 1 bloco já está alocado.

###Objetivos test7:
- Mostrar que a execução intercalada de escritas em arquivos diferentes e tamanhos diferentes, não altera nada no funcionamento do programa.
------------------------------------------------------------------------------------------
Detalhes e explicações do test7:	
O sistema de arquivos é inicializado com 200 blocos, abre 10 arquivos e manipula 2. Intercaladamente, acrescenta 10vezes, 1 bloco no arquivo 1 e 2 blocos no arquivo 2. No final das 10 execuções, o arquivo 1 terá 10blocos de dados, e o arquivo 2 terá 20 blocos de dados. Um buffer de leitura do tamanho do arquivo 2, é usado para ler o arquivo 1( com sucesso ) e depois ler o arquivo 2 ( com sucesso ). Comprovando, mais uma vez, a eficácia do código.

