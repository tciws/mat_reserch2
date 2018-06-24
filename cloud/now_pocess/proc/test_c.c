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
#include <sys/types.h>  // fork/wait
#include <sys/wait.h>   // fork/wait
#include <err.h>
#include <errno.h>

#define PROC_DEB(print_pid) printf("NOW PROCESS ID is No.%d   ",print_pid)
#define P_MAX  2

#define BUFFSIZE 500

typedef struct{
  int send_max;
  int send_min;
  int send_count;
  double sum;
  double sum_sum;
}SEND_MSG;

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
  /////////////////////////////////////////////////////////////////
  int pid[P_MAX];
  int status,child;
  struct timeval tv;
  time_t start;
  time_t end;
  time_t ustart;
  time_t uend;
  time_t tmp_kekka_end,tmp_kekka_start;
  double kekka_time;
  char    END[] = "END";
  char    FILE_END[] = "FILE_END";
  /////////////////////////////////////////////////////////////////
  //time_t kekka_utime[10];
  char    *host = "cs-d30";                /* 相手ホスト名 */
  char* file_array1[5];
  char* file_array2[5];
  FILE_DATA *LOG_FILE;
  //char HOGE[100] = {0};
  //int *size;
  ////////////////////////////////////////////////////////
  gettimeofday(&tv, NULL);
  start = tv.tv_sec;
  ustart = tv.tv_usec;
  ////////////////////////////////////////////////////////
  if(argc < 2){
    printf("please input 10 filename\n");
    exit(1);
  }
  {
    //####################################################
    long sz;
    LOG_FILE = (FILE_DATA *)calloc(argc,sizeof(FILE_DATA));
    char *ptr;  //分割後の文字列が入るポインタ
    //============================================================
    FILE *fp;//読み込んだファイルへのポインタ
    char *filename;//読み込むファイル名(標準入力から与えられる)
    int i,count1 = 0,count2 = 0;
    SEND_MSG RESULT;
    long int tmp1 = 0,tmp2 = 0;
    char BUFF1[70];
    char value[30];
    int times;
    //============================================================
    //####################################################
    for(i=1;i<argc;i++){
      LOG_FILE[i-1].file_name = argv[i];
      if((fp=fopen(LOG_FILE[i-1].file_name, "r")) == NULL){
         perror("can't open file\n");
          exit(1);
      }
      fgets(BUFF1,70, fp); /* 送信メッセージの取得 */
      ptr = strtok(BUFF1,",");
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
    //return -1;
  }
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//ファイル名を、２つの配列の中に格納する処理を記述する必要がある
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//####################################################################//
/*
子プロセス生成。子プロセスは次の行から始まるため、
このような記述をすると、子プロセスが子プロセスを生成しないで済む。
*/
for( child=0 ; child < P_MAX && (pid[child] = fork()) > 0 ; child++ );
//####################################################################//
if (child == P_MAX) {
      for(child = 0 ; child < P_MAX ; child++ ){
        wait(&status);
      }
     /* 終了ステータスのチェック */
      printf("親プロセス終了\n");
    }else if( pid[child] == 0){
  {
    int i;
  for(i = 1; i < argc; i+=2){
    if(child == 0){
      printf("file_array  %s\n",LOG_FILE[i-1].file_name);
    }else{
      printf("file_array  %s\n",LOG_FILE[i].file_name);
    }
    //printf("file_array2   %s\n",LOG_FILE[i-1].file_name);
  }
  }
  //################################################################
  int     port = 0;                 /* 相手ポート番号 */
  if(pid[0] == 0){
    port = 50040; //子プロセス1のポート番号
  }
  if(pid[1] == 0){
    port = 50240; //子プロセス2のポート番号
  }
  int     sockfd;               /* ソケット記述子 */
  struct sockaddr_in      addr, my_addr;
  /* インタネットソケットアドレス構造体 */
  int     addrlen;
  char BUFF1[BUFFSIZE] = {0};
  char BUFF2[BUFFSIZE] = {0};
  int     nbytes;               /* 送信メッセージ長 */
  struct hostent  *hp;          /* 相手ホストエントリ */
  //============================================================
  FILE *fp;//読み込んだファイルへのポインタ
  char *filename;//読み込むファイル名(標準入力から与えられる)
  int i;
  SEND_MSG RESULT;
  //============================================================
  //################################################################
  //子プロセス
  /* 相手ホストエントリの取得 */
  if ((hp = gethostbyname(host)) == NULL) {
    perror("gethostbyname");  /* 相手ホストエントリ取得失敗 */
    exit(1);
  }

  /* 相手アドレスと相手ポート番号の設定 */
    bzero((char *)&addr, sizeof (addr));            /* 0クリア */
    addr.sin_family = AF_INET;                      /* アドレスファミリ */
    bcopy(hp->h_addr, (char *)&addr.sin_addr, hp->h_length);
                                                    /* アドレス */
    addr.sin_port = htons(port);                    /* ポート番号 */

    /* ソケットの生成 */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗");         /* ソケットの生成失敗 */
        exit(1);
    }

    /* 自アドレスと自ポート番号の設定 */
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0クリア */
    my_addr.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr.sin_port = htons(0);                    /* ポート番号 */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("設定失敗");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }

    addrlen = sizeof (addr);

    /* サーバとのコネクション確立 */
    if (connect(sockfd, (struct sockaddr *)&addr, addrlen) < 0) {
      perror("CONNECTION COMPLETE");
      exit(1);
    }

    //printf("Send message: ");
    for(i=1;i<argc;i+=2){
      if(pid[0] == 0){
        if((fp=fopen(LOG_FILE[i-1].file_name, "r")) == NULL){
  	       printf("can't open %s\n",filename);
  	        exit(1);
        }
      }
      if(pid[1] == 0){
        if((fp=fopen(LOG_FILE[i].file_name, "r")) == NULL){
  	       printf("can't open %s\n",filename);
  	        exit(1);
        }
      }
      //filename = LOG_FILE[i].file_name;
      bzero(BUFF1,sizeof(BUFF1));
      nbytes = 0;
      while(feof(fp) == 0){
        fgets(BUFF1, BUFFSIZE, fp); /* 送信メッセージの取得 */
        //printf("%s", BUFF1);
        nbytes = strlen(BUFF1);
        send(sockfd,&nbytes,sizeof(int),0);
        if (send(sockfd, BUFF1, nbytes, 0) < 0) {
        	  perror("送信失敗");
        	  exit(1);
        }
        bzero(BUFF1,sizeof(BUFF1));
      }
      fclose(fp);
      PROC_DEB(child);
      printf("END OF FILE\n");
      nbytes = 1025;
      send(sockfd,&nbytes,sizeof(int),0);
      //send(sockfd,FILE_END,9,0);
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      //管理ノードからの結果を受け取る処理
      PROC_DEB(child);
      printf("RECIEVE RESULT TO MANAGER\n");
      recv(sockfd, &RESULT,sizeof(SEND_MSG),MSG_WAITALL);
      printf("                                                  COMPLETE\n");
      printf("FINAL ANSWER\n");
      printf("max=%d\n",RESULT.send_max);
      printf("min=%d\n",RESULT.send_min);
      printf("sum=%d\n",(int)RESULT.sum);
      printf("std=%d\n",(int)RESULT.sum_sum);
      printf("counter=%d\n",RESULT.send_count);
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      ///////////////////////////////////////////////////////////////
      gettimeofday(&tv, NULL);
      end = tv.tv_sec;
      uend = tv.tv_usec;
      tmp_kekka_start = start*1000000 + ustart;
      tmp_kekka_end = end*1000000 + uend;
      kekka_time = (double)(tmp_kekka_end - tmp_kekka_start)/1000000;
      //kekka_utime[i-1] = uend-ustart;
      PROC_DEB(child);
      printf("FILE -> %2d.log ||| time -> %lf [sec]\n",i,kekka_time);
      ///////////////////////////////////////////////////////////////
    }
    printf("SIGNAL END\n");
    nbytes = 1026;
    send(sockfd,&nbytes,sizeof(int),0);
    //send(sockfd,END,4,0);
    close(sockfd);
  //exit(0);
}else{
    perror("child process") ;
    exit(0);
}
    /*
    time_t time_sum = 0;
    time_t utime_sum = 0;
    for(i = 1; i < argc; i++){
      time_sum += kekka_time[i-1];
      utime_sum += kekka_utime[i-1];
    }
    printf("AVERAGE_TIME %lf [sec]\n",(double)utime_sum/1000000);
    */
    printf("FINAL TIME -> %lf [sec]\nAVERAGE TIME -> %lf[sec]\n",kekka_time,kekka_time/10);
}
