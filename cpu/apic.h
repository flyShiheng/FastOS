
/*
    APIC 包括两个部分：Local APIC 和 I/O APIC。
    Local APIC 每个 CPU 内核都有一个，它负责处理该 CPU 的中断请求。
    I/O APIC 则只有一个，负责将 I/O 设备的中断请求发送给 Local APIC
*/

#pragma once
#include <stdint.h>
#include "../driver/uart.h"

#define APIC_ID      0x020
#define APIC_VERSION 0x030

#define APIC_ICR_LOW  0x300
#define APIC_ICR_HIGH 0x310

void get_apic_base();

uint32_t get_apic_id();

uint32_t get_apic_version();

