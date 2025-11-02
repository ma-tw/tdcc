#include "tdcc.h"

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR) {
        error("Expect lval");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

int jump_count;

void dfs(Node *node) {
    int begin_label, end_label, else_label;
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
            gen_lval(node->children[0]);
            dfs(node->children[1]);

            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_RETURN:
            printf("# ND_RETURN\n");
            dfs(node->children[0]);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_IF:
            printf("# ND_IF\n");
            dfs(node->children[0]);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            if (node->children[2]) {
                else_label = jump_count++;
                printf("  je .Lelse%d\n", else_label);
                dfs(node->children[1]);
                end_label = jump_count++;
                printf("  jmp .Lend%d\n", end_label);
                printf(".Lelse%d:\n", else_label);
                dfs(node->children[2]);
                printf(".Lend%d:\n", end_label);
            } else {
                end_label = jump_count++;
                printf("  je .Lend%d\n", end_label);
                dfs(node->children[1]);
                printf(".Lend%d:\n", end_label);
            }
            return;
        case ND_WHILE:
            printf("# ND_WHILE\n");
            begin_label = jump_count++;
            printf(".Lbegin%d:\n", begin_label);
            dfs(node->children[0]);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            end_label = jump_count++;
            printf("  je .Lend%d\n", end_label);
            dfs(node->children[1]);
            printf("  jmp .Lbegin%d\n", begin_label);
            printf(".Lend%d:\n", end_label);
            return;
        case ND_DO_WHILE:
            printf("# ND_DO_WHILE\n");
            begin_label = jump_count++;
            printf(".Lbegin%d:\n", begin_label);
            dfs(node->children[0]);
            dfs(node->children[1]);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  jne .Lbegin%d\n", begin_label);
            return;
        case ND_FOR:
            printf("# ND_FOR\n");
            if (node->children[0]) {
                dfs(node->children[0]);
            }
            begin_label = jump_count++;
            printf(".Lbegin%d:\n", begin_label);
            if (node->children[1]) {
                dfs(node->children[1]);
                printf("  pop rax\n");
                printf("  cmp rax, 0\n");
            }
            end_label = jump_count++;
            printf("  je .Lend%d\n", end_label);
            dfs(node->children[3]);
            if (node->children[2]) {
                dfs(node->children[2]);
            }
            printf("  jmp .Lbegin%d\n", begin_label);
            printf(".Lend%d:\n", end_label);
            return;
        case ND_BLOCK:
            printf("# ND_BLOCK\n");
            for (int i = 0; i < node->child_count; i++) {
                dfs(node->children[i]);
                printf("  pop rax\n");
            }
            return;
        default:
            ;
    }

    dfs(node->children[0]);
    dfs(node->children[1]);

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
