#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define MAX_WORDS 2000
#define BUFLENGTH 2000
char *words[MAX_WORDS];
int word_count = 0;
int wrong_count = 0;
int row_ct = 1;
int col_ct = 1;
int check = 0;


char* strlwr(char* s) {
    char* p = s;
    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }
    return s;
}

char* strupr(char* s) {
    char* p = s;
    while (*p) {
        *p = toupper((unsigned char)*p);
        p++;
    }
    return s;
}

oid read_lines(int fd, void(*use_line)(void *, char *, char *, char *), void *arg, char *dict, char *filepath) {
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);
    char *original_buf = malloc(BUFLENGTH);

    int pos = 0;
    int bytes;
    int line_start = 0;

  while ((bytes = read(fd, buf + pos, BUFLENGTH - pos)) > 0) {
  if (DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
  memcpy(original_buf, buf, BUFLENGTH);
  int bufend = pos + bytes;

  while (pos < bufend) {
    if (check == 1){
      col_ct = 1;
      check = 0;
    }

      if (buf[pos] == '\n' || buf[pos] == ' ') {
        if(buf[pos] == '\n'){
          row_ct++;
          check = 1;
        }
          char *line_start_ptr = original_buf + line_start;
          int line_length = pos - line_start;
          original_buf[pos] = '\0';
          use_line(arg, line_start_ptr, dict, filepath);
          line_start = pos + 1;
      }
      pos++;
  }

        if (line_start == pos){
            pos = 0;
        }
        else if (line_start > 0){
            int segment_length = pos - line_start;
            memmove(buf, buf + line_start, segment_length); 
            memmove(original_buf, original_buf + line_start, segment_length);
            pos = segment_length;
            if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
        }
        else if(bufend == buflength){
            buflength *= 2;              
            buf = realloc(buf, buflength);
            original_buf = realloc(original_buf, buflength);
            if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
        }
    }
    if (pos > 0){
        if(pos == buflength){
            buf = realloc(buf, buflength + 1);
            original_buf = realloc(original_buf, buflength + 1);
        }
        original_buf[pos] = '\0';
        use_line(arg, original_buf + line_start, dict, filepath);
    }
    free(buf);
    free(original_buf);
}

int compare_each_word(const char *word, const char *dictionary_file) {
    FILE *file;
    char line[10000];

    int found = 0; 
    file = fopen(dictionary_file, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return -1;
    }


    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        if (strcasecmp(word, line) == 0) {
            found = 1;
            break;
        }
    }

    fclose(file);

    return found;
}

void use_line(void *arg, char *line) {
    if (word_count < MAX_WORDS) {
        words[word_count++] = strdup(line);
        if (DEBUG) printf("Stored: %s\n", line);
    } else {
        printf("Reached maximum word count. Cannot store more words.\n");
    }
}


int check_split_words(const char *word, const char *dictionary_file) {
    char *copy_of_word = strdup(word);
    char *token = strtok(copy_of_word, "-");
    int result = 1;

    while (token != NULL) {
        if (compare_each_word(token, dictionary_file) == 0) {
            result = 0;
            break;
        }
        token = strtok(NULL, "-");
    }

    free(copy_of_word);
    return result;
}


