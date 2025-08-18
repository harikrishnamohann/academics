#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include "user.h"

#define VAULT_FILE "./.secrets1"
#define BUF_SIZE 256

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
    printf("Usage: %s [--encode | --decode | --commit]\n", prog_name);
}

void* go_brrr(void* trigger) {
    srand(time(NULL) ^ (uintptr_t)pthread_self());
    uint8_t var = 0;
    while (*(uint8_t*)trigger) {
      if (rand() % 10 < 7) {
        printf("\033[92m%d\033[0m", var);
      } else {
        char *accent_colors[] = {"\033[91m", "\033[96m", "\033[97m",
                                 "\033[33m"};
        int accent_count = 4;
        printf("%s%d\033[0m", accent_colors[rand() % accent_count], var);
      }
      fflush(stdout);
      var = !var;
    }
    return NULL;
}

void handle_termination(int x) {
    printf("Life only moves forward. There is no return at this point. Sorry\n");
}

void drama() {
    signal(SIGINT, handle_termination);
    printf("\nAuto self destruct in T-");
    fflush(stdout);
    for (int second = 10; second > 0; second--) {
      printf("%d ", second);
      fflush(stdout);
      sleep(1.0);
    }
    uint8_t confuse = 1;
    pthread_t splash_thread;
    pthread_create(&splash_thread, NULL, go_brrr, &confuse);
    sleep(1);
    confuse = 0;
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
    secret.str[length] = '\0';
    
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

void read_line(string* buf, char* msg) {
    printf("%s", msg);
    fgets(buf->str, BUF_SIZE, stdin);
    buf->len = strlen(buf->str) - 1;
    buf->str[buf->len] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    string buf = str_init(sizeof(char) * BUF_SIZE * 2);
    string buf_half = { buf.str + BUF_SIZE, BUF_SIZE };

    if (strcmp(argv[1], "--encode") == 0) {
      read_line(&buf, "what's your secret message?\n > ");
      read_line(&buf_half, "Enter pass phrase\n > ");
      encode(buf, buf_half);
    } else if (strcmp(argv[1], "--decode") == 0) {
      read_line(&buf, "pass phrase please...\n > ");
      decode(buf);
      printf("\nsecret: %s\n", buf_half.str);
      drama();
    } else if (strcmp(argv[1], "--commit") == 0) {
        read_line(&buf, "If this is your last moment, what do you have say? : ");        
        snprintf(buf_half.str, BUF_SIZE, "git add . && git commit -m \"%s\"", buf.str);
        system(buf_half.str);
        read_line(&buf, "What's the secret word? : ");
        decode(buf);
        snprintf(buf.str, BUF_SIZE, "git push https://%s:%s@github.com/%s/%s.git", UNAME, buf_half.str, UNAME, REPO);
        system(buf.str);
    } else {
        usage(argv[0]);
        return 1;
    }

    for (int i = 0; i < BUF_SIZE * 2; i++) buf.str[i] = 0;

    str_free(&buf);
    return 0;
}
