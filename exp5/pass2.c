#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>

int main() {
    char label[20], opcode[20], operand[20],code[20], mnemonic[20];
    char symbol[20], address[20],line[100], objcode[7],progName[7] = "", textRecord[100] = "";
    int locctr, startAddr = 0, progLen, firstTextAddr = -1, textLen = 0;
   
    FILE *intermediate = fopen("intermediate.txt", "r"), *optab = fopen("optab.txt", "r"), *symtab = fopen("symtab.txt", "r");
    FILE *codeFile = fopen("code.txt", "w"), *programFile = fopen("program.txt", "w");

    if (!intermediate || !optab || !symtab || !codeFile || !programFile) {
        printf("Error opening files.\n");
        return 1;
    }

    fgets(line, sizeof(line), intermediate);  // read from intermediate to buffer line
    sscanf(line, "%s %s %s", label, opcode, operand); // line is then parsed to get the values
    
    if (strcmp(opcode, "START") == 0) { 
        startAddr = (int)strtol(operand, NULL, 16); // convert hex string to integer
        strcpy(progName, label);
    }
    
    while (fgets(line, sizeof(line), intermediate)) { // read each line from intermediate till EOF
        sscanf(line, "%X %s %s %s", &locctr, label, opcode, operand);

        if (strcmp(opcode, "END") == 0) break;

        if (firstTextAddr == -1) firstTextAddr = locctr;        // record the address of first text record

        // --- Generate object code  ---
        int opfound = 0, symFound = 0; // opcode and symbol found flag 
        rewind(optab); // set file pointer back to start of the file as it would change with the while loop below
        while (fscanf(optab, "%s %s", code, mnemonic) != EOF) {
            if (strcmp(opcode, code) == 0) { // if opcode of intermediate is in optab 
                opfound = 1; // opcode opfound in optab 
                rewind(symtab);
                while (fscanf(symtab, "%s %s", symbol, address) != EOF) {
                    if (strcmp(operand, symbol) == 0) { // operand can have ALPHA,BETA,etc.
                        symFound = 1; // symbol found in symtab
                        sprintf(objcode, "%s%s", mnemonic, address); // concatenate mnemonic and address to object code array 
                        break;
                    }
                }
                if (!symFound) sprintf(objcode, "%s0000", mnemonic); // if symbol not found in symtab, use 0000 as address
                break;
            }
        }

        // --- Add to textRecord + codeFile ---
        if (opfound) { textLen += 3; }  // if opcode found in optab
        else if (strcmp(opcode, "WORD") == 0) {  // if opcode is WORD
            sprintf(objcode, "%06X", atoi(operand));
            textLen += 3;
        }
        else if (strcmp(opcode, "BYTE") == 0) {
            strcpy(objcode, "");
            for (int i = 2; operand[i] != '\''; i++) // i = 2 because we want to skip C and single quote ' , eg C'EOF', go till next single quote (\')
                sprintf(objcode + strlen(objcode),(operand[0] == 'C') ? "%02X" : "%c", operand[i]); // if C, use %02X (to convert to hex ) , else if 'X' %c (just write)
            textLen += strlen(objcode) / 2;
        }
        else continue; // skip if none matched (e.g., RESW/RESB)

        sprintf(textRecord + strlen(textRecord), "^%s", objcode); // append object code to text record
        fprintf(codeFile, "%06X\t%s\t%s\t%s\t%s\n",locctr, label, opcode, operand, objcode); // write to code file
    }

    progLen = locctr - startAddr;
    fprintf(programFile, "H^%-6s^%06X^%06X\n", progName, startAddr, progLen);     // Header record
    fprintf(programFile, "T^%06X^%02X%s\n", firstTextAddr, textLen, textRecord); // Text record
    fprintf(programFile, "E^%06X\n", startAddr);                                // End record

    fclose(intermediate), fclose(optab), fclose(symtab), fclose(codeFile), fclose(programFile);
    printf("<< Pass Two completed >>\n");
    return 0;
} // best of luck !