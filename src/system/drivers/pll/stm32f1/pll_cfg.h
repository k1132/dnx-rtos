#ifndef _PLL_CFG_H_
#define _PLL_CFG_H_
/*=========================================================================*//**
@file    pll_cfg.h

@author  Daniel Zorychta

@brief   PLL configuration file

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

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** CPU target frequency */
#define PLL_CPU_TARGET_FREQ               72000000UL

#define PLL_APB1FREQ                      36000000UL

/** PLL exit statuses */
#define PLL_STATUS_HSE_ERROR              -1
#define PLL_STATUS_PLL_ERROR              -2
#define PLL_STATUS_PLL2_ERROR             -3
#define PLL_STATUS_PLL3_ERROR             -4
#define PLL_STATUS_PLL_SW_ERROR           -5

#define PLL_PART_NONE                     0
#define PLL_DEV_NONE                      0

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

#endif /* _PLL_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
