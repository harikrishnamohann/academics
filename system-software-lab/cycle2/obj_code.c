#include <stdio.h>
#include <string.h>

#define BUF_SIZE 20

void print_tabval(FILE* table, char* entity) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) {
			printf("%s", val);
			return;
		}
	} 
}

int main(int argc, char* argv[]) {
	FILE* optab_file = fopen("./optab", "r");
	FILE* symtab_file = fopen("./symtab", "r");
	if (!optab_file || !symtab_file) {
		perror("error opening files");
	}

	print_tabval(optab_file, argv[1]);
	print_tabval(symtab_file, argv[2]);
	printf("\n");

	fclose(optab_file);
	fclose(symtab_file);
}
