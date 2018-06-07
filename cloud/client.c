#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFSIZE 1024

int main(int argc, char* argv[]){
  char    *host = "mikasa";                /* 相手ホスト名 */
  int     port = 55555;                 /* 相手ポート番号 */
  int     sockfd;               /* ソケット記述子 */
  struct sockaddr_in      addr, my_addr;
  /* インタネットソケットアドレス構造体 */
  int     addrlen;
  char    BUFF[BUFFSIZE];       /* 送信バッファ */
  int     nbytes;               /* 送信メッセージ長 */
  struct hostent  *hp;          /* 相手ホストエントリ */

  if(argc != 2){
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
      perror("コネクション確立");
      exit(1);
    }

    //printf("Send message: ");

    FILE *fp;//読み込んだファイルへのポインタ
    char *filename;//読み込むファイル名(標準入力から与えられる)
    int i;

    for(i=1;i<argc;i++){
      filename = argv[i];
      if((fp=fopen(filename, "r")) == NULL){
	perror("can't open file\n");
	exit(1);
      }

      bzero(&BUFF,sizeof(BUFF));

      while(feof(fp) == 0){
	fgets(BUFF, BUFFSIZE, fp); /* 送信メッセージの取得 */
	nbytes = strlen(BUFF);        /* 送信メッセージ長の設定 */
	//BUFF[nbytes-1]='\0';    /* fgetsで取り込まれた文字列の最後から改行を削除 */
	/* 送信 */
	if (send(sockfd, BUFF, nbytes, 0) != nbytes) {
	  perror("送信失敗");         /* 送信失敗 */
	  exit(1);
	}
	bzero(&BUFF,sizeof(BUFF));
      }
      fclose(fp);



    }
    close(sockfd);
}
