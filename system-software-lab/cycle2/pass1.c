/*
# PASS - 1
Inputs:
	file1: sic program
	file2: optab

Outputs:
	file1: symtab
	file2: intermediate file
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIC_FILE "./input.sic"
#define OPTAB_FILE "./optab"
#define SYMTAB_FILE "./symtab"
#define PASS1_INTR_FILE "./pass1_intermediate_file"

#define INSTRUCTION_SIZE 3
#define BUF_SIZE 20

char opcode[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

// checks if table contains entity in key field in a key-value paired table
// for optab and symtab
bool contains(FILE* table, char* entity) {
	rewind(table);
	char key[BUF_SIZE], val[BUF_SIZE];
	bool contains = false;
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) {
			contains = true;
			break;
		}
	} 
	return contains;
}

// saves current line of the sic program to corresponding fields
// and advances file pointer
void advance_line(FILE* program) {
	int count = fscanf(program, "%s\t%s\t%s\n", label, opcode, operand);
	if (count != 3) {
		printf("error: malformated input found\n");
		exit(1);
	}
}

void write_pass1(FILE* pass1_file, int address) {
	fprintf(pass1_file, "%04X\t%s\t%s\t%s\n", address, label, opcode, operand);
}

// insert symbol to symtab
void insert_symbol(FILE* symtab, char* label, int locctr) {
	fprintf(symtab, "%s\t%04X\n", label, locctr);
}

int eval_byte_length(char* stream) {
	int length = strlen(stream) - 3; // 3 for C'' or X'' characters
	if (stream[0] == 'X') {
		return length / 2;
	}
	return length;
}

int main() {
	FILE* sic_file = fopen(SIC_FILE, "r");
	FILE* optab_file = fopen(OPTAB_FILE, "r");
	FILE* symtab_file = fopen(SYMTAB_FILE, "w+");
	FILE* pass1_file = fopen(PASS1_INTR_FILE, "w");
	if (sic_file == NULL || optab_file == NULL || symtab_file == NULL || pass1_file == NULL) {
		perror("error opening files");
	}
	
	int starting_addr = 0, locctr = 0;
	advance_line(sic_file);
	if (strcmp(opcode, "START") == 0) {
		starting_addr = atoi(operand);
		locctr = starting_addr;
		write_pass1(pass1_file, locctr);
	}
	
	advance_line(sic_file);
	while (strcmp(opcode, "END") != 0) {
		if (label[0] != '-') {
			if (contains(symtab_file, label)) {
				printf("duplicate label found: %s at %d \n", label, locctr);
				goto end_pass1;
			} else {
				insert_symbol(symtab_file, label, locctr);
			}
		}
		
		write_pass1(pass1_file, locctr);
		if (contains(optab_file, opcode)) {
			locctr += INSTRUCTION_SIZE;
		} else {
 			if (strcmp(opcode, "WORD") == 0) {
				locctr += INSTRUCTION_SIZE;
			} else if (strcmp(opcode, "BYTE") == 0) {
				locctr += eval_byte_length(operand);
			} else if (strcmp(opcode, "RESW") == 0) {
				locctr += 3 * atoi(operand);
			} else if (strcmp(opcode, "RESB") == 0) {
				locctr += atoi(operand);
			} else {
				printf("invalid operand found %s at %d\n", opcode, locctr);
				goto end_pass1;
			}
		}
		advance_line(sic_file);
	}
	
	write_pass1(pass1_file, locctr);
	printf("program length: %04X\n", locctr - starting_addr);
	
	end_pass1:
		fclose(sic_file);
		fclose(optab_file);
		fclose(symtab_file);
		fclose(pass1_file);
		return 0;
}
