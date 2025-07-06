
#ifndef UTILS_H
#define UTILS_H
#include "../JackTokenizer/JackTokenizer.h"
#include <stdbool.h>

const char *get_filename_without_extension(const char *filename);
const char *remove_extension(const char *filename);
bool validate_input_file(const char *filename);
void write_xml(bool flag, tokenType type, FILE *out, const char *value);
char *keyword_type_to_string(keywordType kw);

#endif
