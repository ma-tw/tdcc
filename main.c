#include "tdcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("Expected 2 arguments");
        return 1;
    }

    input = argv[1];
    token = tokenize(argv[1]);

    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");  // 26 * 8
    for (int i = 0; code[i]; i++) {
        dfs(code[i]);
        printf("  pop rax\n");
    }

    // 最後の式の結果がRAXにある
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}
