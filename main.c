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
Token* lex_preprocessor();
Token* lex_symbol();

// 除此之外还有一些抽出复用的辅助函数：

int is_keyword(const char *str);
int is_letter(char c);
int is_digit(char c);
void skip_whitespace();
void skip_comment();

int main(int argc, char *argv[]) {
    // 检查是否提供了文件路径
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file_path>\n", argv[0]);
        return 1;
    }

    // 打开指定的C语言源代码文件
    const char *file_path = argv[1];
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", file_path);
        return 1;
    }

    // 获取文件大小并分配内存
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source_code = (char *)malloc(file_size + 1);
    if (!source_code) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    // 读取文件内容
    fread(source_code, 1, file_size, file);
    source_code[file_size] = '\0';  // 确保以null终止

    fclose(file);  // 关闭文件

    // 初始化全局变量
    init_list_node(&token_list_head);
    source_code_ptr = NULL;
    current_index = 0;
    current_line = 0;
    current_column = 0;

    // 调用词法分析器解析源代码
    tokenize(source_code);

    // 打印所有解析到的 Token
    print_tokens();

    // 释放文件内容所占内存
    free(source_code);

    return 0;
}

/**
 * 初始化词法分析器，设置输入的源代码
 * @param source_code 指向源代码字符串的指针
 */
void init_lexer(const char *source_code) {
    source_code_ptr = source_code;  // 设置源代码的指针
    current_index = 0;              // 初始化为第一个字符
}

/**
 * 返回当前处理的字符
 * @return 当前字符
 */
char current_char() {
    return source_code_ptr[current_index];  // 返回当前字符
}

/**
 * 移动到下一个字符
 */
void next_char() {
    if (current_char() == '\n') {
        // 遇到换行符，行号递增，列号重置为1
        current_line++;
        current_column = 1;
    } else {
        // 非换行字符，列号递增
        current_column++;
    }
    current_index++;  // 无论如何，递增索引，指向下一个字符
}

/**
 * 查看下一个字符，但不移动到下一个字符
 * @return 下一个字符
 */
char peek() {
    return source_code_ptr[current_index + 1];  // 返回下一个字符
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
    // 为新 Token 分配内存
    Token *new_token = (Token*)malloc(sizeof(Token));

    // 检查内存分配是否成功
    if (new_token == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for new token\n");
        return new_token;
    }

    // 先用memset初始化全部为0，这样就不用担心了
    memset(new_token, 0, sizeof(*new_token));

    // 设置 Token 类型
    new_token->type = type;

    // 拷贝 Token 值
    strncpy(new_token->value, value, sizeof(new_token->value) - 1);
    // 之前已经memset为0，能够确保字符串结尾为\0，无需再次确保

    // 设置 Token 行和列信息
    new_token->line = line;
    new_token->column = column;

    // 初始化 Token 中的链表节点
    init_list_node(&new_token->node);

    // 返回新创建的 Token
    return new_token;
}

/**
 * 将一个新的 Token 添加到链表中
 * @param new_token 指向要添加的 Token 的指针
 */
void append_token(Token *new_token) {
    list_add_tail(&new_token->node, &token_list_head);  // 将新 Token 插入链表末尾
}

/**
 * 打印链表中的所有 Token
 */
void print_tokens() {
    LIST_NODE *pos;
    list_for_each(pos, &token_list_head) {
        Token *token = list_entry(pos, Token, node);  // 获取 Token 的首地址
        printf("Token: Type=%d, Value=%s, Line=%ld, Column=%ld\n",
               token->type, token->value, token->line, token->column);  // 打印 Token 信息
    }
}

/**
 * 将源代码解析为 Token 流
 * @param source_code 指向源代码字符串的指针
 * @return 返回指向 Token 链表头的指针
 */
