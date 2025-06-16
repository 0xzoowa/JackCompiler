
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

char *get_filename_without_extension(const char *filename);
char *remove_extension(const char *filename);
bool validate_input_file(const char *filename);

#endif
