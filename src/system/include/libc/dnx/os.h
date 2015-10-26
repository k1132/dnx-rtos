/*=========================================================================*//**
@file    os.h

@author  Daniel Zorychta

@brief   dnx system main header

@note    Copyright (C) 2012, 2013  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _OS_H_
#define _OS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <config.h>
#include <stdbool.h>
#include <kernel/syscall.h>
#include <kernel/khooks.h>
#include <mm/mm.h>
#include <drivers/drvctrl.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef _mm_mem_usage_t memstat_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  External object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief u32_t get_free_memory(void)
 * The function <b>get_free_memory</b>() return free memory in bytes. This is
 * the total amount of memory which can be used.
 *
 * @param None
 *
 * @errors None
 *
 * @return Free memory in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Free memory: %d bytes\n", get_free_memory());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_free_memory(void)
{
        return _builtinfunc(mm_get_mem_free);
}

//==============================================================================
/**
 * @brief u32_t get_used_memory(void)
 * The function <b>get_used_memory</b>() return used memory in bytes. This value
 * is a sum of static and dynamic allocated memory.
 *
 * @param None
 *
 * @errors None
 *
 * @return Used memory in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Used memory: %d bytes\n", get_used_memory());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_used_memory(void)
{
        return _builtinfunc(mm_get_mem_usage);
}

//==============================================================================
/**
 * @brief u32_t get_memory_size(void)
 * The function <b>get_memory_size</b>() return total memory size in bytes.
 *
 * @param None
 *
 * @errors None
 *
 * @return Total memory size in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Memory size: %d bytes\n", get_memory_size());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_memory_size(void)
{
        return _builtinfunc(mm_get_mem_size);
}

//==============================================================================
/**
 * @brief stdret_t get_memory_usage_details(memstat_t *stat)
 * The function <b>get_memory_usage_details</b>() return detailed memory usage
 * pointed by <i>stat</i>. <b>memstat_t</b> structure:
 * <pre>
 * typedef struct {
 *         i32_t kernel_memory_usage;
 *         i32_t filesystems_memory_usage;
 *         i32_t network_memory_usage;
 *         i32_t modules_memory_usage;
 *         i32_t applications_memory_usage;
 * } memstat_t;
 * </pre>
 *
 * @param stat      memory information
 *
 * @errors EINVAL
 *
 * @return Return <b>0</b> on success. On error, <b>positive</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * memstat_t stat;
 * if (get_memory_usage_details(&stat) == STD_RET_OK) {
 *         printf("Used memory by kernel      : %d\n"
 *                "Used memory by file system : %d\n"
 *                "Used memory by network     : %d\n"
 *                "Used memory by modules     : %d\n"
 *                "Used memory by applications: %d\n",
 *                stat.kernel_memory_usage,
 *                stat.filesystems_memory_usage,
 *                stat.network_memory_usage,
 *                stat.modules_memory_usage,
 *                stat.applications_memory_usage);
 * } else {
 *         perror(NULL);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int get_memory_usage_details(memstat_t *stat)
{
        return _builtinfunc(mm_get_mem_usage_details, stat);
}

//==============================================================================
/**
 * @brief i32_t get_module_memory_usage(uint module_number)
 * The function <b>get_module_memory_usage</b>() return memory usage by specified
 * module number <i>module_number</i>.
 *
 * @param module_number     module number
 *
 * @errors None
 *
 * @return On success, return amount of used memory by module in bytes.
 * On error, -1 is returned.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_modules = get_number_of_modules();
 * for (uint i = 0; i < number_of_modules; i++) {
 *         printf("%s : %d\n", get_module_name(i), get_module_memory_usage(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline i32_t get_module_memory_usage(uint module_number)
{
        i32_t size = -1;
        _builtinfunc(mm_get_module_mem_usage, module_number, &size);
        return size;
}

//==============================================================================
/**
 * @brief u32_t get_uptime(void)
 * The function <b>get_uptime</b>() return 32-bit uptime in seconds.
 *
 * @param None
 *
 * @errors None
 *
 * @return Uptime value in seconds.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("System works: %d seconds\n", get_uptime());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_uptime(void)
{
        return _builtinfunc(get_uptime_counter);
}

//==============================================================================
/**
 * @brief int get_average_CPU_load(avg_CPU_load_t *avg_CPU_load)
 * The function <b>get_average_CPU_load</b>() fill average CPU load container
 * pointed by <i>avg_CPU_load</i>.
 *
 * @param avg_CPU_load          average CPU load container
 *
 * @errors None
 *
 * @return On success 0 is returned, otherwise -1.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * avg_CPU_load_t avg;
 * get_average_CPU_load(&avg);
 *
 * printf("Average CPU load: %2d.%d%%, %2d.%d%%, %2d.%d%%\n",
 *         avg.min1  / 10, avg.min1  % 10,
 *         avg.min5  / 10, avg.min5  % 10,
 *         avg.min15 / 10, avg.min15 % 10);
 *
 * // ...
 */
