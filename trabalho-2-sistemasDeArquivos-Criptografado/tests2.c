#include <stdlib.h>
#include <stdio.h>
#include "crypfs.h"
#include <assert.h>
#include <string.h> 


#define TESTDIR "testes/"

#define OK(a,b) printf("test%d:%d OK\n",a,b)
#define FAIL(a,b) printf("test%d:%d FAIL\n",a,b)

/* quantos descritores de arquivo por bloco? */
#define NUM_DESCS (BLOCO/sizeof(descritor_t))

/* ultimo bloco para blocos livres */
#define METADADOS (256/NUM_DESCS + 2)

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
int test1(){

  char fsname[80];
  int grade = 0;

  /* nome do arquivo sera testes/testexxx */
  assert(concat(concat(fsname,"TESTDIR"),"test1"));
  strcpy(fsname, "test1Pilla");
  printf("Metadados %d\n", METADADOS); 

  /* poucos blocos */
  if (initfs(fsname, 5)==FALHA){
    OK(1,1);
    grade++;
  }
  else{
    FAIL(1,1);
    if (unlink(fsname)){ /* mentiu sobre criacao do arquivo */
      grade--;
    }
  }


  /* quantidade razoavel de blocos */

  if (initfs(fsname, METADADOS*20)==SUCCESSO){
    OK(1,2);
    grade++;
  }
  else {
      FAIL(1,2);
      /* nao tem como continuar */
      return 0;
  }
  return grade;
}

int test2(){

    char fsname[80];
    int grade = 0;
    cry_desc_t * cryme;
    indice_arquivo_t  FH;

    /* nome do arquivo sera testes/testexxx */
    assert(concat(concat(fsname,"TESTDIR"),"test2"));
  	strcpy(fsname, "test2Pilla");
    /* quantidade razoavel de blocos */

    if (initfs(fsname, METADADOS*100)==SUCCESSO){
      OK(2,1);
      grade++;
    }
    else {
        FAIL(2,1);
        /* nao tem como continuar */
        return 0;
    }

    cryme = cry_openfs(fsname);
    if (!cryme){
      FAIL(2,2);
      /*  nao tem como continuar */
      return 0;
    }
    else{
      OK(2,2);
      grade++;
    }

    /* deve falhar, arquivo ainda nao existe */
    FH = cry_open(cryme, "teste1", LEITURA, 0);
    if (FH==FALHA){
      OK(2,3);
      grade++;
    }
    else{
      FAIL(2,3);
    }

    /* deve falhar, ja que arquivo nao existe */
    FH = cry_open(cryme, "teste2", LEITURA, 3);
    if (FH==FALHA){
      OK(2,4);
      grade++;
    }
    else{
      FAIL(2,4);
    }
    /* escreve um bloco */
    FH = cry_open(cryme, "teste3", ESCRITA, 3);
    if (FH==FALHA){
      FAIL(2,5);
    }
    else{
      OK(2,5);
      if(cry_write(FH, 80, fsname)==SUCCESSO){
        char buffer[4096];
        cry_close(FH);

        FH = cry_open(cryme, "teste3", LEITURA, 3);
        if (FH==FALHA){
          FAIL(2,6);
        }
        else OK(2,6);

        if(cry_read(FH, 80, buffer)!=80){
            FAIL(2,7);
        } else{
          OK(2,7);
          /* verifica se mesmo conteudo que escreveu */
          if (strcmp(buffer, fsname)!=0){
            FAIL(2,8);
          }
          else{
            OK(2,8);
            grade++;
          }
        }
        cry_close(FH);
        /* abre com deslocamento errado */
        FH = cry_open(cryme, "teste3", LEITURA, 5);
        if (FH==FALHA){
          FAIL(2,9);
        }
        else OK(2,9);

        if(cry_read(FH, 80, buffer)!=80){
            FAIL(2,10);
        } else{
          OK(2,10);
          /* verifica se mesmo conteudo que escreveu */
          /* se 2.8 falhou, aqui pode dar certo e falhar */
          if (strcmp(buffer, fsname)==0){
            FAIL(2,11);
          }
          else{
            OK(2,12);
            grade++;
          }
        }
      }
      cry_close(FH);
    }
    return grade;
}



/* Forca a parte de gerencia de blocos livres a se virar */

