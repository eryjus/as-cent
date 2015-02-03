//===============================================================================================
// parser.h -- This file contains the class definition for parsing a line of assembly code.
//
// Theoretically, parsing a line is simple -- it is generally in the form:
//
// [label `:`] [OpCode [Operand1 [`,` Operand2 [`,` Operand3]]]] [`;` Comment]
//
// Though the colon after the label is not necessary for most assemblers, I will require it.
// Note also that the termination of a comment is the end of the line.  Comments do not span
// multiple lines.  However, multiple lines can have comments.
//
// Labels MUST begin in the first position of the line.  Comments and directives can also begin
// in the first position (I am not yet working on directives).  However, if the first position
// is whitespace, there cannot be a label on that line.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
//
//===============================================================================================

#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>

class Parser {
private:
    std::string label;
    std::string opCode;
    std::string operand;
    std::string comment;
    std::string cleanLine;           // this is the line sans comment

public:
    Parser() : label(""), opCode(""), operand(""), comment(""), cleanLine("") {};
    virtual ~Parser() {};

public:
    void ParseLine(std::string line);

public:
    std::string Label(void) const { return label; };
    std::string OpCode(void) const { return opCode; };
    std::string Operand(void) const { return operand; };
    std::string Comment(void) const { return comment; };
    std::string Line(void) const { return cleanLine; };
};

//-----------------------------------------------------------------------------------------------
// Finally, some worker function primarily used in the parser, but could be useful somewhere
// else.
//-----------------------------------------------------------------------------------------------

inline std::string rtrim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return s.substr(0, s.find_last_not_of(delimiters) + 1);
}

//-----------------------------------------------------------------------------------------------

inline std::string ltrim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return s.substr(s.find_first_not_of(delimiters));
}

//-----------------------------------------------------------------------------------------------

inline std::string trim(const std::string& s, const std::string& delimiters = " \f\n\r\t\v")
{
    return ltrim(rtrim(s, delimiters), delimiters);
}

//===============================================================================================

#endif
