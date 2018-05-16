#define PORT 53556
#define SERVER_ADDRESS  "192.168.0.101"
#define BUFLEN 64

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>


struct eventTblTag {
        uint16_t devid;
        uint8_t op;
        uint32_t time;
} eventTbl[100];

main(argc, argv)
{
   printf("Event Recording and Reporting System\n");


   int serverfd = socket(AF_INET, SOCK_DGRAM, 0);


   if (serverfd < 0) {
           puts("socket() failed");
           exit(-1);
   }

   struct sockaddr_in server_addr, client_addr ;

   memset(&server_addr, 0, sizeof(struct sockaddr_in));

   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);

   if (inet_aton(SERVER_ADDRESS, &server_addr.sin_addr)==0) {
       puts("inet_aton() failed");
       exit(-1);
   }

   if (bind(serverfd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1)  {
       printf("bind() failed; errno = %d\n", errno);
       exit(-1);
   }


   uint8_t buf[BUFLEN], recvlen;
   int slen = sizeof(struct sockaddr_in);

   int count;

   for (count = 0; count < 100; count++ ) {    
   if ((recvlen = recvfrom(serverfd, buf, BUFLEN, 0, (struct sockaddr * __restrict__)&client_addr, &slen)) < 0) {
           puts("recvfrom() failed");
           exit(-1);
   }

/*   
   printf("Received packet from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

   int i;                          
   for(i=0; i < recvlen; i++)
      printf("%02X ", buf[i]);
   printf("\n\n"); 
*/

   printf("Received devid = %d, op = %d, time = %d\n", *(uint16_t *)buf, *(uint8_t *)(buf+2), *(uint32_t *)(buf+3) );
   } // count

}
