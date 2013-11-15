/*=========================================================================*//**
@file    progman.h

@author  Daniel Zorychta

@brief   This file support programs layer

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _PROGMAN_H_
#define _PROGMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "core/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define GLOBAL_VARIABLES_SECTION_BEGIN  struct __global_vars__ {
#define GLOBAL_VARIABLES_SECTION_END    };

#define PROGRAM_MAIN(name, argc, argv) \
        const int __prog_##name##_gs__ = sizeof(struct __global_vars__);\
        int _program_##name##_main(argc, argv)

#define _IMPORT_PROGRAM(name)\
        extern const int __prog_##name##_gs__;\
        extern int _program_##name##_main(int, char**)

#define _PROGRAM_CONFIG(name, stack_size) \
        {.program_name  = #name,\
         .main_function = _program_##name##_main,\
         .globals_size  = &__prog_##name##_gs__,\
         .stack_depth   = stack_size}

#define stdin \
        _task_get_data()->f_stdin

#define stdout \
        _task_get_data()->f_stdout

#define stderr \
        _task_get_data()->f_stderr

#define global \
        ((struct __global_vars__*)_task_get_data()->f_mem)

#define create_fast_global(name) \
        struct __global_vars__*name = global

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct _prog_data {
        char      *program_name;
        int      (*main_function)(int, char**);
        const int *globals_size;
        int        stack_depth;
};

typedef struct thread thread_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/
enum program_state {
        PROGRAM_STATE_UNKNOWN,
        PROGRAM_STATE_RUNNING,
        PROGRAM_STATE_ENDED,
        PROGRAM_STATE_NOT_ENOUGH_FREE_MEMORY,
        PROGRAM_STATE_ARGUMENTS_PARSE_ERROR,
        PROGRAM_STATE_DOES_NOT_EXIST,
        PROGRAM_STATE_HANDLE_ERROR,
};

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t   *_program_start         (const char*, const char*, FILE*, FILE*, enum program_state*, int*);
extern int       _program_kill          (task_t*, int);
extern void      exit                   (int);
extern void      abort                  (void);
extern int       system                 (const char*);
extern thread_t *_thread_new            (void (*)(void*), const int, void*);
extern int       _thread_join           (thread_t*);
extern bool      _thread_is_finished    (thread_t*);
extern int       _thread_delete         (thread_t*);
extern int       _thread_cancel         (thread_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline const struct _prog_data *_get_programs_table(void)
{
        extern const struct _prog_data _prog_table[];
        return _prog_table;
}

static inline int _get_programs_table_size(void)
{
        extern const int _prog_table_size;
        return _prog_table_size;
}

#ifdef __cplusplus
}
#endif

#endif /* _PROGMAN_H_ */
/*==============================================================================
  End of file
==============================================================================*/
