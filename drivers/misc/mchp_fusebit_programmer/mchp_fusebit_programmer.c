/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/sys/util.h>

__attribute__((section(".userpage")))
const unsigned char user_nvm_data[256] = {0xDE, 0xAD, 0xBE, 0xEF};
