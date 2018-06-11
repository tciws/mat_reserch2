/* TCPのサーバプログラム */

/* 以下のヘッダファイルは/usr/includeにある */
#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include	<string.h>
#include	<strings.h>
#include	<stdlib.h>
#include	<unistd.h>
#include <math.h>
#define BUFFSIZE 1000            /* メッセージの最大長 */
#define port_file "mngnode.conf" //ポート番号が書いてある設定ファイル
typedef struct{
  int send_max;
  int send_min;
  double send_ave;
  double send_std;
}SENDMSG;
int main(void)
{
    int     port,port_cal;                 /* 自ポート番号*/
    char    port_moji[6];
    int     sockfd,sockfd_cal, acc_sockfd,acc_sockfd_cal;   /* ソケット記述子 */
    struct sockaddr_in      addr, addr_cal,my_addr,my_addr_cal;
                                  /* インタネットソケットアドレス構造体 */
    int     addrlen,addrlen_cal;
    char    BUFF[BUFFSIZE];       /* 受信バッファ */
    int     nbytes;               /* 受信メッセージ長 */
    char *ptr;  //分割後の文字列が入るポインタ
    int TMP_VALUE[10];
    int RESULT[5] = { };
    //----------------自ポート番号の取得-------------------------

    port = 50040;
    printf("my port is %d\n",port);
    port_cal = 50140;
    printf("my port2 is %d\n",port_cal);
    //----------------------------------------------------------

    /* ソケットの生成 */
    if ((sockfd_cal = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗2");         /* ソケットの生成失敗 */
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗");         /* ソケットの生成失敗 */
        exit(1);
    }
    /* 自アドレスと自ポート番号の設定 */
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0クリア */ //クライアントノード用
    my_addr.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr.sin_port = htons(port);                 /* ポート番号 */

    bzero((char *) &my_addr_cal, sizeof(my_addr_cal));      /* 0クリア */ //クライアントノード用
    my_addr_cal.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr_cal.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr_cal.sin_port = htons(port_cal);                 /* ポート番号 */

    if (bind(sockfd_cal, (struct sockaddr *) &my_addr_cal, sizeof(my_addr_cal)) < 0) {
        perror("設定失敗");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("設定失敗2");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }

    /* ソケットを接続待ちの状態にする */
    listen(sockfd_cal, 5);
    listen(sockfd, 5);
    addrlen = sizeof (addr);
    addrlen_cal = sizeof (addr_cal);
    /* クライアントからの接続受付 */
    printf("計算ノードをアクセプト\n");
    if ((acc_sockfd_cal = accept(sockfd_cal, (struct sockaddr *)&addr_cal, &addrlen_cal)) < 0) {
      perror("wait accept2");
      exit(1);
    }
    printf("クライアントノードをアクセプト\n");
    if ((acc_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0) {
      perror("wait accept");
      exit(1);
    }

    //bzero(&BUFF,sizeof(BUFF));
    bzero(TMP_VALUE, sizeof(TMP_VALUE));    /* 受信バッファの0クリア */
    int readsize,i;
    FILE *fp_write;
    char *filename_write = "a.log";
    char value[15];
    int tmp_value;//固定長配列に書き換える？
    int size,count,sum = 0,kekka_max = 0,kekka_min = 0,sum_sum = 0;
    printf("受信待機\n");
      if(recv(acc_sockfd, TMP_VALUE,sizeof(TMP_VALUE),0) != 0){//実際の文字列を受信
          printf("受信しました\n");
          kekka_min = TMP_VALUE[0];
          if (send(acc_sockfd_cal, TMP_VALUE, sizeof(TMP_VALUE), 0) < 0) {
              perror("計算ノードへの送信失敗");
              exit(1);
          }else{
            printf("計算ノードへの送信完了\n");
          }
        bzero(TMP_VALUE,sizeof(TMP_VALUE));
        bzero(RESULT,sizeof(RESULT));
      }else{
        printf("BREAK\n");
      }
      //計算ノードからの受け取り処理
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
      SENDMSG MSG_PACK;
      /*
      typedef struct{
        int send_max;
        int send_min;
        double send_ave;
        double send_std;
      }SENDMSG;
      */
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if(recv(acc_sockfd_cal, RESULT,sizeof(RESULT),0) != 0){//実際の文字列を受信
        //計算ノードから値を受信、本実装では無限ループで実装する
          printf("計算ノードから受信しました\n");
          printf("0->%d\n",RESULT[0]);//最大値
          printf("1->%d\n",RESULT[1]);//最小値
          printf("2->%d\n",RESULT[2]);//合計値
          printf("3->%d\n",RESULT[3]);//二乗合計値
          printf("4->%d\n",RESULT[4]);//要素の個数
          printf("平均値の計算\n");
          MSG_PACK.send_ave = (double)RESULT[2]/RESULT[4];//後日書き換えあり
          printf("標準偏差の計算\n");
          MSG_PACK.send_max = RESULT[0];
          MSG_PACK.send_min = RESULT[1];
          //MSG_PACK.send_std = (double)RESULT[3]/RESULT[4]-
          if (send(acc_sockfd, &MSG_PACK, sizeof(MSG_PACK), 0) < 0) {
              perror("計算ノードへの送信失敗");
              exit(1);
          }else{
            printf("計算ノードへの送信完了\n");
          }
        }
    close(acc_sockfd);  /* ソケット記述子の削除 */
    close(acc_sockfd_cal);  /* ソケット記述子の削除 */
    close(sockfd);      /* ソケットの終了 */
    close(sockfd_cal);      /* ソケットの終了 */
}