int test3(){

    char fsname[80];
    int grade = 0;
    cry_desc_t * cryme;
    indice_arquivo_t  FH;

    char buffer[4096];
    int i;

    /* teste com um monte de a */
    for(i=0; i<4095; i++){
      buffer[i] = 'a';
    }
    buffer[4095]='\0';

    /* nome do arquivo sera testes/testexxx */
    assert(concat(concat(fsname,"TESTDIR"),"test3"));
  	strcpy(fsname, "test3Pilla");
    /* quantidade razoavel de blocos */

    if (initfs(fsname, METADADOS+30)==SUCCESSO){ // MODIFICADO AQUI PARA MAIS 30, SE NÃO, NAO FAZ SENTIDO O WHILE(CRY_WRITE)
      OK(3,1);
    }
    else {
        FAIL(3,1);
        /* nao tem como continuar */
        return grade;
    }

    cryme = cry_openfs(fsname);
    if (!cryme){
      FAIL(3,2);
      /*  nao tem como continuar */
      return grade;
    }
    else{
      OK(3,2);
    }


    /* escreve ate nao conseguir mais */
    FH = cry_open(cryme, "teste3", ESCRITA, 3);
    if (FH==FALHA){
      FAIL(3,3);
      return grade;
    }

    int bytes;
    OK(3,3);
    uint32_t vezes = 0;

    while (cry_write(FH, 4096, buffer) == SUCCESSO){
      vezes++;
      /*/if (vezes>30){ /*Escrevendo mais blocos que existem no FS */ 
      if (vezes > 32){ /* EXPLICAR QUE É POR CAUSA DOS METADADOS, Quando perguntando a ele num final de aula, ele disse que os metadados poderiam ser contiguos, sendo contiguos eles ocupam 17 blocos */
       	FAIL(3,4);
        return grade;
      }
    }
    if (vezes<0){
      FAIL(3,4);
      return grade;
    }
    OK(3,4);

    cry_delete(FH);

    FH = cry_open(cryme, "teste3", LEITURA, 3);
    if (FH==SUCCESSO){ /* foi removido */
      FAIL(3,5);
      return grade;
    }
    OK(3,5);
    grade++;

    /* escreve vezes blocos - 1, deve ter que usar um bloco que foi liberado */

    FH = cry_open(cryme, "teste4", ESCRITA, 3);
    if (FH==FALHA){
      FAIL(3,7);
      return grade;
    }
    else OK(3,7);

    for(i=0; i<vezes; i++){
      if (cry_write(FH, 4096, buffer)==FALHA){
        FAIL(3,8);
        cry_close(FH);
        return grade;
      }
    }
    cry_close(FH);
    OK(3,8);
    grade++;

    /* testa se escreveu bonitinho */
    FH = cry_open(cryme, "teste4", LEITURA, 3);
    if (FH==FALHA){
      FAIL(3,9);
      return grade;
    }
    OK(3,9);
    for(i=0; i<vezes; i++){
      char tmp[4096];
      if (cry_read(FH, 4096, tmp)==FALHA){
        FAIL(3,10);
        cry_close(FH);
        return grade;
      }
      if(strcmp(buffer,tmp)!=0){
        FAIL(3,11);
        return grade+1;
      }
    }
    OK(3,10);
    OK(3,11);
    grade=+4;

    cry_close(FH);
    return grade;
}





/* verifica se consegue ter 256 arquivos no sistema */
int test4(){

  char fsname[80];
  int grade = 0;
  int i;
  char * buffer = "aaaaaa";
  cry_desc_t * cryme;
  indice_arquivo_t  FH;


  /* nome do arquivo sera testes/testexxx */
  assert(concat(concat(fsname,"TESTDIR"),"test1"));
  strcpy(fsname, "test4Pilla");
  /* quantidade razoavel de blocos */

  if (initfs(fsname, METADADOS*1000)==SUCCESSO){
    OK(4,1);
  }
  else {
      FAIL(4,1);
      /* nao tem como continuar */
      return grade;
  }


  cryme = cry_openfs(fsname);
  if (!cryme){
    FAIL(4,2);
    /*  nao tem como continuar */
    return grade;
  }
  else{
    OK(4,2);
  }

  /* cria 256 arquivos de 1 bloco */
  for(i=0; i<256; i++){
    char name[80];
    sprintf(name,"a%d",i);
    FH = cry_open(cryme, name, ESCRITA, i);
    if (FH==FALHA){
      FAIL(4,3);
      return grade;
    }
    if (cry_write(FH,80, name)==0){
      FAIL(4,4);
      return grade;
    }
    cry_close(FH);
  }
  OK(4,3);
  OK(4,4);
  grade++;

  /* abre todo mundo, verifica conteudo e apaga */
  /* cria 256 arquivos de 1 bloco */
  for(i=0; i<256; i++){
    char name[80];
    char tmp[80];
    sprintf(name,"a%d",i);
    FH = cry_open(cryme, name, LEITURA, i);
    if (FH==FALHA){
      FAIL(4,5);
      return grade;
    }
    if (cry_read(FH,80, tmp)==0){
      FAIL(4,6);
      return grade;
    }
    if (strcmp(tmp,name)!=0){
      FAIL(4,7);
      return grade+1;
    }
    cry_close(FH);
  }
  OK(4,5);
  OK(4,6);
  OK(4,7);

  grade=+4;




  return grade;
}



/* faz o show acontecer */
int main(int argc, char ** argv){

  int grades[4], maxgrades[4], pesos[4];
  float finalgrade = 0.0;

#define GRADEIT(x)\
  printf("TESTE %d - nota %d de %d, total %f\n", x, grades[x-1],\
   maxgrades[x-1], grades[x-1]/(float)maxgrades[x-1]); \
   finalgrade = finalgrade + grades[x-1]/(float)maxgrades[x-1]


  maxgrades[0]=2; pesos[0]=2;
  maxgrades[1]=6; pesos[1]=3;
  maxgrades[2]=6; pesos[2]=3;
  maxgrades[3]=5; pesos[3]=2;

  grades[0] = grades[1] = grades[2] = grades[3] = 0;

  assert(pesos[0]+pesos[1]+pesos[2]+pesos[3] == 10);

  grades[0]=test1();
  GRADEIT(1);
  if(grades[0]>0){
    grades[1]=test2();
    GRADEIT(2);
    if(grades[1]>0){
      grades[2]=test3();
      GRADEIT(3);
    }
    grades[3]=test4();
    GRADEIT(4);
  }

  printf("NOTA FINAL %.2f\n",finalgrade);

  return 0;
}
