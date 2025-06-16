# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g


# Target binary
TARGET = JackAnalyzer

# Object files
OBJS = JackAnalyzer.o Utilities/Utils.o CompilationEngine/CompilationEngine.o JackTokenizer/JackTokenizer.o

# Final target
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

# Object file rules with dependencies
JackAnalyzer.o: JackAnalyzer.c Utilities/Utils.h CompilationEngine/CompilationEngine.h JackTokenizer/JackTokenizer.h
	$(CC) $(CFLAGS) -c JackAnalyzer.c

CompilationEngine/CompilationEngine.o: CompilationEngine/CompilationEngine.c CompilationEngine/CompilationEngine.h
	$(CC) $(CFLAGS) -c CompilationEngine/CompilationEngine.c -o CompilationEngine/CompilationEngine.o 

JackTokenizer/JackTokenizer.o: JackTokenizer/JackTokenizer.c JackTokenizer/JackTokenizer.h
	$(CC) $(CFLAGS) -c JackTokenizer/JackTokenizer.c -o JackTokenizer/JackTokenizer.o

Utilities/Utils.o: Utilities/Utils.c Utilities/Utils.h
	$(CC) $(CFLAGS) -c Utilities/Utils.c -o Utilities/Utils.o

# Clean up
clean:
	rm -f $(TARGET) $(OBJS)
