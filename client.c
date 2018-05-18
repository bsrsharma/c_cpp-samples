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


struct eventTag {
        uint16_t devid;
        uint8_t op;
        uint32_t time;
} event;

main(argc, argv)
{
   printf("Device Event Transmitter Program\n");


   int serverfd = socket(AF_INET, SOCK_DGRAM, 0);


   if (serverfd < 0) {
           puts("socket() failed");
           exit(-1);
   }

   struct sockaddr_in server_addr;

   memset(&server_addr, 0, sizeof(struct sockaddr_in));

   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);

   if (inet_aton(SERVER_ADDRESS, &server_addr.sin_addr)==0) {
       puts("inet_aton() failed");
       exit(-1);
   }



   uint8_t buf[BUFLEN];
   int slen = sizeof(struct sockaddr_in);

   int count;

   for (count = 0; count < 100; count++) {   
   event.devid = rand() % 65536;
   event.op = rand() % 256;
   event.time = rand();

   int offset = 0;
   memcpy(buf+offset, &event.devid, sizeof(event.devid));
   offset += sizeof(event.devid);
   memcpy(buf+offset, &event.op, sizeof(event.op));
   offset += sizeof(event.op);
   memcpy(buf+offset, &event.time, sizeof(event.time));
   offset += sizeof(event.time);

   if (sendto(serverfd, buf, offset, 0, (struct sockaddr * __restrict__)&server_addr, slen)==-1) {
           puts("sendto() failed");
           exit(-1);
   }

/*
   printf("Sent packet to %s:%d\n", 
               inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

   int i;                          
   for(i=0; i < offset; i++)
      printf("%02X ", buf[i]);
   printf("\n\n");  

*/

   printf("Sent devid = %d, op = %d, time = %d\n", event.devid, event.op, event.time);

   usleep(10000);
   } // count
}
