#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>

#define VAULT_FILE "./.secrets1"
#define BUF_SIZE 256
#define UNAME "harikrishnamohann"
#define REPO "academics"

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

bool is_hacking;
void* freak_out(void* arg) {
    srand(time(NULL) ^ (uintptr_t)pthread_self());
    uint8_t var = 0;
    while (is_hacking) {
        if (rand() % 10 < 7) {
            printf("\033[92m%d\033[0m", var);
        } else {
            char* accent_colors[] = {"\033[91m", "\033[96m", "\033[97m", "\033[33m"};
            int accent_count = 4;
            printf("%s%d\033[0m", accent_colors[rand() % accent_count], var);
        }
        fflush(stdout);
        var = !var;
    }
    return NULL;
}

void panic_and_erase() {
    is_hacking = true;
    pthread_t splash_thread;
    pthread_create(&splash_thread, NULL, freak_out, NULL);
    sleep(1);
    is_hacking = false;
    pthread_join(splash_thread, NULL);
    system("clear");
}

void decode(string key) {
    FILE *vault = fopen(VAULT_FILE, "r");
    assert(vault != NULL);

    fseek(vault, 0, SEEK_END);
    uint32_t length = ftell(vault);
    rewind(vault);
    string secret = { .str = key.str + BUF_SIZE, .len = length };
    for (int i = 0; i < length; i++) {
        secret.str[i] = (fgetc(vault) ^ key.str[i % key.len]);
    }
    
    fclose(vault);
}

void encode(string secret, string key) {
    FILE *fp = fopen(VAULT_FILE, "w");
    assert(fp != NULL);

    for (int i = 0; i < secret.len; i++) {
        fputc(secret.str[i] ^ key.str[i % key.len], fp);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    string buf = str_init(sizeof(char) * BUF_SIZE * 2);
    string buf_half = { buf.str + BUF_SIZE, BUF_SIZE };
    if (strcmp(argv[1], "--encode") == 0) {
      printf("what's your secret message?\n > ");
      fgets(buf.str, buf.len / 2, stdin);

      printf("Enter pass phrase\n > ");
      fgets(buf_half.str, buf_half.len, stdin);

      string secret = { buf.str, strlen(buf.str) - 1 };
      string key = { buf_half.str, strlen(buf_half.str) - 1 };
      encode(secret, key);
    } else if (strcmp(argv[1], "--decode") == 0) {
      printf("pass phrase please...\n > ");
      fgets(buf.str, buf.len, stdin);
      buf.len = strlen(buf.str) - 1;
      decode(buf);
      printf("%s\nAuto self destruct in... ", buf_half.str);
      fflush(stdout);
      for (int second = 10; second > 0; second--) {
          printf("%d ", second);
          fflush(stdout);
          sleep(1.0);
      }
      panic_and_erase();
    } else if (strcmp(argv[1], "--commit") == 0) {
        printf("If this is your last moment, what would you say? : ");        
        fgets(buf.str, buf.len, stdin);
        buf.len = strlen(buf.str);
        buf.str[buf.len - 1] = '\0';
        snprintf(buf_half.str, BUF_SIZE, "git add . && git commit -m \"%s\"", buf.str);
        system(buf_half.str);
        printf("What's the secret word? : ");
        fgets(buf.str, buf.len, stdin);
        buf.len = strlen(buf.str) - 1;
        decode(buf); // buf_half contains the key
        snprintf(buf.str, BUF_SIZE, "git push https://%s:%s@github.com/%s/%s.git", UNAME, buf_half.str, UNAME, REPO);
        system(buf.str);
    } else {
        usage(argv[0]);
        return 1;
    }

    str_free(&buf);
    return 0;
}
