#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,
    TK_NUM, // 整数
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

extern LVar *locals;  // 先頭

extern Token *token;

typedef enum {
    ND_ASSIGN,
    ND_LVAR,
    ND_RETURN,
    ND_IF,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs, *rhs, *third, *fourth;  // third, fourth: else, for
    int val;    // kind が ND_NUM の場合のみ
    int offset; // kind が ND_LVAR の場合のみ
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool consume(char *op);
void expect(char *op);
int expect_number();

bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

bool is_alnum_us(char p);

Token *tokenize(char *p);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *sum();
Node *term();
Node *unary();
Node *factor();

extern int jump_count;

void dfs(Node *node);

extern char *input;
extern Node *code[];