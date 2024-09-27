//
// Created by huangcheng on 2024/9/27.
//

// 现在只有一个词法分析器的组件
// 语言标准是C89
// 目标是所有合法的C89代码都能被该组件解析为完整 Token 流
// 目前用链表表示流（流是一个概念型的数据结构，用什么实现都行）

#include <stdio.h>
#include "list.h"
#include <string.h>
#include <stdarg.h>

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
    TOKEN_COMMENT,     // 注释
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

// C89关键字表
const char* keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while"
};

// Token 结构体
typedef struct token_struct {
    TokenType type;     // token单元的类型
    char value[256];    // token单元的值
    long line;          // 该token所在行
    long column;        // 该token起始列
    LIST_NODE node;     // 双向链表结点
} Token;

// Token链表的头结点
LIST_HEAD(token_list_head);
// 源代码字符串指针
const char *source_code_ptr;
// 当前读取位置（从0开始）
long current_index;
// 当前行号
long current_line;
// 当前列号
long current_column;

// 需要实现的函数有三个功能部分：
// 字符处理
// Token处理
// 词法分析（识别各种类型的token的过程）

// 字符处理

void init_lexer(const char *source_code);
char current_char();
void next_char();
char peek();

// token处理

Token* create_token(TokenType type, const char *value, long line, long column);
void append_token(Token *new_token);
void print_tokens();

// 词法分析

Token* tokenize(const char *source_code);
Token* lex_identifier_or_keyword();
Token* lex_number();
Token* lex_operator();
Token* lex_string();
Token* lex_char();
Token* lex_comment();
Token* lex_preprocessor();
Token* lex_symbol();

// 除此之外还有一些抽出复用的辅助函数：

int is_keyword(const char *str);
int is_letter(char c);
int is_digit(char c);
void skip_whitespace();
void skip_comment();

int main() {
    // 初始化全局变量
    init_list_node(&token_list_head);
    source_code_ptr = NULL;
    current_index = 0;
    current_line = 0;
    current_column = 0;

    return 0;
}

/**
 * 初始化词法分析器，设置输入的源代码
 * @param source_code 指向源代码字符串的指针
 */
void init_lexer(const char *source_code) {

}

/**
 * 返回当前处理的字符
 * @return 当前字符
 */
char current_char() {
    return '\0';
}

/**
 * 移动到下一个字符
 */
void next_char() {

}

/**
 * 查看下一个字符，但不移动到下一个字符
 * @return 下一个字符
 */
char peek() {
    return '\0';
}

// token处理

/**
 * 创建一个新的 Token 并初始化其类型和值
 * @param type Token 的类型
 * @param value Token 的值
 * @param line Token 所在行号
 * @param column Token 起始列号
 * @return 返回指向新创建 Token 的指针
 */
Token* create_token(TokenType type, const char *value, long line, long column) {
    return NULL;
}

/**
 * 将一个新的 Token 添加到链表中
 * @param new_token 指向要添加的 Token 的指针
 */
void append_token(Token *new_token) {

}

/**
 * 打印链表中的所有 Token
 */
void print_tokens() {

}

/**
 * 将源代码解析为 Token 流
 * @param source_code 指向源代码字符串的指针
 * @return 返回指向 Token 链表头的指针
 */
Token* tokenize(const char *source_code) {
    return NULL;
}

/**
 * 解析标识符或关键字
 * @return 返回解析到的 Token
 */
Token* lex_identifier_or_keyword() {
    return NULL;
}

/**
 * 解析数字常量（整数或浮点数）
 * @return 返回解析到的 Token
 */
Token* lex_number() {
    return NULL;
}

/**
 * 解析操作符
 * @return 返回解析到的 Token
 */
Token* lex_operator() {
    return NULL;
}

/**
 * 解析字符串常量
 * @return 返回解析到的 Token
 */
Token* lex_string() {
    return NULL;
}

/**
 * 解析字符常量
 * @return 返回解析到的 Token
 */
Token* lex_char() {
    return NULL;
}

/**
 * 解析注释
 * @return 返回解析到的 Token
 */
Token* lex_comment() {
    return NULL;
}

/**
 * 解析预处理指令
 * @return 返回解析到的 Token
 */
Token* lex_preprocessor() {
    return NULL;
}

/**
 * 解析符号（如括号、逗号等）
 * @return 返回解析到的 Token
 */
Token* lex_symbol() {
    return NULL;
}

/**
 * 判断字符串是否是关键字
 * @param str 要检查的字符串
 * @return 是关键字返回1，否则返回0
 */
int is_keyword(const char *str) {
    return 0;
}

/**
 * 判断字符是否是字母
 * @param c 要检查的字符
 * @return 是字母返回1，否则返回0
 */
int is_letter(char c) {
    return 0;
}

/**
 * 判断字符是否是数字
 * @param c 要检查的字符
 * @return 是数字返回1，否则返回0
 */
int is_digit(char c) {
    return 0;
}

/**
 * 跳过空白字符（空格、制表符、换行符等）
 */
void skip_whitespace() {

}

/**
 * 跳过注释
 */
void skip_comment() {

}
