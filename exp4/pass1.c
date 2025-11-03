#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char label[10], opcode[10], operand[20], code[10], mnemonic[10];
    int locctr, start = 0, length;

    FILE *input = fopen("input.txt", "r"), *optab = fopen("optab.txt", "r");
    FILE *symtab = fopen("symtab.txt", "w"), *intermediate = fopen("intermediate.txt", "w");

    if (!input || !optab || !symtab || !intermediate) {
        printf("Error opening files.\n");
        return 1;
    }

    fscanf(input, "%s %s %s", label, opcode, operand); // Read first line

    if (strcmp(opcode, "START") == 0) { // if opcode = START
        start = (int)strtol(operand, NULL, 16);
        locctr = start;
        fprintf(intermediate, "%s\t%s\t%s\t%s\n", "", label, opcode, operand); // Write line to intermediate file
        fscanf(input, "%s %s %s", label, opcode, operand); //read next input line
    }
    else locctr = 0; // Default start address

    while (strcmp(opcode, "END") != 0) { // while opcode is not END
        fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand); // Write to intermediate file

        if (strcmp(label, "**") != 0) 
            fprintf(symtab, "%s\t%04X\n", label, locctr); // Write label and locctr to symtab if label is not "**"

        int found = 0; // assume opcode not found in optab
        rewind(optab); // Reset file pointer to the beginning of optab

        while (fscanf(optab, "%s %s", code, mnemonic) != EOF) {  // search optab for opcode
            if (strcmp(code, opcode) == 0) {
                found = 1;
                locctr += 3;
                break;
            }
        }

        if (!found) { // when opcode is not found in optab i.e it is a directive
            if (strcmp(opcode, "WORD") == 0)       locctr += 3;
            else if (strcmp(opcode, "RESW") == 0)  locctr += 3 * atoi(operand);
            else if (strcmp(opcode, "RESB") == 0)  locctr += atoi(operand);
            else if (strcmp(opcode, "BYTE") == 0) {
                if  (operand[0] == 'C')      locctr += strlen(operand) - 3; 
                else if (operand[0] == 'X')  locctr += (strlen(operand) - 3) / 2; 
            }
        }
        fscanf(input, "%s %s %s", label, opcode, operand); // Read next input line
    }

    fprintf(intermediate, "%04X\t%s\t%s\n", locctr, label, opcode); // Write last line to intermediate file

    length = locctr - start;
    printf("Length of the code: %X \n", length);

    fclose(input),fclose(optab),fclose(symtab),fclose(intermediate);
    return 0;
}