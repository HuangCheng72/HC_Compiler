//
// Created by huangcheng on 2024/9/27.
//

// 现在只有一个词法分析器的组件
// 语言标准是C89

#include <stdio.h>
#include "lexer/lexer.h"

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

    // 调用词法分析器解析源代码
    // 并打印所有解析到的 Token
    print_tokens(tokenize(source_code));

    // 释放文件内容所占内存
    free(source_code);

    return 0;
}
