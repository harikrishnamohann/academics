#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define VAULT_FILE "./.secrets"
#define BUF_SIZE 256

void usage(const char *prog_name) {
    printf("Usage: %s [--encode | --decode]\n", prog_name);
}

void decode(int key) {
    FILE *fp = fopen(VAULT_FILE, "r");
    if (fp == NULL) {
        printf("vault not found\n");
        return;
    }

    char buf[BUF_SIZE];
    size_t bytes_read = fread(buf, sizeof(char), sizeof(buf), fp);
    for (int i = 0; i < bytes_read; i++) {
        buf[i] ^= key;
    }
    printf("%s\n", buf);
    fclose(fp);
}

void encode(char* secret, int size, int key) {
    FILE *fp = fopen(VAULT_FILE, "w");
    assert(fp != NULL);

    for (int i = 0; i < size; i++) {
        secret[i] ^= key;
    }
    fwrite(secret, sizeof(char), size, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    int key;
    if (strcmp(argv[1], "--encode") == 0) {
      printf("what's your secret message?\n > ");
      char buf[BUF_SIZE];
      fgets(buf, sizeof(buf), stdin);
      printf("authentication key please\n > ");
      scanf("%d", &key);
      encode(buf, strlen(buf) - 1, key);
    } else if (strcmp(argv[1], "--decode") == 0) {
      printf("Feeling hungry?\n > ");
      scanf("%d", &key);
      decode(key);
    } else {
        usage(argv[0]);
        return 1;
    }

    return 0;
}
