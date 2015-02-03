/*
*****************************************************************************************************************************************
* OpCodeTable.c -- This file contains the OpCode validation table and the functions to read this table.  This table is read-only, so
*                  there will be no functions to update it at all.
*
* So, we have gone through several iterations on the format of this table.  What I have struggled with (causing me to dump the work
* several times) is how the different opcodes needs to be filtered down (as I am not a strong asm programmer).  What I have finally
* been able to work out is that there are 2 key dimensions to an opcode's availability: 1) the bit-mode the processor is in (16-bit,
* 32-bit, or 64-bit) and will be referred to the addressing modes, and the architecture it is running (8086, 186, 286, 386, 486, PENT,
* or P6).  In addition, there will be additional technologies that will be available which have instructions (and these can be turned
* on/off with assembler directives).  These are: 3DNOW, CYRIX, FPU, KATMAI, MMX, SSE, SSE2, PRIV, UNDOC, WILLAMETTE, and X64.  Finally,
* there are certain instructions that are not available in long mode.
*
* So, I think the easiest thing to do at this point is to build an 8086 assembler (16-bit addressing) and generate the code from that.
* I will produce an elf object file and allow a another linker to link my programs for now.  With that, I will also be building the
* constructs to allow the assembler to be expanded to other instruction sets for additional processors.  The first 8086 processor
* will be a daunting enough task to complete.
*
* The mnemonic names will be stored in an array of strings.  Each string will be a 16-byte string, inclding the terminating null.
* Overlaid over this string are 2 8-byte long integers, which will allow for some quick (64-bit) comparisons against the 2 strings.
* I like this algorithm since it will allow for 2 operations to compare rather than 16.
*
* Now, there will be several OpCodeTable entries for most of the mnemonics.  Having a single array of mnemonic entries appears
* prefereable, eliminating duplication.
*
* There are 3 enum types (CPU, Bits, and Flags) which will be bitwise or'd together for any given OpCode in the OpCodeTable.  These
* comosite flags will indicate under which circumstances any particular OpCode may be used.  The exception is the _NOLONG flag, which
* operates as a disqualification flag where an OpCode is not available when the CPU is operating in Long Mode.
*
* For the first iteration of this assembler, I am going to ignore floating point operations.  The roadmap I will use to implement
* the assembler is:
* 0.0 -- All 8086 operations EXCEPT FPU operations
* 0.1 -- All 8086 operations INCLUDING FPU operations
* 0.2 -- Add in all 186 operations (still 16-bit addressing)
* 0.3 -- Add in all 286 operations (protected mode 16-bit)
* 0.4 -- Add in all 386 operations (protected mode 32-bit addressing)
* 0.5 -- Add in 486 operations
* 0.6 -- Add in 586 operations
* 0.7 -- Add in 686 operations
* 0.8 -- Complete all remaining operations except 64-bit support
* 0.9 -- Add in long mode support
*
*     Date     Tracker  Pgmr  Description
*  ----------  -------  ----  ----------------------------------------------------------------------------------------------------------
*  01/09/2015  Initial  ADCL  This is the initial version.  I have gone through several iterations to date.
*
*****************************************************************************************************************************************
*/

#include <stdint.h>
#include <string.h>

/*
 * -- This enumerated type is used to indicate which CPUs an OpCode can be executed on.
 *    ---------------------------------------------------------------------------------
 */
typedef enum {
    _8086       = 0x00000001,
    _186        = 0x00000003,
    _286        = 0x00000007,
    _386        = 0x0000000f,
    _486        = 0x0000001f,
    _586        = 0x0000003f,
    _PENTIUM    = _586,
    _PENT       = _586,
    _686        = 0x0000007f,
    _PPRO       = _686,
    _P2         = _686,
    _P3         = 0x000000ff,
    _KATMAI     = _P3,
    _P4         = 0x000001ff,
    _WILLAMETTE = _P4,
    _PRESCOTT   = 0x000003ff,
    _X64        = 0x000007ff,
    _IA64       = 0x00000fff,
} CPU;

typedef enum {
    _16BITS     = 0x00001000,
    _32BITS     = 0x00002000,
    _64BITS     = 0x00004000,
} Bits;

typedef enum {
    _PRIV       = 0x00010000,
    _NOLONG     = 0x00080000,
    _3DNOW      = 0x00100000,
    _CYRIX      = 0x00200000,
    _FPU        = 0x00400000,
    _MMX        = 0x00800000,
    _SSE        = 0x01000000,
    _SSE2       = 0x02000000,
    _UNDOC      = 0x80000000,
} Flags;



