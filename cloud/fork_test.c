#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sys/types.h>  // fork/wait
#include <unistd.h>     // fork/sleep
#include <sys/wait.h>   // fork/wait

#include <err.h>
#include <errno.h>

#define P_MAX  10             //プロセス数
int main(){
	int pid[P_MAX];
  int val = 0,i;
	/*
	子プロセス生成。子プロセスは次の行から始まるため、
	このような記述をすると、子プロセスが子プロセスを生成しないで済む。
	*/
	for( i=0 ; i < P_MAX && (pid[i] = fork()) > 0 ; i++ );

	if( i == P_MAX ){			//親プロセスはすべての子プロセスの終了を待つ
		for(  i = 0 ; i < P_MAX ; i++ ){
			wait(&val);
		}
	}else if( pid[i] == 0){		//子プロセス
		printf("child:%d\n",i);
		exit(0);
	}else{
	    perror("child process") ;
	    exit(0);
	}
	return 0;
}
