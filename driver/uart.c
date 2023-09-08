#include "uart.h"

void uart_init() {
    // 设置波特率为9600，8位数据位，1位停止位，无奇偶校验
    port_byte_out(SERIAL_PORT_BASE + SERIAL_LINE_CTRL_REG_OFFSET, 0x80);  // 写入线控制寄存器设置允许访问DLL和DLH寄存器
    port_byte_out(SERIAL_PORT_BASE + SERIAL_DATA_REG_OFFSET, 0x03);       // 写入DLL值(低8位) 115200/9600 = 0x03
    port_byte_out(SERIAL_PORT_BASE + SERIAL_DATA_REG_OFFSET, 0x00);       // 写入DLH值(高8位)

    // 配置数据位、停止位和奇偶校验位
    port_byte_out(SERIAL_PORT_BASE + SERIAL_LINE_CTRL_REG_OFFSET, 0x03);  // 设置8位数据位，1位停止位，无奇偶校验
    port_byte_out(SERIAL_PORT_BASE + 2, 0xC7);                            // 禁用FIFO，使其成为单字节
    port_byte_out(SERIAL_PORT_BASE + 4, 0x0B);                            // 允许接收和发送
}

void printk_char(char c) {
    // while((port_byte_in(SERIAL_PORT_BASE + 5) & 0x20) == 0);  // 等待串口准备好
    port_byte_out(SERIAL_PORT_BASE + SERIAL_DATA_REG_OFFSET, c);
}

void printk(const char *fmt, ... ) {
    char buf[1024] = {0}; 
    int i = 0;
    va_list args;
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    for (int i = 0; buf[i] != '\0'; i++) {
        printk_char(buf[i]);
    }
}

// n /= base，返回余数部分 __res
#define do_div(n, base)                                                        \
  ({                                                                           \
    int __res;                                                                 \
    __asm__("divq %%rcx" : "=a"(n), "=d"(__res) : "0"(n), "1"(0), "c"(base));  \
    __res;                                                                     \
  })

/**
 * @brief 计算字符串长度
 *
 * @param String
 * @return int
 */
static inline int strlen(char *String) {
  register int __res;
  __asm__ __volatile__("cld	\n\t"
                       "repne	\n\t"
                       "scasb	\n\t"
                       "notl	%0	\n\t"
                       "decl	%0	\n\t"
                       : "=c"(__res)
                       : "D"(String), "a"(0), "0"(0xffffffff)
                       :);
  return __res;
}

/**
 * @brief
 * 将数值字母组成的字符串转换为整数值。连续读取字符串，直到读取到不是数值字母为止
 *
 * @param s
 * @return int 返回转换后的整数
 */
static int skip_atoi(const char **s) {
  int i = 0;

  while (is_digit(**s))
    i = 10 * i + *((*s)++) - '0';

  return i;
}

/**
 * @brief 将变量 num 转换为 base 进制数，输出每一位的字符到 str 中
 *
 * @param str
 * @param num 待转换变量
 * @param base 进制
 * @param size
 * @param precision
 * @param type
 * @return char*
 */
