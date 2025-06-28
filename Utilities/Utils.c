#include "Utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool validate_input_file(const char *filename)
{

    // check that the file is a valid file

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return false;
    }
    fclose(file);

    // check if valid extension

    const char *dot = strrchr(filename, '.'); // returns substring starting at position of the found character
    if (!dot || strcmp(dot, ".jack") != 0)
    {
        fprintf(stderr, "Invalid file extension (expected .jack): %s\n", filename);
        return false;
    }
    return true;
}

const char *remove_extension(const char *filename)
{
    char *result;

    char *dot = strrchr(filename, '.');

    if (!dot)
    {
        return filename;
    }
    size_t len = dot - filename;

    result = (char *)malloc(len + 1);

    if (result)
    {
        strncpy(result, filename, len);
        result[len] = '\0';

        return result;
    }
    return NULL;
}

const char *get_filename_without_extension(const char *filename)
{
    char *result;

    char *dot = strrchr(filename, '.');

    char *slash = strrchr(filename, '/'); // unix
    if (!slash)
    {
        slash = strrchr(filename, '\\'); // windows
    }
    const char *base = (slash) ? slash + 1 : filename; // sets base to just after the slash eg /out => (slash == /) and (slash + 1 == o); pointer arithmetic

    if (!dot || (dot < base))
    {
        return base;
    }

    size_t len = dot - base;

    result = (char *)malloc(len + 1);

    if (result)
    {
        strncpy(result, base, len);
        result[len] = '\0';

        return result;
    }
    return NULL;
}
