#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INSTRUCTION_SIZE 3
#define BUF_SIZE 20

char opcode[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

// checks if table contains entity in key field in a key-value paired table
// for optab and symtab
bool contains(FILE* table, char* entity) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) return true;
	} 
	return false;
}

// saves current line of the sic program to corresponding buffers
// and advances file pointer
void advance_line(FILE* program) {
	fscanf(program, "%s\t%s\t%s\n", label, opcode, operand);
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
	if (stream[0] == 'X') return length / 2;
	return length;
}

int main() {
	FILE* sic_file = fopen("./input.sic", "r");
	FILE* optab_file = fopen("./optab", "r");
	FILE* symtab_file = fopen("./symtab", "w+");
	FILE* pass1_file = fopen("./pass1_intermediate_file", "w");
	if (!sic_file || !optab_file || !symtab_file || !pass1_file) {
		perror("error opening files");
	}
	
	int starting_addr = 0, locctr = 0;
	advance_line(sic_file);
	if (strcmp(opcode, "START") == 0) {
		starting_addr = (int)strtol(operand, NULL, 16);
		locctr = starting_addr;
		write_pass1(pass1_file, locctr);
	}
	
	advance_line(sic_file);
	while (strcmp(opcode, "END") != 0) {
		if (strcmp(label, "**") != 0) {
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
	rewind(pass1_file); // writing program length as first 4 letters in the file.
	fprintf(pass1_file, "%04X", locctr - starting_addr);
	
	end_pass1:
		fclose(sic_file);
		fclose(optab_file);
		fclose(symtab_file);
		fclose(pass1_file);
		return 0;
}
