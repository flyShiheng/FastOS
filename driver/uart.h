#pragma once
#include "./port.h"
#include "./stdarg.h"


#define SERIAL_PORT_BASE 0x3F8
#define SERIAL_DATA_REG_OFFSET 0
#define SERIAL_LINE_CTRL_REG_OFFSET 3

#define ZEROPAD 1  /* pad with zero 0扩展 */
#define SIGN 2     /* unsigned/signed long */
#define PLUS 4     /* show plus 正负号 */
#define SPACE 8    /* space if plus 在整数前显示一个空格 */
#define LEFT 16    /* left justified 左对齐 */
#define SPECIAL 32 /* 0x 16进制数 */
#define SMALL 64   /* use 'abcdef' instead of 'ABCDEF' 使用小写字母 */

// 判断一个字符是不是数值字母
#define is_digit(c) ((c) >= '0' && (c) <= '9')

void uart_init();

void printk_char(char c);

void printk(const char *fmt, ... );

int vsprintf(char *buf, const char *fmt, va_list args);
