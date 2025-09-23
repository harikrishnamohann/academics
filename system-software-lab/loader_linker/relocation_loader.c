#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef union {
	unsigned int raw: 24;
	unsigned char mask[3];
} ObjCode;

int main() {
	FILE* fp1 = fopen("obj_code", "r");
	char buf[12];
	unsigned int start_addr = 0;
	fscanf(fp1, "%s", buf);
	if (strcmp(buf, "H") == 0) {
		fscanf(fp1, "%s", buf);
		printf("program name: %s\n", buf);
		fscanf(fp1, "%x", &start_addr);
		fscanf(fp1, "%s", buf);
		printf("length: %s\n", buf);
	} else {
		printf("invalid obj program\n");
		return 1;
	}
	ObjCode obj = {0};
	
	fscanf(fp1, "%s", buf);
	while (strcmp(buf, "E") != 0) {
		if (strcmp(buf, "T") == 0) {
			fscanf(fp1, "%s %s %s", buf, buf, buf);
		}
		obj.raw = strtol(buf, NULL, 16);
		for (int i = 2; i >= 0; i--) {
			printf("%04X: %02X\n", start_addr++, obj.mask[i]);
		}

		fscanf(fp1, "%s", buf);
	}

	fclose(fp1);
	return 0;
}	
