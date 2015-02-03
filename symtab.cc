
#include "symtab.h"

//-----------------------------------------------------------------------------------------------
// Create an instance of the symbol tables -- there will be 3:
// 1) The symbol table for identifiers (right now includes registers and directives)
// 2) The symbol table for numbers
// 3) The symbol table for strings
//
// With that said, I might even have a symbol table for directives and another for registers.
// Time will tell if I need this.
//-----------------------------------------------------------------------------------------------

static SymbolTable _symTab;     // This is a staticly allocated symbol table
SymbolTable &symTab = _symTab;  // This is a reference to above, but globally available

static SymbolTable _intTab;     // This is a staticly allocated symbol table
SymbolTable &intTab = _intTab;  // This is a reference to above, but globally available

static SymbolTable _strTab;     // This is a staticly allocated symbol table
SymbolTable &strTab = _strTab;  // This is a reference to above, but globally available
