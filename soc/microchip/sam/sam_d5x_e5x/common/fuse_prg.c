/*
 * Copyright 2026 Microchip Technology Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/sys/util.h>
#include <soc.h>

__attribute__((section("USER_ROW")))const fuses_user_fuses_registers_t user_nvm_data = {0,0xdeadbeef,0xdadadada};
//  = {0, 0, 0, 0, 0xff, 0xff, 0x24, 0x95}
// const unsigned char user_nvm_data[256] = {0, 0, 0, 0, 0x11, 0x00, 0xdc, 0x1d};
// const unsigned char user_nvm_data[256] = {0x0, 0x0, 0x0, 0x0, 0xDE, 0xAD, 0xBE, 0xEF};
