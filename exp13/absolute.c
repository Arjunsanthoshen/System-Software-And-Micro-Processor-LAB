#include <stdio.h>
#include <stdlib.h> // for atoi
#include <string.h> // for strncpy

int main(){
	int i,addr;
	char line[50],strtaddr[7];
	FILE *prog = fopen("program.txt","r");

	if(!prog){ // file not found
		printf("error opening file: No such file or directory error opening file");
		return 1; 
	}
// if (fscanf(prog, "%s", line) == 1) { // use this to check if input file is empty
	fscanf(prog, "%s", line); // read the first line
	printf("program name is: ");	
	for(i = 2 ; i < 8 ; i++) // i=2 to skip H and ^, i < 8 to read only 6 characters of program name	
		printf("%c",line[i]); // print program name

	while (fscanf(prog,"%s",line)==1){ // == 1 checks that one item was successfully read (so the loop runs until EOF)
		if(line[0]=='T'){
			strncpy(strtaddr,line+2,6); // copy 6 characters from position 2 of line to strtaddr
			strtaddr[6]='\0'; // adding null terminator at the end of strtaddr to make it a valid string
			addr = atoi(strtaddr); //Converts the starting address string "001000" into an integer value 1000.
			for (i = 12; line[i] != '\0'; ) { // i=12 to skip T, ^, starting address(6)^, lenght(2)^, till the end of line
    			if (line[i] != '^') {
        			printf("\n%d :\t %c%c", addr++, line[i], line[i+1]);
        			i += 2;
    			} else // if line[i] is '^'
        			i++; // skip the '^' character
			}
		} else if(line[0]=='E') break;
	} 

	fclose(prog);
	return 0;
}