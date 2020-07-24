#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Program developed for NWEN243, Victoria University of Wellington
   Author: Kris Bubendorfer (c) 2014-15.
   Uses a modified caeser I & II (in 2015)

   Compile:  gcc -o decode decode.c

   See encode for  examples on using it
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


int in(char c, char* s, int pos){
  // Assume everything is already in the same case
  int i;

  for(i = 0; i < pos; i++)
    if(c == s[i]) return 1;
    
  return 0;
} 


void buildtable (char* key, char* decode){ // this changed from encode

    /*
     * Building a decoding table from the encode table + a key
     */

    int i, pos;
    int keyLen = strlen(key); //length of the key with whitespace/punctuation
    fixkey(key); // fix the key, i.e., uppercase and remove whitespace and punctuation
    char encode[26]; char temp[26];

    /*
     * Building the encoding table
     */

    //removing duplicates from the key
    char newKey[strlen(key)], c;
    pos = 0;
    for (i = 0; i < strlen(key); i++){
        c = key[i];
        if (strchr(newKey, c) == NULL){
            newKey[pos] = c;
            pos++;
        }
    }
    newKey[pos] = '\0';

    //inserting the key into encode table
    pos = keyLen-1;
    for (i = 0; i < strlen(newKey); i++){
        if (pos > 25){ pos = 0; }
        encode[pos] = newKey[i];
        pos++;
    }

    //continue inserting alphabet from last key char
    c = newKey[strlen(newKey)-1];
    for(i = 0; i < 26; i++){
        if (++c > 'Z'){ c = 'A'; }
        if (pos > 25){ pos = 0; }
        if (strchr(newKey, c) == NULL){
            encode[pos] = c;
            pos++;
        }
    }

    /*
     * Building the decode table
     */

    //building a temp array to hold the alphabet
    c = 'A';
    for (i = 0; i < 26; i++){ temp[i] = c++; }

    //building the decode table from the encode table
    c = 'A';
    for (i = 0; i < 26; i++){
        pos = strchr(encode, c++) - encode;
        decode[i] = temp[pos];
    }

}

int main(int argc, char **argv){
  // format will be: 'program' key {encode|decode}
  // We'll be using stdin and stdout for files to encode and decode.

  // first allocate some space for our translation table.

  char* decode = (char*)malloc(sizeof(char)*26); // this changed from encode
  char ch;

  if(argc != 2){
    fprintf(stderr,"format is: '%s' key", argv[0]);
    exit(1);
  }

  // Build translation tables, and ensure key is upcased and alpha chars only.

  buildtable(argv[1], decode); // this changed from encode

  // write the key to stderr (so it doesn't break our pipes)

  fprintf(stderr,"key: %s - %lu\n", decode, strlen(decode));


  // the following code does the translations.  Characters are read 
  // one-by-one from stdin, translated and written to stdout.

    ch = fgetc(stdin);
    while (!feof(stdin)) {
      if(isalpha(ch))          // only decrypt alpha chars
	fputc(decode[ch-'A'], stdout);
      else
	fputc(ch, stdout);
      ch = fgetc(stdin);      // get next char from stdin
    }
}

