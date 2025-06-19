/**
 * creates a JackTokenizer from the Xxx.jack input file;
 * creates an output file named Xxx.xml; and
 * uses the JackTokenizer and the CompilationEngine to parse the input file and write the parsed code to the output file.
 *      => the process method will handle this
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include "Utilities/Utils.h"
#include "CompilationEngine/CompilationEngine.h"
#include "JackTokenizer/JackTokenizer.h"

#define MAX_CHAR 256

char out_file[MAX_CHAR];
char outT_file[MAX_CHAR];
char path[PATH_MAX];

void process();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Not enough arguments, please provide a filename or path.");
        return 1;
    }
    const char *input = argv[1];

    DIR *dir = opendir(input);
    if (dir == NULL) // if null, should be a file
    {
        bool is_valid_file = validate_input_file(input);
        if (!is_valid_file)
        {
            exit(EXIT_FAILURE);
        }

        // construct output file with the specified extension
        char *filename = remove_extension(input);
        snprintf(outT_file, sizeof(out_file), "%sT.xml", filename);
        snprintf(out_file, sizeof(out_file), "%s.xml", filename);

        // process(input)
    }
    else
    {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            // skip .(current dir) and ..(parent dir)
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(path, sizeof(path), "%s/%s", input, entry->d_name);
            bool is_valid_file = validate_input_file(path);
            if (!is_valid_file)
            {
                continue;
            }
            snprintf(outT_file, sizeof(out_file), "%s/%sT.xml", input, remove_extension(entry->d_name));
            snprintf(out_file, sizeof(out_file), "%s/%s.xml", input, remove_extension(entry->d_name));
            // process(path);
        }
        closedir(dir);
    }
    return 0;
}

void process() {}