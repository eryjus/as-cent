//===============================================================================================
// symtab.h -- This file contains the classes and definitions for the as-cent symbol table
// implementation.
//
// The first thing to note is that all symbols are global to the file.  This means that there is
// no need to manage any scope.  When we begin reading the top-level file we create a symbol
// table and that table lives with all its symbols until we finish with the file.
//
// I expect these class definitions to change as I see new requirements.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
//
//===============================================================================================

#if 0
//#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include "in-file.h"

#include <map>
#include <string>
#include <algorithm>

//-----------------------------------------------------------------------------------------------
// The Symbol class will contain all the attributes of a symbol used by the assembler.  I expect
// that this class definition will evolve over time as I find needs to track all the attributes
// required.
//
// Note: there are 2 ways to create an instance of Symbol:
// Symbol::Factory(...) -- this will create a new instance of Symbol using the new operator and
// return a pointer to this new Symbol instance.
// new Symbol(...) -- this is a more common way to create a new symbol instance.
//
// The reason to use `Symbol::Factory` over `new Symbol` is this: function chaining.
// Symbol::Factory allows for a cleaner representation of the following:
//
//     symTab->Insert(Symbol::Factory("reg")->SetType(SYM_REG));
//
// As I write this first iteration, there is not much need for this kind of construct as the
// Type can be set simply in the constructor -- but I'm planning ahead.
//-----------------------------------------------------------------------------------------------
class Symbol {
public:
    typedef enum {SYM_UNK, SYM_DIRECTIVE, SYM_OPCODE, SYM_REG, SYM_QUALIFIER,
            SYM_LABEL} SymType;
    static const std::string SymTypeStrings[];

protected:
    std::string symName;           // This is a duplicate of the key value
    SymType type;
    std::string fileName;
    long lineNum;

public:
    Symbol(const std::string &n, SymType t = SYM_UNK) : symName(n), type(t),
            fileName(InputFile::FileName()), lineNum(InputFile::LineNum())
            { std::transform(symName.begin(), symName.end(), symName.begin(), ::tolower); };
    static Symbol *Factory(const std::string &n, SymType t = SYM_UNK)
            { return new Symbol(n, t); };
    virtual ~Symbol() {};

public:
    Symbol *SetType(SymType t) { type = t; return this; };
    SymType GetType(void) const { return type; };
    const std::string &GetTypeStr(void) const { return SymTypeStrings[type]; };
    std::string Key(void) const { return symName; };
};

//-----------------------------------------------------------------------------------------------
// These 2 typedefs are here to make coding easier (and the resulting code easier to read!)
//-----------------------------------------------------------------------------------------------
typedef std::map<std::string, Symbol *> Map;  // The map pair is referred to as a Map
typedef Map::iterator Iter;                   // Needed for lookups

//-----------------------------------------------------------------------------------------------
// The SymbolTable class will be the whole symbol table -- in all its glory!  Disappointed?  I
// think it's kinda anti-climactic.
//-----------------------------------------------------------------------------------------------
class SymbolTable : protected Map {
public:
    Symbol *Lookup(std::string key)
            { std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                Iter tmp = find(key); return (tmp == end()?NULL:tmp->second); };
    Symbol *Insert(Symbol *sym) { (*this)[sym->Key()] = sym; return sym; };

    void Print(void);
};

//-----------------------------------------------------------------------------------------------
// Some global declarations so that other source files can access the symbol tables instances --
// collectively refferred to as the "Symbol Table".
//-----------------------------------------------------------------------------------------------
extern SymbolTable &symTab;
extern SymbolTable &intTab;
extern SymbolTable &strTab;

#endif