static char *number(char *str, long num, int base, int size, int precision,
                    int type) {
  char c, sign, tmp[50];
  const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int i;

  if (type & SMALL) // 输出时使用小写字母
    digits = "0123456789abcdefghijklmnopqrstuvwxyz";
  if (type & LEFT) // 若需要右对齐，则不能使用 0 填充不足 size 的部分
    type &= ~ZEROPAD;
  if (base < 2 || base > 36) // 只支持输出 2~36 进制数
    return 0;

  c = (type & ZEROPAD) ? '0' : ' '; // 初始化填充用的字符
  sign = 0;
  if (type & SIGN && num < 0) { // 对于有符号的负数，要显示负号
    sign = '-';
    num = -num;
  } else // 否则符号由 PLUS 和 SPACE 决定
    sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);

  if (sign)
    size--; // 负号占用一个宽度
  if (type & SPECIAL)
    if (base == 16) // 16 进制数 0x 占用两个宽度
      size -= 2;
    else if (base == 8) // 16 进制数 前导 0 占用一个宽度
      size--;

  i = 0;
  if (num == 0)
    tmp[i++] = '0';
  else // 将数值转换为字符串，数值的每一位在 tmp 中由低位到高位存储
    while (num != 0)
      tmp[i++] = digits[do_div(num, base)];

  if (i > precision)
    precision = i;
  size -= precision; // 去除数值部分，还需要输出多少字符
  if (!(type & (ZEROPAD + LEFT)))
    while (size-- > 0)
      *str++ = ' ';

  if (sign) // 输出符号
    *str++ = sign;

  if (type & SPECIAL) // 输出进制标志
    if (base == 8)
      *str++ = '0';
    else if (base == 16) {
      *str++ = '0';
      *str++ = digits[33]; // x,X
    }

  if (!(type & LEFT)) // 右对齐，在左边不足宽度处补字符 c
    while (size-- > 0)
      *str++ = c;

  while (i < precision--) // 输出精度要求的前导 0
    *str++ = '0';
  while (i-- > 0) // 倒叙输出字符串，即从高位到低位输出
    *str++ = tmp[i];
  while (size-- > 0) // 左对齐，在右边不足宽度处补空格
    *str++ = ' ';

  return str;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
  char *str, *s;
  int flags;
  // field_width 指整体输出长度，precision 指数值部分的输出长度
  int field_width; // 要输出的字符的最小数目。如果输出的值短于该数，结果会用空格填充。如果输出的值长于该数，结果不会被截断
  int precision; //  要输出的数字的最小位数。如果写入的值短于该数，结果会用前导零来填充。如果写入的值长于该数，结果不会被截断。精度为
                 //  0 意味着不写入任何字符。
  int len, i;

  int qualifier; // "%" 后面格式字符前的格式修饰符，修饰整数格式

  // 遍历 fmt 字符串，处理所有 "%" 指定的格式化输出，输出内容到全局 buf 缓冲
  for (str = buf; *fmt; fmt++) {
    if (*fmt != '%') { // 当前字符不是 "%"，直接输出到缓冲区
      *str++ = *fmt;
      continue;
    }

    // % : flags : width : precision : qualifier : (fmt)
    // 1. 收集格式符的标志位
    flags = 0;
  repeat:
    fmt++;
    switch (*fmt) {
    case '-':
      flags |= LEFT; // 在 width 内左对齐，默认是右对齐
      goto repeat;
    case '+':
      flags |= PLUS; // 强制在结果之前显示加号或减号（+ 或 -）
      goto repeat;
    case ' ':
      flags |= SPACE; // 如果没有写入任何符号，则在该值前面插入一个空格
      goto repeat;
    case '#':
      flags |= SPECIAL; // 与 o、x 或 X 说明符一起使用时，非零值前面会分别显示
                        // 0、0x 或 0X
      goto repeat;
    case '0':
      flags |= ZEROPAD; // 不足 width 的部分使用 0 填充，默认是空格
      goto repeat;
    }

    // 2. 获取输出字符宽度
    field_width = -1;
    if (is_digit(*fmt)) // 宽度在 format 字符串中指定，直接读出数字
      field_width = skip_atoi(&fmt);
    else if (*fmt == '*') { // 宽度在 format 字符串中未指定，则由扩展参数给出
      fmt++;
      field_width = va_arg(args, int);
      if (field_width < 0) { // 扩展参数为负数，将负号看做 flags
        field_width = -field_width;
        flags |= LEFT;
      }
    }

    // 3. 获取输出字符的精度
    precision = -1;
    if (*fmt == '.') {
      fmt++;
      if (is_digit(*fmt)) {
        precision = skip_atoi(&fmt);
      } else if (*fmt == '*') {
        fmt++;
        precision = va_arg(args, int);
      }

      if (precision < 0)
        precision = 0;
    }

    // 4. 处理格式修饰符，支持 h,l,L,Z 修饰整数格式
    qualifier = -1;
    if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z') {
      qualifier = *fmt;
      fmt++;
    }

    // 5. 根据前面获取的信息和格式符进行输出
    // 格式符支持 c,s,o,p,x,X,d,i,u,n,%
    switch (*fmt) {
    case 'c':              // 输出一个字符
      if (!(flags & LEFT)) // 右对齐，在左边不足宽度处补空格
        while (--field_width > 0)
          *str++ = ' ';
      *str++ = (unsigned char)va_arg(args, int);
      while (--field_width > 0) // 左对齐，在右边不足宽度处补空格
        *str++ = ' ';
      break;

    case 's': // 输出一个字符串
      s = va_arg(args, char *);
      if (!s)
        s = '\0';
      len = strlen(s);
      if (precision < 0)
        precision = len;
      else if (len > precision)
        len = precision; // 精度限制了字符串的输出长度

      if (!(flags & LEFT)) // 右对齐，在左边不足宽度处补空格
        while (len < field_width--)
          *str++ = ' ';
      for (i = 0; i < len; i++) // 输出指定长度的字符串
        *str++ = *s++;
      while (len < field_width--) // 左对齐，在右边不足宽度处补空格
        *str++ = ' ';
      break;

    case 'o': // 按 8 进制输出无符号整数
      if (qualifier == 'l')
        str = number(str, va_arg(args, unsigned long), 8, field_width,
                     precision, flags);
      else
        str = number(str, va_arg(args, unsigned int), 8, field_width, precision,
                     flags);
      break;

    case 'p': // 输出指针地址
      if (field_width == -1) {
        field_width = 2 * sizeof(void *);
        flags |= ZEROPAD;
      }

      str = number(str, (unsigned long)va_arg(args, void *), 16, field_width,
                   precision, flags);
      break;

    case 'x': // 按 16 进制小写输出无符号整数
      flags |= SMALL;
    case 'X': // 按 16 进制大写输出无符号整数
      if (qualifier == 'l')
        str = number(str, va_arg(args, unsigned long), 16, field_width,
                     precision, flags);
      else
        str = number(str, va_arg(args, unsigned int), 16, field_width,
                     precision, flags);
      break;

    case 'd':
    case 'i': // d 和 i 都表示输出有符号 10 进制数
      flags |= SIGN;
    case 'u': // 输出无符号 10 进制数
      if (qualifier == 'l')
        str = number(str, va_arg(args, unsigned long), 10, field_width,
                     precision, flags);
      else
        str = number(str, va_arg(args, unsigned int), 10, field_width,
                     precision, flags);
      break;

    case 'n': // 将当前已格式化的字符串长度赋值给参数
      if (qualifier == 'l') {
        long *ip = va_arg(args, long *);
        *ip = (str - buf);
      } else {
        int *ip = va_arg(args, int *);
        *ip = (str - buf);
      }
      break;

    case '%': // %% 输出一个 %
      *str++ = '%';
      break;

    default: // % 之后跟着不被支持的格式符，直接当做字符串输出
      *str++ = '%';
      if (*fmt)
        *str++ = *fmt;
      else
        fmt--;
      break;
    }
  }
  *str = '\0';
  return str - buf; // 返回格式化后的字符串长度
}