Token* tokenize(const char *source_code) {
    // 初始化词法分析器
    init_lexer(source_code);
    // 更新行号为第一行
    current_line = 1;
    // 更新列号为第一列
    current_column = 1;

    // 循环遍历源代码的每一个字符，直到文件结束
    while (current_char() != '\0') {
        skip_whitespace();  // 跳过空白字符

        // 当前字符的类型判断
        if (is_letter(current_char())) {
            // 解析标识符或关键字
            Token* token = lex_identifier_or_keyword();
            append_token(token);
        } else if (is_digit(current_char())) {
            // 解析数字
            Token* token = lex_number();
            append_token(token);
        } else if (current_char() == '/' && (peek() == '/' || peek() == '*')) {
            // 不再解析注释，直接跳过
            skip_comment();
        } else if (current_char() == '#') {
            // 解析预处理指令
            Token* token = lex_preprocessor();
            append_token(token);
        } else if (current_char() == '"' || current_char() == '\'') {
            // 解析字符串或字符常量
            if (current_char() == '"') {
                Token* token = lex_string();
                append_token(token);
            } else {
                Token* token = lex_char();
                append_token(token);
            }
        } else if (strchr("+-*/=!><&|?:", current_char())) {
            // 解析运算符
            Token* token = lex_operator();
            append_token(token);
        } else if (strchr("(){}[];,.", current_char())) {
            // 解析符号
            Token* token = lex_symbol();
            append_token(token);
        } else {
            // 未知字符，忽略或报错处理
            fprintf(stderr, "Warning: Unrecognized character '%c' at line %ld, column %ld\n",
                    current_char(), current_line, current_column);
            next_char();  // 跳过这个字符，继续处理
        }
    }

    // 解析结束后，添加文件结束标记
    Token* eof_token = create_token(TOKEN_EOF, "EOF", current_line, current_column);
    append_token(eof_token);

    // 返回链表头结点
    return list_entry(token_list_head.next, Token, node);
}

/**
 * 解析标识符或关键字
 * @return 返回解析到的 Token
 */
Token* lex_identifier_or_keyword() {
    char buffer[256];
    int length = 0;

    // 读取标识符的字符，直到遇到非字母、数字或下划线的字符
    while (is_letter(current_char()) || is_digit(current_char())) {
        buffer[length++] = current_char();
        next_char();
    }
    buffer[length] = '\0';  // 终止符

    // 判断是否是关键字
    TokenType type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;

    // 创建Token
    return create_token(type, buffer, current_line, current_column - length);
}

/**
 * 解析数字常量（整数或浮点数）
 * @return 返回解析到的 Token
 */
Token* lex_number() {
    char buffer[256];
    int length = 0;
    int is_float = 0;

    // 处理数字（整数或浮点数）
    while (is_digit(current_char()) || current_char() == '.') {
        if (current_char() == '.') {
            is_float = 1;
        }
        buffer[length++] = current_char();
        next_char();
    }

    // 检查科学记数法
    if (current_char() == 'e' || current_char() == 'E') {
        buffer[length++] = current_char();
        next_char();
        if (current_char() == '+' || current_char() == '-') {
            buffer[length++] = current_char();
            next_char();
        }
        while (is_digit(current_char())) {
            buffer[length++] = current_char();
            next_char();
        }
        is_float = 1;
    }

    buffer[length] = '\0';  // 终止符

    TokenType type = is_float ? TOKEN_FLOAT : TOKEN_INT;
    return create_token(type, buffer, current_line, current_column - length);
}

/**
 * 解析操作符
 * @return 返回解析到的 Token
 */
Token* lex_operator() {
    char buffer[3];  // 运算符长度通常不会超过2个字符
    int length = 0;

    buffer[length++] = current_char();
    next_char();

    // 检查双字符运算符
    if ((buffer[0] == '+' && current_char() == '+') ||
        (buffer[0] == '-' && current_char() == '-') ||
        (buffer[0] == '=' && current_char() == '=') ||
        (buffer[0] == '!' && current_char() == '=') ||
        (buffer[0] == '&' && current_char() == '&') ||
        (buffer[0] == '|' && current_char() == '|')) {
        buffer[length++] = current_char();
        next_char();
    }
    buffer[length] = '\0';

    // 支持问号 ? 和冒号 : 作为运算符
    if (buffer[0] == '?' || buffer[0] == ':') {
        return create_token(TOKEN_OPERATOR, buffer, current_line, current_column - length);
    }

    return create_token(TOKEN_OPERATOR, buffer, current_line, current_column - length);
}

/**
 * 解析字符串常量
 * @return 返回解析到的 Token
 */
