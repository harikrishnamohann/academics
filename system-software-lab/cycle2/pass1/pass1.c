#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 20
#define INSTRUCTION_LENGTH 3
char instruction[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

int contains(FILE* table, char* search_key) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(search_key, key) == 0) {
			return 1;
		}
	} 
	return 0;
}

int main() {
	FILE* input_sic = fopen("input.sic", "r");
	FILE* optab = fopen("optab", "r");
	FILE* symtab = fopen("symtab", "w+");
	FILE* intermediate = fopen("pass1_intermediate_file", "w");
	
	int starting_addr = 0, locctr = 0;
	fscanf(input_sic, "%s\t%s\t%s\n", label, instruction, operand);
	if (strcmp(instruction, "START") == 0) {
		starting_addr = (int)strtol(operand, NULL, 16);
		locctr = starting_addr;
		fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);
	} 

	fscanf(input_sic, "%s\t%s\t%s\n", label, instruction, operand);

	while (strcmp(instruction, "END") != 0) {
		if (strcmp(label, "**") != 0) {
			if (contains(symtab, label)) {
				printf("duplicate label found: %s at %d \n", label, locctr);
				exit(1);
			} else {
				fprintf(symtab, "%s\t%04X\n", label, locctr);
			}
		}

		fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);

		if (contains(optab, instruction)) {
			locctr += INSTRUCTION_LENGTH;
		} else if (strcmp(instruction, "WORD") == 0) {
			locctr += INSTRUCTION_LENGTH;
		} else if (strcmp(instruction, "BYTE") == 0) {
			int length = strlen(operand) - INSTRUCTION_LENGTH; // 3 for C'' or X'' characters
			if (operand[0] == 'X') length /= 2;
			locctr += length;
		} else if (strcmp(instruction, "RESW") == 0) {
			locctr += INSTRUCTION_LENGTH * atoi(operand);
		} else if (strcmp(instruction, "RESB") == 0) {
			locctr += atoi(operand);
		} else {
			printf("invalid operand found: %s at %04X\n", instruction, locctr);
			return 1;
		}

		fscanf(input_sic, "%s\t%s\t%s\n", label, instruction, operand);
	}
	
	fprintf(intermediate, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);
	rewind(intermediate); // writing program length as first 4 letters in the file.
	fprintf(intermediate, "%04X", locctr - starting_addr);
	
	fclose(input_sic);
	fclose(optab);
	fclose(symtab);
	fclose(intermediate);
	return 0;
}
