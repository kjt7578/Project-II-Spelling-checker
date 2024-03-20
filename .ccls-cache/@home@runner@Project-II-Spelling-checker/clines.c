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
#define BUFLENGTH 2000
char *words[MAX_WORDS];
int word_count = 0;
int wrong_count = 0;
int row_ct = 0;
int col_ct = 0;
int check = 0;


// Function to converta word to lowercase 
char* strlwr(char* s) {
    char* p = s;
    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }
    return s;
}

// Function to convert a word to uppercase
char* strupr(char* s) {
    char* p = s;
    while (*p) {
        *p = toupper((unsigned char)*p);
        p++;
    }
    return s;
}

// Function to determine if a file has a ".txt" extension
int is_it_txt_file(const char *filename) {
    // Get the length of the filename
    size_t length_of_filename = strlen(filename);

    // Check if the length of the filename is greater than or equal to 4 and the last 4 characters match ".txt"
    return length_of_filename >= 4 && strcmp(filename + length_of_filename - 4, ".txt") == 0;
}


// Determines if the specified path corresponds to a directory.
int is_it_directory(const char *dir_path) {
    struct stat path_stat;

    // Retrieve file status information for the given path.
    stat(dir_path, &path_stat);

    // Return a non-zero value if the path is a directory.
    return S_ISDIR(path_stat.st_mode);
}

/*
void read_lines(int fd, void(*use_line)(void *, char *, char *, char *), void *arg, char *dict, char *filepath) {
    int buflength = BUFLENGTH;
    char *buf = malloc(BUFLENGTH);
    char *original_buf = malloc(BUFLENGTH); 

    int pos = 0;
    int bytes;
    int line_start = 0;

    while ((bytes = read(fd, buf + pos, BUFLENGTH - pos)) > 0) {
        if (DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
        
        memcpy(original_buf, buf, bytes); 

        int bufend = pos + bytes;

        while (pos < bufend) {
            if (check == 1) {
                col_ct = 1;
                check = 0;
            }

            if (buf[pos] == '\n' || buf[pos] == ' ') {
                if (buf[pos] == '\n') {
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

        if (line_start == pos) {
            pos = 0;
        }
        else if (line_start > 0) {
            int segment_length = pos - line_start;
            memmove(buf, buf + line_start, segment_length);
            memmove(original_buf, original_buf + line_start, segment_length);
            pos = segment_length;
            if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
        }
        else if (bufend == buflength) {
            buflength *= 2;
            buf = realloc(buf, buflength);
            original_buf = realloc(original_buf, buflength); // original_buf의 크기를 조정
            if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
        }
    }
    if (pos > 0) {
        if (pos == buflength) {
            buf = realloc(buf, buflength + 1);
            original_buf = realloc(original_buf, buflength + 1); // original_buf의 크기를 조정
        }
        original_buf[pos] = '\0';
        use_line(arg, original_buf + line_start, dict, filepath);
    }
    free(buf);
    free(original_buf);
}
*/

// Function to determine if a path corresponds to a regular file
int is_it_regular_file(const char *path) {
    // Structure to store file information
    struct stat path_stat;

    // Get the file information for the given path
    stat(path, &path_stat);

    // Check if the file information indicates that the path corresponds to a regular file
    return S_ISREG(path_stat.st_mode);
}

// Function to determine if a character is a valid 
int is_it_word_character(char c) {
    // Check if the character is alphanumeric (a letter or a number) or it is a hyphen or an apostrophe
    return isalnum(c) || c == '-' || c == '\'';
}


/*
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
*/

void use_line(void *arg, char *line) {
    if (word_count < MAX_WORDS) {
        words[word_count++] = strdup(line);
        if (DEBUG) printf("Stored: %s\n", line);
    } else {
        printf("Reached maximum word count. Cannot store more words.\n");
    }
}

// Function to compare two words for sorting in alphabetical order
int compare_words(const void *a, const void *b) {
    // Cast the input pointers to pointers to strings
    const char *word1 = *(const char **)a;
    const char *word2 = *(const char **)b;

    // Compare the two strings and return the result
    return strcmp(word1, word2);
}


