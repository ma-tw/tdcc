#include "tdcc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("Expect lval");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void dfs(Node *node) {
    switch (node->kind) {
        case ND_NUM:
            printf("# ND_NUM\n");
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            printf("# ND_LVAR\n");
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            printf("# ND_ASSIGN\n");
            gen_lval(node->lhs);
            dfs(node->rhs);

            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        default:
            ;
    }

    dfs(node->lhs);
    dfs(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_EQ:
            printf("# ND_EQ\n");
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("# ND_NE\n");
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("# ND_LT\n");
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("# ND_LE\n");
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_ADD:
            printf("# ND_ADD\n");
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("# ND_SUB\n");
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("# ND_MUL\n");
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("# ND_DIV\n");
            printf("  cqo\n");  // rax を拡張
            printf("  idiv rdi\n");
            break;
        default:
            error("Unreachable");
            break;
    }

    printf("  push rax\n");
}
