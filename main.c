#include <stdio.h>
#include "SymbolTable/SymbolTable.h"

int main(int argc, char *argv[])
{

    symbol_table_init();

    define("gameInstance", "Game", "static"); // static Game gameInstance;
    define("score", "int", "static");         // static int score;

    define("x", "int", "field");      // field int x;
    define("y", "int", "field");      // field int y;
    define("width", "int", "field");  // field int width;
    define("height", "int", "field"); // field int height;

    define("this", "ClassName", "arg"); // Implicit 'this' passed in as arg 0 for methods
    define("dx", "int", "arg");
    define("dy", "int", "arg");

    reset();

    define("tempX", "int", "var");
    define("tempY", "int", "var");

    // reset();

    print_symbol();
}