/*
int check_split_words(const char *word, const char *dictionary_file) {
    char *copy_of_word = strdup(word);
    char *token = strtok(copy_of_word, "-");
    int result = 1;

    while (token != NULL) {
        if (compare_word(token, dictionary_file) == 0) {
            result = 0;
            break;
        }
        token = strtok(NULL, "-");
    }

    free(copy_of_word);
    return result;
}
*/

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


/*
void print_line(void *st, char *line, char *dict, char *filepath) {
    int *p = st;
    int line_number = *p;

    int fd_dict = open(dict, O_RDONLY);
    if (fd_dict < 0) {
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
        if (compare_word(line, dict) == 0) {
            char lowercase_word[100];
            strcpy(lowercase_word, line);
            strlwr(lowercase_word);

            if (compare_word(lowercase_word, dict) == 0) {
                char initial_capital_word[100];
                strncpy(initial_capital_word, line, 1);
                strcpy(initial_capital_word + 1, lowercase_word + 1);
                if (compare_word(initial_capital_word, dict) == 0) {
                    char all_caps_word[100];
                    strcpy(all_caps_word, line);
                    strupr(all_caps_word);
                    if (compare_word(all_caps_word, dict) == 0) {
                        if (isalpha(line[0]) && strlen(line) > 1 && strspn(line + 1, "abcdefghijklmnopqrstuvwxyz") == strlen(line) - 1) {
                        } else {
                            wrong_count++;
                          printf("%s (%d,%d): %s\n", filepath, row_ct, col_ct, original_line);
                          col_ct++;
                        }
                    }
                }
            } else {
                if (compare_word(lowercase_word, dict) != 0) {
                    char initial_capital_word[100];
                    strncpy(initial_capital_word, line, 1);
                    strcpy(initial_capital_word + 1, lowercase_word + 1);
                    if (compare_word(initial_capital_word, dict) == 0) {
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
*/





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
                read_lines(fd, print_line, &n, dict, filepath);
                printf("Your Wrong word count is: %d \n\n", wrong_count);\
                row_ct = 0;
                col_ct = 0;
                close(fd);
            }
        }
        closedir(d);
    } else {
        perror("Directory open error");
    }
}


// Function to search for a word in a dictionary
int search_the_word(const char *word, char **dict, int dict_size) {
    // Create a lower case version of the word
    char lower_case_word[BUFLENGTH];
    strncpy(lower_case_word, word, BUFLENGTH - 1);
    lower_case_word[BUFLENGTH - 1] = '\0';
    strlwr(lower_case_word);

    // Iterate through the dictionary
    for (int i = 0; i < dict_size; i++) {
        // Check if the current dictionary word matches the lower case word
        if (strcmp(lower_case_word, dict[i]) == 0) {
            // Check if the capitalization of the word is correct
            return check_capitalization(word, dict[i]);
        }
    }
    // The word was not found in the dictionary
    return 0;
}

void spellchecker(const char *filename, char **dict, int dict_size) {
    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFLENGTH], word[BUFLENGTH] = {0};
    ssize_t bytes_read;
    int word_length = 0;
    int line_number = 1, column_number = 1;
    int column_of_word_start = 1;

    // Read the file buffer by buffer
    while ((bytes_read = read(fd, buffer, BUFLENGTH)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            // Check if the character is a part of a word
            if (is_it_word_character(buffer[i])) {
                // If yes, add it to the word buffer
                if (word_length < BUFLENGTH - 1) {
                    word[word_length++] = buffer[i];
                }
            } else {
                // If the character is not a part of a word
                if (word_length > 0) {
                    // If there's a word in the buffer, process it
                    word[word_length] = '\0';
                    // Check if the word exists in the dictionary
                    if (!search_the_word(word, dict, dict_size)) {
                        // Print the misspelled word along with its position
                        printf("%s (%d,%d): %s\n", filename, line_number, column_of_word_start, word);
                    }
                    word_length = 0;
                }
                // If the character is a newline, update line and column numbers
                if (buffer[i] == '\n') {
                    line_number++;
                    column_number = 0;
                }
                column_of_word_start = column_number + 1;
            }
            column_number++;
        }
    } 

    // Check for the last word in case the file does not end with a newline
    if (word_length > 0) {
        // If there's a word in the buffer, process it
        word[word_length] = '\0';
        // Check if the word exists in the dictionary
        if (!search_the_word(word, dict, dict_size)) {
            // Print the misspelled word along with its position
            printf("%s (%d,%d): %s\n", filename, line_number, column_of_word_start, word);
        }
    }

    // Close the file
    close(fd);
}

