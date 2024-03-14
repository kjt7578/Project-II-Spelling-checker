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

#define MAX_WORDS 512
#define BUFLENGTH 256
const char *specialChars = ",\"()[]{}.?':/@*#$%^&._=1234567890";
char *words[MAX_WORDS];
int word_count = 0;
int wrong_count = 0;

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

void read_lines(int fd, void(*use_line)(void *, char *, char *), void *arg, char *dict){
  int buflength = BUFLENGTH;
  char *buf = malloc(BUFLENGTH);

  int pos = 0;  
  int bytes;      
  int line_start;

  while((bytes = read(fd, buf + pos, buflength - pos)) > 0){
    if(DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
    line_start = 0;
    int bufend = pos + bytes;

    if (DEBUG) printf("Written bytes: %d\n", bytes);

    while(pos < bufend){
      if(DEBUG) printf("start %d, pos %d, char '%c'\n",line_start, pos, buf[pos]);

      if (pos == line_start && strchr("\"(['{", buf[pos]) != NULL) {
          line_start++;
      } else if (strchr(specialChars, buf[pos]) != NULL) {
          buf[pos] = '\0';
      }

      if(buf[pos] == '\n' || buf[pos] == ' '){
        buf[pos] = '\0'; 

        use_line(arg, buf + line_start, dict);
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
      pos = segment_length;
      if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
    }
    else if(bufend == buflength){
      buflength *= 2;              
      buf = realloc(buf, buflength);
      if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
    }
  } 
  if (pos > 0){
    if(pos == buflength){
      buf = realloc(buf, buflength + 1);
    }
    buf[pos] = '\0';
    use_line(arg, buf + line_start, dict);
  }
  free(buf);
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

void print_line(void *st, char *line, char *dict){
  int *p = st;
  
  int fd_dict = open(dict, O_RDONLY);
  if(fd_dict < 0){
    perror(dict);
    exit(EXIT_FAILURE);
  }

  if (strchr(line, '-') != NULL) {
      if (check_split_words(line, dict) == 0) {
          wrong_count++;
          printf("The compound word '%s' is not in the dictionary.\n", line);
      } else {
          printf("The compound word '%s' is in the dictionary.\n", line);
      }
  } else {
  if (compare_each_word(line, dict) == 0) {
    char lowercase_word[100];
    strcpy(lowercase_word, line);
    strlwr(lowercase_word);

    if (compare_each_word(lowercase_word, dict) == 0) {
        char initial_capital_word[100];
        strncpy(initial_capital_word, line, 1);
        strcpy(initial_capital_word + 1, lowercase_word + 1);
        if (compare_each_word(initial_capital_word, dict) == 0) {
            char all_caps_word[100];
            strcpy(all_caps_word, line);
            strupr(all_caps_word);
            if (compare_each_word(all_caps_word, dict) == 0) {
                if (isalpha(line[0]) && strlen(line) > 1 && strspn(line + 1, "abcdefghijklmnopqrstuvwxyz") == strlen(line) - 1) {
                } else {
                    wrong_count++;
                    printf("The word '%s' is not in the dictionary.\n", line);
                }
            }
        }
    } else {
        if (compare_each_word(lowercase_word, dict) != 0) {
            char initial_capital_word[100];
            strncpy(initial_capital_word, line, 1);
            strcpy(initial_capital_word + 1, lowercase_word + 1);
            if (compare_each_word(initial_capital_word, dict) == 0) {
            } else {
                wrong_count++;
                printf("The word '%s' is not in the dictionary.\n", line);
            }
        }
    }
  } else {
    printf("The word '%s' is in the dictionary.\n", line);
  }
     }
  if (DEBUG) printf("%d: %s\n", *p, line);
  (*p)++;
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
                printf("Opening file: %s\n", filepath);

                int fd = open(filepath, O_RDONLY);
                if(fd < 0) {
                    perror(filepath);
                    continue;
                }

                int n = 0;
                wrong_count = 0;
                read_lines(fd, print_line, &n, dict);
                printf("Your Wrong word count is: %d \n\n", wrong_count);


                close(fd);
            }
        }
        closedir(d);
    } else {
        perror("Directory open error");
    }
}


int main(int argc, char **argv) {
    char *dict_dir = argc > 1 ? argv[1] : ".";
    char *dict = "words";

    search_directory(dict_dir, dict);

    return EXIT_SUCCESS;
}