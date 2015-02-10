//===============================================================================================
// as-cent.cc -- This file contains the main function and additional smaller functions.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
//
//===============================================================================================

#include "in-file.hpp"
#include "parser.hpp"
#include "symtab.h"

#include <iostream>
#include <iomanip>

void InitSymbols(void);

//-----------------------------------------------------------------------------------------------
// main() is the main entry point.  It will evolve over time as more compnents of the assembler
// are developed.
//-----------------------------------------------------------------------------------------------
int main(void)
{
    std::string file = "test.asm";
    std::string line;
    int num = 0;

    InitSymbols();
#if 0
    InputFile::Factory(file);
    Parser parser;

    try {
        while (1) {
            line = InputFile::NextLine();
            std::cout << std::setw(5) << (++ num) << ": " << line << std::endl;

//            parser.ParseLine(line);
            std::cout << "\t Clean Line: <" << parser.Line() << ">\n";
            std::cout << std::endl;
        }
    } catch(...) {
    }
#endif
    return 0;
}

//-----------------------------------------------------------------------------------------------
// InitSymbols() is used to initialize the symbol table to the initial state -- with only the
// supported registers, opcodes, and directives.
//-----------------------------------------------------------------------------------------------
void InitSymbols(void)
{
#if 0
    // -- Start by setting up all the possible directives
    //    -----------------------------------------------
    symTab.Insert(Symbol::Factory("CPU", Symbol::SYM_DIRECTIVE));
    symTab.Insert(Symbol::Factory("8086", Symbol::SYM_DIRECTIVE));
    symTab.Insert(Symbol::Factory("MODEL", Symbol::SYM_DIRECTIVE));
    symTab.Insert(Symbol::Factory("ORG", Symbol::SYM_DIRECTIVE));
    symTab.Insert(Symbol::Factory("EVEV", Symbol::SYM_DIRECTIVE));
    symTab.Insert(Symbol::Factory("EQU", Symbol::SYM_DIRECTIVE));

    // -- Now, let's establish the register list
    //    --------------------------------------
    symTab.Insert(Symbol::Factory("AL", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("AH", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("AX", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("BL", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("BH", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("BX", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("CL", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("CH", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("CX", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("DL", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("DH", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("DX", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("SI", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("DI", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("SP", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("BP", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("CS", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("DS", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("ES", Symbol::SYM_REG));
    symTab.Insert(Symbol::Factory("SS", Symbol::SYM_REG));

    // -- Next, we setup the qualiiers
    //    ----------------------------
    symTab.Insert(Symbol::Factory("BYTE", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("BYTE PTR", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("WORD", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("WORD PTR", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("DWORD", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("DWORD PTR", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("CS:", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("DS:", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("ES:", Symbol::SYM_QUALIFIER));
    symTab.Insert(Symbol::Factory("SS:", Symbol::SYM_QUALIFIER));

    // -- Finally, the opcodes
    //    --------------------


    symTab.Print();
#endif
}

//===============================================================================================
