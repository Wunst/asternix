/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/pc/pc.h
 * Legacy PC chipset
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#ifndef PC_H
#define PC_H

#include "../../../kernel.h"

void pic_init(u8 offset);

void pit_init(void);

#endif
