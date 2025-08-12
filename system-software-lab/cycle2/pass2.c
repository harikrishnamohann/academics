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

int main() {
	FILE* optab_file = fopen("optab", "r");
	FILE* intermediate_file = fopen("pass1_intermediate_file", "r");
	

