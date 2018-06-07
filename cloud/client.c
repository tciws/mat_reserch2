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
  char    *host = "mikasa";                /* $BAj<j%[%9%HL>(B */
  int     port = 55555;                 /* $BAj<j%]!<%HHV9f(B */
  int     sockfd;               /* $B%=%1%C%H5-=R;R(B */
  struct sockaddr_in      addr, my_addr;                  
  /* $B%$%s%?%M%C%H%=%1%C%H%"%I%l%99=B$BN(B */
  int     addrlen;                                    
  char    BUFF[BUFFSIZE];       /* $BAw?.%P%C%U%!(B */
  int     nbytes;               /* $BAw?.%a%C%;!<%8D9(B */
  struct hostent  *hp;          /* $BAj<j%[%9%H%(%s%H%j(B */
  
  if(argc != 2){
    printf("please input 10 filename\n");
    exit(1);
  }

  /* $BAj<j%[%9%H%(%s%H%j$N<hF@(B */                    
  if ((hp = gethostbyname(host)) == NULL) {
    perror("gethostbyname");  /* $BAj<j%[%9%H%(%s%H%j<hF@<:GT(B */
    exit(1);                                   
  }      

  /* $BAj<j%"%I%l%9$HAj<j%]!<%HHV9f$N@_Dj(B */             
    bzero((char *)&addr, sizeof (addr));            /* 0$B%/%j%"(B */
    addr.sin_family = AF_INET;                      /* $B%"%I%l%9%U%!%_%j(B */
    bcopy(hp->h_addr, (char *)&addr.sin_addr, hp->h_length);
                                                    /* $B%"%I%l%9(B */
    addr.sin_port = htons(port);                    /* $B%]!<%HHV9f(B */
                                                           
    /* $B%=%1%C%H$N@8@.(B */                                              
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("$B%=%1%C%H@8@.<:GT(B");         /* $B%=%1%C%H$N@8@.<:GT(B */
        exit(1);                                        
    }                                                   
                                                  
    /* $B<+%"%I%l%9$H<+%]!<%HHV9f$N@_Dj(B */
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0$B%/%j%"(B */
    my_addr.sin_family = AF_INET;                   /* $B%"%I%l%9%U%!%_%j(B */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* $B%"%I%l%9(B */
    my_addr.sin_port = htons(0);                    /* $B%]!<%HHV9f(B */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("$B@_Dj<:GT(B");         /* $B<+%"%I%l%9$H<+%]!<%HHV9f$N@_Dj<:GT(B */
        exit(1);                          
    }                                      
                                       
    addrlen = sizeof (addr);                 

    /* $B%5!<%P$H$N%3%M%/%7%g%s3NN)(B */
    if (connect(sockfd, (struct sockaddr *)&addr, addrlen) < 0) {
      perror("$B%3%M%/%7%g%s3NN)(B");
      exit(1);
    }

    //printf("Send message: ");

    FILE *fp;//$BFI$_9~$s$@%U%!%$%k$X$N%]%$%s%?(B
    char *filename;//$BFI$_9~$`%U%!%$%kL>(B($BI8=`F~NO$+$iM?$($i$l$k(B)
    int i;
    
    for(i=1;i<argc;i++){
      filename = argv[i];
      if((fp=fopen(filename, "r")) == NULL){
	perror("can't open file\n");
	exit(1);
      }
      
      bzero(&BUFF,sizeof(BUFF));
      
      while(feof(fp) == 0){
	fgets(BUFF, BUFFSIZE, fp); /* $BAw?.%a%C%;!<%8$N<hF@(B */        
	nbytes = strlen(BUFF);        /* $BAw?.%a%C%;!<%8D9$N@_Dj(B */      
	//BUFF[nbytes-1]='\0';    /* fgets$B$G<h$j9~$^$l$?J8;zNs$N:G8e$+$i2~9T$r:o=|(B */
	/* $BAw?.(B */                                 
	if (send(sockfd, BUFF, nbytes, 0) != nbytes) {
	  perror("$BAw?.<:GT(B");         /* $BAw?.<:GT(B */
	  exit(1);                               
	}
	bzero(&BUFF,sizeof(BUFF));
      }
      fclose(fp);


      
    }
    close(sockfd);
}
