#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define VAULT_FILE "./.secrets1"
#define BUF_SIZE 512

typedef struct {
    char* str;
    size_t len;
} string;

string str_init(size_t size) {
    return (string) {
        .len = size,
        .str = malloc(sizeof(char) * size)
    };
}

void str_free(string* str) {
    str->len = 0;
    free(str->str);
    str->str = NULL;
}

void usage(const char *prog_name) {
    printf("Usage: %s [--encode | --decode]\n", prog_name);
}

void decode(string key) {
    FILE *vault = fopen(VAULT_FILE, "r");
    if (vault == NULL) {
        printf("vault not found\n");
        return;
    }

    string secret = { key.str + (key.len / 2), key.len / 2 };
    key.len = strlen(key.str) - 1;
    size_t bytes_read = fread(secret.str, sizeof(char), secret.len, vault);
    for (int i = 0; i < bytes_read; i++) {
        putchar(secret.str[i] ^ key.str[i % key.len]);
    }
    putchar('\n');
    fclose(vault);
}

void encode(string secret, string key) {
    FILE *fp = fopen(VAULT_FILE, "w");
    assert(fp != NULL);

    for (int i = 0; i < secret.len; i++) {
        secret.str[i] ^= key.str[i % key.len]; 
    }
    fwrite(secret.str, sizeof(char), secret.len, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    string buf = str_init(sizeof(char) * BUF_SIZE);
    if (strcmp(argv[1], "--encode") == 0) {
      printf("what's your secret message?\n > ");
      fgets(buf.str, buf.len / 2, stdin);

      printf("Enter pass phrase\n > ");
      fgets(buf.str + (buf.len / 2), buf.len / 2, stdin);

      string secret = { buf.str, strlen(buf.str) - 1 };
      string key = { buf.str + (buf.len / 2), strlen(buf.str + (buf.len / 2)) - 1 };
      encode(secret, key);
    } else if (strcmp(argv[1], "--decode") == 0) {
      printf("pass phrase please...\n > ");
      fgets(buf.str, buf.len, stdin);
      decode(buf);
    } else {
        usage(argv[0]);
        return 1;
    }

    str_free(&buf);
    return 0;
}