//==============================================================================
static inline int get_average_CPU_load(avg_CPU_load_t *avg_CPU_load)
{
        return _builtinfunc(get_average_CPU_load, avg_CPU_load);
}

//==============================================================================
/**
 * @brief u32_t get_tick_counter(void)
 * The function <b>get_tick_counter</b>() return number of system clock ticks.
 *
 * @param None
 *
 * @errors None
 *
 * @return Number of system's ticks.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Context was switched %d times\n", get_tick_counter());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_tick_counter(void)
{
        return _builtinfunc(kernel_get_tick_counter);
}

//==============================================================================
/**
 * @brief u32_t get_time_ms(void)
 * The function <b>get_time_ms</b>() return number of milliseconds which
 * elapsed after kernel start. Function is similar to <b>get_uptime</b>(), except
 * that return milliseconds instead of seconds. In this function the tick
 * counter value is calculated to milliseconds, what means that resolution of
 * this counter depends on system tick counter increase value.
 *
 * @param None
 *
 * @errors None
 *
 * @return System work time in milliseconds.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("System works by %d ms\n", get_time_ms());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_time_ms(void)
{
        return _builtinfunc(kernel_get_time_ms);
}

//==============================================================================
/**
 * @brief const char *get_platform_name(void)
 * The function <b>get_platform_name</b>() return platform name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Platform name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Platform name: %s\n", get_platform_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_platform_name(void)
{
        return _CPUCTL_PLATFORM_NAME;
}

//==============================================================================
/**
 * @brief const char *get_OS_name(void)
 * The function <b>get_OS_name</b>() return operating system name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Operating system name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Operating system name: %s\n", get_OS_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_OS_name(void)
{
        return "dnx RTOS";
}

//==============================================================================
/**
 * @brief const char *get_OS_version(void)
 * The function <b>get_OS_version</b>() return operating system version string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Operating system version string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Operating system version: %s\n", get_OS_version());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_OS_version(void)
{
        return "1.7.0";
}

//==============================================================================
/**
 * @brief const char *get_OS_codename(void)
 * The function <b>get_OS_codename</b>() return a codename string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Codename string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Codename: %s\n", get_OS_codename());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_OS_codename(void)
{
        return "Caribou";
}

//==============================================================================
/**
 * @brief const char *get_kernel_name(void)
 * The function <b>get_kernel_name</b>() return kernel name string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Kernel name string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Kernel name: %s\n", get_kernel_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_kernel_name(void)
{
        return _KERNEL_NAME;
}

//==============================================================================
/**
 * @brief const char *get_kernel_version(void)
 * The function <b>get_kernel_version</b>() return kernel version string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Kernel version string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Kernel version: %s\n", get_kernel_version());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_kernel_version(void)
{
        return _KERNEL_VERSION;
}

//==============================================================================
/**
 * @brief const char *get_author_name(void)
 * The function <b>get_author_name</b>() return author name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Author name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Author name: %s\n", get_author_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_author_name(void)
{
        return "Daniel Zorychta";
}

//==============================================================================
/**
 * @brief const char *get_author_email(void)
 * The function <b>get_author_email</b>() return author's email address.
 *
 * @param None
 *
 * @errors None
 *
 * @return Author's email address.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Author's email: %s\n", get_author_email());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_author_email(void)
{
        return "<daniel.zorychta@gmail.com>";
}

//==============================================================================
/**
 * @brief const char *get_host_name(void)
 * The function <b>get_host_name</b>() return host name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return host name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Host name: %s\n", get_host_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_host_name(void)
{
        return CONFIG_HOSTNAME;
}

//==============================================================================
/**
 * @brief const char *get_user_name(void)
 * The function <b>get_user_name</b>() return current user name. Function
 * return always "root" name, because user handling is not supported yet.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return user name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("User name: %s\n", get_user_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_user_name(void)
{
        return "root";
}

//==============================================================================
/**
 * @brief const char *get_module_name(size_t modno)
 * The function <b>get_module_name</b>() return module name selected by <i>modno</i>
 * index.
 *
 * @param modno     module number
 *
 * @errors None
 *
 * @return Return module name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module name: %s\n", get_module_name(0));
 *
 * // ...
 */
//==============================================================================
static inline const char *get_module_name(size_t modno)
{
        return _builtinfunc(module_get_name, modno);
}

//==============================================================================
/**
 * @brief int get_module_ID(const char *name)
 * The function <b>get_module_ID</b>() return module ID selected by
 * name pointed by <i>name</i>.
 *
 * @param name     module name
 *
 * @errors None
 *
 * @return On success, return module index (ID). On error, -1 is returned.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module ID: %d\n", get_module_ID("crc"));
 *
 * // ...
 */
