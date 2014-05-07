/*=========================================================================*//**
@file    unistd.h

@author  Daniel Zorychta

@brief   Unix standard library.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief void sleep(const uint seconds)
 * The <b>sleep</b>() makes the calling thread sleep until seconds <i>seconds</i>
 * have elapsed.
 *
 * @param seconds   number of seconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * sleep(2);
 * // code here will be executed after 2s sleep
 * // ...
 */
//==============================================================================
static inline void sleep(const uint seconds)
{
        _sleep(seconds);
}

//==============================================================================
/**
 * @brief void sleep_ms(const uint milliseconds)
 * The <b>sleep_ms</b>() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed.
 *
 * @param milliseconds      number of milliseconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * sleep_ms(10);
 * // code here will be executed after 10ms sleep
 * // ...
 */
//==============================================================================
static inline void sleep_ms(const uint milliseconds)
{
        _sleep_ms(milliseconds);
}

//==============================================================================
/**
 * @brief void usleep(const uint microseconds)
 * The <b>usleep</b>() makes the calling thread sleep until microseconds
 * <i>microseconds</i> have elapsed.<p>
 *
 * Function is not supported by dnx RTOS. Function sleep task at least 1ms.
 *
 * @param microseconds      number of microseconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * usleep(10);
 * // code here will be executed at least after 1ms sleep (shall be 10us)
 * // ...
 */
//==============================================================================
static inline void usleep(const uint microseconds)
{
        (void) microseconds;

        vTaskDelay(1);
}

//==============================================================================
/**
 * @brief int prepare_sleep_until(void)
 * The <b>prepare_sleep_until</b>() function prepare tick counter to call
 * <b>sleep_until_ms</b>() and <b>sleep_until</b>() functions.
 *
 * @param None
 *
 * @errors None
 *
 * @return Current tick counter.
 *
 * @example
 * #include <dnx/os.h>
 * #include <unistd.h>
 *
 * // ...
 * int ref_time = prepare_sleep_until();
 *
 * for (;;) {
 *         // ...
 *
 *         sleep_until_ms(10, &ref_time);
 * }
 * // ...
 */
//==============================================================================
static inline int prepare_sleep_until(void)
{
        return _kernel_get_tick_counter();
}

//==============================================================================
/**
 * @brief void sleep_until_ms(const uint milliseconds, int *ref_time_ticks)
 * The <b>sleep_until_ms</b>() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed. Function produces more precise delay.
 *
 * @param microseconds      number of microseconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 * #include <unistd.h>
 *
 * // ...
 * int ref_time = get_tick_counter();
 *
 * for (;;) {
 *         // ...
 *
 *         sleep_until_ms(10, &ref_time);
 * }
 * // ...
 */
//==============================================================================
static inline void sleep_until_ms(const uint milliseconds, int *ref_time_ticks)
{
        _sleep_until_ms(milliseconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief void sleep_until(const uint seconds, int *ref_time_ticks)
 * The <b>sleep_until</b>() makes the calling thread sleep until seconds
 * <i>seconds</i> have elapsed. Function produces more precise delay.
 *
 * @param seconds       number of seconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 * #include <unistd.h>
 *
 * // ...
 * int ref_time = get_tick_counter();
 *
 * for (;;) {
 *         // ...
 *
 *         sleep_until(1, &ref_time);
 * }
 * // ...
 */
//==============================================================================
static inline void sleep_until(const uint seconds, int *ref_time_ticks)
{
        _sleep_until(seconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief char *getcwd(char *buf, size_t size)
 * The <b>getcwd</b>() function copies an absolute pathname of the current
 * working directory to the array pointed to by <i>buf</i>, which is of length
 * <i>size</i>.
 *
 * @param buf       buffer to store path
 * @param size      buffer length
 *
 * @errors None
 *
 * @return On success, these functions return a pointer to a string containing
 * the pathname of the current working directory. In the case <b>getcwd</b>() is the
 * same value as <i>buf</i>.
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * char *buf[100];
 * getcwd(buf, 100);
 * // ...
 */
//==============================================================================
static inline char *getcwd(char *buf, size_t size)
{
        return strncpy(buf, _task_get_data()->f_cwd, size);
}

//==============================================================================
/**
 * @brief int chown(const char *pathname, int owner, int group)
 * The <b>chown</b>() changes the ownership of the file specified by <i>pathname</i>.<p>
 *
 * This function is not supported by dnx RTOS, because users and groups are
 * not implemented yet.
 *
 * @param pathname      path to file
 * @param owner         owner ID
 * @param group         group ID
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return On success, zero is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * chown("/foo/bar", 1000, 1000);
 * // ...
 */
//==============================================================================
static inline int chown(const char *pathname, int owner, int group)
{
        return vfs_chown(pathname, owner, group);
}

//==============================================================================
/**
 * @brief void sync(void)
 * The <b>sync</b>() synchronize system files with buffers.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <unistd.h>
 *
 * // ...
 * sync();
 * // ...
 */
//==============================================================================
static inline void sync(void)
{
        vfs_sync();
}

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H_ */
/*==============================================================================
  End of file
==============================================================================*/