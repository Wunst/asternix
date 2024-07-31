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
#define INODE_PATTERN "WWLLLLLWWLLLLLLLLLLLLLLLLLLLLLLS12"

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
    if (fs->sblock.signature != 0xef53) {
        printf("Error: ext2: signature did not match (was: %4x)\n",
                fs->sblock.signature);
        return false;
    }

    // Check version.
    if (fs->sblock.majorver != 1) {
        printf("Error: ext2: major version %u unsupported (use 1)\n",
                fs->sblock.majorver);
        return false;
    }

    // Block size is 1024 shifted left by stored value. Panic if too large.
    // FIXME: what is too large? (-> multiplied)
    if (fs->sblock.blksizesh >= 22) {
        printf("Error: ext2: Block size too large for 32 bit (1024 << %u)\n",
                fs->sblock.blksizesh);
        return false;
    }
    fs->blksize = 1024 << fs->sblock.blksizesh;

    fs->data = data;

    printf("ext2: Opened filesystem '%s' with %u blocks, %u inodes"
            "(%u, %u free), block size %u\n", fs->sblock.label,
            fs->sblock.numblocks, fs->sblock.numinodes, fs->sblock.freeblocks,
            fs->sblock.freeinodes, fs->blksize);

    return true;
}

bool ext2_readinode(ext2fs *fs, ext2_inode *buf, u32 ino)
{
    if (ino > fs->sblock.numinodes) {
        return false;
    }

    // Inodes are stored in block groups.
    u32 group = (ino - 1) / fs->sblock.grpinodes;
    u32 inoingrp = (ino - 1) % fs->sblock.grpinodes;
    
    // First, find the Block Group Descriptor Table. The BGDT starts at the
    // block following the superblock.
    u32 bgdtno = (2047 / fs->blksize) + 1;
    char *bgdt = &fs->data[bgdtno * fs->blksize];

    // Find the correct entry and the starting block of the inode table.
    char *bgdtentry = &bgdt[group * 32];
    u32 inotableno;
    readble(&bgdtentry[8], "L", &inotableno);

    // FIXME: Tell if inode is not allocated

    // Then index for the inode itself.
    char *inotable = &fs->data[inotableno * fs->blksize];
    char *inode = &inotable[inoingrp * fs->sblock.inosize];

    // FIXME: Read symlink, device, diracl differently from blocks (endianness)
    readble(inode, INODE_PATTERN,
            &buf->mode,
            &buf->uid,
            &buf->size_lo,
            &buf->atime,
            &buf->ctime,
            &buf->mtime,
            &buf->deltime,
            &buf->gid,
            &buf->numlinks,
            &buf->sectors,
            &buf->flags,
            &buf->oss1,
            &buf->blocks[0],
            &buf->blocks[1],
            &buf->blocks[2],
            &buf->blocks[3],
            &buf->blocks[4],
            &buf->blocks[5],
            &buf->blocks[6],
            &buf->blocks[7],
            &buf->blocks[8],
            &buf->blocks[9],
            &buf->blocks[10],
            &buf->blocks[11],
            &buf->blockptr,
            &buf->blockdptr,
            &buf->blocktptr,
            &buf->generation,
            &buf->fileacl,
            &buf->size_hi,
            &buf->fragment,
            &buf->oss2);

    printf("Successfully read the inode with\n"
            "Mode: %4o; Uid: %u; Gid: %u\n"
            "Lower half of size: %u\n"
            "Accessed: %u; Created: %u; Modified: %u; Deleted: %u\n"
            "Hard links: %u; Disk sectors: %u\n"
            "First block: %u\n",
            buf->mode, buf->uid, buf->gid, buf->size_lo, buf->atime,
            buf->ctime, buf->mtime, buf->deltime, buf->numlinks, buf->sectors,
            buf->blocks[0]);

    return true;
}
