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
#include <assert.h>

#define P_MAX  2             //プロセス数

typedef struct{
  int send_max;
  int send_min;
  int send_count;
  double sum;
  double sum_sum;
}SEND_MSG;

/////////////////////////////////////////
/////////////////////////////////////////
long long int counter_array[10];
////////////////////////////////////////
////////////////////////////////////////
//void __attribute__((optimize("O0"))) counter_value(int tid);
long long int* get_counter_by_tid(long long int tid){
  return &counter_array[tid];
}
void __attribute__((optimize("O0")))counter_value(long long int tid){
volatile long long int *counter;
//トランザクション固有のカウンタ変数を取得
  counter = get_counter_by_tid(tid);
  (*counter)++;
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
int main(void){

  int pid[P_MAX];
  int status,child;

  for( child=0 ; child < P_MAX && (pid[child] = fork()) > 0 ; child++ );

  if( child == P_MAX ){			//親プロセスはすべての子プロセスの終了を待つ
    for(  child = 0 ; child < P_MAX ; child++ ){
      wait(&status);
    }
  }else if( pid[child] == 0){

    char    *host = "cs-d20";                /* 相手ホスト名 */
    int     port = 0;                 /* 相手ポート番号 */
    if(child == 0){
      port = 50240;
    }
    if(child == 1){
      port = 50340;
    }
    int     sockfd;               /* ソケット記述子 */
    struct sockaddr_in      addr, my_addr;
    /* インタネットソケットアドレス構造体 */
    int     addrlen;
    int BUFF1[2560] = { }; //170Kbyte
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
      printf("CONNECTION COMPLETE\n");
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      //int RESULT[5] = { };
      int kekka_max=0,kekka_min=INT_MAX,count;
      double sum = 0.0,sum_sum = 0.0;
      int     nbytes;               /* 送信メッセージ長 */
      SEND_MSG RESULT;
      RESULT.send_count = 0;
      long long int tid = 0;
      int count_value = 0;
      long long int total = 0;
      int temp;
      int i;
      int tmp_count = 0;
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      //---------------------------------------------------------------
      //管理ノードからのデータを受信

      if (connect(sockfd, (struct sockaddr *)&addr, addrlen) < 0) {
        perror("CONNECTION COMPLETE");
        exit(1);
      }
    recv(sockfd, &tid,sizeof(long long int),MSG_WAITALL);//文字数を取得
    printf("RECEIVE tid[%ld]\n",tid);
    while(1){
      //受け取るサイズを管理ノードから受け取る
      recv(sockfd, &nbytes,sizeof(int),MSG_WAITALL);//文字数を取得
      printf("nbytes = %d\n", nbytes);
      if(nbytes == 16000){
        assert(total == *get_counter_by_tid(tid)); /* assert.h */
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
        RESULT.send_count = tmp_count;
        //RESULT.send_count = count;
        //==========================================================
        printf("SEND RESULT TO MANAGER NODE...\n");
        send(sockfd,&RESULT,sizeof(SEND_MSG),0);
        //==========================================================
        kekka_max=0;
        kekka_min=INT_MAX;
        sum = 0.0;
        sum_sum = 0.0;
        total = 0;
        //count = 0;
        tmp_count = 0;
        //RESULT.send_count = 0;
        recv(sockfd, &tid,sizeof(long long int),MSG_WAITALL);//文字数を取得
        printf("RECEIVEll tid[%ld]\n",tid);
      }else if(nbytes == 26000){
        printf("SIGNAL END\n");
        break;
      }else{
      if(recv(sockfd, BUFF1,nbytes*sizeof(int),MSG_WAITALL) != 0){//実際の文字列を受信
          printf("RECEIVE COMPLETE\n");
          //printf("%d\n",BUFF1[0]);
          //kekka_min = BUFF1[0];
          //要素の計算
          for(count = 0; count < nbytes;count++){
          //printf("count = %10d ||| value = %10d\n",RESULT.send_count,BUFF1[count]);
            //kekka_max = max(BUFF1[count],kekka_max);
            //kekka_min = min(BUFF1[count],kekka_min);
            if(BUFF1[count] > kekka_max){
              kekka_max = BUFF1[count];
            }
            if(BUFF1[count] < kekka_min){
              kekka_min = BUFF1[count];
            }
            sum += (double)BUFF1[count];
            sum_sum += (double)BUFF1[count]*BUFF1[count];
            tmp_count++;
            //RESULT.send_count++;

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            //負荷処理
            count_value=BUFF1[count];
            for (i = 0; i < count_value; i++) {
            //printf("%d\n",i);
            //temp = (int)tid;
            counter_value(tid); /* valueの回数分実行 */
            }
            total += count_value;
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
          }
            //printf("MAX = %10d ||| MIN = %10d\n",kekka_max,kekka_min);
          /*
          RESULT[0] = kekka_max;
          RESULT[1] = kekka_min;
          RESULT[2] = sum;
          RESULT[3] = sum_sum;
          RESULT[4] = count;
          */
        }else{
           printf("RECEIVE ERROR\n");
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

    exit(0);
  }else{
      perror("child process") ;
      exit(0);
  }
}
