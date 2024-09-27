//
// Created by Administrator on 2024/9/28.
//

#ifndef HC_COMPILER_LEXER_H
#define HC_COMPILER_LEXER_H

// 词法分析器的目标是所有合法的C89代码都能被该组件解析为完整 Token 流
// 目前用链表表示流（流是一个概念型的数据结构，用什么实现都行）

#include <stdio.h>
#include "../list.h"
#include <string.h>

// 定义 Token 类型
typedef enum {
    TOKEN_KEYWORD,     // 关键字
    TOKEN_IDENTIFIER,  // 标识符
    TOKEN_INT,         // 整数常量
    TOKEN_FLOAT,       // 浮点常量
    TOKEN_OPERATOR,    // 操作符
    TOKEN_STRING,      // 字符串常量
    TOKEN_CHAR,        // 字符常量
    TOKEN_PREPROCESSOR, // 预处理指令
    TOKEN_LPAREN,      // '('
    TOKEN_RPAREN,      // ')'
    TOKEN_LBRACE,      // '{'
    TOKEN_RBRACE,      // '}'
    TOKEN_LBRACKET,    // '['
    TOKEN_RBRACKET,    // ']'
    TOKEN_SEMICOLON,   // ';'
    TOKEN_COMMA,       // ','
    TOKEN_PERIOD,      // '.'
    TOKEN_EOF          // 文件结束
} TokenType;

// Token 结构体
typedef struct token_struct {
    TokenType type;     // token单元的类型
    char value[256];    // token单元的值
    long line;          // 该token所在行
    long column;        // 该token起始列
    LIST_NODE node;     // 双向链表结点
} Token;

/**
 * 打印链表中的所有 Token
 * @param token_list_head 指向要打印的 Token 链表头结点的指针
 */
void print_tokens(LIST_NODE *token_list_head);

/**
 * 将源代码解析为 Token 流
 * @param source_code 指向源代码字符串的指针
 * @return 返回指向 Token 链表头结点的指针
 */
LIST_NODE *tokenize(const char *source_code);

#endif //HC_COMPILER_LEXER_H
