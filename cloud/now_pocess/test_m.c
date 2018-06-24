/* TCPのサーバプログラム */

/* 以下のヘッダファイルは/usr/includeにある */
#include        <stdio.h>
#include <math.h>
#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netinet/in.h>
#include	<string.h>
#include	<strings.h>
#include	<stdlib.h>
#include	<unistd.h>
#include <limits.h>
#define BUFFSIZE 1000            /* メッセージの最大長 */
#define VALUE_SIZE 5120        //VALUE格納用
#define port_file "mngnode.conf" //ポート番号が書いてある設定ファイル

typedef struct{
  int send_max;
  int send_min;
  int send_count;
  double sum;
  double sum_sum;
}SEND_MSG;

int main(void)
{
    int     port;                 //クライアントノード用ポート番号
    int     port_comp;            //計算ノード用ポート番号
    char    port_moji[6];
    int     sockfd, acc_sockfd;   //クライアントノード用．ソケット記述子
    int     sockfd_comp, acc_sockfd_comp;   //計算ノード用，ソケット記述子
    int     acc_sockfd_comp2;   //計算ノード用，ソケット記述子
    int     acc_sockfd_comp3;   //計算ノード用，ソケット記述子
    struct sockaddr_in      addr, my_addr;
                                  //クライアントノード用，インタネットソケットアドレス構造体

    struct sockaddr_in      addr_comp, my_addr_comp;
                                 //計算ノード用，インタネットソケットアドレス構造体
    int     addrlen;             //クライアントノード用
    int     addrlen_comp;             //計算ノード用

    char    BUFF[BUFFSIZE];       /* 受信バッファ */
    //int     nbytes;               /* 受信メッセージ長 */
    char *ptr;  //分割後の文字列が入るポインタ
    //----------------自ポート番号の取得-------------------------
    FILE *fp_port;
    if((fp_port = fopen(port_file, "r")) == NULL){
      perror("can't open file\n");
      exit(1);
    }
    fgets(port_moji, 6, fp_port); //クライアントノード用ポート番号
    //port = atoi(port_moji);
    port = 50040;

    fgets(port_moji, 6, fp_port); //計算ノード用ポート番号
    //port_comp = atoi(port_moji);
    port_comp = 50140;

    fclose(fp_port);
    printf("client node port is %d\n",port);
    printf("compute node port is %d\n",port_comp);
    //----------------------------------------------------------

    //===================ソケット生成===============================
    //計算ノード用ソケット作成
    if ((sockfd_comp = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗");         /* ソケットの生成失敗 */
        exit(1);
    }
    //クライアントノードのソケット生成
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ソケット生成失敗_COMP");         /* ソケットの生成失敗 */
        exit(1);
    }
    //===============================================================
    //===================ポートとアドレスの設定===============================
    //計算ノード側の自ポートとアドレスの設定
    bzero((char *) &my_addr_comp, sizeof(my_addr_comp));      /* 0クリア */
    my_addr_comp.sin_family = AF_INET;                   /* アドレスファミリ */
    my_addr_comp.sin_addr.s_addr = htonl(INADDR_ANY);    /* アドレス */
    my_addr_comp.sin_port = htons(port_comp);                 /* ポート番号 */
    if (bind(sockfd_comp, (struct sockaddr *) &my_addr_comp, sizeof(my_addr_comp)) < 0) {
        perror("設定失敗_COMP");         /* 自アドレスと自ポート番号の設定失敗 */
        exit(1);
    }

    /* ソケットを接続待ちの状態にする */
    listen(sockfd_comp, 5);

    //クライアント側の自ポートとアドレスの設定
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
    //===============================================================

    addrlen = sizeof (addr);
    addrlen_comp = sizeof (addr_comp);

    bzero(BUFF, sizeof(BUFF));    /* 受信バッファの0クリア */

    /* 計算ノードからの接続受付 */
    printf("Waiting Compute Node No.1....");
    if ((acc_sockfd_comp = accept(sockfd_comp, (struct sockaddr *)&addr_comp, &addrlen_comp)) < 0) {
      perror("wait accept");
      exit(1);
    }
    printf("                        COMPULETE\n");

    printf("Waiting Compute Node No.2....");
    if ((acc_sockfd_comp2 = accept(sockfd_comp, (struct sockaddr *)&addr_comp, &addrlen_comp)) < 0) {
      perror("wait accept");
      exit(1);
    }
    printf("                        COMPULETE\n");

    printf("Waiting Compute Node No.3....");
    if ((acc_sockfd_comp3 = accept(sockfd_comp, (struct sockaddr *)&addr_comp, &addrlen_comp)) < 0) {
      perror("wait accept");
      exit(1);
    }
    printf("                        COMPULETE\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* クライアントからの接続受付 */
    printf("Waiting Client Node....");
    if ((acc_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0) {
      perror("wait accept");
      exit(1);
    }
    printf("                        COMPULETE\n");
    bzero(&BUFF,sizeof(BUFF));
    //==========================================================================
    //メセージ通信に必要な変数
    int readsize,i;
    FILE *fp_write;
    char *filename_write = "a.log";
    char value[30];//一時格納用
    int VALUE_BUFF[VALUE_SIZE];//VALUE格納用
    int VALUE_BUFF2[VALUE_SIZE];//VALUE格納用
    int VALUE_BUFF3[VALUE_SIZE];//VALUE格納用
    int kakunouyou = 0;
    //int tmp_max = 0,tmp_min = INT_MAX;
    double kekka_tmp;
    long long int tid = 0;
    int print_counter = 0;
    //int file_num[10] = ; //ファイル内の値が
    /* コピー先ファイルのオープン */
    //if ((fp_write = fopen(filename_write, "w")) == NULL) {
    //	perror("can't open recvfile");
    //	exit(1);
    //}
    int size,count = 0,kekka1,kekka2,nbytes;
    int value_counter1,value_counter2,value_counter3;
    int tmp_max,tmp_min;
    SEND_MSG RESULT;
    SEND_MSG COMPUTE_NODE_RESULT[3];
    //==========================================================================
    /////////////////////////////////////////////////////////////////////////////
    //compute node 1
    printf("COMPUTE NODE No.1 -> SEND tid[%ld]\n",tid);
      send(acc_sockfd_comp,&tid,sizeof(long long int),0); //トランザクションIDを送信
    //compute node 2
    printf("COMPUTE NODE No.2 -> SEND tid[%ld]\n",tid);
      send(acc_sockfd_comp2,&tid,sizeof(long long int),0); //トランザクションIDを送信
    //compute node 3
    printf("COMPUTE NODE No.3 -> SEND tid[%ld]\n",tid);
      send(acc_sockfd_comp3,&tid,sizeof(long long int),0); //トランザクションIDを送信
      ///////////////////////////////////////////////////////////////////////////
    while(1){
      recv(acc_sockfd, &size,sizeof(int),MSG_WAITALL);//文字数を取得
      //printf("->->->->->->----%d\n",size);
      if(size == 1025){
        tid++;
        //バッファサイズを送信
        printf("count = %d\n", count);
        //nbytes = count;
        count = 0;
///////////////////////////////////////////////////////////////////////////////////////
        //compute node 1
        send(acc_sockfd_comp,&value_counter1,sizeof(int),0);
        if (send(acc_sockfd_comp, VALUE_BUFF, value_counter1*sizeof(int), 0) < 0) {
            perror("COMPUTE NODE 1 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 1 SEND COMPLETE\n");
        }
        value_counter1 = 0;
        //compute node 2
        send(acc_sockfd_comp2,&value_counter2,sizeof(int),0);
        if (send(acc_sockfd_comp2, VALUE_BUFF2, value_counter2*sizeof(int), 0) < 0) {
            perror("COMPUTE NODE 2 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 2 SEND COMPLETE\n");
        }
        value_counter2 = 0;
        //compute node 3
        send(acc_sockfd_comp3,&value_counter3,sizeof(int),0);
        if (send(acc_sockfd_comp3, VALUE_BUFF3, value_counter3*sizeof(int), 0) < 0) {
            perror("COMPUTE NODE 3 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 3 SEND COMPLETE\n");
        }
        value_counter3 = 0;
////////////////////////////////////////////////////////////////////////////////////////
        //ファイル終了を計算ノードに送信
        nbytes = 16000;
        send(acc_sockfd_comp,&nbytes,sizeof(int),0);
        send(acc_sockfd_comp2,&nbytes,sizeof(int),0);
        send(acc_sockfd_comp3,&nbytes,sizeof(int),0);

        //VALUE_BUFFを初期化
          bzero(VALUE_BUFF,sizeof(VALUE_BUFF));
        //countを0に
        count = 0;
        print_counter = 0;
        //結果を計算ノードから受ける処理
        //============================================================
        //printf("RECEIVE RESULT.....\n");
        printf("COMPUTE NODE No.1 ->->->->");
        recv(acc_sockfd_comp, &COMPUTE_NODE_RESULT[0],sizeof(SEND_MSG),MSG_WAITALL);
        printf("                                  COMPLETE\nCOMPUTE NODE No.2 ->->->->");
        recv(acc_sockfd_comp2, &COMPUTE_NODE_RESULT[1],sizeof(SEND_MSG),MSG_WAITALL);
        printf("                                  COMPLETE\nCOMPUTE NODE No.3 ->->->->");
        recv(acc_sockfd_comp3, &COMPUTE_NODE_RESULT[2],sizeof(SEND_MSG),MSG_WAITALL);
        printf("                                  COMPLETE\n");
        //============================================================
        /////////////////////////////////////////////////////////////////////////////
        //計算ノードの結果を集約
        //RESULT.send_max=COMPUTE_NODE_RESULT[0].send_max;
        //RESULT.send_min=COMPUTE_NODE_RESULT[0].send_min;

        tmp_max = COMPUTE_NODE_RESULT[0].send_max;
        tmp_max = max(COMPUTE_NODE_RESULT[1].send_max,tmp_max);
        RESULT.send_max = max(COMPUTE_NODE_RESULT[2].send_max,tmp_max);

        tmp_min = COMPUTE_NODE_RESULT[0].send_min;
        tmp_min = min(COMPUTE_NODE_RESULT[1].send_min,tmp_min);
        RESULT.send_min = min(COMPUTE_NODE_RESULT[2].send_min,tmp_min);

        RESULT.sum=COMPUTE_NODE_RESULT[0].sum + COMPUTE_NODE_RESULT[1].sum + COMPUTE_NODE_RESULT[2].sum;
        RESULT.sum_sum=COMPUTE_NODE_RESULT[0].sum_sum + COMPUTE_NODE_RESULT[1].sum_sum + COMPUTE_NODE_RESULT[2].sum_sum;
        RESULT.send_count = COMPUTE_NODE_RESULT[0].send_count + COMPUTE_NODE_RESULT[1].send_count + COMPUTE_NODE_RESULT[2].send_count;
        /////////////////////////////////////////////////////////////////////////////
        RESULT.sum = RESULT.sum/(double)RESULT.send_count;
        //kekka_tmp = RESULT.sum_sum/(double)RESULT.send_count - (RESULT.sum/(double)RESULT.send_count)*(RESULT.sum/(double)RESULT.send_count);
        //RESULT.sum_sum = RESULT.sum_sum/(double)RESULT.send_count;
        kekka_tmp = RESULT.sum_sum / RESULT.send_count - RESULT.sum*RESULT.sum;
        //RESULT.sum_sum = (RESULT.sum/(double)RESULT.send_count)*(RESULT.sum/(double)RESULT.send_count);
        RESULT.sum_sum = sqrt(kekka_tmp);
        //結果をクライアントノードに送信
        //************************************************************
        printf("SEND RESULT TO CLIENT NODE...\n");
        send(acc_sockfd,&RESULT,sizeof(SEND_MSG),0);
        //************************************************************
        ////////////////////////////////////////////////////////////////////////////
        printf("COMPUTE NODE No.1 -> SEND tid[%ld]\n",tid);
        send(acc_sockfd_comp,&tid,sizeof(long long int),0); //トランザクションIDを送信
        //compute node 2
        printf("COMPUTE NODE No.2 -> SEND tid[%ld]\n",tid);
          send(acc_sockfd_comp2,&tid,sizeof(long long int),0); //トランザクションIDを送信
        //compute node 3
        printf("COMPUTE NODE No.3 -> SEND tid[%ld]\n",tid);
          send(acc_sockfd_comp3,&tid,sizeof(long long int),0); //トランザクションIDを送信
        //////////////////////////////////////////////////////////////////////////////
        printf("NEXT FILE...\n");
        bzero(BUFF,sizeof(BUFF));
      }else if(size >= 1026){
        //通信終了を計算ノードに送信
        nbytes = 26000;
        send(acc_sockfd_comp,&nbytes,sizeof(int),0);
        send(acc_sockfd_comp2,&nbytes,sizeof(int),0);
        send(acc_sockfd_comp3,&nbytes,sizeof(int),0);
        printf("SIGNAL...END...\n");
        break;
      }else{
      if((readsize = recv(acc_sockfd, BUFF, size,MSG_WAITALL)) != 0){//実際の文字列を受信
        //文字列の先頭８文字がFILE_ENDの場合
        //printf("%s",BUFF);

        ptr = strtok(BUFF,",");
            //1つ目のトークン表示
            //puts(ptr);
            //トークンがNULLになるまでループ
            i = 0;
            while( ptr = strtok(NULL, ",") ){
              if(i == 0 && ptr != NULL){
                //value = strtok(tp,"=");
                print_counter++;
                strncpy(value,ptr+7,strlen(ptr)-7);
                if(count % 3 == 0){
                  VALUE_BUFF[value_counter1] = atoi(value);
                  printf("value1 -> %10d\n", VALUE_BUFF[value_counter1]);
                  value_counter1++;
                }else if(count % 3 == 1){
                  VALUE_BUFF2[value_counter2] = atoi(value);
                  printf("value2 -> %10d\n", VALUE_BUFF2[value_counter2]);
                  value_counter2++;
                }else if(count % 3 == 2){
                  VALUE_BUFF3[value_counter3] = atoi(value);
                  printf("value3 -> %10d\n", VALUE_BUFF3[value_counter3]);
                  value_counter3++;
                }
                bzero(value,sizeof(value));
                //tmp_max = max(tmp_max,VALUE_BUFF[count]);
                //tmp_min = min(tmp_min,VALUE_BUFF[count]);
                //printf("MAX = %d ||| MIN = %d\n",tmp_max,tmp_min);
                count++; //VALUEの数を確認する用
              }
                i++;
            }
        //printf("\n================================\n");
        bzero(BUFF,sizeof(BUFF));
      }
      if(3*VALUE_SIZE == count){
        //バッファサイズを送信
        printf("BUFF if FULL\n");
        //////////////////////////
        //////////////////////////
        nbytes = VALUE_SIZE;
        //////////////////////////
        //////////////////////////
        count = 0;
///////////////////////////////////////////////////////////////////////////////////
        //compute node 1
        send(acc_sockfd_comp,&nbytes,sizeof(int),0);
        if (send(acc_sockfd_comp, VALUE_BUFF, nbytes*sizeof(int), 0) < 0){
            perror("COMPUTE NODE 1 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 1 SEND COMPLETE\n");
        }
        value_counter1 = 0;
        //compute node 2
        send(acc_sockfd_comp2,&nbytes,sizeof(int),0);
        if (send(acc_sockfd_comp2, VALUE_BUFF2, nbytes*sizeof(int), 0) < 0){
            perror("COMPUTE NODE 2 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 2 SEND COMPLETE\n");
        }
        value_counter2 = 0;
        //compute_node 3
        send(acc_sockfd_comp3,&nbytes,sizeof(int),0);
        if (send(acc_sockfd_comp3, VALUE_BUFF3, nbytes*sizeof(int), 0) < 0){
            perror("COMPUTE NODE 3 SEND ERROR");
            exit(1);
        }else{
          printf("COMPUTE NODE 3 SEND COMPLETE\n");
        }
        value_counter3 = 0;
        nbytes = 0;
///////////////////////////////////////////////////////////////////////////////////
      }
    }
      /*
      else{
        printf("BREAK\n");
        break;
      }
      */
    }

    close(acc_sockfd);  /* ソケット記述子の削除 */
    close(sockfd);      /* ソケットの終了 */
    close(acc_sockfd_comp);  /* ソケット記述子の削除 */
    close(sockfd_comp);      /* ソケットの終了 */
    close(acc_sockfd_comp2);  /* ソケット記述子の削除 */
    close(acc_sockfd_comp3);  /* ソケット記述子の削除 */

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
