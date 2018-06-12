#include <stdio.h>
#include <string.h>

int main(){
    char str[] = "2015-03-28 00:00:00.0 INFO seq=1, value=7774280, state=default";
    char* tp;
    char value[100];
    int i;
    /* 最初に見つかった区切り文字（","）に "\0" を挿入する */
    tp = strtok(str, ",");
    //printf("%s\n", str);
    /* strtok() の戻り値は最初のトークンの位置 */
    //printf("%s\n", tp);

    /* 第一引数に NULL を指定すると前回の str 値の続きから始める */
    /* strtok() の戻り値は次のトークンの位置 */
    /* 区切り文字が見つからなかったら NULL が返ってくる */
    i = 0;
    while( tp = strtok(NULL, ",") ){
      if(i == 0 && tp != NULL){
        //value = strtok(tp,"=");
        strncpy(value,tp+7,strlen(tp)-7);
        printf("%s\n", value);
      }
        i++;
    }

    return 0;
}
