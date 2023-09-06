#pragma once
#include "./port.h"

#define SERIAL_PORT_BASE 0x3F8
#define SERIAL_DATA_REG_OFFSET 0
#define SERIAL_LINE_CTRL_REG_OFFSET 3

void uart_init();

void printk_char(char c);

void printk(const char* msg);
