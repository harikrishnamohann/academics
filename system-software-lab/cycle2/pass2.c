#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTRUCTION_SIZE 3
#define BUF_SIZE 20

char address[BUF_SIZE], opcode[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

// checks if entity is present in given key-value paired table
// and returns value field if entity is matched in key field.
// for symtab and optab checking. returns -1 if not found
int contains(FILE* table, char* entity) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) {
			return (int)strtol(val, NULL, 16);
		}
	} 
	return -1;
}

void read_pass1(FILE* fp) {
	fscanf(fp, "%s\t%s\t%s\t%s\n", address, label, opcode, operand);
}

int main() {
	FILE* optab_file = fopen("optab", "r");
	FILE* pass1_file = fopen("pass1_intermediate_file", "r");
	FILE* pass2_file = fopen("pass2_intermediate_file", "w");
	FILE* obj_file = fopen("obj_code", "w");
	
	read_pass1(pass1_file);
	if (strcmp(opcode, "START") == 0) {
		fscanf(obj_file, "H %s %s %s\n", label, operand, address); 
	}
	
	end:
		fclose(optab_file);
		fclose(pass1_file);
		fclose(pass2_file);
		fclose(obj_file);
		return 0;
}
	
	

