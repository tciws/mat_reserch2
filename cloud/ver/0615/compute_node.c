#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

typedef struct{
  int send_max;
  int send_min;
  int send_count;
  double sum;
  double sum_sum;
}SEND_MSG;

int main(void){
  char    *host = "cs-d10";                /* 相手ホスト名 */
  int     port = 50140;                 /* 相手ポート番号 */
  int     sockfd;               /* ソケット記述子 */
  struct sockaddr_in      addr, my_addr;
  /* インタネットソケットアドレス構造体 */
  int     addrlen;
  int BUFF1[15360] = { }; //170Kbyte
  //int Post[5] = { };
  struct hostent  *hp;          /* 相手ホストエントリ */
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
    //自アドレスと自ポート番号の設定
    bzero((char *) &my_addr, sizeof(my_addr));      //0クリア
    my_addr.sin_family = AF_INET;                   //アドレスファミリ
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //アドレス
    my_addr.sin_port = htons(0);                    //ポート番号
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("設定失敗");         //自アドレスと自ポート番号の設定失敗
        exit(1);
    }
    addrlen = sizeof (addr);
    /* サーバとのコネクション確立 */
    printf("コネクション確立しました\n");
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //int RESULT[5] = { };
    int kekka_max=0,kekka_min=0,count;
    double sum = 0.0,sum_sum = 0.0;
    int     nbytes;               /* 送信メッセージ長 */
    SEND_MSG RESULT;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //---------------------------------------------------------------
    //管理ノードからのデータを受信

    if (connect(sockfd, (struct sockaddr *)&addr, addrlen) < 0) {
      perror("コネクション確立");
      exit(1);
    }
  while(1){
    //受け取るサイズを管理ノードから受け取る
    recv(sockfd, &nbytes,sizeof(int),MSG_WAITALL);//文字数を取得
    printf("nbytes = %d\n", nbytes);
    if(nbytes == 16000){
      printf("FINAL ANSWER\n");
      printf("max=%d\n",kekka_max);
      printf("min=%d\n",kekka_min);
      printf("sum=%lf\n",sum);
      printf("sum_sum=%lf\n",sum_sum);
      printf("FILE END...\n");

      RESULT.send_max = kekka_max;
      RESULT.send_min = kekka_min;
      RESULT.sum = sum;
      RESULT.sum_sum = sum_sum;
      RESULT.send_count = count;
      //==========================================================
      printf("管理ノードに結果を送信\n");
      send(sockfd,&RESULT,sizeof(SEND_MSG),0);
      //==========================================================
      kekka_max=0;
      kekka_min=0;
      sum = 0.0;
      sum_sum = 0.0;
      count = 0;
    }else if(nbytes == 26000){
      printf("通信終了\n");
      break;
    }else{
    if(recv(sockfd, BUFF1,nbytes*sizeof(int),MSG_WAITALL) != 0){//実際の文字列を受信
        printf("受信しました\n");
        //printf("%d\n",BUFF1[0]);
        kekka_min = BUFF1[0];
        //要素の計算
        for(count = 0; count < nbytes;count++){
          printf("count = %10d||| value = %10d\n",count,BUFF1[count]);
          kekka_max = max(BUFF1[count],kekka_max);
          kekka_min = min(BUFF1[count],kekka_min);
          sum += (double)BUFF1[count];
          sum_sum += (double)BUFF1[count]*BUFF1[count];
        }

        /*
        RESULT[0] = kekka_max;
        RESULT[1] = kekka_min;
        RESULT[2] = sum;
        RESULT[3] = sum_sum;
        RESULT[4] = count;
        */
      }else{
         printf("受信失敗\n");
         return -1;
      }
    }
  }
  /*
        if (send(sockfd, RESULT, sizeof(RESULT), 0) < 0) {
            perror("送信失敗");
            exit(1);
        }else{
          printf("送信完了\n");
        }
*/
      //----------------------------------------------------------
    close(sockfd);
}
//最大値を返す関数
int max(int a,int b){
  if(a >= b){
    return a;
  }
  if(a < b){
    return b;
  }
  return 0;
}
//最小値を返す関数
int min(int a,int b){
  if(a < b){
    return a;
  }
  if(a >= b){
    return b;
  }
  return 0;
}
