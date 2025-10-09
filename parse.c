#include "tdcc.h"

char *input;
Token *token;

LVar *locals;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - input;
    fprintf(stderr, "%s\n", input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || strncmp(token->str, op, token->len) != 0) {
        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT) return NULL;
    Token *ret = token;
    token = token->next;
    return ret;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || strncmp(token->str, op, token->len) != 0) {
        error_at(token->str, "Not %s", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Node *code[100];

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *ret = calloc(1, sizeof(Token));
    ret->kind = kind;
    ret->str = str;
    ret->len = len;
    cur->next = ret;
    return ret;
}

bool is_alnum_us(char p) {
    return isalnum(p) || p == '_';
}

bool matches(char *p, char *keyword) {
    int kwlen = strlen(keyword);
    return strlen(p) >= kwlen && strncmp(p, keyword, kwlen) == 0;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p != '\0') {
        if (isspace(*p)) {
            p++;
        } else if (matches(p, "==") || matches(p, "!=") || matches(p, "<=") || matches(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
        } else if (strchr("<>+-*/()=;", *p) != NULL) {  // 1文字記号
            cur = new_token(TK_RESERVED, cur, p++, 1);
        } else if (matches(p, "return") && !is_alnum_us(p[6])) {
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
        } else if (matches(p, "if") && !is_alnum_us(p[2])) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
        } else if (matches(p, "else") && !is_alnum_us(p[4])) {
            cur = new_token(TK_RESERVED, cur, p, 4);
            p += 4;
        } else if (matches(p, "while") && !is_alnum_us(p[5])) {
            cur = new_token(TK_RESERVED, cur, p, 5);
            p += 5;
        } else if (matches(p, "do") && !is_alnum_us(p[2])) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
        } else if (matches(p, "for") && !is_alnum_us(p[3])) {
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
        } else if (isalpha(*p) || *p == '_') {
            char *start = p;
            while (is_alnum_us(*p)) p++;
            cur = new_token(TK_IDENT, cur, start, p - start);
        } else if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, -1);
            cur->val = strtol(p, &p, 10);
        } else {
            error_at(p, "Unable to tokenize");
        }
    }

    new_token(TK_EOF, cur, p, -1);
    return head.next;
}

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && strncmp(tok->str, var->name, var->len) == 0) {
            return var;
        }
    }
    return NULL;
}

Node *new_node(NodeKind kind, int child_count, ...) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->child_count = child_count;
    if (child_count > 0) {
        node->children = calloc(child_count, sizeof(Node *));
        va_list ap;
        va_start(ap, child_count);
        for (int i = 0; i < child_count; i++) {
            node->children[i] = va_arg(ap, Node *);
        }
        va_end(ap);
    }
    return node;
}

Node *new_node_num(int val) {
    Node *node = new_node(ND_NUM, 0);
    node->val = val;
    return node;
}

void program() {
    int pos = 0;
    while (!at_eof()) code[pos++] = stmt();
    code[pos] = NULL;
}

Node *stmt() {
    Node *node;
    if (consume("if")) {
        expect("(");
        Node *cond = expr();
        expect(")");
        Node *true_stmt = stmt();
        Node *false_stmt = NULL;
        if (consume("else")) {
            false_stmt = stmt();
        }
        node = new_node(ND_IF, 3, cond, true_stmt, false_stmt);
    } else if (consume("while")) {
        expect("(");
        Node *cond = expr();
        expect(")");
        Node *loop_stmt = stmt();
        node = new_node(ND_WHILE, 2, cond, loop_stmt);
    } else if (consume("do")) {
        Node *loop_stmt = stmt();
        expect("while");
        expect("(");
        Node *cond = expr();
        expect(")");
        expect(";");
        node = new_node(ND_DO_WHILE, 2, loop_stmt, cond);
    } else if (consume("for")) {
        expect("(");
        Node *init = NULL, *cond = NULL, *update = NULL;
        if (!consume(";")) {
            init = expr();
            expect(";");
        }
        if (!consume(";")) {
            cond = expr();
            expect(";");
        }
        if (!consume(")")) {
            update = expr();
            expect(")");
        }
        Node *loop_stmt = stmt();
        node = new_node(ND_FOR, 4, init, cond, update, loop_stmt);
    } else if (consume("return")) {
        node = new_node(ND_RETURN, 2, expr(), NULL);
        expect(";");
    } else {
        node = expr();
        expect(";");
    }
    return node;
}

Node *expr() {
    Node *node = assign();
    return node;
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) node = new_node(ND_ASSIGN, 2, node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, 2, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NE, 2, node, relational());
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = sum();

    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, 2, node, sum());
        } else if (consume("<=")) {
            node = new_node(ND_LE, 2, node, sum());
        } else if (consume(">")) {
            node = new_node(ND_LT, 2, sum(), node);
        } else if (consume(">=")) {
            node = new_node(ND_LE, 2, sum(), node);
        } else {
            return node;
        }
    }
}

Node *sum() {
    Node *node = term();

    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, 2, node, term());
        } else if (consume("-")) {
            node = new_node(ND_SUB, 2, node, term());
        } else {
            return node;
        }
    }
}

Node *term() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, 2, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, 2, node, unary());
        } else {
            return node;
        }
    }
}

Node *unary() {
    if (consume("+")) {
        return factor();
    }
    if (consume("-")) {
        return new_node(ND_SUB, 2, new_node_num(0), factor());
    }
    return factor();
}

Node *factor() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            // lvar を先頭にして2番目を今までの locals に
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}
