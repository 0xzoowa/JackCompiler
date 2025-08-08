#ifndef VMWRITER_H
#define VMWRITER_H

void vm_writer_init(char *out);
void write_push(const char *segment, int index);
void write_pop(const char *segment, int index);
void write_arithmetic(const char *command);
void write_label(char *label);
void write_goto(char *label);
void write_if(char *label);
void write_call(char *name, int n_args);
void write_function(char *name, int n_vars);
void write_return();
void close();

#endif