void scandirectory_recursively(const char *name_of_dir, char **dict, int dict_size) {
    DIR *d;
    struct dirent *dir;

    // Open the directory
    if (!(d = opendir(name_of_dir))) {
        perror("Can not open directory");
        return;
    }

    // Iterate through directory entries
    while ((dir = readdir(d)) != NULL) {
        // Skip current and parent directory entries
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;

        // Construct the full path of the entry
        char dir_path[BUFLENGTH];
        snprintf(dir_path, sizeof(dir_path), "%s/%s", name_of_dir, dir->d_name);

        // Check if the entry is a directory
        if (is_it_directory(dir_path)) {
            // Scan subdirectories recursively 
            scandirectory_recursively(dir_path, dict, dict_size);
        } 
        // Check if the entry is a regular file and a text file
        else if (is_it_regular_file(dir_path) && is_it_txt_file(dir->d_name)) {
            // Perform spell-checking on text files
            spellchecker(dir_path, dict, dict_size);
        }
    }

    // Close the directory
    closedir(d);
}


/*
int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dictionary> <file/dir> \n", argv[0]);
        return EXIT_FAILURE;
    }


    int cap = MAX_WORDS;
    char **dict = malloc(cap * sizeof(char *));
    if (!dict) {
        fprintf(stderr, "Failed to allocate memory for dictionary\n");
        return EXIT_FAILURE;
    }
    int dict_size = 0;

    // Open the dictionary file
    int dict_fd = open(argv[1], O_RDONLY);
    if (dict_fd == -1) {
        perror("Error opening dictionary file");
        free(dict);
        return EXIT_FAILURE;
    }


    char dict_buffer[BUFLENGTH];
    ssize_t bytes_read;
    int dict_word_len = 0;
    char word[BUFLENGTH];

    while ((bytes_read = read(dict_fd, dict_buffer, BUFLENGTH)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            if (dict_buffer[i] != '\n') {
                word[dict_word_len++] = dict_buffer[i];
            } else {
                word[dict_word_len] = '\0';
                if (dict_size == cap) {
                    cap *= 2;
                    char **new_dictionary = realloc(dict, cap * sizeof(char *));
                    if (!new_dictionary) {
                        perror("Failed to reallocate memory for the dictionary");
                        close(dict_fd);
                        for (int j = 0; j < dict_size; j++) free(dict[j]);
                        free(dict);
                        return EXIT_FAILURE;
                    }
                    dict = new_dictionary;
                }
                dict[dict_size++] = strdup(word);
                dict_word_len = 0;
            }
        }
    }

    // Close the dictionary file
    close(dict_fd);

    // Sort the dictionary array alphabetically
    qsort(dict, dict_size, sizeof(char *), compare_words);

    // Perform spell-checking on the provided directory or file
    if (is_it_directory(argv[2])) {
        scandirectory_recursively(argv[2], dict, dict_size); // Start recursive directory scanning
    } else if (is_it_regular_file(argv[2]) && is_it_txt_file(argv[2])) {
        spellchecker(argv[2], dict, dict_size); // Perform spell-checking on the single text file
    } else {
        fprintf(stderr, "%s is not a valid directory or .txt file\n", argv[2]);
    }


    for (int i = 0; i < dict_size; ++i) {
        free(dict[i]);
    }
    free(dict);

    return EXIT_SUCCESS;
}
*/


