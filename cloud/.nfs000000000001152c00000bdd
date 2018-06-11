#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFSIZE 1024
int main(int argc, char* argv[]){
	//char *s1="abc";
	//char *s2="def";
char BUFF[BUFFSIZE];
char *tmp;
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

bzero(&BUFF,sizeof(BUFF));
nbytes = 0;
while(feof(fp) == 0){
fgets(tmp, BUFFSIZE, fp); /* 送信メッセージの取得 */
printf("%s", tmp);
//fread(BUFF, sizeof(char), 1023, fp);
if(nbytes+strlen(tmp)<=BUFFSIZE){
  printf("##%d\n",nbytes);
  strncat(BUFF, tmp, strlen(tmp));
  nbytes = strlen(BUFF);        /* 送信メッセージ長の設定 */
}else{
  printf("送信処理を書く\n");
  bzero(&BUFF,sizeof(BUFF));
  }
}
fclose(fp);
	//printf("付加後のs2の値：%s", tmp);
}
