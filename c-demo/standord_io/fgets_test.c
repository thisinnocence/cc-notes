#include <stdio.h>
#include <string.h>

// fgets: get string from file pointer.
//
// http://www.cplusplus.com/reference/cstdio/fgets
// Reads characters from stream and stores them as a C string into str until (num-1) characters
//   have been read or either a newline or the end-of-file is reached, whichever happens first.
//
// A newline character makes fgets stop reading
// A terminating null character is automatically appended
//
// return:
// On success, the function returns str.(first input arg)
// If the end-of-file is encountered while attempting to read a characterothe pointer 
//   returned is a null pointer (and the contents of str remain unchanged)

int main() {
    char buff[5] = {0};
    char *str;
    while ((str = fgets(buff, sizeof(buff), stdin)) != NULL) {
        printf("[%s]", str);
        memset(buff, 0, sizeof(buff));
    }
    printf("\n");
    return 0;
}

/*
root@ubuntu:/media/sf_VMshare/test# echo "hello hi hi" | ./a.out
[hell][o hi][ hi]
*/
