Names: Kelvin Ihezue (netID: ki120)

My program conducts spell checks on text files by comparing the words within the files to a supplied dictionary. The client provides the dictionary as the first argument and a file path as the second argument. If the provided path points to a directory, the program traverses the directory recursively and applies our spell-checking function to each text file it encounters, excluding files that start with a "." and are of the .txt type.

When a misspelled word is found, my program outputs the word along with its originating file, line number, and column number. Initially, the program reads the dictionary and organizes it alphabetically based on ASCII values before executing the search operation on the words extracted from the text files.

I precisely utilized POSIX functions for file handling operations, ensuring compatibility across platforms. Furthermore, our design allows the program to handle both individual text files and directories containing multiple text files. To fulfill the recursive directory traversal requirement, we implemented recursion in my `scandirectory_recursively` function. Additionally, I incorporated error handling mechanisms to address scenarios where files cannot be opened or memory allocation errors occur.

Testcases and scenerios (succeeded)
- Functions to validate capitalized words in the three variations; regualar, initial capital and all capitals worked.
- Functions to check if the file corresponded the directory, if it's a regualar file and if its a text file worked.
-Function (is_word_character) to detect if the string is a word or a spcial character worked.
- Function to check if the word read from the txt file is in the dictionary worked.
- Spellchecker function worked as intended.
- (Scan_directory_recursively) Function that goes through the direectory and applies spellcheker function to every file that ends in .txt until no further dirctories are found.


Limitations
- Function to test for hypenated words failed.


