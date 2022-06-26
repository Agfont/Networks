/* Código simples de um cliente de dns.
 * Não é o código ideal mas é suficiente para exemplificar o
 * funcionamento da função gethostbyname.
 *
 * Prof. Daniel Batista em 29/08/2011. Baseado em código do livro do
 * Stevens.
 *
 * Bugs? Tente consertar primeiro! Depois me envie email :) batista@ime.usp.br
 */
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
   char   *ptr, **pptr;
   char   str [INET_ADDRSTRLEN];
   struct hostent *hptr;

   if (argc==1) {
      fprintf(stderr,"Uso: %s <nome1> [[nome2][nome3]...]\n",argv[0]);
      exit(1);
   }

   while (--argc > 0) {
      ptr = *++argv;
      /* Para entender a struct devolvida: man gethosbyname */
      if ( (hptr = gethostbyname (ptr) ) == NULL) {
         fprintf(stderr,"[erro do gethostbyname para o nome: %s: %s]\n", ptr, hstrerror (h_errno) );
         continue;
      }
      printf ("nome oficial: %s\n", hptr->h_name);

      for (pptr = hptr->h_aliases; *pptr != NULL; pptr++)
         printf ("\talias: %s\n", *pptr);

      switch (hptr->h_addrtype) {
         case AF_INET:
            pptr = hptr->h_addr_list;
            for (; *pptr != NULL; pptr++) {
               if ((inet_ntop (hptr->h_addrtype, *pptr, str, sizeof (str))) != NULL)
                  printf ("\tendereco: %s\n", str);
               else {
                  fprintf(stderr,"[inet_ntop]\n");
                  exit(1);
               }
            }
            break;

         default:
            fprintf(stderr,"[tipo de endereco desconhecido]\n");
            break;
      }
   }
   exit(0);
}
