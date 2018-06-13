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
#define VALUEMAX 15360           //Valueの値を格納できる配列のサイズ
#define port_file "mngnode.conf" //ポート番号が書いてある設定ファイル
int main(void)
{
    int     port,port_cal;                 /* 自ポート番号*/
    char    port_moji[6];
    int     sockfd, acc_sockfd,sockfd_cal, acc_sockfd_cal;   /* ソケット記述子 */
    struct sockaddr_in      addr,addr_cal, my_addr,my_addr_cal;
                                  /* インタネットソケットアドレス構造体 */
    int     addrlen,addrlen_cal;
    char    BUFF[BUFFSIZE];       /* 受信バッファ */
    int     nbytes;               /* 受信メッセージ長 */
    char *ptr;  //分割後の文字列が入るポインタ
    //----------------自ポート番号の取得-------------------------
    FILE *fp_port;
    if((fp_port = fopen(port_file, "r")) == NULL){
      perror("can't open file\n");
      exit(1);
    }
    fgets(port_moji, 6, fp_port); //クライアントノードのポート番号取得
    port = atoi(port_moji);

    fgets(port_moji, 6, fp_port); //計算ノードのポート番号取得
    port_cal = atoi(port_moji);

    fclose(fp_port);
    printf("client node is %d\n",port);
    printf("compute node is %d\n",port_cal);
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
    //計算ノード専用の自アドレス設定
    bzero((char *) &my_addr_cal, sizeof(my_addr_cal));      /* 0クリア */ //クライアントノード用
    my_addr_cal.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr_cal.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr_cal.sin_port = htons(port_cal);                 /* ポート番号 */

    //クライアントノード専用の自アドレス設定
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0クリア */
    my_addr.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr.sin_port = htons(port);                 /* ポート番号 */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("設定失敗");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }

    /* ソケットを接続待ちの状態にする */
    listen(sockfd_cal, 5); //計算ノード専用
    listen(sockfd, 5); //クライアントノード専用

    addrlen_cal = sizeof (addr_cal); //計算ノード専用
    addrlen = sizeof (addr);//クライアントノード専用
    bzero(BUFF, sizeof(BUFF));    /* 受信バッファの0クリア */
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    bzero(&BUFF,sizeof(BUFF));

    int readsize,i;
    FILE *fp_write;
    char *filename_write = "a.log";
    char value[15];
    int tmp_value;//固定長配列に書き換える？

    /* コピー先ファイルのオープン */
    //if ((fp_write = fopen(filename_write, "w")) == NULL) {
    //	perror("can't open recvfile");
    //	exit(1);
    //}
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //////////////////////////////////////////////////////////////
    int size,count=0;
    int TMP_VALUE[VALUEMAX] = { }; //切り取ったValueの値を入れる配列
    //////////////////////////////////////////////////////////////
    //クライアントノードからのメッセージを受け取ってValueの値を切り取る処理
    while(1){
      recv(acc_sockfd, &size,sizeof(int),0);//文字数を取得
      printf("->->->->->->----%d\n",size);
      if((readsize = recv(acc_sockfd, BUFF, size,0)) != 0){//実際の文字列を受信
        printf("%s",BUFF);

        ptr = strtok(BUFF,",");
            //1つ目のトークン表示
            //puts(ptr);
            //トークンがNULLになるまでループ
            i = 0;
            while( ptr = strtok(NULL, ",") ){
              if(i == 0 && ptr != NULL){
                //value = strtok(tp,"=");
                strncpy(value,ptr+7,strlen(ptr)-7);
                tmp_value = atoi(value);
                printf("%d\n", tmp_value);
                count++;
              }
                i++;
            }
            //バッファがいっぱいになったときの処理を書く
            if(count == VALUEMAX){
              printf("バッファがいっぱいになりました\n計算ノードへの送信を開始します\n");
              //計算ノードへの送信処理を書く
              //======================================================================
              //送信処理
              if (send(acc_sockfd_cal, TMP_VALUE, sizeof(TMP_VALUE), 0) < 0) {
                  perror("計算ノードへの送信失敗");
                  exit(1);
              }else{
                printf("計算ノードへの送信完了\n");
              }
              bzero(TMP_VALUE,sizeof(TMP_VALUE)); //送信バッファ初期化
              count = 0; //カウンタ初期化
              //======================================================================
              //printf("送信完了しました\n");
            }

            //ファイルの終端に来たとき
            if(count == VALUEMAX){
              printf("バッファがいっぱいになりました\n計算ノードへの送信を開始します\n");
              //計算ノードへの送信処理を書く
              //======================================================================
              //送信処理
              if (send(acc_sockfd_cal, TMP_VALUE, sizeof(TMP_VALUE), 0) < 0) {
                  perror("計算ノードへの送信失敗");
                  exit(1);
              }else{
                printf("計算ノードへの送信完了\n");
              }
              bzero(TMP_VALUE,sizeof(TMP_VALUE)); //送信バッファ初期化
              count = 0; //カウンタ初期化
              //======================================================================
              //printf("送信完了しました\n");
            }
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
