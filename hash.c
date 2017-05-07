#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>

unsigned char *myMD5(const unsigned char inp[], unsigned int n, unsigned char outp[] )
{
    
    return MD5(inp, n, outp);

}



unsigned char *makeNiceStr(const unsigned char inp[], unsigned int n, unsigned char outp[])
{
  
   static char niceStr[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

   int i;
   int myInt;
   char c;
    
   for (i = 0; i < n; i++)
   {     
      myInt = inp[i] & 31;
      c = niceStr[myInt];
      outp[i] = c;
   }
   outp[i] = 0;
   return outp;
}

int main()
{
   printf("Hash for 'Hello'\n");
   char inp[80];
   strcpy(inp, "Hello");
   unsigned char hash[16];
   myMD5(inp, strlen(inp), hash);
   char outp[80];

   puts(makeNiceStr(hash, 16, outp));
   
   // hash a bunch of names
   
   strcpy(inp, "Delaware Pennsylvania California Texas...");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Ecuador Japan Bhutan Paraguay");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Russia Romania");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Turkmenistan Denmark");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Tajikistan Jamaica");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Panama Djibouti Nepal");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));

   strcpy(inp, "Bolivia Srilanka Singapore");
   puts(inp);
   puts(makeNiceStr(myMD5(inp, strlen(inp), hash), 16, outp));


   // Generate an array of random Bytes

   FILE *fp = fopen("arrs.dat", "w");

   unsigned char rb[30];
   unsigned int rblen = 0;  // length of the random string

   int i,j,k;

   for (i = 0; i < 10000; i++)
   {     
      for (j = 0; j < 2; j++)
      {	
         for (k = 0; k < 30; k++)
	 {	   
            rb[k] = rand() % 256;
            rblen = rand() % 20;
	 }	    
         fputs(makeNiceStr(rb, 10+rblen, outp), fp);
         fputs(" , ", fp);
      }
      for (k=0; k < 30; k++)
      {	   
         rb[k] = rand() % 256;
         rblen = rand() % 20;
      }	    
      fputs(makeNiceStr(rb, 10+rblen, outp), fp);
      fputs("\n", fp);
   }	
   fclose(fp);

   puts("wrote arrs.dat\n");
}  
