#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define MAX_WORDS 512
#define BUFLENGTH 256
/*if BUFLENGTH is low(16,20,4)? or might certain line), printing last line cause error: idk the reason*/

const char *specialChars = ",\"()[]{}.?':/@*#$%^&._=";
char *words[MAX_WORDS];
int word_count = 0;
int wrong_count = 0;

void read_lines(int fd, void(*use_line)(void *, char *, char *), void *arg, char *dict){
        //  filename  , variable for print_line        ,  n: line count
  int buflength = BUFLENGTH;
  char *buf = malloc(BUFLENGTH);

  int pos = 0;    // position of arr
  int bytes;      // bytes that written
  int line_start; // location of line starts

  while((bytes = read(fd, buf + pos, buflength - pos)) > 0){  // read return num of written bytes
    if(DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
    line_start = 0;
    int bufend = pos + bytes;

    if (DEBUG) printf("Written bytes: %d\n", bytes);

    while(pos < bufend){  // when line changed while buffer is not full
      if(DEBUG) printf("start %d, pos %d, char '%c'\n",line_start, pos, buf[pos]);

      if (pos == line_start && strchr("\"(['{", buf[pos]) != NULL) {
          line_start++;
      } else if (strchr(specialChars, buf[pos]) != NULL) {
          buf[pos] = '\0';
      }

      if(buf[pos] == '\n' || buf[pos] == ' '){    // when found line change or space 
        buf[pos] = '\0';      
        use_line(arg, buf + line_start, dict);           // print that line
        line_start = pos + 1;                      // set linestart as the beginning of the line
      }
      pos++;
    }

    if (line_start == pos){  // no partial line
      pos = 0;
    }
    else if (line_start > 0){ // partial line. fill the buffer with partial words
      int segment_length = pos - line_start;
      memmove(buf, buf + line_start, segment_length); 
      // move memory (length of segment_length) which is starting buf + line_start to buf
      pos = segment_length;
      if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
    }
    else if(bufend == buflength){    // when partial line fills buffer
      buflength *= 2;              
      buf = realloc(buf, buflength); // increase buffer size
      if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
    }
  } // 25 while end
  if (pos > 0){          //when the last line is after eof?
    if(pos == buflength){
      buf = realloc(buf, buflength + 1);
    }
    buf[pos] = '\0';
    use_line(arg, buf + line_start, dict);
  }
  free(buf);
}

// ************* KELVIN'S PART ***************
// Function to compare a word with a dictionary
int compare_each_word(const char *word, const char *dictionary_file) {
    FILE *file;  // Declare a variable to store a file pointer
    char line[10000];  // Declare a character array to store each line from the dictionary file

    int found = 0;  // Declare an integer variable to indicate whether the word is found in the dictionary or not


    // Open the dictionary file in read-only mode 
    file = fopen(dictionary_file, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return -1;
    }

    // Read each line from the dictionary file
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Compare the word with the current line from the dictionary
        if (strcasecmp(word, line) == 0) {
            found = 1;  // If the word matches the current line, set the found variable to 1
            break;     // Exit the loop
        }
    }

    // Close the dictionary file
    fclose(file);

    return found;
}
// ***************** COMPARE FUNCTION ENDS *********

void use_line(void *arg, char *line) {
    if (word_count < MAX_WORDS) {
        words[word_count++] = strdup(line);
        if (DEBUG) printf("Stored: %s\n", line);
    } else {
        printf("Reached maximum word count. Cannot store more words.\n");
    }
}

//*****************************************************************
void print_line(void *st, char *line, char *dict){
  int *p = st;  //set pointer pointing n (line count)

//DEBUG
  //char *dict = "words";
  // if we specified any dict file, open that file (argv[1]), if not, words
  int fd_dict = open(dict, O_RDONLY);
  if(fd_dict < 0){
    perror(dict);
    exit(EXIT_FAILURE);
  }

  //const char* dictionary_file = "words";
  if (compare_each_word(line, dict) == 1) {
      printf("The word '%s' is Correct.\n", line);
  } else {
      printf("The word '%s' is not in the dictionary.\n", line);
    wrong_count++;
  }


  if (DEBUG) printf("%d: %s\n", *p, line);
  (*p)++;  //pointing next line
}

//*****************************************************************
int main(int argc, char **argv){
  char *dict = argc > 2 ? argv[1] : "words";
  // if we specified any dict file, open that file (argv[1]), if not, words
  int fd_dict = open(dict, O_RDONLY);
  if(fd_dict < 0){
    perror(dict);
    exit(EXIT_FAILURE);
  }

  char *fname = argc > 2 ? argv[2] : "test.txt";
  // if we specified any txt file, open that file (argv[2]), if not, test.txt
  int fd = open(fname, O_RDONLY);
  if(fd < 0){
    perror(fname);
    exit(EXIT_FAILURE);
  }

  const char *word = " "; // Declare a const char pointer to the word to be checked

  int n = 0; //line counts
  read_lines(fd, print_line, &n, dict);

  printf("Your Wrong word count is: %d \n", wrong_count);

  return EXIT_SUCCESS;; // Return 0 to indicate successful execution

}