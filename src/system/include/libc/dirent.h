/*=========================================================================*//**
@file    dirent.h

@author  Daniel Zorychta

@brief   Directory handling

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup dirent-h <dirent.h>

The library provides directory handling functions.

*/
/**@{*/

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <kernel/syscall.h>

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
#ifdef DOXYGEN
        /**
         * @brief Directory container
         *
         * The type represent directory object. Fields are private.
         */
        typedef struct {} DIR;

        /* type defined in sys/types.h */
        /** @brief Directory entry. */
        typedef struct dirent {
                char   *name;           //!< File name
                u64_t   size;           //!< File size in bytes
                tfile_t filetype;       //!< File type
                dev_t   dev;            //!< Device address (if file type is driver)
        } dirent_t;
#else
        #ifndef __DIR_TYPE_DEFINED__
                typedef struct vfs_dir DIR;
        #endif
#endif

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
 * @brief Function opens selected directory.
 *
 * Function opens a directory stream corresponding to the directory <i>name</i>, and
 * returns a pointer to the directory stream. The stream is positioned at the first
 * entry in the directory.
 *
 * @param name          directory path
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | ...
 *
 * @return A pointer to the directory stream. On error, <b>NULL</b> is returned,
 * and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <stdio.h>
        #include <dirent.h>
        #include <errno.h>

        // ...

        errno = 0;

        DIR *dir = opendir("/foo/bar");
        if (dir) {

                dirent_t *dirent = readdir(dir);
                while (dirent->name != NULL) {
                        // ...
                }

                closedir(dir);
        } else {
                perror("/foo/bar");
        }

        // ...
   @endcode
 */
//==============================================================================
static inline DIR *opendir(const char *name)
{
        DIR *dir = NULL;
        syscall(SYSCALL_OPENDIR, &dir, name);
        return dir;
}

//==============================================================================
/**
 * @brief Function closes selected directory stream.
 *
 * Function closes the directory stream associated with <i>dir</i>. The directory
 * stream descriptor <i>dir</i> is not available after this call.
 *
 * @param dir           pinter to directory object
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | ...
 *
 * @return Return \b 0 on success. On error, \b -1 is returned,
 * and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <stdio.h>
        #include <dirent.h>
        #include <errno.h>

        // ...

        errno = 0;

        DIR *dir = opendir("/foo/bar");
        if (dir) {

                dirent_t *dirent = readdir(dir);
                while (dirent->name != NULL) {
                        // ...
                }

                closedir(dir);
        } else {
                perror("/foo/bar");
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int closedir(DIR *dir)
{
        int r = -1;
        syscall(SYSCALL_CLOSEDIR, &r, dir);
        return r;
}

//==============================================================================
/**
 * @brief Function reads entry from opened directory stream.
 *
 * Function returns a pointer to object <b>dirent_t</b> type representing the
 * next directory entry in the directory stream pointed to by <i>dir</i>.<p>
 *
 * @param dir           directory object
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | ...
 *
 * @return On success, function returns a pointer to a <b>dirent_t</b> type. If
 * the end of the directory stream is reached, field <b>name</b> of <b>dirent_t</b>
 * type is <b>NULL</b>. If an error occurs, NULL-object and <b>errno</b> is set
 * appropriately.
 *
 * @b Example
 * @code
        #include <stdio.h>
        #include <dirent.h>
        #include <errno.h>

        // ...

        errno = 0;

        DIR *dir = opendir("/foo/bar");
        if (dir) {

                dirent_t *dirent = readdir(dir);
                while (dirent->name != NULL) {
                        // ...
                }

                closedir(dir);
        } else {
                perror("/foo/bar");
        }

        // ...
   @endcode
 */
//==============================================================================
static inline struct dirent *readdir(DIR *dir)
{
        struct dirent *dirent = NULL;
        syscall(SYSCALL_READDIR, &dirent, dir);
        return dirent;
}

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
