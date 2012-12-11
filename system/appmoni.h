#ifndef APPMONI_H_
#define APPMONI_H_
/*=============================================================================================*//**
@file    appmoni.h

@author  Daniel Zorychta

@brief   This module is used to monitoring all applications

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "systypes.h"
#include "memman.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/* USER CFG: enable (1) or disable (0) application monitoring */
#define APP_MONITORING_ENABLE             (0)

/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
#if (APP_MONITORING_ENABLE > 0)
extern void *moni_malloc(u32_t size);
extern void *moni_calloc(u32_t nmemb, u32_t msize);
extern void  moni_free  (void *mem);
#else
#define moni_malloc(size)           mm_malloc(size)
#define moni_calloc(nmemb, msize)   mm_calloc(nmemb, msize)
#define moni_free(mem)              mm_free(mem)
#endif


#ifdef __cplusplus
}
#endif

#endif /* APPMONI_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
