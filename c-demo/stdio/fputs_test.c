#include <stdio.h>

// fputs : file put string

// Writes the C string pointed by str to the stream.
// null-character is not copied to the stream.
// stream: Pointer to a FILE object 

int main ()
{
   char sentence [256];
   fgets (sentence, sizeof(sentence), stdin);

   // On success, a non-negative value is returned.
   // On error, the function returns EOF and sets the error indicator (ferror).
   fputs (sentence, stdout);
   return 0;
}