typedef union Mnemonic {
    char name[16];
    uint64_t code[2];
} Mnemonic;

enum {
    AAA,
    AAD,
    AAM,
    AAS,
    ADC,
    ADD,
    AND,
    CALL,
    CBW,
    CLC,
    CLD,
    CLI,
    CMC,
    CMP,
    CMPSB,
    CMPSW,
    CWD,
    DAA,
    DAS,
    DEC,
    DIV,
    FWAIT,
    HLT,
    IDIV,
    IMUL,
    IN,
    INC,
    INT,
    INT3,
    INT03,
    INTO,
    IRET,
    JA,
    JAE,
    JB,
    JBE,
    JC,
    JCXZ,
    JE,
    JG,
    JGE,
    JL,
    JLE,
    JNA,
    JNAE,
    JNB,
    JNBE,
    JNC,
    JNE,
    JNG,
    JNGE,
    JNL,
    JNLE,
    JO,
    JP,
    JPE,
    JPO,
    JS,
    JZ,
    JMP,
    LAHF,
    LDS,
    LES,
    LEA,
    LODSB,
    LODSW,
    LOOP,
    LOOPE,
    LOOPNE,
    LOOPNZ,
    LOOPZ,
    MOV,
    MOVSB,
    MOVSW,
    MUL,
    NEG,
    NOP,
    NOT,
    OR,
    OUT,
    PAUSE,
    POP,
    POPF,
    PUSH,
    PUSHF,
    RCL,
    RCR,
    RET,
    RETF,
    ROL,
    ROR,
    SAHF,
    SAL,
    SAR,
    SBB,
    SCASB,
    SCASW,
    SHL,
    SHR,
    STC,
    STD,
    STI,
    STOSB,
    STOSW,
    SUB,
    TEST,
    WAIT,
    XCHG,
    XLAT,
    XOR,
};