int main(int argc, char **argv) {
    char *txt_dir = argc > 1 ? argv[1] : ".";
    char *dict = "fruit";

    search_directory(txt_dir, dict);

    return EXIT_SUCCESS;
}



// Garbage Collector 


/*
void read_lines(int fd, void(*use_line)(void *, char *, char *, char *), void *arg, char *dict, char *filepath) {
    int buflength = BUFLENGTH;
    int buf_length = BUFLENGTH; // New variable to track buffer length
    char *buf = malloc(BUFLENGTH);
    char *original_buf = malloc(BUFLENGTH);

    int pos = 0;
    int bytes;
    int line_start = 0;

    while ((bytes = read(fd, buf + pos, buf_length - pos)) > 0) { // Use buf_length instead of BUFLENGTH
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
            buf_length *= 2; // Update buf_length
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
*/


/*
int compare_each_word(const char *word, const char *dictionary_file) {
    FILE *file;
    char line[10000];

    int found = 0;
    file = fopen(dictionary_file, "r");
    if (file == NULL) {
        perror("Error opening dictionary file");
        return -1;
    }

    char lowercase_word[100];
    char initial_capital_word[100];
    char all_caps_word[100];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        strcpy(lowercase_word, word);
        strlwr(lowercase_word);

        strncpy(initial_capital_word, word, 1);
        strcpy(initial_capital_word + 1, lowercase_word + 1);

        strcpy(all_caps_word, word);
        strupr(all_caps_word);

        if (strcasecmp(word, line) == 0 || strcasecmp(initial_capital_word, line) == 0 || strcasecmp(all_caps_word, line) == 0) {
            found = 1;
            break;
        }
    }

    fclose(file);

    return found;
}
*/


/*
void use_line(void *arg, char *line, char *dict, char *filepath) {
    if (word_count < MAX_WORDS) {
        words[word_count++] = strdup(line);
        if (DEBUG) printf("Stored: %s\n", line);
    } else {
        printf("Reached maximum word count. Cannot store more words.\n");
    }
}
*/


/*
int main(int argc, char **argv) {
    if (argc < 3) {
        printf(stderr, "Usage: %s <dictionary> <file/dir> [<file2> <file3> ...]\n", argv[0]);
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
*/





/*
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
*/


// int check_capitalization(const char *my_word, const char *dict) {
//     // Check if the input word matches the dictionary word exactly
//     if (strcmp(my_word, dict) == 0) {
//         return 1; // If the words match, the input word is correctly capitalized
//     }

//     // Check if the lengths of the input word and dictionary word match
//     if (strlen(my_word) != strlen(dict)) {
//         return 0; // If the lengths don't match, the input word is not correctly capitalized
//     }

//     // Initialize variables to track capitalization
//     int is_all_uppercase = 1, is_first_letter_uppercase_rest_lowercase = 1;

//     // Check if the input word is all uppercase
//     for (int i = 0; my_word[i]; ++i) {
//         if (!islower(my_word[i])) {
//             is_all_uppercase = 0;
//             break;
//         }
//     }

//     // If the input word is all uppercase, then it is correctly capitalized
//     if (is_all_uppercase) {
//         return 1;
//     }

//     // Check if the first letter of the input word matches the first letter of the dictionary word
//     if (my_word[0] != toupper(dict[0])) {
//         is_first_letter_uppercase_rest_lowercase = 0;
//     }

//     // Check if the remaining letters of the input word match the corresponding letters in the dictionary word
//     for (int i = 1; my_word[i]; ++i) {
//         if (my_word[i] != tolower(dict[i])) {
//             is_first_letter_uppercase_rest_lowercase = 0;
//             break;
//         }
//     }

//     return is_first_letter_uppercase_rest_lowercase; // Input word is correctly capitalized if the first letter is uppercase and the rest are lowercase
// }


