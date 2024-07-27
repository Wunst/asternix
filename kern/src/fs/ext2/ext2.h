/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * fs/ext2/ext2.h
 * Second Extended Filesystem (ext2)
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#ifndef EXT2_H
#define EXT2_H

#include <stddef.h>

#include "../../kernel.h"

typedef struct {
    char *data;         // Backing "device", only RAM disk for now.
    u32 numinodes;
    u32 numblocks;
    u32 freeblocks;
    u32 freeinodes;
    u32 blksize;
    u32 inosize;
    u32 grpblocks;      // Number of blocks per block group.
    u32 grpinodes;      // Number of inodes per block group.
} ext2fs;

typedef struct {
    u64 size;
    u16 mode;
    u16 uid;
    u32 atime;
    u32 ctime;
    u32 mtime;
    u32 deltime;
    u16 gid;
    u16 numlinks;
    u32 sectors;
    u32 flags;
    u32 blocks[12]; // Direct block pointers.
    u32 blockptr;   // Singly indirect blocks (ptr to ptrs).
    u32 blockdptr;  // Doubly indirect blocks (ptr to ptrs to ptrs).
    u32 blocktptr;  // Triply indirect blocks (ptr to ptrs to ptrs to ptrs).
} ext2_inode;

bool ext2_fsopen(ext2fs *fs, char *data);
bool ext2_readinode(ext2fs *fs, ext2_inode *buf, u32 ino);

#endif
