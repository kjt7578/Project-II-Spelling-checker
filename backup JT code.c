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

void read_lines(int fd, void(*use_line)(void *, char *, char *, char *), void *arg, char *dict, char *filepath) {
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
                row_ct++;
                check = 0;
            }

            if (buf[pos] == 10) {
                check = 1;
            }

            if (buf[pos] == 10 || buf[pos] == 32) {
                char *line_start_ptr = original_buf + line_start;
                //int line_length = pos - line_start;
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


// **************** CAP FOR TEXT FILES ENDS **************
void print_line(void *st, char *line, char *dict, char *filepath) {
    int *p = st;
    int line_number = *p;

    int fd_dict = open(dict, O_RDONLY);
    if (fd_dict < 0) {
        printf("File open error");
        wrong_count++;
        perror(dict);
        exit(EXIT_FAILURE);
    }

    char original_line[100];
    strcpy(original_line, line);
    strip_special_chars(line);


    int column_number = 0;
    char *ptr = line;
    while (*ptr != '\0' && *ptr != original_line[column_number]) {
        column_number++;
        ptr++;
    }

    if (strchr(line, '-') != NULL) {
        if (check_split_words(line, dict) == 0) {
            wrong_count++;
            printf("%s (%d,%d): %s\n", filepath, row_ct, col_ct, original_line);
            col_ct++;
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
                            printf("%s (%d,%d): %s\n", filepath, row_ct, col_ct, original_line);
                            col_ct++;
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
                        printf("%s (%d,%d): %s\n", filepath, row_ct, col_ct, original_line);
                        col_ct++;
                    }
                }
            }
        }
        else {
            //printf("%s (%d,%d): %s : CORRECT\n", filepath, row_ct, col_ct, original_line);
            col_ct++;
        }
    }
    if (DEBUG) printf("%d: %s\n", *p, line);
    (*p)++;
}


//*****************************************************************
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
