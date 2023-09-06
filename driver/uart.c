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

void printk(const char* msg) {
    for (int i = 0; msg[i] != '\0'; i++) {
        printk_char(msg[i]);
    }
}
