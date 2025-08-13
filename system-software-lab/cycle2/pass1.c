#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INSTRUCTION_SIZE 3
#define BUF_SIZE 20

char instruction[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

// checks if entity is present in given key-value paired table
// and returns value field if entity is matched in key field.
// for symtab and optab checking. returns -1 if not found
bool contains(FILE* table, char* entity) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) {
			return true;
		}
	} 
	return false;
}

int eval_const_length(char* stream) {
	int length = strlen(stream) - 3; // 3 for C'' or X'' characters
	if (stream[0] == 'X') return length / 2;
	return length;
}

int main() {
	FILE* sic_file = fopen("input.sic", "r");
	FILE* optab_file = fopen("optab", "r");
	FILE* symtab_file = fopen("symtab", "w+");
	FILE* pass1_file = fopen("pass1_intermediate_file", "w");
	
	int starting_addr = 0, locctr = 0;
	fscanf(sic_file, "%s\t%s\t%s\n", label, instruction, operand);
	if (strcmp(instruction, "START") == 0) {
		starting_addr = (int)strtol(operand, NULL, 16);
		locctr = starting_addr;
		fprintf(pass1_file, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);
	} 
	fscanf(sic_file, "%s\t%s\t%s\n", label, instruction, operand);
	while (strcmp(instruction, "END") != 0) {
		if (strcmp(label, "**") != 0) {
			if (contains(symtab_file, label)) {
				printf("duplicate label found: %s at %d \n", label, locctr);
				exit(1);
			} else {
				fprintf(symtab_file, "%s\t%04X\n", label, locctr);
			}
		}
		fprintf(pass1_file, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);
		if (contains(optab_file, instruction)) {
			locctr += INSTRUCTION_SIZE;
		} else if (strcmp(instruction, "WORD") == 0) {
			locctr += INSTRUCTION_SIZE;
		} else if (strcmp(instruction, "BYTE") == 0) {
			locctr += eval_const_length(operand);
		} else if (strcmp(instruction, "RESW") == 0) {
			locctr += INSTRUCTION_SIZE * atoi(operand);
		} else if (strcmp(instruction, "RESB") == 0) {
			locctr += atoi(operand);
		} else {
			printf("invalid operand found: %s at %04X\n", instruction, locctr);
			exit(1);
		}
		fscanf(sic_file, "%s\t%s\t%s\n", label, instruction, operand);
	}
	
	fprintf(pass1_file, "%04X\t%s\t%s\t%s\n", locctr, label, instruction, operand);
	rewind(pass1_file); // writing program length as first 4 letters in the file.
	fprintf(pass1_file, "%04X", locctr - starting_addr);
	
	fclose(sic_file);
	fclose(optab_file);
	fclose(symtab_file);
	fclose(pass1_file);
	return 0;
}
