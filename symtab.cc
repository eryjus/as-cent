
#include "symtab.h"

#include <iostream>

#if 0

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

const std::string Symbol::SymTypeStrings[] = {"SYM_UNK",
        "SYM_DIRECTIVE",
        "SYM_OPCODE",
        "SYM_REG",
        "SYM_QUALIFIER",
        "SYM_LABEL"};


void SymbolTable::Print(void)
{
    for (Iter i = begin(); i != end(); i ++) {
        Symbol *sym = i->second;
        std::cout << sym->Key() << " of type " << sym->GetTypeStr() << std::endl;
    }
}

#endif
