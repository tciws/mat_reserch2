#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
  char    *host = "cs-d10";                /* 相手ホスト名 */
  int     port = 50040;                 /* 相手ポート番号 */
  int     sockfd;               /* ソケット記述子 */
  struct sockaddr_in      addr, my_addr;
  /* インタネットソケットアドレス構造体 */
  int     addrlen;
  int BUFF1[10] = {1,2,3,4,5,6,7,8,9,10};
  int Post[5] = { };
  int     nbytes;               /* 送信メッセージ長 */
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
    if (connect(sockfd, (struct sockaddr *)&addr, addrlen) < 0) {
      perror("コネクション確立");
      exit(1);
    }
    //printf("Send message: ");
    printf("送信中\n");
    if (send(sockfd, BUFF1, sizeof(int)*10, 0) < 0) {
        perror("送信失敗");
        exit(1);
    }else{
          printf("送信しました。\n受信待機\n");
        if((recv(sockfd, Post, sizeof(Post),MSG_WAITALL)) != 0){//実際の文字列を受信
          printf("MAX=%d\n",Post[0]);
          printf("MIN=%d\n",Post[1]);
          printf("SUM=%d\n",Post[2]);
          printf("SUM AND SUM=%d\n",Post[3]);
          printf("COUNTER=%d\n",Post[4]);
          printf("\n================================\n");
        }
      }
    close(sockfd);
}
