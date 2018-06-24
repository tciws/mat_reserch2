#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFFSIZE 1024

typedef struct{
  char *file_name;
  long file_size;
}FILE_DATA;

int compare_long(const void *a, const void *b){
  if(((FILE_DATA*)a)->file_size - ((FILE_DATA*)b)->file_size >= 0){
    return 1;
  }
  if(((FILE_DATA*)a)->file_size - ((FILE_DATA*)b)->file_size < 0){
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]){

  FILE *fp;//読み込んだファイルへのポインタ
  char *filename;//読み込むファイル名(標準入力から与えられる)
  char value[30];//一時格納用
  int i,times;
  char BUFF[1024];
  long sz;
  FILE_DATA *LOG_FILE;
  LOG_FILE = (FILE_DATA *)calloc(argc,sizeof(FILE_DATA));
  char *ptr;  //分割後の文字列が入るポインタ

  if(argc < 2){
    printf("please input 10 filename\n");
    exit(1);
  }

  for(i=1;i<argc;i++){
    LOG_FILE[i-1].file_name = argv[i];
    if((fp=fopen(LOG_FILE[i-1].file_name, "r")) == NULL){
       perror("can't open file\n");
        exit(1);
    }
    fgets(BUFF,BUFFSIZE, fp); /* 送信メッセージの取得 */
    ptr = strtok(BUFF,",");
        //1つ目のトークン表示
        times = 0;
        while( ptr = strtok(NULL, ",") ){
          if(times == 0 && ptr != NULL){
            strncpy(value,ptr+7,strlen(ptr)-7);
              LOG_FILE[i-1].file_size = (long)atoi(value);
            bzero(value,sizeof(value));
          }
      times++;
    }
    fseek( fp, 0, SEEK_END );
  sz = ftell( fp );
  LOG_FILE[i-1].file_size*=sz;
  printf( "%sファイルのサイズ:%20ld バイト \n", LOG_FILE[i-1].file_name,LOG_FILE[i-1].file_size);
  fclose( fp );
  }
  qsort(LOG_FILE, argc-1, sizeof(*LOG_FILE), compare_long);
  printf("=============================================================\n");
  for(i=1;i<argc;i++){
      printf( "%sファイルのサイズ:%20ld バイト \n", LOG_FILE[i-1].file_name,LOG_FILE[i-1].file_size);
  }
  return 0;
}
