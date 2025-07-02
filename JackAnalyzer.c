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
char out[MAX_CHAR];
char path[PATH_MAX];

void process(const char *in, char *outT);
void write_xml(tokenType type, FILE *out);
char *keyword_type_to_string(keywordType kw);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Not enough arguments, please provide a filename or path.");
        return 0;
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
        const char *filename = remove_extension(input);
        snprintf(out, sizeof(out_file), "%sTT.xml", filename);
        snprintf(out_file, sizeof(out_file), "%s.xml", filename);

        process(input, out);
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
            else
            {
                snprintf(out, sizeof(out_file), "%s/%sTT.xml", input, remove_extension(entry->d_name));
                snprintf(out_file, sizeof(out_file), "%s/%s.xml", input, remove_extension(entry->d_name));
                process(path, out);
            }
        }
        closedir(dir);
    }
    return 1;
}

void process(const char *in, char *outT)
{
    FILE *out = fopen(outT, "w");
    if (out == NULL)
    {
        fprintf(stderr, "Error: Could not create output file %s\n", outT);
        exit(EXIT_FAILURE);
    }
    tokenizer_create(in, out);
    advance(); // sets the current character => current_char
    while (has_more_tokens())
    {
        tokenType type_of_token = token_type();
        if (type_of_token != INVALID_TYPE)
        {
            write_xml(type_of_token, out); // valid token
        }
        else
        {
            advance();
        }
    }
    tokenizer_destroy();
}