Token* lex_string() {
    char buffer[256];
    int length = 0;

    next_char();  // 跳过开头的双引号

    while (current_char() != '"' && current_char() != '\0') {
        if (current_char() == '\\') {  // 处理转义字符
            buffer[length++] = current_char();
            next_char();
        }
        buffer[length++] = current_char();
        next_char();
    }

    next_char();  // 跳过结尾的双引号
    buffer[length] = '\0';

    return create_token(TOKEN_STRING, buffer, current_line, current_column - length);
}

/**
 * 解析字符常量
 * @return 返回解析到的 Token
 */
Token* lex_char() {
    char buffer[3];
    int length = 0;

    next_char();  // 跳过开头的单引号

    if (current_char() == '\\') {  // 处理转义字符
        buffer[length++] = current_char();
        next_char();
    }
    buffer[length++] = current_char();
    next_char();

    next_char();  // 跳过结尾的单引号
    buffer[length] = '\0';

    return create_token(TOKEN_CHAR, buffer, current_line, current_column - length);
}

/**
 * 解析预处理指令
 * @return 返回解析到的 Token
 */
Token* lex_preprocessor() {
    char buffer[256];
    int length = 0;

    while (current_char() != '\n' && current_char() != '\0') {
        buffer[length++] = current_char();
        next_char();
    }

    buffer[length] = '\0';
    return create_token(TOKEN_PREPROCESSOR, buffer, current_line, current_column - length);
}

/**
 * 解析符号（如括号、逗号等）
 * @return 返回解析到的 Token
 */
Token* lex_symbol() {
    char buffer[2];
    buffer[0] = current_char();  // 读取当前字符
    buffer[1] = '\0';

    TokenType type;
    switch (buffer[0]) {
        case '(': type = TOKEN_LPAREN; break;
        case ')': type = TOKEN_RPAREN; break;
        case '{': type = TOKEN_LBRACE; break;
        case '}': type = TOKEN_RBRACE; break;
        case '[': type = TOKEN_LBRACKET; break;
        case ']': type = TOKEN_RBRACKET; break;
        case ';': type = TOKEN_SEMICOLON; break;  // 分号解析
        case ',': type = TOKEN_COMMA; break;
        case '.': type = TOKEN_PERIOD; break;
        default: return NULL;
    }

    next_char();  // 移动到下一个字符
    return create_token(type, buffer, current_line, current_column - 1);
}

/**
 * 判断字符串是否是关键字
 * @param str 要检查的字符串
 * @return 是关键字返回1，否则返回0
 */
int is_keyword(const char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;  // 如果匹配，返回1
        }
    }
    return 0;  // 否则返回0
}

/**
 * 判断字符是否是字母
 * @param c 要检查的字符
 * @return 是字母返回1，否则返回0
 */
int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';  // C语言标识符允许下划线
}

/**
 * 判断字符是否是数字
 * @param c 要检查的字符
 * @return 是数字返回1，否则返回0
 */
int is_digit(char c) {
    return c >= '0' && c <= '9';  // 判断是否是数字
}

/**
 * 跳过空白字符（空格、制表符、换行符等）
 */
void skip_whitespace() {
    while (current_char() == ' ' || current_char() == '\t' || current_char() == '\n' || current_char() == '\r') {
        next_char();  // 跳过当前空白字符
    }
}

/**
 * 跳过注释
 */
void skip_comment() {
    if (current_char() == '/' && peek() == '/') {  // 行注释
        next_char();  // 跳过 '/'
        next_char();  // 跳过第二个 '/'

        while (current_char() != '\n' && current_char() != '\0') {  // 一直到行尾或者文件结束
            next_char();
        }
        next_char();  // 跳过换行符
    }
    else if (current_char() == '/' && peek() == '*') {  // 块注释
        next_char();  // 跳过 '/'
        next_char();  // 跳过 '*'

        while (current_char() != '\0') {
            if (current_char() == '*' && peek() == '/') {  // 检查块注释结束符号 '*/'
                next_char();  // 跳过 '*'
                next_char();  // 跳过 '/'
                break;
            }
            next_char();  // 否则继续跳过注释内的字符
        }
    }
}
