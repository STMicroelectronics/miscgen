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
  printf("Usage: miscgen <aPrio> <aTry> <aSuccess> <bPrio> <bTry> <bSuccess> [file]\n");
  printf("   xPrio = value between 0 and 15\n");
  printf("   xTry = value between 0 and 7\n");
  printf("   xSuccess = value 0 (false) or 1 (true)\n");
  printf("   [file] = name of the output file (opt), misc.img by default\n");
}

static void write_misc_suffix() {
  int ret = 0;
  int fd;

  fd = open(misc_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

  if(fd < 0) {
    printf("%s, open %s failed, fd %d, errno %d\n", __FUNCTION__, misc_file, fd, errno);
    return;
  }

  lseek(fd, BOOTCTRL_OFFSET_SUFFIX, SEEK_SET);
  ret = write(fd, &bctrl, sizeof(struct bootloader_control));

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

  if(argc < 7) {
    usage();
    return 1;
  }

  if ((atoi(argv[1]) > 15) || (atoi(argv[4]) > 15)
    || (atoi(argv[2]) > 7) || (atoi(argv[5]) > 7)
    || (atoi(argv[3]) > 1) || (atoi(argv[6]) > 1)) {
    usage();
    return 1;
  }

  // update bootctrl based on input parameters
  bctrl.recovery_tries_remaining = 7;
  bctrl.nb_slot = BOOTCTRL_NUM_SLOT;
  bctrl.slot_info[0].priority = atoi(argv[1]);
  bctrl.slot_info[0].tries_remaining = atoi(argv[2]);
  bctrl.slot_info[0].successful_boot = atoi(argv[3]);
  bctrl.slot_info[0].verity_corrupted = 0;
  bctrl.slot_info[1].priority = atoi(argv[4]);
  bctrl.slot_info[1].tries_remaining = atoi(argv[5]);
  bctrl.slot_info[1].successful_boot = atoi(argv[6]);
  bctrl.slot_info[1].verity_corrupted = 0;

  //  merge status (0=None, 1=Unknown, 2=SNAPSHOTTED, 3=MERGING, 4=CANCELLED
  bctrl.merge_status = 0;

  if(argc >= 8)
    misc_file = argv[7];

  printf("output file: %s\n", misc_file);

  bctrl.magic = BOOT_CTRL_MAGIC;
  bctrl.version = BOOT_CTRL_VERSION;

  bctrl.crc32_le = crc32(0, (uint8_t *)&bctrl, sizeof(struct bootloader_control) - sizeof(uint32_t));

  printf("crc: %d\n", bctrl.crc32_le);

  for(i = 0; i < BOOTCTRL_NUM_SLOT; i++) {
    printf("slot %d: prio %d, try %d, success %d\n", i,
      bctrl.slot_info[i].priority,
      bctrl.slot_info[i].tries_remaining,
      bctrl.slot_info[i].successful_boot);
  }

  write_misc_suffix();

  return 0;
}
