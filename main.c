#include "tdcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("Expected 2 arguments");
        return 1;
    }

    input = argv[1];
    token = tokenize(argv[1]);

    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    dfs(node);

    // スタックの先頭に最終的な答え
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
