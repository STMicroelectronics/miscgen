/*
 * Copyright 2015 The Android Open Source Project
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 * Copyright (C) 2019 STMicroelectronics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <zlib.h>

// Added to help integrating c++11 header file
#define constexpr static
typedef struct misc_virtual_ab_message misc_virtual_ab_message;
#include "bootloader.h"

#include "private/boot_control_definition.h"

// struct boot_ctrl occupies the slot_suffix field of struct bootloader_message
#define BOOTCTRL_OFFSET_SUFFIX offsetof(struct bootloader_message_ab, slot_suffix)
#define BOOTCTRL_NUM_SLOT 2

static struct bootloader_control bctrl;
static char *misc_file = "misc.img";

static void usage() {
  printf("Usage: miscread [file]\n");
  printf("   [file] = name of the input file (opt), misc.img by default\n");
}

static void read_misc_suffix() {
  int ret = 0;
  int fd;

  fd = open(misc_file, O_RDONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

  if(fd < 0) {
    printf("%s, open %s failed, fd %d, errno %d\n", __FUNCTION__, misc_file, fd, errno);
    return;
  }

  lseek(fd, BOOTCTRL_OFFSET_SUFFIX, SEEK_SET);
  ret = read(fd, &bctrl, sizeof(struct bootloader_control));

  if(ret != sizeof(struct bootloader_control)) {
    close(fd);
    printf("%s, write failed, expect %u, actual %d\n", __FUNCTION__, (unsigned int)sizeof(struct bootloader_control), ret);
    return;
  }

  fsync(fd);
  close(fd);

  return;
}

int main(int argc, char *argv[])
{
  int i;

  if(argc < 1) {
    usage();
    return 1;
  }

  if(argc >= 1)
    misc_file = argv[1];

  read_misc_suffix();

  printf("magic: %d\n", bctrl.magic);
  printf("version: %d\n", bctrl.version);
  printf("crc32_le: %d\n", bctrl.crc32_le);

  printf("recovery_tries_remaining: %d\n", bctrl.recovery_tries_remaining);
  printf("merge_status: %d\n", bctrl.merge_status);
  printf("nb_slot: %d\n", bctrl.nb_slot);
  for (i=0; i<bctrl.nb_slot; i++) {
    printf("slot %d priority: %d\n", i, bctrl.slot_info[i].priority);
    printf("slot %d tries_remaining: %d\n", i, bctrl.slot_info[i].tries_remaining);
    printf("slot %d successful_boot: %d\n", i, bctrl.slot_info[i].successful_boot);
    printf("slot %d verity_corrupted: %d\n", i, bctrl.slot_info[i].verity_corrupted);
  }

  return 0;
}
