/* Código simples de um cliente http que envia um HEAD pro servidor.
 * Não é o código ideal (deveria ter returns ou exits após os erros
 * das funções por exemplo) mas é suficiente para exemplificar o
 * conceito.
 * 
 * RFC do HTTP: http://www.faqs.org/rfcs/rfc2616.html
 *
 * Prof. Daniel Batista em 21/08/2011. Modificado em cima do cliente
 * do daytime da aula 04
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 100

int main(int argc, char **argv) {
   int sockfd, n;
   char recvline[MAXLINE + 1];
   char sndline[MAXLINE + 1];
   struct sockaddr_in servaddr;
   
   if (argc != 2) {
      fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
      exit(1);
   }   

   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      fprintf(stderr,"socket error :( \n");
   
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(80);  /* HTTP padrao roda na 80 */

   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
      fprintf(stderr,"inet_pton error for %s :(\n", argv[1]);
   
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
      fprintf(stderr,"connect error :(\n");

   strcpy(sndline,"HEAD / HTTP/1.1\nhost: www.google.com.br\n\n");
   if (write(sockfd,sndline,strlen(sndline)) < 0)
      fprintf(stderr,"write error :(\n"); 
   
   while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
       recvline[n] = 0;
       if (fputs(recvline, stdout) == EOF)
         fprintf(stderr,"fputs error :(\n");
   }
   if (n < 0)
      fprintf(stderr,"read error :(\n");
   
   exit(0);
}