void strip_special_chars(char* word) {
    char* start = word;
    char* end;

    while (*start && !isalpha((unsigned char)*start)) {
        start++;
    }
    if (*start == 0) {
        *word = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && !isalpha((unsigned char)*end)) {
        end--;
    }

    memmove(word, start, end - start + 1);
    word[end - start + 1] = '\0';
}




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

  char original_line[100];
  strcpy(original_line, line);
  strip_special_chars(line);

  if (strchr(line, '-') != NULL) {
      if (check_split_words(line, dict) == 0) {
          wrong_count++;
          printf("The compound word '%s' is not in the dictionary.\n", original_line);
      } else {
          printf("The compound word '%s' is in the dictionary.\n", original_line);
      }
  } else {
  //******** CHECKING FOR CAPITALIZATION IN THE TEXT FILES ************* KELVIN'S PART

  // Problem with the If Statement: Checking for words that are All-Uppercase and All-lowercase works, but checking Mixed-case letters like "heLLo" doesnt work. Needs to be fixed!
  // Tip: Just store all the capital letter words in your data structure.


  if (compare_each_word(line, dict) == 0) { // Check if the word is not found in the dictionary
    // Convert the word to lowercase
    char lowercase_word[100];
    strcpy(lowercase_word, line); // Copy the word to the lowercase_word array
    strlwr(lowercase_word); // Convert the lowercase_word array to lowercase

    // Check if the lowercase word is in the dictionary
    if (compare_each_word(lowercase_word, dict) == 0) { // Check if the lowercase word is not found in the dictionary
        // Check if the word with initial capitalization is in the dictionary
        char initial_capital_word[100];
        strncpy(initial_capital_word, line, 1); // Copy the first character of the word to the initial_capital_word array
        strcpy(initial_capital_word + 1, lowercase_word + 1); // Copy the rest of the characters from the lowercase word to the initial_capital_word array
        if (compare_each_word(initial_capital_word, dict) == 0) { // Check if the initial_capital_word is not found in the dictionary
            // Check if the word in all capitals is in the dictionary
            char all_caps_word[100];
            strcpy(all_caps_word, line); // Copy the word to the all_caps_word array
            strupr(all_caps_word); // Convert the all_caps_word array to all capitals
            if (compare_each_word(all_caps_word, dict) == 0) { // Check if the all_caps_word is not found in the dictionary
                // Check if the word has a single upper case letter followed by all lower case letters
                if (isalpha(line[0]) && strlen(line) > 1 && strspn(line + 1, "abcdefghijklmnopqrstuvwxyz") == strlen(line) - 1) {
                    // If the word is found in the dictionary, do nothing
                } else {
                    // If the word is not in the dictionary, increment the wrong word count
                    wrong_count++;
                    // Print a message indicating that the word is not in the dictionary
                    printf("The word '%s' is not in the dictionary.\n", original_line);
                }
            }
        } else {
            // Check if the word with initial capitalization is found in the dictionary, do nothing
        }
    } else {
        // Check if the lowercase word is found in the dictionary
        if (compare_each_word(lowercase_word, dict) != 0) {
            // Check if the word with initial capitalization is in the dictionary
            char initial_capital_word[100];
            strncpy(initial_capital_word, line, 1); // Copy the first character of the word to the initial_capital_word array
            strcpy(initial_capital_word + 1, lowercase_word + 1); // Copy the rest of the characters from the lowercase word to the initial_capital_word array
            if (compare_each_word(initial_capital_word, dict) == 0) {
                // If the word with initial capitalization is found in the dictionary, do nothing
            } else {
                // If the word with initial capitalization is not found in the dictionary, increment the wrong word count
                wrong_count++;
                // Print a message indicating that the word is not in the dictionary
                printf("The word '%s' is not in the dictionary.\n", original_line);
            }
        }
    }
  } else {
    // If the word is found in the dictionary, print a message indicating so
    printf("The word '%s' is in the dictionary.\n", original_line);
  }
  }

  if (DEBUG) printf("%d: %s\n", *p, original_line);
  (*p)++; // pointing next line

}  


void search_directory(char *dir_path, char *dict) {
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }

            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, dir->d_name);

            struct stat path_stat;
            stat(filepath, &path_stat);

            if (S_ISDIR(path_stat.st_mode)) {
                search_directory(filepath, dict);
            } else if (strstr(dir->d_name, ".txt") != NULL) {
                row_ct = 1;
                col_ct = 1;
                //printf("Opening file: %s\n", filepath);

                int fd = open(filepath, O_RDONLY);
                if(fd < 0) {
                    perror(filepath);
                    printf("File open error");
                    wrong_count++;
                    continue;
                }

                int n = 0;
                //wrong_count = 0;
                read_lines(fd, print_line, &n, dict, filepath);
                //printf("Your Wrong word count is: %d \n\n", wrong_count);
                close(fd);
            }
        }
        closedir(d);
    } else {
        printf("Directory open error");
        wrong_count++;
    }

}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s <dictionary> <file/dir> [<file2> <file3> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *dict = argv[1];
  if (access(dict, F_OK) == -1) {
      printf("Dictionary file '%s' does not exist.\n", dict);
      return EXIT_FAILURE;
  }
    for (int i = 2; i < argc; i++) {
        struct stat path_stat;
        stat(argv[i], &path_stat);
        if (S_ISDIR(path_stat.st_mode)) {
            search_directory(argv[i], dict);
        } else if (strstr(argv[i], ".txt") != NULL) {
            int fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                perror(argv[i]);
                printf("File open error");
                wrong_count++;
                continue;
            }

            int n = 0;
            row_ct = 1;
            col_ct = 1;
            read_lines(fd, print_line, &n, dict, argv[i]);
            close(fd);
        } else {
            printf("Invalid argument: %s\n", argv[i]);
        }
    }

    if (wrong_count == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
