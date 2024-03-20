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

#define MAX_WORDS 1033
#define BUFLENGTH 1033



// Function to convert a word to lowercase
void to_lower(char *my_word) {
  for (int i = 0; my_word[i]; i++) {
    my_word[i] = tolower(my_word[i]);
  }
}


// Function to determine if a file has a ".txt" extension
int is_it_txt_file(const char *file_name) {
    // Get the length of the filename
    size_t length_of_filename = strlen(file_name);

    // Check if the length of the filename is greater than or equal to 4 and the last 4 characters match ".txt"
    return length_of_filename >= 4 && strcmp(file_name + length_of_filename - 4, ".txt") == 0;
}


// Function to compare two words for sorting
int compare_words(const void *a, const void *b) {
    // Cast the input pointers to pointers to strings
    const char *word1 = *(const char **)a;
    const char *word2 = *(const char **)b;

    // Compare the two strings and return the result
    return strcmp(word1, word2);
}


int is_it_regular_file(const char *dir_path) {
    // Structure to store file information
    struct stat path_stat;

    // Get the file information for the given path
    stat(dir_path, &path_stat);

    // Check if the file information indicates that the path corresponds to a regular file
    return S_ISREG(path_stat.st_mode);
}

// Determines if the specified path corresponds to a directory.
int is_it_directory(const char *dir_path) {
    struct stat path_stat;

    // Retrieve file status information for the given path.
    stat(dir_path, &path_stat);

    // Return a non-zero value if the path is a directory.
    return S_ISDIR(path_stat.st_mode);
}


// Function to determine if a character is a valid 
int is_it_word_character(char k) {
    // Check if the character is alphanumeric (a letter or a number) or it is a hyphen or an apostrophe
    return isalnum(k) || k == '-' || k == '\'';
}


int check_capitalization(const char *my_word, const char *dict) {
    // Check if the input word matches the dictionary word exactly
    if (strcmp(my_word, dict) == 0) {
        return 1; // If the words match, the input word is correctly capitalized
    }

    // Check if the lengths of the input word and dictionary word match
    if (strlen(my_word) != strlen(dict)) {
        return 0; // If the lengths don't match, the input word is not correctly capitalized
    }

    // Initialize variables to track capitalization
    int is_all_uppercase = 1, is_first_letter_uppercase_rest_lowercase = 1;

    // Check if the input word is all uppercase
    for (int i = 0; my_word[i]; ++i) {
        if (!islower(my_word[i])) {
            is_all_uppercase = 0;
            break;
        }
    }
  
    // If the input word is all uppercase, then it is correctly capitalized
    if (is_all_uppercase) {
        return 1;
    }

    // Check if the first letter of the input word matches the first letter of the dictionary word
    if (my_word[0] != toupper(dict[0])) {
        is_first_letter_uppercase_rest_lowercase = 0;
    }

    // Check if the remaining letters of the input word match the corresponding letters in the dictionary word
    for (int i = 1; my_word[i]; ++i) {
        if (my_word[i] != tolower(dict[i])) {
            is_first_letter_uppercase_rest_lowercase = 0;
            break;
        }
    }

    return is_first_letter_uppercase_rest_lowercase; // Input word is correctly capitalized if the first letter is uppercase and the rest are lowercase
}


// Function to search for a word in a dictionary
int search_the_word(const char *my_word, char **dict, int dict_size) {
    // Create a lower case version of the word
    char lower_case_word[BUFLENGTH];
    strncpy(lower_case_word, my_word, BUFLENGTH - 1);
    lower_case_word[BUFLENGTH - 1] = '\0';
    to_lower(lower_case_word);

    // Iterate through the dictionary
    for (int i = 0; i < dict_size; i++) {
        // Check if the current dictionary word matches the lower case word
        if (strcmp(lower_case_word, dict[i]) == 0) {
            // Check if the capitalization of the word is correct
            return check_capitalization(my_word, dict[i]);
        }
    }
    // The word was not found in the dictionary
    return 0;
}


void spellchecker(const char *file_name, char **dict, int dict_size) {
    // Open the file
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFLENGTH], word[BUFLENGTH] = {0};
    ssize_t bytes_read;
    int line_number = 1, column_number = 1;
    int word_length = 0;
    int column_of_word_start = 1;

    // Read the file buffer by buffer
    while ((bytes_read = read(fd, buffer, BUFLENGTH)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            // Check if the character is a part of a word
            if (is_it_word_character(buffer[i])) {
                // If yes, add it to the word buffer
                if (word_length < BUFLENGTH - 1) {
                    my_word[word_length++] = buffer[i];
                }
            } else {
                // If the character is not a part of a word
                if (word_length > 0) {
                    // If there's a word in the buffer, process it
                    my_word[word_length] = '\0';
                    // Check if the word exists in the dictionary
                    if (!search_the_word(my_word, dict, dict_size)) {
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
        my_word[word_length] = '\0';
        // Check if the word exists in the dictionary
        if (!search_word(my_word, dict, dict_size)) {
            // Print the misspelled word along with its position
            printf("%s (%d,%d): %s\n", file_name, line_number, column_of_word_start, my_word);
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
        perror("Unable to open directory");
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

// Main function
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
    int dict_word_length = 0;
    char word[BUFLENGTH];

    while ((bytes_read = read(dict_fd, dict_buffer, BUFLENGTH)) > 0) {
        for (int i = 0; i < dict_bytes_read; ++i) {
            if (dict_buffer[i] != '\n') {
                word[dict_word_length++] = dict_buffer[i];
            } else {
                word[dict_word_length] = '\0';
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
                dict[dict_size++] = strdup(dict_word);
                dict_word_length = 0;
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