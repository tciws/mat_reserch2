#include  <stdio.h>
#include	<string.h>
#include	<strings.h>
#include	<stdlib.h>
#include <sys/time.h>
int main(void){
  struct timeval tv;
  time_t start;
  time_t end;
  time_t ustart;
  time_t uend;
  time_t tmp_kekka_end,tmp_kekka_start;
  double kekka_time;
  char BUFF[100];
  char *ptr;
  char value[30];
  int i;
  FILE *fp;//読み込んだファイルへのポインタ
  gettimeofday(&tv, NULL);
  start = tv.tv_sec;
  ustart = tv.tv_usec;
  if((fp=fopen("6.log", "r")) == NULL){
     perror("can't open file\n");
      exit(1);
  }
  while(feof(fp) == 0){
    fgets(BUFF, 100, fp); /* 送信メッセージの取得 */

    ptr = strtok(BUFF,",");
        //1つ目のトークン表示
        //puts(ptr);
        //トークンがNULLになるまでループ
        i = 0;
        while( ptr = strtok(NULL, ",") ){
          if(i == 0 && ptr != NULL){
            strncpy(value,ptr+7,strlen(ptr)-7);
              printf("%d\n",atoi(value));
            bzero(value,sizeof(value));
          }
          bzero(BUFF,sizeof(BUFF));
            i++;
        }

    //printf("%s", BUFF1);
  }
  gettimeofday(&tv, NULL);
  end = tv.tv_sec;
  uend = tv.tv_usec;
  tmp_kekka_start = start*1000000 + ustart;
  tmp_kekka_end = end*1000000 + uend;
  kekka_time = (double)(tmp_kekka_end - tmp_kekka_start)/1000000;
  //kekka_utime[i-1] = uend-ustart;
  printf("FILE -> %2d.log ||| time -> %lf [sec]\n",i,kekka_time);
  return 0;
}
