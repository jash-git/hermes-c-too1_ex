#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: No text provided.\n");
        printf("{\"error\": \"No text provided\"}\n");
        return 1;
    }

    const char *text = argv[1];
    long chars = 0;
    long words = 0;
    long lines = 0;
    int in_word = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        chars++;
        if (text[i] == '\n') {
            lines++;
        }
        if (isspace((unsigned char)text[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    // 如果有字元但沒有換行符號，至少算作 1 行
    if (chars > 0 && lines == 0) {
        lines = 1;
    }

    // 以 JSON 格式輸出結果
    printf("{\n");
    printf("  \"chars\": %ld,\n", chars);
    printf("  \"words\": %ld,\n", words);\n    printf("  \"lines\": %ld\n", lines);\n    printf("}\n");

    return 0;
}