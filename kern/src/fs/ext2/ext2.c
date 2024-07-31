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
#include "ext2.h"

#include "../../kernel.h"

#define SBLOCK_PATTERN "LLLLLLLLLLLLLWWWWWWLLLLWWLWWLLLS16S16S64LBBWS16LLL"

bool ext2_fsopen(ext2fs *fs, char *data)
{
    // Read the superblock. The superblock is always at 1K.
    char *sblock = &data[1024];
    readble(sblock, SBLOCK_PATTERN,
        &fs->sblock.numinodes,
        &fs->sblock.numblocks,
        &fs->sblock.resblocks,
        &fs->sblock.freeblocks,
        &fs->sblock.freeinodes,
        &fs->sblock.sblockno,
        &fs->sblock.blksizesh,
        &fs->sblock.fragsizesh,
        &fs->sblock.grpblocks,
        &fs->sblock.grpfrags,
        &fs->sblock.grpinodes,
        &fs->sblock.mounttime,
        &fs->sblock.writtentime,
        &fs->sblock.mounts,
        &fs->sblock.fsckmounts,
        &fs->sblock.signature,
        &fs->sblock.fsstate,
        &fs->sblock.errpolicy,
        &fs->sblock.minorver,
        &fs->sblock.fscktime,
        &fs->sblock.fsckinterv,
        &fs->sblock.os,
        &fs->sblock.majorver,
        &fs->sblock.resuid,
        &fs->sblock.resgid,
        &fs->sblock.firstino,
        &fs->sblock.inosize,
        &fs->sblock.blkgrp,
        &fs->sblock.optfeatures,
        &fs->sblock.reqfeatures,
        &fs->sblock.rwreqfeatures,
        &fs->sblock.fsid,
        &fs->sblock.label,
        &fs->sblock.lastmount,
        &fs->sblock.compression,
        &fs->sblock.allocfiles,
        &fs->sblock.allocdirs,
        &fs->sblock._unused,
        &fs->sblock.journalid,
        &fs->sblock.journalino,
        &fs->sblock.journaldev,
        &fs->sblock.orphanino);

    // Check the signature.

    printf("Opened ext2fs with %d blocks, %d inodes (%d, %d free)\n",
            fs->sblock.numblocks, fs->sblock.numinodes, fs->sblock.freeblocks, fs->sblock.freeinodes);

    return true;
}

bool ext2_readinode(ext2fs *fs, ext2_inode *buf, u32 ino)
{
    /*if (ino > fs->numinodes) {
        return false;
    }

    // Inodes are stored in block groups.
    u32 group = (ino - 1) / fs->grpinodes;
    u32 inoingrp = (ino - 1) % fs->grpinodes;
    
    // First, find the Block Group Descriptor Table. The BGDT starts at the
    // block following the superblock.
    u32 bgdtno = (2047 / fs->blksize) + 1;
    char *bgdt = &fs->data[bgdtno * fs->blksize];

    // Find the correct entry and the starting block of the inode table.
    char *bgdtentry = &bgdt[group * 32];
    u32 inotableno = readle32(&bgdtentry[8]);

    // FIXME: Tell if inode is not allocated

    // Then index for the inode itself.
    char *inotable = &fs->data[inotableno * fs->blksize];
    char *inode = &inotable[inoingrp * fs->inosize];

    buf->mode      = readle16(&inode[0]);
    buf->uid       = readle16(&inode[2]);
    buf->size      = (u64)readle32(&inode[108]) << 32 | readle32(&inode[4]);
    buf->atime     = readle32(&inode[8]);
    buf->ctime     = readle32(&inode[12]);
    buf->mtime     = readle32(&inode[16]);
    buf->deltime   = readle32(&inode[20]);
    buf->gid       = readle16(&inode[24]);
    buf->numlinks  = readle16(&inode[26]);
    buf->sectors   = readle32(&inode[28]);
    buf->flags     = readle32(&inode[32]);
    for (int i = 0; i <= 11; i++) {
        buf->blocks[i] = readle32(&inode[36 + i * 4]);
    }
    buf->blockptr  = readle32(&inode[88]);
    buf->blockdptr = readle32(&inode[92]);
    buf->blocktptr = readle32(&inode[96]);

    printf("Successfully read the inode with\n"
            "Mode: %4o; Uid: %u; Gid: %u\n"
            "Size: %u bytes\n"
            "Accessed: %u; Created: %u; Modified: %u; Deleted: %u\n"
            "Hard links: %u; Disk sectors: %u\n"
            "First block: %u\n",
            buf->mode, buf->uid, buf->gid, buf->size, buf->atime, buf->ctime,
            buf->mtime, buf->deltime, buf->numlinks, buf->sectors,
            buf->blocks[0]);*/

    return true;
}
