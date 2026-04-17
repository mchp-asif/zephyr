# Copyright 2026 Microchip Technology Inc.
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(fuse_prg.c)


zephyr_linker_sources(SECTIONS userpage.ld)