//==============================================================================
static inline int get_module_ID(const char *name)
{
        return _builtinfunc(module_get_ID, name);
}

//==============================================================================
/**
 * @brief int get_module_ID2(dev_t dev)
 * The function <b>get_module_ID2</b>() return module ID stored in dev_t type.
 *
 * @param dev           device ID
 *
 * @errors None
 *
 * @return Return module ID.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module ID: %d\n", get_module_ID2(dev));
 *
 * // ...
 */
//==============================================================================
static inline int get_module_ID2(dev_t dev)
{
        return _builtinfunc(dev_t__extract_modno, dev);
}

//==============================================================================
/**
 * @brief int get_module_major(dev_t dev)
 * The function <b>get_module_major</b>() return module major number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @errors None
 *
 * @return Return module major number.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module ID: %d\n", get_module_major(dev));
 *
 * // ...
 */
//==============================================================================
static inline int get_module_major(dev_t dev)
{
        return _builtinfunc(dev_t__extract_major, dev);
}

//==============================================================================
/**
 * @brief int get_module_minor(dev_t dev)
 * The function <b>get_module_minor</b>() return module minor number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @errors None
 *
 * @return Return module minor number.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module ID: %d\n", get_module_minor(dev));
 *
 * // ...
 */
//==============================================================================
static inline int get_module_minor(dev_t dev)
{
        return _builtinfunc(dev_t__extract_minor, dev);
}

//==============================================================================
/**
 * @brief uint get_number_of_modules(void)
 * The function <b>get_number_of_modules</b>() return number of registered
 * modules in system.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return number of registered modules in system.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_modules = get_number_of_modules();
 * for (uint i = 0; i < number_of_modules; i++) {
 *         printf("%s : %d\n", get_module_name(i), get_module_memory_usage(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline uint get_number_of_modules(void)
{
        return _builtinfunc(module_get_count);
}

//==============================================================================
/**
 * @brief int is_module_active(size_t n, bool *active)
 * The function <b>is_module_active</b>() return <b>true</b> if module is activated,
 * otherwise <b>false</b>.
 *
 * @param n             module index
 * @param active        module state
 *
 * @errors None
 *
 * @return On success return ESUCC. On error returns EINVAL.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * bool active;
 * if (is_module_active(0, &active) == ESUCC && active) {
 *         // module active ...
 * } else {
 *         // error or module inactive ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline ssize_t get_number_of_module_instances(size_t n)
{
        return _builtinfunc(module_get_number_of_instances, n);
}

//==============================================================================
/**
 * @brief void restart_system(void)
 * The function <b>restart_system</b>() restart system. Reset operation can
 * be software or hardware, but this depends on CPU port implementation.
 *
 * @param None
 *
 * @errors None
 *
 * @return This function not return.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * // e.g. user requests system restart
 * restart_system();
 */
//==============================================================================
static inline void restart_system(void)
{
        syscall(SYSCALL_RESTART, NULL);
}

//==============================================================================
/**
 * @brief  int syslog_enable(const char *path)
 * The function <b>syslog_enable</b>() enable system logging function. Log
 * messages will be write to the file pointed by <i>path</i>. If file exists and
 * is correctly opened then 0 is returned, otherwise -1 and appropriate errno
 * value is set.
 *
 * @param  path         log file path
 *
 * @errors Any
 *
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
static inline int syslog_enable(const char *path)
{
        int r = -1;
        syscall(SYSCALL_SYSLOGENABLE, &r, path);
        return r;
}

//==============================================================================
/**
 * @brief  int syslog_disable()
 * The function <b>int syslog_disable</b>() disable system logging functionallity.
 * On success 0 is returned, otherwise -1 and appropriate errno value is set.
 *
 * @param  None
 *
 * @errors Any
 *
 * @return On success 0 is returned, otherwise -1 and appropriate errno value is set.
 */
//==============================================================================
static inline int syslog_disable()
{
        int r = -1;
        syscall(SYSCALL_SYSLOGDISABLE, &r);
        return r;
}

//==============================================================================
/**
 * @brief  bool detect_kernel_panic(bool showmsg)
 * The function <b>detect_kernel_panic</b>() detect if in the last session
 * the Kernel Panic occurred. Kernel Panic message is show if <i>showmsg</i> is
 * set to <b>true</b>. Function return <b>true</b> if Kernel Panic is detected,
 * otherwise <b>false</b>.
 *
 * @param  showmsg      show message if kernel panic occurred
 *
 * @errors None
 *
 * @return If kernel panic occurred then true is returned, otherwise false.
 */
//==============================================================================
static inline bool detect_kernel_panic(bool showmsg)
{
        bool r = false;
        syscall(SYSCALL_KERNELPANICDETECT, &r, &showmsg);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _OS_H_ */
/*==============================================================================
  End of file
==============================================================================*/