#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFSIZE 1024
int main(int argc, char* argv[]){
	//char *s1="abc";
	//char *s2="def";
char BUFF1[BUFFSIZE-100];
char BUFF2[100];
int nbytes;
	//printf("付加前のs1の値：%s", BUFF);
	//printf("付加前のs2の値：%s", tmp);
if(argc != 2){
  printf("please input 10 filename\n");
  exit(1);
}
FILE *fp;//読み込んだファイルへのポインタ
char *filename;//読み込むファイル名(標準入力から与えられる)
int i;

filename = argv[1];
if((fp=fopen(filename, "r")) == NULL){
  perror("can't open file\n");
  exit(1);
}

bzero(BUFF2,sizeof(BUFF2));
nbytes = 0;
while(feof(fp) == 0){
  fgets(BUFF2, BUFFSIZE, fp); /* 送信メッセージの取得 */
  //printf("%s", BUFF2);
//fread(BUFF, sizeof(char), 1023, fp);
  //printf("##%d\n",nbytes);
  if(nbytes+strlen(BUFF2)<=BUFFSIZE-100){
    strncat(BUFF1, BUFF2, strlen(BUFF2));
    nbytes += strlen(BUFF2);
  }else{
    printf("変態紳士%lu\n",nbytes+strlen(BUFF2));
    nbytes = 0;
    printf("送信処理を書く\n");
    //
    bzero(BUFF1,sizeof(BUFF1));
  }
  bzero(BUFF2,sizeof(BUFF2));
}
fclose(fp);
	//printf("付加後のs2の値：%s", tmp);
}
