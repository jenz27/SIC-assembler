SIC Assembler Project
This project implements both One-Pass Assembler and Two-Pass Assembler in C for the SIC (Simplified Instructional Computer) architecture.

Files in the Project
1PassAssembler.c → Implementation of One-Pass Assembler
2PassAssembler.c → Implementation of Two-Pass Assembler
opcodes.txt → Contains opcode table (mnemonic and corresponding machine code)
sample_input.txt → Sample assembly program input
intermediate.txt → Intermediate file generated during assembly process
Commands to Run Two-Pass Assembler
gcc twopass.c -o twopass
./twopass
Commands to Run One-Pass Assembler
gcc onepass.c -o onepass
./onepass
Assumptions
Input is read from a file named sample_input.txt
The input file must be placed in the same directory as the source code
opcodes.txt must also be present in the same directory
Output object code is displayed on the terminal
Intermediate file (intermediate.txt) is generated automatically when required
How it Works
One-Pass Assembler
Processes the source code in a single pass
Builds symbol table and generates object code simultaneously
Handles forward references using fix-up mechanism
Two-Pass Assembler
Pass 1: - Reads source program - Assigns addresses to instructions - Builds symbol table - Generates intermediate file

Pass 2: - Uses intermediate file and symbol table - Generates final object code

