#ifndef _DNXMODULE_H_
#define _DNXMODULE_H_
/*=========================================================================*//**
@file    dnxmodule.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write modules.

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/systypes.h"
#include "core/vfs.h"
#include "core/sysmoni.h"
#include "user/regdrv.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef _DNX_H_
#error "dnx.h and dnxmodule.h shall never included together!"
#endif

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_modcalloc(size_t__nmemb, size_t__msize, regdrv_get_module_number(__module_name__))

#undef  malloc
#define malloc(size_t__size)                    sysm_modmalloc(size_t__size, regdrv_get_module_number(__module_name__))

#undef  free
#define free(void__pmem)                        sysm_modfree(void__pmem, regdrv_get_module_number(__module_name__))

#define _MODULE_INIT(modname)                   stdret_t _##modname##_init(void **driver_handle, u8_t major, u8_t minor)
#define MODULE_INIT(modname)                    static const char *__module_name__ = #modname; _MODULE_INIT(modname)
#define MODULE_RELEASE(modname)                 stdret_t _##modname##_release(void *driver_handle)
#define MODULE_OPEN(modname)                    stdret_t _##modname##_open(void *driver_handle)
#define MODULE_CLOSE(modname)                   stdret_t _##modname##_close(void *driver_handle)
#define MODULE_WRITE(modname)                   size_t _##modname##_write(void *driver_handle, const void *src, size_t item_size, size_t n_items, u64_t lseek)
#define MODULE_READ(modname)                    size_t _##modname##_read(void *driver_handle, void *dst, size_t item_size, size_t n_items, u64_t lseek)
#define MODULE_IOCTL(modname)                   stdret_t _##modname##_ioctl(void *driver_handle, int iorq, va_list args)
#define MODULE_FLUSH(modname)                   stdret_t _##modname##_flush(void *driver_handle)
#define MODULE_INFO(modname)                    stdret_t _##modname##_info(void *driver_handle, struct vfs_dev_info *info)

#define DRIVER_INTERFACE(modname)\
extern _MODULE_INIT(modname);    \
extern MODULE_RELEASE(modname);  \
extern MODULE_OPEN(modname);     \
extern MODULE_CLOSE(modname);    \
extern MODULE_WRITE(modname);    \
extern MODULE_READ(modname);     \
extern MODULE_IOCTL(modname);    \
extern MODULE_FLUSH(modname);    \
extern MODULE_INFO(modname)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
