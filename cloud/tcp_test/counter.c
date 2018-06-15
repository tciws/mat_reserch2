#include<stdio.h>
#include <limits.h>
#include <time.h>
int main(void){
  char tmp = 0;
  long int tmp2 = 0;
  long int i = 0;
  long int kekka = LONG_MAX;
  clock_t start,end;
  start = clock();
  for(i = 0; i<= (long int)INT_MAX;i++){
    tmp++;
    printf("%c\n",tmp);
    //tmp2++;
  }
  end = clock();
  printf("%d[ms]かかりました\n",(int)(end-start));
  return 0;
}
