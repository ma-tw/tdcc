#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, // 記号
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

extern Token *token;

typedef enum {
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
    Node *lhs, *rhs;
    int val;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool consume(char *op);
void expect(char *op);
int expect_number();

bool at_eof();

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

Token *tokenize(char *p);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();
Node *equality();
Node *relational();
Node *sum();
Node *term();
Node *unary();
Node *factor();

void dfs(Node *node);

extern char *input;
