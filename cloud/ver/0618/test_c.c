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

#define BUFFSIZE 1000

typedef struct{
  int send_max;
  int send_min;
  int send_count;
  double sum;
  double sum_sum;
}SEND_MSG;

int main(int argc, char* argv[]){
  struct timeval tv;
  time_t start;
  time_t end;
  time_t ustart;
  time_t uend;
  time_t tmp_kekka_end,tmp_kekka_start;
  double kekka_time;
  //time_t kekka_utime[10];
  char    *host = "cs-d20";                /* 相手ホスト名 */
  int     port = 50040;                 /* 相手ポート番号 */
  int     sockfd;               /* ソケット記述子 */
  struct sockaddr_in      addr, my_addr;
  /* インタネットソケットアドレス構造体 */
  int     addrlen;
  char BUFF1[BUFFSIZE-100] = {0};
  char BUFF2[100];
  char HOGE[100] = {0};
  char    END[] = "END";
  char    FILE_END[] = "FILE_END";
  int     nbytes;               /* 送信メッセージ長 */
  struct hostent  *hp;          /* 相手ホストエントリ */
  int *size;
  ////////////////////////////////////////////////////////
  gettimeofday(&tv, NULL);
  start = tv.tv_sec;
  ustart = tv.tv_usec;
  ////////////////////////////////////////////////////////
  if(argc < 2){
    printf("please input 10 filename\n");
    exit(1);
  }

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
    //============================================================
    FILE *fp;//読み込んだファイルへのポインタ
    char *filename;//読み込むファイル名(標準入力から与えられる)
    int i;
    SEND_MSG RESULT;
    //============================================================
    for(i=1;i<argc;i++){
      filename = argv[i];
      if((fp=fopen(filename, "r")) == NULL){
	       perror("can't open file\n");
	        exit(1);
      }
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
      printf("END OF FILE\n");
      nbytes = 1025;
      send(sockfd,&nbytes,sizeof(int),0);
      //send(sockfd,FILE_END,9,0);
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      //管理ノードからの結果を受け取る処理
      printf("RECIEVE RESULT TO MANAGER\n");
      recv(sockfd, &RESULT,sizeof(SEND_MSG),MSG_WAITALL);
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
      printf("FILE -> %2d.log ||| time -> %lf [sec]\n",i,kekka_time);
      ///////////////////////////////////////////////////////////////
    }
    printf("SIGNAL END\n");
    nbytes = 1026;
    send(sockfd,&nbytes,sizeof(int),0);
    //send(sockfd,END,4,0);
    close(sockfd);
    /*
    time_t time_sum = 0;
    time_t utime_sum = 0;
    for(i = 1; i < argc; i++){
      time_sum += kekka_time[i-1];
      utime_sum += kekka_utime[i-1];
    }
    printf("AVERAGE_TIME %lf [sec]\n",(double)utime_sum/1000000);
    */
    printf("FINAL TIME -> %lf [sec]\nAVERAGE TIME -> %lf[sec]\n",i,kekka_time,kekka_time/10);
}
