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
#define OPTAB_FILE "./optab.txt"
#define SYMTAB_FILE "./symtab.txt"
#define PASS1_INTR_FILE "./pass1_intermediate_file.dat"

#define OPCODE_LENGTH 3
#define BUF_SIZE 20

char opcode[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

bool contains(FILE* table, char* value) {
	char key[BUF_SIZE], val[BUF_SIZE];
	bool contains = false;
	fscanf(table, "%s\t%s\n", key, val);
	do {
		if (strcmp(value, key) == 0) {
			contains = true;
			break;
		}
	} while (fscanf(table, "%s\t%s\n", key, val) != EOF);
	rewind(table);
	return contains;
}

void advance_line(FILE* program) {
	memset(opcode, '\0', sizeof(char) * BUF_SIZE);
	memset(operand, '\0', sizeof(char) * BUF_SIZE);
	memset(label, '\0', sizeof(char) * BUF_SIZE);
	fscanf(program, "%s\t%s\t%s\n", label, opcode, operand);
	// printf("%s\t%s\t%s\n", label, opcode, operand);
}

void write_to_pass1_file(FILE* output, int address) {
	fprintf(output, "%4d\t%s\t%s\t%s\n", address, label, opcode, operand);
}

void insert_label(FILE* symtab, char* label, int locctr) {
	fprintf(symtab, "%s\t%s\n", label, locctr);
}

int main() {
	FILE* sic_program = fopen(SIC_FILE, "r");
	FILE* optab = fopen(OPTAB_FILE, "r");
	FILE* symtab = fopen(SYMTAB_FILE, "w");
	FILE* pass1_output = fopen(PASS1_INTR_FILE, "w");
	if (sic_program == NULL || optab == NULL || symtab == NULL || pass1_output == NULL) {
		perror("error opening files");
	}
	
	int starting_addr = 0, locctr = 0;
	advance_line(sic_program);
	if (strcmp(opcode, "START") == 0) {
		starting_addr = atoi(operand);
		locctr = starting_addr;
		write_to_pass1_file(pass1_output, locctr);
	}
	

	while (strcmp(opcode, "END") == 0) {
		advance_line(sic_program);
		if (*label != '-') {
			if (contains(symtab, label)) {
				printf("duplicate label found: %s at %d \n", label, locctr);
				goto end_pass1;
			} else {
				insert_label(symtab, label, locctr);
			}
		}
		
		if (contains(optab, opcode)) {
			if (strcmp(opcode, "WORD") == 0) {
				locctr += OPCODE_LENGTH;
			} else if (strcmp(opcode, "RESW") == 0) {
				locctr += 3 * strlen(operand);
				// todo
			}
		} else {
			printf("invalid operand found %s at %d\n", opcode, locctr);
			goto end_pass1;
		}
		
	}
	
	
	
	end_pass1:
		fclose(sic_program);
		fclose(optab);
		fclose(symtab);
		fclose(pass1_output);
		return 0;
}
