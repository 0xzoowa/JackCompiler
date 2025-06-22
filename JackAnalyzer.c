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

void process(FILE *in, FILE *outT);
void write_xml(tokenType type);

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

        process(input, outT_file);
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
            process(path, outT_file);
        }
        closedir(dir);
    }
    return 0;
}

void process(FILE *in, FILE *outT)
{
    tokenizer_create(in, outT);
    while (has_more_tokens())
    {
        advance(); // sets the current character => current_char
        tokenType type_of_token = token_type();
        write_xml(type_of_token);
    }
    tokenizer_destroy();
}

// writexml based on the token type
void write_xml(tokenType type)
{
    if (!outT_file)
    {
        fprintf(stderr, "write_xml: outT_file not initialized.\n");
        return;
    }

    if (type == KEYWORD)
    {
        fprintf(outT_file, "<keyword> %s </keyword>\n", keyword());
    }
    else if (type == SYMBOL)
    {
        const char *sym = symbol();
        if (strcmp(sym, "<") == 0)
            fprintf(outT_file, "<symbol> &lt; </symbol>\n");
        else if (strcmp(sym, ">") == 0)
            fprintf(outT_file, "<symbol> &gt; </symbol>\n");
        else if (strcmp(sym, "&") == 0)
            fprintf(outT_file, "<symbol> &amp; </symbol>\n");
        else
            fprintf(outT_file, "<symbol> %s </symbol>\n", sym);
    }
    else if (type == IDENTIFIER)
    {
        fprintf(outT_file, "<identifier> %s </identifier>\n", identifier());
    }
    else if (type == INT_CONST)
    {
        fprintf(outT_file, "<integerConstant> %d </integerConstant>\n", int_val());
    }
    else if (type == STRING_CONST)
    {
        fprintf(outT_file, "<stringConstant> %s </stringConstant>\n", string_val());
    }
    else
    {
        fprintf(stderr, "write_xml: unknown token type %d\n", type);
    }
}
