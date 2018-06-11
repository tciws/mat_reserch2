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
#define BUFFSIZE 1000            /* メッセージの最大長 */
#define port_file "mngnode.conf" //ポート番号が書いてある設定ファイル
int main(void)
{
    int     port;                 /* 自ポート番号*/
    char    port_moji[6];
    int     sockfd, acc_sockfd;   /* ソケット記述子 */
    struct sockaddr_in      addr, my_addr;
                                  /* インタネットソケットアドレス構造体 */
    int     addrlen;
    char    BUFF[BUFFSIZE];       /* 受信バッファ */
    int     nbytes;               /* 受信メッセージ長 */
    char *ptr;  //分割後の文字列が入るポインタ
    int TMP_VALUE[10];
    int RESULT[5] = { };
    //----------------自ポート番号の取得-------------------------

    port = 50040;
    printf("my port is %d\n",port);
    //----------------------------------------------------------

    /* ソケットの生成 */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗");         /* ソケットの生成失敗 */
        exit(1);
    }

    /* 自アドレスと自ポート番号の設定 */
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0クリア */
    my_addr.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr.sin_port = htons(port);                 /* ポート番号 */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("設定失敗");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }

    /* ソケットを接続待ちの状態にする */
    listen(sockfd, 5);

    addrlen = sizeof (addr);

    /* クライアントからの接続受付 */
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
        for(count = 0; count < 10;count++){
          printf("%d\n",TMP_VALUE[count]);
          kekka_max = max(TMP_VALUE[count],kekka_max);
          kekka_min = min(TMP_VALUE[count],kekka_min);
          sum += TMP_VALUE[count];
          sum_sum +=TMP_VALUE[count]*TMP_VALUE[count];
        }
        RESULT[0] = kekka_max;
        RESULT[1] = kekka_min;
        RESULT[2] = sum;
        RESULT[3] = sum_sum;
        RESULT[4] = count;
        if (send(acc_sockfd, RESULT, sizeof(RESULT), 0) < 0) {
            perror("送信失敗");
            exit(1);
        }else{
          printf("送信完了\n");
        }
        bzero(TMP_VALUE,sizeof(TMP_VALUE));
      }else{
        printf("BREAK\n");
      }

    close(acc_sockfd);  /* ソケット記述子の削除 */
    close(sockfd);      /* ソケットの終了 */
}
int max(int a,int b){
  if(a >= b){
    return a;
  }
  if(a < b){
    return b;
  }
  return 0;
}
int min(int a,int b){
  if(a < b){
    return a;
  }
  if(a >= b){
    return b;
  }
  return 0;
}
