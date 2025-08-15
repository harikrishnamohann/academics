#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTRUCTION_LENGTH 3
#define BUF_SIZE 30
#define TEXT_RECORD_LIMIT 0x1e
char opcode[BUF_SIZE], address[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

int lookup_table(FILE* table, char* search_key, char* dest) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(search_key, key) == 0) {
      strcpy(dest, val);		    
			return 1;
		}
	} 
	return 0;
}

int parse_addressing_mode() {
  int last_ch = strlen(operand) - 1;
  if (operand[last_ch] == 'X' && operand[last_ch - 1] == ',') {
    operand[last_ch - 1] = '\0';
    return 0x8000;
  }
  return 0x0000;
}

int parse_constant() {
  int const_size = strlen(operand) - 3;
  char buf[BUFSIZ];
  strcpy(buf, operand + 2);
  buf[const_size] = '\0';
  if (operand[0] == 'X') { // the operand is a hexadecimal number
    const_size = INSTRUCTION_LENGTH;
    int constant = strtol(buf, NULL, 16);
    sprintf(operand, "%06X", constant);
  } else if (operand[0] == 'C') {
    operand[0] = '\0';
    for (int i = 0; i < const_size; i++) {
      sprintf(operand, "%s%02X", operand, buf[i]);
    }
  }
  return const_size;
}

void read_line(FILE* fp) {
  fscanf(fp, "%s\t%s\t%s\t%s\n", address, label, opcode, operand);
}

void write_listing(FILE* fp) {
  fprintf(fp, "%s\t%s\t%s\t%s\n", address, label, opcode, operand);
}

void update_text_length(FILE* obj_file, int len_pos, int new_length) {
  fseek(obj_file, len_pos, SEEK_SET);
  fprintf(obj_file, "%02X", new_length);
  fseek(obj_file, 0, SEEK_END);
}

void write_obj(FILE* obj_file) {
  fprintf(obj_file, " %s%s", opcode, operand);
}

int init_text_record(FILE* obj_file) {
  fprintf(obj_file, "\nT 00%s ", address);
  int pos = (int)ftell(obj_file);
  fprintf(obj_file, "**");
  return pos;
}

int main() {
  FILE* pass1_intermediate = fopen("pass1_intermediate_file", "r");
  FILE* symtab = fopen("symtab", "r");
  FILE* optab = fopen("optab", "r");
  FILE* obj_file = fopen("obj_code", "w");
  FILE* listing = fopen("listing_file", "w");

  read_line(pass1_intermediate);
  if (strcmp(opcode, "START") == 0) {
    write_listing(listing);
    fprintf(obj_file, "H %s 00%s 00%s", label, operand, address);
  }
  read_line(pass1_intermediate);
  int starting_address = strtol(address, NULL, 16);

  int record_length = 0;
  int length_position = init_text_record(obj_file);

  while (strcmp(opcode, "END") != 0) {
    int operand_address = 0x0000;
    int instruction_length = 0;
    if (lookup_table(optab, opcode, opcode)) { // now optab value is stored in opcode[] 
      if (strcmp(operand, "**") != 0) { // operand field is not empty
        int addressing_mode = parse_addressing_mode();
        if (lookup_table(symtab, operand, operand)) { // symtab address is stored in operand[]
          operand_address = (int)strtol(operand, NULL, 16) | addressing_mode;
        }
      }
      sprintf(operand, "%04X", operand_address);
      instruction_length = INSTRUCTION_LENGTH;
    } else if (strcmp(opcode, "BYTE") == 0) {
      strcpy(opcode, "");
      instruction_length = parse_constant();
    } else if (strcmp(opcode, "WORD") == 0) {
      strcpy(opcode, "");
      sprintf(operand, "%06X", atoi(operand));
      instruction_length = INSTRUCTION_LENGTH;
    } else if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
      write_listing(listing);
      read_line(pass1_intermediate);
      continue;
    } else {
      printf("Error: Unknown opcode '%s'\n", opcode);
      exit(1);
    }

    if (record_length + instruction_length > TEXT_RECORD_LIMIT) {
      update_text_length(obj_file, length_position, record_length);
      length_position = init_text_record(obj_file);
      record_length = 0;
    }
    record_length += instruction_length;
    update_text_length(obj_file, length_position, record_length);
    write_obj(obj_file);
    write_listing(listing);

    read_line(pass1_intermediate);
  }

  fprintf(obj_file, "\nE %06X\n", starting_address);

  fclose(pass1_intermediate);
  fclose(symtab);
  fclose(obj_file);
  fclose(optab);
  fclose(listing);
  return 0;
}
