#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INSTRUCTION_LENGTH 3
#define BUF_SIZE 30
#define T_LIMIT 30

char opcode[BUF_SIZE], address[BUF_SIZE], operand[BUF_SIZE], label[BUF_SIZE];

typedef enum {
  address_t,
  opcode_t,
} search_type;

bool search_and_load(FILE* table, char* entity, search_type type) {
	char key[BUF_SIZE], val[BUF_SIZE];
	rewind(table);
	while (fscanf(table, "%s\t%s\n", key, val) != EOF) {
		if (strcmp(entity, key) == 0) {
		  if (type == address_t) {
        strcpy(operand, val);		    
		  } else if (type == opcode_t) {
		    strcpy(opcode, val);
		  }
			return true;
		}
	} 
	return false;
}

typedef enum {
  direct = 0x0,
  indexed = 0x8000,
} addressing_type;

addressing_type parse_addressing_mode() {
  int last_ch = strlen(operand) - 1;
  if (operand[last_ch] == 'X' && operand[last_ch - 1] == ',') {
    operand[last_ch - 1] = '\0';
    return indexed;
  }
  return direct;
}

int parse_constant() {
  int const_size = strlen(operand) - 3;
  char buf[BUFSIZ];
  strcpy(buf, operand + 2);
  buf[const_size] = '\0';
  if (operand[0] == 'X') { // the operand is a hexadecimal number
    const_size = INSTRUCTION_LENGTH;
  }
  strcpy(operand, buf);
  return const_size;
}

void read_line(FILE* fp) {
  fscanf(fp, "%s\t%s\t%s\t%s\n", address, label, opcode, operand);
}

void write_listing(FILE* fp) {
  fprintf(fp, "%s\t%s\t%s\t%s\n", address, label, opcode, operand);
}

void update_text_size(FILE* obj_file, int len_pos, int new_length) {
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
  FILE* pass2_intermediate = fopen("pass2_intermediate_file", "w");

  read_line(pass1_intermediate);
  if (strcmp(opcode, "START") == 0) {
    write_listing(pass2_intermediate);
    fprintf(obj_file, "H %s %s %s", label, operand, address);
  }
  read_line(pass1_intermediate);

  int text_rec_size = 0;
  int len_pos = init_text_record(obj_file);
  while (strcmp(opcode, "END") != 0) {
    int operand_address = 0x0000;
    int obj_length = 0;
    if (search_and_load(optab, opcode, opcode_t)) { // now optab value is stored in opcode[] 
      if (strcmp(operand, "**") != 0) { // operand field is not empty
        addressing_type addr_mode = parse_addressing_mode();
        if (search_and_load(symtab, operand, address_t)) { // symtab address is stored in operand[]
          operand_address = (int)strtol(operand, NULL, 16);
          operand_address = operand_address ^ addr_mode;
        }
      }
      sprintf(operand, "%04X", operand_address);
      obj_length = INSTRUCTION_LENGTH;
    } else if (strcmp(opcode, "BYTE") == 0) {
      obj_length = parse_constant();
    } else if (strcmp(opcode, "WORD") == 0) {
      opcode[0] = '\0';
      int op = atoi(operand);
      sprintf(operand, "%06X", op);
      obj_length = INSTRUCTION_LENGTH;
    } else if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
      read_line(pass1_intermediate);
      continue;
    } else {
      printf("invalid opcode found: %s\n", opcode);
      exit(1);
    }

    if (text_rec_size + obj_length > T_LIMIT) {
        update_text_size(obj_file, len_pos, text_rec_size);
        len_pos = init_text_record(obj_file);
        text_rec_size = obj_length;
        update_text_size(obj_file, len_pos, text_rec_size);
        write_obj(obj_file);
    } else {
      text_rec_size += obj_length;
      update_text_size(obj_file, len_pos, text_rec_size);
      write_obj(obj_file);
      write_listing(pass2_intermediate);
    }
    read_line(pass1_intermediate);
  }

  fclose(pass1_intermediate);
  fclose(symtab);
  fclose(obj_file);
  fclose(optab);
  fclose(pass2_intermediate);
  return 0;
}
