/* TCPのサーバプログラム */

/* 以下のヘッダファイルは/usr/includeにある */
#include        <stdio.h>
#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netinet/in.h>
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
    //----------------自ポート番号の取得-------------------------
    FILE *fp_port;
    if((fp_port = fopen(port_file, "r")) == NULL){
      perror("can't open file\n");
      exit(1);
    }
    fgets(port_moji, 6, fp_port);
    port = atoi(port_moji);

    fclose(fp_port);
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
    bzero(BUFF, sizeof(BUFF));    /* 受信バッファの0クリア */

    /* クライアントからの接続受付 */
    if ((acc_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0) {
      perror("wait accept");
      exit(1);
    }

    bzero(&BUFF,sizeof(BUFF));

    int readsize,i;
    FILE *fp_write;
    char *filename_write = "a.log";
    char value[15];

    /* コピー先ファイルのオープン */
    //if ((fp_write = fopen(filename_write, "w")) == NULL) {
    //	perror("can't open recvfile");
    //	exit(1);
    //}
    int size;
    while(1){
      recv(acc_sockfd, &size,sizeof(int),0);//文字数を取得
      printf("->->->->->->----%d\n",size);
      if((readsize = recv(acc_sockfd, BUFF, size,0)) != 0){//実際の文字列を受信
        printf("%s",BUFF);
        printf("\n================================\n");
        bzero(BUFF,sizeof(BUFF));
      }else{
        printf("BREAK\n");
        break;
      }
    }

    close(acc_sockfd);  /* ソケット記述子の削除 */

    close(sockfd);      /* ソケットの終了 */

}
