#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INSTR_SIZ 3

bool search_tab(FILE* fp, char* entity) {
  rewind(fp);
  char key[32], val[32];
  fscanf(fp, "%s %s", key, val);
  while (!feof(fp)) {
    if (strcmp(key, entity) == 0) return true;
    fscanf(fp, "%s %s", key, val);
  }
  return false;
}

int main() {
  FILE* input = fopen("input", "r");
  FILE* optab = fopen("optab", "r");
  FILE* symtab = fopen("symtab", "w+");
  FILE* intmt = fopen("intermediate", "w");
  FILE* length = fopen("length", "w");

  char label[32], opcode[32], operand[32];
  int locctr = 0, start_addr = 0;
  fscanf(input, "%s %s %s", label, opcode, operand);
  if (strcmp(opcode, "START") == 0) {
    locctr = start_addr = strtol(operand, NULL, 16);
    fprintf(intmt, "**\t%s\t%s\t%s\n", label, opcode, operand);
  }

  fscanf(input, "%s %s %s", label, opcode, operand);

  while (strcmp(opcode, "END") != 0) {
    if (strcmp(label, "**") != 0) {
      if (!search_tab(symtab, label)) {
        fprintf(symtab, "%s\t%04X\n", label, locctr);
      } else {
        printf("duplicate label found: %s\n", label);
        exit(-1);
      }
    }
    fprintf(intmt, "%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);
    if (search_tab(optab, opcode)) {
      locctr += INSTR_SIZ;
    } else if (strcmp(opcode, "WORD") == 0) {
      locctr += INSTR_SIZ;
    } else if (strcmp(opcode, "BYTE") == 0) {
      int len = strlen(operand) - 3;
      if (*operand == 'X') len /= 2;
      locctr += len;
    } else if (strcmp(opcode, "RESW") == 0) {
      locctr += INSTR_SIZ * atoi(operand);
    } else if (strcmp(opcode, "RESB") == 0) {
      locctr += atoi(operand);
    } else {
      printf("invalid operand found\n");
      exit(-1);
    }
    fscanf(input, "%s %s %s", label, opcode, operand);
  }

  fprintf(intmt, "%04X\t%s\t%s\t%s\n", locctr, label, opcode, operand);
  fprintf(length, "%04X", locctr - start_addr);

  fclose(length);
  fclose(intmt);
  fclose(symtab);
  fclose(optab);
  fclose(input);
  return 0;
}
