as-cent is an assembler written specifically to compile Century-64.  The reason for writing this assembler is to improve on the error messages delivered by the assembler.  I hope that the resulting messages will be able to help improve the stability of the code for Century-64.
For the moment, I am only working on the 8086 instruction set.  The following shows the roadmap for the as-cent assembler:
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
