#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Program developed for NWEN243, Victoria University of Wellington
   Author: Kris Bubendorfer (c) 2015.
   Uses a modified caeser I & II

   Compile:  gcc -o encode encode.c

   // Make a file with a plain aplhabet in it:

   % cat > test
   ABCDEFGHIJKLMNOPQRSTUVWXYZ
   ^D

   // test it - this should be the correct result when encoding.

   % cat test | encode "i came, i saw"
   key: HJKLNOPQRTUVICAMESWXYZBDFG - 26
   HJKLNOPQRTUVICAMESWXYZBDFG

 */

char upcase(char ch){
  if(islower(ch))
    ch -= 'a' - 'A';
  return ch;
}

char* fixkey(char* s){
  unsigned int i, j;
  char plain[26]; // assume key < length of alphabet, local array on stack, will go away!

  for(i = 0, j = 0; i < strlen(s); i++){
    if(isalpha(s[i])){
      plain[j++] = upcase(s[i]);
    }
  }
  plain[j] = '\0'; 
  return strcpy(s, plain);
}

void buildtable (char* key, char* encode) {
    /*
     * Building an encoding table from a key
     */

    int i;
    int keyLen = strlen(key); //length of the key with whitespace/punctuation
    fixkey(key); // fix the key, i.e., uppercase and remove whitespace and punctuation

    //removing duplicates from the key
    char newKey[strlen(key)], c;
    int pos = 0;
    for (i = 0; i < strlen(key); i++) {
        c = key[i];
        if (strchr(newKey, c) == NULL) {
            newKey[pos] = c;
            pos++;
        }
    }
    newKey[pos] = '\0';

    //inserting the key into encode table
    pos = keyLen - 1;
    for (i = 0; i < strlen(newKey); i++) {
        if (pos > 25) { pos = 0; }
        encode[pos] = newKey[i];
        pos++;
    }

    //continue inserting alphabet from last key char
    c = newKey[strlen(newKey) - 1];
    for (i = 0; i < 26; i++) {
        if (++c > 'Z') { c = 'A'; }
        if (pos > 25) { pos = 0; }
        if (strchr(newKey, c) == NULL) {
            encode[pos] = c;
            pos++;
        }
    }
}



int main(int argc, char **argv){
  // format will be: 'program' key {encode|decode}
  // We'll be using stdin and stdout for files to encode and decode.

  // first allocate some space for our translation table.

  char* encode = (char*)malloc(sizeof(char)*26);
  char ch;

  if(argc != 2){
    printf("format is: '%s' key", argv[0]);
    exit(1);
  }

  // Build translation tables, and ensure key is upcased and alpha chars only.

  buildtable(argv[1], encode);

  // write the key to stderr (so it doesn't break our pipes)

  fprintf(stderr,"key: %s - %lu\n", encode, strlen(encode));

  // the following code does the translations.  Characters are read 
  // one-by-one from stdin, translated and written to stdout.

  ch = fgetc(stdin);
  while (!feof(stdin)) {
    if(isalpha(ch)){        // only encrypt alpha chars
      ch = upcase(ch);      // make it uppercase
      fputc(encode[ch-'A'], stdout);
    }else 
      fputc(ch, stdout);
    ch = fgetc(stdin);      // get next char from stdin
  }
}
  
