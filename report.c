#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "sqlite3.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
      printf("%s = %s ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


main(argc, argv)
{
   printf("\nEvent Reporting System\n\n");


// Open the report Database

   /* Database open API

   int sqlite3_open(
      const char *filename,   // Database filename (UTF-8) 
      sqlite3 **ppDb          // OUT: SQLite db handle
   );
   */

   sqlite3 *pDb;
   int rc;
   char *zErrMsg = 0;

   rc = sqlite3_open("DeviceDB", &pDb);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pDb));
      sqlite3_close(pDb);
      return(-1);
   }

   // Print a menu and get choice of report

   char choice[4];

   for (;;) {

   puts("\n\n1. List total number of each operation performed by the devices");
   puts("2. List operations sorted by average time");
   puts("3. List all device records");
   puts("4. Delete all device records");
   puts("5. End this program");

   fputs("\n\n   Enter a choice (1..5): ", stdout);

   fgets(choice, 3, stdin);

   //printf("You selected %s\n", choice);



   switch (atoi(choice)) {

   case 1:

   // b. Calculate the total number of each operations performed by the devices

    rc = sqlite3_exec(pDb,
         "SELECT OP, COUNT(*) FROM DEVICE_REPORT GROUP BY OP;",
         callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    break;

    case 2:   

   // c. Create a list of operations sorted by average time

    rc = sqlite3_exec(pDb,
         "SELECT OP, AVG(TIME) as AvgTime FROM DEVICE_REPORT GROUP BY OP ORDER BY AvgTime;",
         callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }   
    break;

   case 3:

   // d. Read and report each device record

    rc = sqlite3_exec(pDb,
         "SELECT * FROM DEVICE_REPORT GROUP BY ID;",
         callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }   
    break;

    case 4:

   // e. Flush/reset the module 

    rc = sqlite3_exec(pDb,
         "DELETE FROM DEVICE_REPORT;",
         NULL, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }   
    break;

    case 5:
   // Close Database

   sqlite3_close(pDb);
   break;

   default:
            printf("   Enter choice(1..5)\n\n");

   } // switch (choice)

   if (atoi(choice) == 5)
      break;

   } // for (;;)
}
