#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

FILE *out_file = NULL;
static int label_counter = 0;

bool is_valid_arithmetic(const char *cmd);

void vm_writer_init(char *out)
{

    out_file = fopen(out, "w");
    if (!out_file)
    {
        fprintf(stderr, "Error: Could not create output file %s\n", out);
        exit(EXIT_FAILURE);
    }
}
void write_push(const char *segment, int index)
{
    if (!segment || index < 0)
    {
        fprintf(stderr, "Invalid push arguments\n");
        return;
    }
    fprintf(out_file, "push %s %i\n", segment, index);
}
void write_pop(const char *segment, int index)
{
    if (!segment || index < 0)
    {
        fprintf(stderr, "Invalid push arguments\n");
        return;
    }
    fprintf(out_file, "pop %s %i\n", segment, index);
}
void write_arithmetic(const char *command)
{
    if (!is_valid_arithmetic(command))
    {
        fprintf(stderr, "Error: Invalid arithmetic command '%s'\n", command);
        return;
    }
    fprintf(out_file, "%s\n", command);
}
void write_label(char *label)
{
    fprintf(out_file, "label %s\n", label);
}
void write_goto(char *label)
{
    fprintf(out_file, "goto %s\n", label);
}
void write_if(char *label)
{
    fprintf(out_file, "if-goto %s\n", label);
}
void write_call(char *name, int n_args)
{
    fprintf(out_file, "call %s %d\n", name, n_args);
}
void write_function(char *name, int n_vars)
{
    fprintf(out_file, "call %s %d\n", name, n_vars);
}
void write_return()
{
    fprintf(out_file, "return\n");
}
void close()
{
    if (out_file != NULL)
    {
        fclose(out_file);
        out_file = NULL;
    }
}
bool is_valid_arithmetic(const char *cmd)
{
    const char *valid_cmds[] = {
        "add", "sub", "neg",
        "eq", "gt", "lt",
        "and", "or", "not"};
    size_t n = sizeof(valid_cmds) / sizeof(valid_cmds[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (strcmp(cmd, valid_cmds[i]) == 0)
            return true;
    }
    return false;
}
