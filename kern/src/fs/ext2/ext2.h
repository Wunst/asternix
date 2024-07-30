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

// The ext2 superblock as stored on disk.
typedef struct {
    u32 numinodes;
    u32 numblocks;

    u32 resblocks;  // Number of blocks reserved for superuser.

    u32 freeblocks;
    u32 freeinodes;

    u32 sblockno;   // Block number of the block containing the superblock.

    u32 blksizesh;  // block size = 1024 << blksizesh
    u32 fragsizesh; // fragment size = 1024 << fragsizesh

    u32 grpblocks;  // Number of blocks per block group.
    u32 grpinodes;  // Number of inodes per block group.

    u32 mounttime;
    u32 writtentime;

    u16 mounts;     // Mounts since last fsck.
    u16 fsckmounts; // Mounts allowed betweens fscks.

    u16 signature;

    u16 fsstate;

    u16 errpolicy;  // What to do if an error is detected.

    u16 minorver;

    u32 fscktime;   // Time of last fsck.
    u32 fsckinterv; // Interval allowed between fscks.

    u32 os;

    u32 majorver;

    u16 resuid;     // UID that can use reserved blocks.
    u16 resgid;     // GID that can use reserved blocks.

    // Extended superblock fields (majorver >= 1).
    u32 firstino;   // First non-reserved inode.
    u16 inosize;

    u16 blkgrp;     // Block this superblock is part of (if backup copy).

    u32 optfeatures;
    // FIXME: Handle required features.
    u32 reqfeatures;
    u32 rwreqfeatures;

    char fsid[16];
    char label[16];

    char lastmount[64];

    u32 compression;

    u8 allocfiles;
    u8 allocdirs;

    u16 _unused;

    char journalid[16];
    u32 journalino;
    u32 journaldev;
    u32 orphanino;
} ext2_sblock;

typedef struct {
    char *data;         // Backing "device", only RAM disk for now.
    ext2_sblock sblock;
} ext2fs;

// An ext2 inode as stored on disk.
typedef struct {
    u16 mode;       // Inode type and permissions.
    u16 uid;

    u32 size_lo;

    u32 atime;
    u32 ctime;
    u32 mtime;
    u32 deltime;

    u16 gid;

    u16 numlinks;   // Number of hard links to this inode.

    u32 sectors;

    // TODO: Handle flags.
    u32 flags;

    u32 oss1;       // OS Specific Value #1 (unused).

    union {
        u32 blocks[12];     // Direct block pointers.
        char symlink[48];
        // TODO: Device special files.
    };

    u32 blockptr;   // Singly indirect blocks (ptr to ptrs).
    u32 blockdptr;  // Doubly indirect blocks (ptr to ptrs to ptrs).
    u32 blocktptr;  // Triply indirect blocks (ptr to ptrs to ptrs to ptrs).

    u32 generation; // Generation number.

    u32 fileacl;
    
    union {
        u32 size_hi;
        u32 diracl;
    };

    u8 oss2[12];    // OS Specific Value #2.
} ext2_inode;

bool ext2_fsopen(ext2fs *fs, char *data);
bool ext2_readinode(ext2fs *fs, ext2_inode *buf, u32 ino);

#endif
