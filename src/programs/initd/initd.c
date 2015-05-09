/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   Initialization daemon

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dnx/misc.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <unistd.h>

#include "stm32f1/gpio_cfg.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        const char *str;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Program main function
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
int_main(initd, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        int result = 0;

        if (argc == 2 && strcmp(argv[1], "--child") == 0) {

                process_stat_t stat;
                if (process_stat(getpid(), &stat) == 0) {
                        printf("Name: %s\n", stat.name);
                        printf("PID: %d\n", stat.pid);
                        printf("Files: %d\n", stat.files_count);
                        printf("Dirs: %d\n", stat.dir_count);
                        printf("Mutexes: %d\n", stat.mutexes_count);
                        printf("Semaphores: %d\n", stat.semaphores_count);
                        printf("Queues: %d\n", stat.queue_count);
                        printf("Threads: %d\n", stat.threads_count);
                        printf("Mem blocks: %d\n", stat.memory_block_count);
                        printf("Memory usage: %d\n", stat.memory_usage);
                        printf("CPU load cnt: %d\n", stat.cpu_load_cnt);
                        printf("Stack size: %d\n", stat.stack_size);
                        printf("Stack max usage: %d\n", stat.stack_max_usage);
                } else {
                        perror(NULL);
                }


                sleep(2);
                puts("Hello! I'm child of initd parent!");

                printf("I have PID: %d\n", getpid());

                global->str = "Works!";

                int i = 0;
                while (true) {
//                        GPIO_SET_PIN(PB14);

                        printf("=== Sec: %d\n", i++);

//                        disable_CPU_load_measurement();
                        process_stat_t stat;
                        if (process_stat(getpid(), &stat) == 0) {
                                printf("Name: %s\n", stat.name);
                                printf("PID: %d\n", stat.pid);
                                printf("Files: %d\n", stat.files_count);
                                printf("Dirs: %d\n", stat.dir_count);
                                printf("Mutexes: %d\n", stat.mutexes_count);
                                printf("Semaphores: %d\n", stat.semaphores_count);
                                printf("Queues: %d\n", stat.queue_count);
                                printf("Threads: %d\n", stat.threads_count);
                                printf("Mem blocks: %d\n", stat.memory_block_count);
                                printf("Memory usage: %d\n", stat.memory_usage);
                                printf("Stack size: %d\n", stat.stack_size);
                                printf("Stack max usage: %d\n", stat.stack_max_usage);
                                printf("CPU load cnt: %d\n", stat.cpu_load_cnt);

                                u32_t tct = stat.cpu_load_total_cnt;
                                printf("CPU total cnt: %d\n", tct);

                                printf("CPU load: %d.%02d%%\n",
                                       stat.cpu_load_cnt * 100 / tct,
                                       (stat.cpu_load_cnt * 1000 / tct) % 10
                                       );
                        } else {
                                perror(NULL);
                        }
                        enable_CPU_load_measurement();

//                        for (int i = 0; i < 1000000; i++) {
//                                __asm("nop");
//                        }

//                        char *blk1 = malloc(16);
//                        if (blk1) {
//                                memset(blk1, '1', 16);
//
//                                printf("blk1: %p\n", blk1);
//
//                                char *blk2 = malloc(16);
//                                if (blk2) {
//
//                                        memset(blk2, '2', 16);
//
//                                        printf("blk2: %p\n", blk2);
//
//                                        puts("BLK OK");
//                                        free(blk2);
//                                }
//
//                                free(blk1);
//                        }

//                        GPIO_CLEAR_PIN(PB14);
                        sleep(1);
                }

        } else {
                result = mount("lfs", "", "/");
                result = mkdir("/dev", 0777);
                result = driver_init("gpio", "/dev/gpio");
                result = driver_init("afiom", NULL);
//                result = driver_init("pll", NULL);
                result = driver_init("uart2", "/dev/ttyS0");
                result = driver_init("tty0", "/dev/tty0");

                result = syslog_enable("/dev/tty0");

                detect_kernel_panic(true);

                result = driver_init("tty1", "/dev/tty1");

                stdout = fopen("/dev/tty0", "w");

                printf("Hello world! I'm using syscalls!\n");


                puts("Starting child...");

                const process_attr_t attr = {
                       .cwd       = "/",
                       .p_stdin   = "/dev/tty1",
                       .p_stdout  = "/dev/tty1",
                       .p_stderr  = "/dev/tty1",
                       .no_parent = true
                };

                pid_t pid = process_create("initd --child", &attr);
                printf("Child PID: %d\n", pid);

                printf("Result: %d\n", result);
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
