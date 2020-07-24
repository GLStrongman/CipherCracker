#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TEXT_SIZE 200000  // Note, the longer the text the more likely you will get a good 'decode' from the start.
#define ALEN 26         // Number of chars in ENGLISH alphabet
#define CHFREQ "ETAONRISHDLFCMUGYPWBVKJXQZ" // Characters in order of appearance in English documents.
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* Program developed for NWEN243, Victoria University of Wellington
   Author: Kris Bubendorfer, this extended version (c) 2015
   LAB: 2

   This program applies a basic frequency analysis on a cyphertext.  It has been extended over the 2014 version to
   solve polyalphabetic cyphers - by brute force.  In this case, it applies the frequency analysis for different 
   numbers of n keys (polyalphabetic Caeser).  Obviously it will need a cypher of about n times
   the typical length for a monoalphabetic cypher.

   Program is used like this:

   Compile:  gcc -o crack crack.c

   Test file (ctext): JWRLS, XSSH PZK JH HES BJFV, UZU (this is not a realistic length piece of cypher text)

   crack n

   Argument:

   n number of keys to try

   ---

   % cat ctext | crack 1
   ALICE, MEET YOU AT THE PARK, BOB   <-- of course it won't be this correct.  Don't worry about that for the -d option.
   AMFDE, UEET LNH AT TIE RASC, ONO   <-- this is what it really looks like, a larger sample is better, this is short.


 */

char upcase(char ch) {
    if (islower(ch))
        ch -= 'a' - 'A';
    return ch;
}

int main(int argc, char **argv) {

    // first allocate some space for our input text (we will read from stdin).

    char *text = (char *) malloc(sizeof(char) * TEXT_SIZE + 1);
    char ch;
    int n, i, y, pos, j, keys;

    if (argc > 1 && (n = atoi(argv[1])) > 0);
    else {
        fprintf(stderr, "Malformed argument, use: crack [n], n > 0\n");
        exit(-1);
    } // get the command line argument n

    // Now read TEXT_SIZE or feof worth of characters (whichever is smaller) and convert to uppercase as we do it.
    // Added: changed to count frequencies as we read it in

    for (i = 0, ch = fgetc(stdin); i < TEXT_SIZE && !feof(stdin); i++, ch = fgetc(stdin)) {
        text[i] = (ch = (isalpha(ch) ? upcase(ch) : ch));
    }
    text[i] = '\0'; // terminate the string properly.

    /*
     * Cracking a polyalphabetic cipher by breaking the text into n
     * subtexts, analysing the frequencies of this, and using that
     * to build a potential decode table.
     */

    //looping n times, so that if n is 2, it will loop twice (n=1 & n=2)
    keys = n;
    for (n = 1; n <= keys; n++){
        //dividing the text into n subtexts
        int size = (int) strlen(text);
        char subtext[n][TEXT_SIZE];
        for (i = 0; i < n; i++) {
            for (y = i, pos = 0; y < size; y += n) {
                subtext[i][pos++] = text[y];
            }
            subtext[i][pos] = '\0';
        }

        //analysing the frequencies of each subtext
        int freq[n][26];
        memset(freq, 0, sizeof(freq[0][0]) * n * 26);
        for (i = 0; i < n; i++) {
            for (y = 0; y < (int) strlen(subtext[i]); y++) {
                freq[i][(subtext[i][y]) - 'A']++;
            }
        }

        //mapping most frequent encoded characters to the most used english characters
        pos = -1; int val = -1, index, num;
        char engfreq[26] = CHFREQ;
        char hifreq[26];
        char t;
        for (i = 0; i < n; i++){
            index = 0;
            for (y = 0; y < 26; y++){
                for (j = 0; j < 26; j++){
                    if (freq[i][j] > val){
                        pos = j;
                        val = freq[i][j];
                    }
                }
                freq[i][pos] = -1;
                hifreq[index++] = pos+'A';
                pos = -1; val = -1;
            }

            //decoding the subtexts based on the map made before
            for (y = 0; y < (int)strlen(subtext[i]); y++){
                t = subtext[i][y];
                if (strchr(hifreq, t)){
                    num = strchr(hifreq, t) - hifreq;
                    subtext[i][y] = engfreq[num];
                }
            }
        }

        //merging the decoded subtexts back
        for (i = 0; i < n; i++) {
            for (y = i, pos = 0; pos < (int)strlen(subtext[i]); y += n) {
                text[y] = subtext[i][pos++];
            }
            text[size+1] = '\0';
        }

        //writing the text to the output file
        for (i = 0; i < size; i++){
            fputc(text[i], stdout);
        }
        fputc('\n', stdout);
    }
}
