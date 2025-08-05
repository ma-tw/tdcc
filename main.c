#include "tdcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("Expected 2 arguments");
        return 1;
    }

    input = argv[1];
    token = tokenize(argv[1]);
    locals = calloc(1, sizeof(LVar));
    locals->len = -1;  // sentinel

    program();

    int locals_count = 0;
    for (LVar *var = locals; var; var = var->next) locals_count++;
    locals_count--;  // sentinel
    
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", 8 * locals_count);
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
