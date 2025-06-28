
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

const char *get_filename_without_extension(const char *filename);
const char *remove_extension(const char *filename);
bool validate_input_file(const char *filename);

#endif
