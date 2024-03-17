Names: Kelvin Ihezue (netID: ki120) and Jeongtae Kim (netID: jk2065)

1)Descriptions of functions in lines.c

- readlines: This function operates by reading lines from a file descriptor referred to as fd and applies a specified function, use_line, to each line. It utilizes two buffers, namely buf and original_buf, to hold the file's contents, dynamically resizing them as necessary. The process involves iterating through these buffers, identifying newline or space characters, recording the line's starting position, computing its length, and then invoking the use_line function for each line encountered. Following line processing, the function adjusts the buffer positions and sizes accordingly. Upon exhausting all bytes from the file descriptor, any remaining bytes in the buffer will undergo processing via the use_line function, after which memory allocated for the buffers is deallocated.

- compare_each_word: This function compares a given word to each line in a dictionary file and returns an integer indicating whether the word is found in the dictionary or not. It declares a file pointer and a character array to store each line from the dictionary file. The function opens the dictionary file in read-only mode and reads each line from the file, removing any trailing newline characters. It then compares the given word to each line using the strcasecmp function, which compares the strings case-insensitively. If the word matches a line in the dictionary, the function sets the found variable to 1 and exits the loop. Finally, the function closes the dictionary file and returns the found variable.

- use_line: This function takes in a void pointer arg and a character pointer line as arguments. It checks if the number of words stored so far is less than the maximum word count. If it is, the function stores the line in the words array using the strdup function and increments the word_count variable. If the maximum word count has been reached, the function prints an error message. The DEBUG flag is used to print debugging information.

- check_split_words: This function checks if a hyphenated word is spelled correctly by splitting it into individual words and comparing each word to a dictionary file. It takes in a character pointer word and a character pointer dictionary_file as arguments. The function creates a copy of the word using the strdup function and splits it into individual words using the strtok function. It then compares each word to the dictionary file using the compare_each_word function. If any of the words are not found in the dictionary, the function sets the result variable to 0 and exits the loop. Finally, the function frees the memory allocated for the copy of the word and returns the result variable.

- strip_special_chars: This function removes any non-alphabetic characters from the beginning and end of a given word. It takes in a character pointer word as an argument. The function initializes two pointers, start and end, to point to the beginning and end of the word, respectively. It then iterates through the word, skipping any non-alphabetic characters at the beginning, and sets start to point to the first alphabetic character. If the word is empty, the function sets the first character of the word to the null character and returns. The function then iterates through the word again, starting from the end, and sets end to point to the last alphabetic character. Finally, the function copies the alphabetic characters from start to end to the beginning of the word and sets the null character at the end of the copied substring.

- print_line: (Code In Progress)

- search_directory: This function searches a directory and its subdirectories for text files and checks the spelling of each word in the files against a given dictionary. It takes in a character pointer dir_path and a character pointer dict as arguments. The function opens the directory using the opendir function and iterates through the entries in the directory using the readdir function. If the entry is a directory, the function recursively calls itself with the path of the subdirectory. If the entry is a text file, the function opens the file and reads each line using the read_lines function, checking the spelling of each word in the line against the dictionary using the compare_each_word function. The function prints the number of incorrectly spelled words found in the file. If the directory cannot be opened, the function prints an error message.

- main: This function is the main entry point of the program and takes in command line arguments to specify the directory to search and the dictionary file to use for spelling checks. If no arguments are provided, it defaults to searching the current directory and using a dictionary file named "words". The function then calls the search_directory function to perform the spelling check.

- Output form
file directory (number of the line, number of the word): worng word



