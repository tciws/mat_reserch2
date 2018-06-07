/* TCP$B$N%5!<%P%W%m%0%i%`(B */

/* $B0J2<$N%X%C%@%U%!%$%k$O(B/usr/include$B$K$"$k(B */
#include        <stdio.h>                                          
#include        <sys/types.h>                                      
#include        <sys/socket.h>                            
#include        <netinet/in.h>                                    
#include	<string.h>
#include	<strings.h>
#include	<stdlib.h>
#include	<unistd.h>                                                                      
#define BUFFSIZE 1024            /* $B%a%C%;!<%8$N:GBgD9(B */       
#define port_file "mngnode.conf" //$B%]!<%HHV9f$,=q$$$F$"$k@_Dj%U%!%$%k(B

int main(void)                                              
{                                                             
    int     port;                 /* $B<+%]!<%HHV9f(B*/
    char    port_moji[6];
    int     sockfd, acc_sockfd;   /* $B%=%1%C%H5-=R;R(B */
    struct sockaddr_in      addr, my_addr;
                                  /* $B%$%s%?%M%C%H%=%1%C%H%"%I%l%99=B$BN(B */
    int     addrlen;                                             
    char    BUFF[BUFFSIZE];       /* $B<u?.%P%C%U%!(B */
    int     nbytes;               /* $B<u?.%a%C%;!<%8D9(B */
    //----------------$B<+%]!<%HHV9f$N<hF@(B-------------------------
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
    
    /* $B%=%1%C%H$N@8@.(B */                                              
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("$B%=%1%C%H@8@.<:GT(B");         /* $B%=%1%C%H$N@8@.<:GT(B */
        exit(1);                                              
    }                                                       
                                                             
    /* $B<+%"%I%l%9$H<+%]!<%HHV9f$N@_Dj(B */
    bzero((char *) &my_addr, sizeof(my_addr));      /* 0$B%/%j%"(B */
    my_addr.sin_family = AF_INET;                   /* $B%"%I%l%9%U%!%_%j(B */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* $B%"%I%l%9(B */
    my_addr.sin_port = htons(port);                 /* $B%]!<%HHV9f(B */
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("$B@_Dj<:GT(B");         /* $B<+%"%I%l%9$H<+%]!<%HHV9f$N@_Dj<:GT(B */
        exit(1);                                  
    }                                          
                                                   
    /* $B%=%1%C%H$r@\B3BT$A$N>uBV$K$9$k(B */
    listen(sockfd, 5);

    addrlen = sizeof (addr);                         
    bzero(BUFF, sizeof(BUFF));    /* $B<u?.%P%C%U%!$N(B0$B%/%j%"(B */

    /* $B%/%i%$%"%s%H$+$i$N@\B3<uIU(B */
    if ((acc_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addrlen)) < 0) {
      perror("wait accept");
      exit(1);
    }

    bzero(&BUFF,sizeof(BUFF));
    
    int readsize;
    FILE *fp_write;
    char *filename_write = "a.log";

    char value[15];
    
    /* $B%3%T!<@h%U%!%$%k$N%*!<%W%s(B */    
    //if ((fp_write = fopen(filename_write, "w")) == NULL) {
    //	perror("can't open recvfile");
    //	exit(1);
    //}
    
    while((readsize = recv(acc_sockfd, BUFF, 512,0)) != 0){
      printf("%s",BUFF);
      //  if(fwrite(&BUFF, readsize, 1, fp_write) != 1){
      //perror("fwrite");
      //}	
      bzero(&BUFF,sizeof(BUFF));
    }
    //close(fp_write);
    
    
    



    
    close(acc_sockfd);  /* $B%=%1%C%H5-=R;R$N:o=|(B */

    close(sockfd);      /* $B%=%1%C%H$N=*N;(B */

}
