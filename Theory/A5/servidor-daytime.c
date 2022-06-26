/* Código simples de um servidor daytime. Não é o código ideal (deveria ter
 * returns ou exits após os erros das funções por exemplo) mas é suficiente
 * para começar a entender como se escreve servidores TCP usando sockets.
 * 
 * RFC do daytime: http://www.faqs.org/rfcs/rfc867.html
 *
 * Prof. Daniel Batista em 26/04/2021. Baseado no código disponibilizado no livro do Stevens
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */

#include <time.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#define MAXLINE 100
#define LISTENQ 1

int main(int argc, char **argv) {
   int listenfd, connfd;
   struct sockaddr_in servaddr;
   char buff[MAXLINE];
   time_t ticks;
   
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   if (listenfd < 0)
      fprintf(stderr,"socket error :(\n");
   
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(13);
   
   if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
      fprintf(stderr,"bind error :(\n");

   if (listen(listenfd, LISTENQ) < 0)
      fprintf(stderr,"listen error :(\n");
   
   for ( ; ; ) {
      connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
      if (connfd < 0)
         fprintf(stderr,"accept error :(\n");
      
      ticks = time(NULL);
      snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
      if (write(connfd, buff, strlen(buff)) < 0)
         fprintf(stderr,"write error :(\n");
      
      close(connfd);
   }
   return(0);
}