const Mnemonic mnemonics[] = {
    {{'A', 'A', 'A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'A', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'A', 'M','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'A', 'S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'D', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'D', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'A', 'N', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'A', 'L', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'B', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'L', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'L', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'L', 'I','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'M', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'M', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'M', 'P', 'S', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'M', 'P', 'S', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'C', 'W', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'D', 'A', 'A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'D', 'A', 'S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'D', 'E', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'D', 'I', 'V','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'F', 'W', 'A', 'I', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'H', 'L', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'D', 'I', 'V','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'M', 'U', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N', 'T', '3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N', 'T', '0', '3','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'N', 'T', 'O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'I', 'R', 'E', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'A', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'B', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'C', 'X', 'Z','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'G', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'L', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'A', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'B', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'G', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'N', 'L', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'P', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'P', 'O','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'Z','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'J', 'M', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'A', 'H', 'F','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'D', 'S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'E', 'S','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'E', 'A','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'D', 'S', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'D', 'S', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'O', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'O', 'P', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'O', 'P', 'N', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'O', 'P', 'N', 'Z','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'L', 'O', 'O', 'P', 'Z','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'M', 'O', 'V','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'M', 'O', 'V', 'S', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'M', 'O', 'V', 'S', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'M', 'U', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'N', 'E', 'G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'N', 'O', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'N', 'O', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'O', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'O', 'U', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'P', 'A', 'U', 'S', 'E','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'P', 'O', 'P','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'P', 'O', 'P', 'F','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'P', 'U', 'S', 'H','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'P', 'U', 'S', 'H', 'F','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'C', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'C', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'E', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'E', 'T', 'F','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'O', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'R', 'O', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'A', 'H', 'F','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'A', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'A', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'B', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'C', 'A', 'S', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'C', 'A', 'S', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'H', 'L','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'H', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'T', 'C','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'T', 'D','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'T', 'I','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'T', 'O', 'S', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'T', 'O', 'S', 'W','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'S', 'U', 'B','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'T', 'E', 'S', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'W', 'A', 'I', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'X', 'C', 'H', 'G','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'X', 'L', 'A', 'T','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
    {{'X', 'O', 'R','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'}},
};

typedef enum {
    PRE_NONE = 0,

    LOCK = 0x8000,
    REPNE = 0x4000,
    REPE = 0x2000,

    CS = 0x0800,
    SS = 0x0400,
    DS = 0x0200,
    ES = 0x0100,
    FS = 0x0080,
    GS = 0x0040,
    HINT_N = 0x0020,
    HINT_Y = 0x0010,

    O32 = 0x0008,
    O16 = 0x0004,

    A32 = 0x0002,
    A16 = 0x0001,
} Prefixes;

typedef enum {
    REX_NONE = 0,

    REX_B = 0x41,
    REX_X = 0x42,
    REX_R = 0x44,
    REX_W = 0x48,

    REX = 0x40,
} RexPrefix;

typedef enum {
    OP_NONE = 0,
    REL8,
    REL16,
    REL32,
    PTR16_16,
    PTR16_32,
    R8,
    R16,
    R32,
    R64,
    IMM8,
    IMM16,
    IMM32,
    IMM64,
    RM8,
    RM16,
    RM32,
    RM64,
    M,
    M8,
    M16,
    M32,
    M64,
    M128,
    M16_16,
    M16_32,
    M16_64,
    M16M32,
    M16M16,
    M32M32,
    M16M64,
    MOFFS8,
    MOFFS16,
    MOFFS32,
    MOFFS64,
    SREG,
    M32FP,
    M64FP,
    M80FP,
    M16INT,
    M32INT,
    M64INT,
    ST0,
    STi,
    MM,
    MM_M32,
    MM_M64,
    XMM,
    XMM_M32,
    XMM_M64,
    XMM_M128,
    XMM0,
    YMM,
    M256,
    YMM_M256,
    YMM0,
    SRC1,
    SRC2,
    SRC3,
    SRC,
    DST,
    REG_AL,
    REG_AX,
    REG_CL,
    REG_CX,
    REG_DX,
    REG_BX,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,
    REG_EAX,
    REG_ECX,
    REG_EDX,
    REG_EBX,
    REG_ESP,
    REG_EBP,
    REG_ESI,
    REG_EDI,
    REG_RAX,
    REG_CS,
    REG_DS,
    REG_ES,
    REG_FS,
    REG_GS,
    REG_SS,
    ONE,
} OperandType;

typedef struct OpCodeEntry {
    const Mnemonic *mnemonic;
    Prefixes legacyPrefixes;
    RexPrefix rexPrefix;
    uint8_t byteCode[3];

    uint8_t modRM_Op:3;
    uint8_t modRM_Part:1;
    uint8_t opcodeBytes:3;

    OperandType type[3];
    uint8_t size;
    uint32_t flags;
} OpCodeEntry;

#define _(x)  (&mnemonics[x])
#define Y 0b1u
#define N 0b0u

const OpCodeEntry OpCodeTable[] = {
    {   _(AAA),        PRE_NONE, REX_NONE, {0x37, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(AAD),        PRE_NONE, REX_NONE, {0xd5, 0x0a, 0x00}, 00u, N, 02u, { OP_NONE, OP_NONE, OP_NONE}, 2u, _8086|_16BITS|_NOLONG},
    {   _(AAD),        PRE_NONE, REX_NONE, {0xd5, 0x00, 0x00}, 00u, N, 01u, {    IMM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS|_NOLONG},

    {   _(AAM),        PRE_NONE, REX_NONE, {0xd4, 0x0a, 0x00}, 00u, N, 02u, { OP_NONE, OP_NONE, OP_NONE}, 2u, _8086|_16BITS|_NOLONG},
    {   _(AAM),        PRE_NONE, REX_NONE, {0xd4, 0x00, 0x00}, 00u, N, 01u, {    IMM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS|_NOLONG},

    {   _(AAS),        PRE_NONE, REX_NONE, {0x3f, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(ADC),        PRE_NONE, REX_NONE, {0x14, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADC),        PRE_NONE, REX_NONE, {0x15, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 02u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 02u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 02u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x10, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x11, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x12, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADC),            LOCK, REX_NONE, {0x13, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {   _(ADD),        PRE_NONE, REX_NONE, {0x04, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADD),        PRE_NONE, REX_NONE, {0x05, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 00u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 00u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 00u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x00, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x01, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x02, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ADD),            LOCK, REX_NONE, {0x03, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {   _(AND),        PRE_NONE, REX_NONE, {0x24, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(AND),        PRE_NONE, REX_NONE, {0x25, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 04u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 04u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 04u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x20, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x21, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x22, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(AND),            LOCK, REX_NONE, {0x23, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {  _(CALL),        PRE_NONE, REX_NONE, {0xe8, 0x00, 0x00}, 00u, N, 01u, {   REL16, OP_NONE, OP_NONE}, 3u, _8086|_16BITS},
    {  _(CALL),        PRE_NONE, REX_NONE, {0xff, 0x00, 0x00}, 02u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(CALL),        PRE_NONE, REX_NONE, {0x9a, 0x00, 0x00}, 00u, N, 01u, {PTR16_16, OP_NONE, OP_NONE}, 5u, _8086|_16BITS},
    {  _(CALL),        PRE_NONE, REX_NONE, {0xff, 0x00, 0x00}, 03u, Y, 01u, {  M16_16, OP_NONE, OP_NONE}, 6u, _8086|_16BITS},

    {   _(CBW),        PRE_NONE, REX_NONE, {0x98, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CLC),        PRE_NONE, REX_NONE, {0xf8, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CLD),        PRE_NONE, REX_NONE, {0xfc, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CLI),        PRE_NONE, REX_NONE, {0xfa, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CMC),        PRE_NONE, REX_NONE, {0xf5, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CMP),        PRE_NONE, REX_NONE, {0x3c, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(CMP),        PRE_NONE, REX_NONE, {0x3d, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 07u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 07u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 07u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x38, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x39, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x3a, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(CMP),            LOCK, REX_NONE, {0x3b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    { _(CMPSB), REPE|REPNE|LOCK, REX_NONE, {0xa6, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(CMPSW), REPE|REPNE|LOCK, REX_NONE, {0xa7, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(CWD),        PRE_NONE, REX_NONE, {0x99, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(DAA),        PRE_NONE, REX_NONE, {0x27, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(DAS),        PRE_NONE, REX_NONE, {0x2f, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(DEC),            LOCK, REX_NONE, {0xfe, 0x00, 0x00}, 01u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(DEC),            LOCK, REX_NONE, {0xff, 0x00, 0x00}, 01u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x48, 0x00, 0x00}, 00u, N, 01u, {  REG_AX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x49, 0x00, 0x00}, 00u, N, 01u, {  REG_CX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4a, 0x00, 0x00}, 00u, N, 01u, {  REG_DX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4b, 0x00, 0x00}, 00u, N, 01u, {  REG_BX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4c, 0x00, 0x00}, 00u, N, 01u, {  REG_SP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4d, 0x00, 0x00}, 00u, N, 01u, {  REG_BP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4e, 0x00, 0x00}, 00u, N, 01u, {  REG_SI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(DEC),        PRE_NONE, REX_NONE, {0x4f, 0x00, 0x00}, 00u, N, 01u, {  REG_DI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(DIV),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 06u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(DIV),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 06u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    { _(FWAIT),        PRE_NONE, REX_NONE, {0x9b, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(HLT),        PRE_NONE, REX_NONE, {0xf4, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_PRIV},

    {  _(IDIV),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 07u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(IDIV),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 07u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {  _(IMUL),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 05u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(IMUL),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 05u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(IMUL),            LOCK, REX_NONE, {0x0f, 0xaf, 0x00}, 00u, N, 02u, {     R16,    RM16, OP_NONE}, 3u, _8086|_16BITS},
    {  _(IMUL),            LOCK, REX_NONE, {0x6b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16,    IMM8}, 3u, _8086|_16BITS},
    {  _(IMUL),            LOCK, REX_NONE, {0x69, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16,   IMM16}, 4u, _8086|_16BITS},

    {    _(IN),        PRE_NONE, REX_NONE, {0xe4, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {    _(IN),        PRE_NONE, REX_NONE, {0xe5, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {    _(IN),        PRE_NONE, REX_NONE, {0xec, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,  REG_DX, OP_NONE}, 1u, _8086|_16BITS},
    {    _(IN),        PRE_NONE, REX_NONE, {0xed, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_DX, OP_NONE}, 1u, _8086|_16BITS},

    {   _(INC),            LOCK, REX_NONE, {0xfe, 0x00, 0x00}, 00u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(INC),            LOCK, REX_NONE, {0xff, 0x00, 0x00}, 00u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(INC),        PRE_NONE, REX_NONE, {0x40, 0x00, 0x00}, 00u, N, 01u, {  REG_AX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x41, 0x00, 0x00}, 00u, N, 01u, {  REG_CX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x42, 0x00, 0x00}, 00u, N, 01u, {  REG_DX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x43, 0x00, 0x00}, 00u, N, 01u, {  REG_BX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x44, 0x00, 0x00}, 00u, N, 01u, {  REG_SP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x45, 0x00, 0x00}, 00u, N, 01u, {  REG_BP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x46, 0x00, 0x00}, 00u, N, 01u, {  REG_SI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},
    {   _(INC),        PRE_NONE, REX_NONE, {0x47, 0x00, 0x00}, 00u, N, 01u, {  REG_DI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS|_NOLONG},

    {   _(INT),        PRE_NONE, REX_NONE, {0xcd, 0x00, 0x00}, 00u, N, 01u, {    IMM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {  _(INT3),        PRE_NONE, REX_NONE, {0xcc, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(INT03),        PRE_NONE, REX_NONE, {0xcc, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(INTO),        PRE_NONE, REX_NONE, {0xce, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(IRET),        PRE_NONE, REX_NONE, {0xcf, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

// -- The following Jcc OpCodes are grouped together as one since they are all very similar
    {    _(JA),        PRE_NONE, REX_NONE, {0x77, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JAE),        PRE_NONE, REX_NONE, {0x73, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JB),        PRE_NONE, REX_NONE, {0x72, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JBE),        PRE_NONE, REX_NONE, {0x76, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JC),        PRE_NONE, REX_NONE, {0x72, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(JCXZ),        PRE_NONE, REX_NONE, {0xe3, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JE),        PRE_NONE, REX_NONE, {0x74, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JG),        PRE_NONE, REX_NONE, {0x7f, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JGE),        PRE_NONE, REX_NONE, {0x7d, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JL),        PRE_NONE, REX_NONE, {0x7c, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JLE),        PRE_NONE, REX_NONE, {0x7e, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNA),        PRE_NONE, REX_NONE, {0x76, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(JNAE),        PRE_NONE, REX_NONE, {0x72, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNB),        PRE_NONE, REX_NONE, {0x73, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(JNBE),        PRE_NONE, REX_NONE, {0x77, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNC),        PRE_NONE, REX_NONE, {0x73, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNE),        PRE_NONE, REX_NONE, {0x75, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNG),        PRE_NONE, REX_NONE, {0x7e, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(JNGE),        PRE_NONE, REX_NONE, {0x7c, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JNL),        PRE_NONE, REX_NONE, {0x7d, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(JNLE),        PRE_NONE, REX_NONE, {0x7f, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JO),        PRE_NONE, REX_NONE, {0x70, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JP),        PRE_NONE, REX_NONE, {0x7a, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JPE),        PRE_NONE, REX_NONE, {0x7a, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JPO),        PRE_NONE, REX_NONE, {0x7b, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JS),        PRE_NONE, REX_NONE, {0x78, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {    _(JZ),        PRE_NONE, REX_NONE, {0x74, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {   _(JMP),        PRE_NONE, REX_NONE, {0xeb, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JMP),        PRE_NONE, REX_NONE, {0xe9, 0x00, 0x00}, 00u, N, 01u, {   REL16, OP_NONE, OP_NONE}, 3u, _8086|_16BITS},
    {   _(JMP),        PRE_NONE, REX_NONE, {0xff, 0x00, 0x00}, 04u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(JMP),        PRE_NONE, REX_NONE, {0xea, 0x00, 0x00}, 00u, N, 01u, {PTR16_16, OP_NONE, OP_NONE}, 5u, _8086|_16BITS},
    {   _(JMP),        PRE_NONE, REX_NONE, {0xff, 0x00, 0x00}, 05u, Y, 01u, {  M16_16, OP_NONE, OP_NONE}, 6u, _8086|_16BITS},

    {  _(LAHF),        PRE_NONE, REX_NONE, {0x9f, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(LDS),        PRE_NONE, REX_NONE, {0xc5, 0x00, 0x00}, 00u, N, 01u, {     R16,  M16_16, OP_NONE}, 6u, _8086|_16BITS|_NOLONG},
    {   _(LES),        PRE_NONE, REX_NONE, {0xc4, 0x00, 0x00}, 00u, N, 01u, {     R16,  M16_16, OP_NONE}, 6u, _8086|_16BITS|_NOLONG},

    {   _(LEA),        PRE_NONE, REX_NONE, {0xbd, 0x00, 0x00}, 00u, N, 01u, {     R16,       M, OP_NONE}, 4u, _8086|_16BITS},

    { _(LODSB), REPE|REPNE|LOCK, REX_NONE, {0xac, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(LODSW), REPE|REPNE|LOCK, REX_NONE, {0xad, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(LOOP),        PRE_NONE, REX_NONE, {0xe2, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    { _(LOOPE),        PRE_NONE, REX_NONE, {0xe1, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {_(LOOPNE),        PRE_NONE, REX_NONE, {0xe0, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {_(LOOPNZ),        PRE_NONE, REX_NONE, {0xe0, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    { _(LOOPZ),        PRE_NONE, REX_NONE, {0xe1, 0x00, 0x00}, 00u, N, 01u, {    REL8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {   _(MOV),            LOCK, REX_NONE, {0x88, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0x89, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0x8a, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0x8b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0x8c, 0x00, 0x00}, 00u, N, 01u, {    RM16,    SREG, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0x8e, 0x00, 0x00}, 00u, N, 01u, {    SREG,    RM16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),           DS|ES, REX_NONE, {0xa0, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,  MOFFS8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),           DS|ES, REX_NONE, {0xa1, 0x00, 0x00}, 00u, N, 01u, {  REG_AX, MOFFS16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(MOV),           DS|ES, REX_NONE, {0xa2, 0x00, 0x00}, 00u, N, 01u, {  MOFFS8,  REG_AL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),           DS|ES, REX_NONE, {0xa3, 0x00, 0x00}, 00u, N, 01u, { MOFFS16,  REG_AX, OP_NONE}, 3u, _8086|_16BITS},
    {   _(MOV),        PRE_NONE, REX_NONE, {0xb0, 0x00, 0x00}, 00u, N, 01u, {      R8,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),        PRE_NONE, REX_NONE, {0xb1, 0x00, 0x00}, 00u, N, 01u, {     R16,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0xc6, 0x00, 0x00}, 00u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MOV),            LOCK, REX_NONE, {0xc7, 0x00, 0x00}, 00u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 3u, _8086|_16BITS},

    { _(MOVSB), REPE|REPNE|LOCK, REX_NONE, {0xa4, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(MOVSW), REPE|REPNE|LOCK, REX_NONE, {0xa5, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(MUL),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 04u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(MUL),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 04u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {   _(NEG),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 03u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(NEG),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 03u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {   _(NOP),        PRE_NONE, REX_NONE, {0x90, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(NOT),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 02u, Y, 01u, {     RM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(NOT),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 02u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {    _(OR),        PRE_NONE, REX_NONE, {0x0c, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {    _(OR),        PRE_NONE, REX_NONE, {0x0d, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 01u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 01u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 01u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x08, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x09, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x0a, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {    _(OR),            LOCK, REX_NONE, {0x0b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {   _(OUT),        PRE_NONE, REX_NONE, {0xe6, 0x00, 0x00}, 00u, N, 01u, {    IMM8,  REG_AL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(OUT),        PRE_NONE, REX_NONE, {0xe7, 0x00, 0x00}, 00u, N, 01u, {    IMM8,  REG_AX, OP_NONE}, 2u, _8086|_16BITS},
    {   _(OUT),        PRE_NONE, REX_NONE, {0xee, 0x00, 0x00}, 00u, N, 01u, {  REG_DX,  REG_AL, OP_NONE}, 1u, _8086|_16BITS},
    {   _(OUT),        PRE_NONE, REX_NONE, {0xef, 0x00, 0x00}, 00u, N, 01u, {  REG_DX,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},

    { _(PAUSE),        PRE_NONE, REX_NONE, {0xf3, 0x90, 0x00}, 00u, N, 02u, { OP_NONE, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},

    {   _(POP),            LOCK, REX_NONE, {0x8f, 0x00, 0x00}, 00u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x58, 0x00, 0x00}, 00u, N, 01u, {  REG_AX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x59, 0x00, 0x00}, 00u, N, 01u, {  REG_CX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5a, 0x00, 0x00}, 00u, N, 01u, {  REG_DX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5b, 0x00, 0x00}, 00u, N, 01u, {  REG_BX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5c, 0x00, 0x00}, 00u, N, 01u, {  REG_SP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5d, 0x00, 0x00}, 00u, N, 01u, {  REG_BP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5e, 0x00, 0x00}, 00u, N, 01u, {  REG_SI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x5f, 0x00, 0x00}, 00u, N, 01u, {  REG_DI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x1f, 0x00, 0x00}, 00u, N, 01u, {  REG_DS, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x07, 0x00, 0x00}, 00u, N, 01u, {  REG_ES, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(POP),        PRE_NONE, REX_NONE, {0x17, 0x00, 0x00}, 00u, N, 01u, {  REG_SS, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(POPF),        PRE_NONE, REX_NONE, {0x9d, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(PUSH),        PRE_NONE, REX_NONE, {0xff, 0x00, 0x00}, 06u, Y, 01u, {    RM16, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x6a, 0x00, 0x00}, 06u, Y, 01u, {    IMM8, OP_NONE, OP_NONE}, 2u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x68, 0x00, 0x00}, 06u, Y, 01u, {   IMM16, OP_NONE, OP_NONE}, 3u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x50, 0x00, 0x00}, 00u, N, 01u, {  REG_AX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x51, 0x00, 0x00}, 00u, N, 01u, {  REG_CX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x52, 0x00, 0x00}, 00u, N, 01u, {  REG_DX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x53, 0x00, 0x00}, 00u, N, 01u, {  REG_BX, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x54, 0x00, 0x00}, 00u, N, 01u, {  REG_SP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x55, 0x00, 0x00}, 00u, N, 01u, {  REG_BP, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x56, 0x00, 0x00}, 00u, N, 01u, {  REG_SI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x57, 0x00, 0x00}, 00u, N, 01u, {  REG_DI, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x0e, 0x00, 0x00}, 00u, N, 01u, {  REG_CS, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x1e, 0x00, 0x00}, 00u, N, 01u, {  REG_DS, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x06, 0x00, 0x00}, 00u, N, 01u, {  REG_ES, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(PUSH),        PRE_NONE, REX_NONE, {0x16, 0x00, 0x00}, 00u, N, 01u, {  REG_SS, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(PUSHF),        PRE_NONE, REX_NONE, {0x9c, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(RCL),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 02u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCL),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 02u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCL),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 02u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(RCL),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 02u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCL),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 02u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCL),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 02u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(RCR),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 03u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCR),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 03u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCR),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 03u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(RCR),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 03u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCR),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 03u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(RCR),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 03u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(RET),        PRE_NONE, REX_NONE, {0xc3, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {   _(RET),        PRE_NONE, REX_NONE, {0xc2, 0x00, 0x00}, 00u, N, 01u, {   IMM16, OP_NONE, OP_NONE}, 3u, _8086|_16BITS},

    {  _(RETF),        PRE_NONE, REX_NONE, {0xcb, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},
    {  _(RETF),        PRE_NONE, REX_NONE, {0xca, 0x00, 0x00}, 00u, N, 01u, {   IMM16, OP_NONE, OP_NONE}, 3u, _8086|_16BITS},

    {   _(ROL),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 00u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROL),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 00u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROL),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 00u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ROL),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 00u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROL),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 00u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROL),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 00u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(ROR),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 01u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROR),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 01u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROR),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 01u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(ROR),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 01u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROR),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 01u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(ROR),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 01u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {  _(SAHF),        PRE_NONE, REX_NONE, {0x9e, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(SAL),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 04u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAL),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 04u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAL),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 04u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SAL),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 04u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAL),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 04u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAL),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 04u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(SAR),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 07u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAR),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 07u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAR),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 07u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SAR),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 07u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAR),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 07u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SAR),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 07u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(SHL),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 04u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHL),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 04u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHL),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 04u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SHL),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 04u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHL),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 04u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHL),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 04u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(SHR),        PRE_NONE, REX_NONE, {0xd0, 0x00, 0x00}, 05u, Y, 01u, {     RM8,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHR),        PRE_NONE, REX_NONE, {0xd2, 0x00, 0x00}, 05u, Y, 01u, {     RM8,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHR),        PRE_NONE, REX_NONE, {0xc0, 0x00, 0x00}, 05u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SHR),        PRE_NONE, REX_NONE, {0xd1, 0x00, 0x00}, 05u, Y, 01u, {    RM16,     ONE, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHR),        PRE_NONE, REX_NONE, {0xd3, 0x00, 0x00}, 05u, Y, 01u, {    RM16,  REG_CL, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SHR),        PRE_NONE, REX_NONE, {0xc1, 0x00, 0x00}, 05u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},

    {   _(SBB),        PRE_NONE, REX_NONE, {0x1c, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SBB),        PRE_NONE, REX_NONE, {0x1d, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 03u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 03u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 03u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x18, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x19, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x1a, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SBB),            LOCK, REX_NONE, {0x1b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    { _(SCASB), REPE|REPNE|LOCK, REX_NONE, {0xae, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(SCASW), REPE|REPNE|LOCK, REX_NONE, {0xaf, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(STC),        PRE_NONE, REX_NONE, {0xf9, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(STD),        PRE_NONE, REX_NONE, {0xfd, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(STI),        PRE_NONE, REX_NONE, {0xfb, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(STOSB), REPE|REPNE|LOCK, REX_NONE, {0xaa, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    { _(STOSW), REPE|REPNE|LOCK, REX_NONE, {0xab, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(SUB),        PRE_NONE, REX_NONE, {0x2c, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SUB),        PRE_NONE, REX_NONE, {0x2d, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 05u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 05u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 05u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x28, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x29, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x2a, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(SUB),            LOCK, REX_NONE, {0x2b, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {  _(TEST),        PRE_NONE, REX_NONE, {0xa8, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {  _(TEST),        PRE_NONE, REX_NONE, {0xa9, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {  _(TEST),            LOCK, REX_NONE, {0xf6, 0x00, 0x00}, 00u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {  _(TEST),            LOCK, REX_NONE, {0xf7, 0x00, 0x00}, 00u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {  _(TEST),            LOCK, REX_NONE, {0x84, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {  _(TEST),            LOCK, REX_NONE, {0x85, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},

    {  _(WAIT),        PRE_NONE, REX_NONE, {0x9b, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {  _(XCHG),        PRE_NONE, REX_NONE, {0x90, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x91, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_CX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x92, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_DX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x93, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_BX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x94, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_SP, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x95, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_BP, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x96, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_SI, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x97, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,  REG_DI, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x91, 0x00, 0x00}, 00u, N, 01u, {  REG_CX,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x92, 0x00, 0x00}, 00u, N, 01u, {  REG_DX,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x93, 0x00, 0x00}, 00u, N, 01u, {  REG_BX,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x94, 0x00, 0x00}, 00u, N, 01u, {  REG_SP,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x95, 0x00, 0x00}, 00u, N, 01u, {  REG_BP,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x96, 0x00, 0x00}, 00u, N, 01u, {  REG_SI,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),        PRE_NONE, REX_NONE, {0x97, 0x00, 0x00}, 00u, N, 01u, {  REG_DI,  REG_AX, OP_NONE}, 1u, _8086|_16BITS},
    {  _(XCHG),            LOCK, REX_NONE, {0x86, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {  _(XCHG),            LOCK, REX_NONE, {0x86, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {  _(XCHG),            LOCK, REX_NONE, {0x87, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {  _(XCHG),            LOCK, REX_NONE, {0x87, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},

    {  _(XLAT),      DS|ES|LOCK, REX_NONE, {0xd7, 0x00, 0x00}, 00u, N, 01u, { OP_NONE, OP_NONE, OP_NONE}, 1u, _8086|_16BITS},

    {   _(XOR),        PRE_NONE, REX_NONE, {0x34, 0x00, 0x00}, 00u, N, 01u, {  REG_AL,    IMM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(XOR),        PRE_NONE, REX_NONE, {0x35, 0x00, 0x00}, 00u, N, 01u, {  REG_AX,   IMM16, OP_NONE}, 3u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x80, 0x00, 0x00}, 06u, Y, 01u, {     RM8,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x81, 0x00, 0x00}, 06u, Y, 01u, {    RM16,   IMM16, OP_NONE}, 4u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x83, 0x00, 0x00}, 06u, Y, 01u, {    RM16,    IMM8, OP_NONE}, 3u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x30, 0x00, 0x00}, 00u, N, 01u, {     RM8,      R8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x31, 0x00, 0x00}, 00u, N, 01u, {    RM16,     R16, OP_NONE}, 2u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x32, 0x00, 0x00}, 00u, N, 01u, {      R8,     RM8, OP_NONE}, 2u, _8086|_16BITS},
    {   _(XOR),            LOCK, REX_NONE, {0x33, 0x00, 0x00}, 00u, N, 01u, {     R16,    RM16, OP_NONE}, 2u, _8086|_16BITS},
};

#define ENTRY_COUNT    (sizeof(OpCodeTable) / sizeof(OpCodeEntry))

typedef enum {
    false = 0,
    true = !false,
} bool;

static inline Mnemonic MakeMnemonic(const char *op)
{
    Mnemonic rv;

    rv.code[0] = rv.code[1] = 0;
    strcpy(rv.name, op);

    return rv;
}

static inline bool CompareMnemonic(Mnemonic mn1, Mnemonic mn2)
{
    return (mn1.code[0] == mn2.code[0] && mn1.code[1] == mn2.code[1]);
}

