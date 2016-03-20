/**
 * @defgroup fs-h "fs/fs.h"
 *
 * This library is used by each file system, contains file system-specific
 * functions and @subpage sysfunc-h. This is main and only one library that shall
 * be used by file system code.
 *
 * @{
 */

#include "drivers/ioctl_requests.h"
#ifndef DOXYGEN /* disabled for Doxygen documentation */
#undef sys_zalloc
#undef sys_malloc
#undef sys_free
inline void* operator new     (size_t size) {void *mem = NULL; _kmalloc(_MM_FS, size, &mem); return mem;}\
inline void* operator new[]   (size_t size) {void *mem = NULL; _kmalloc(_MM_FS, size, &mem); return mem;}\
inline void  operator delete  (void* ptr  ) {_sysfree(&ptr);}\
inline void  operator delete[](void* ptr  ) {_sysfree(&ptr);}
#define _FS_EXTERN_C extern "C"
#else
#define _FS_EXTERN_C
#endif
#endif /* DOXYGEN */

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of initialization function.
 *
 * Function created by this macro is called by system when file system has to
 * be initialized.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void **</b>]        file system memory handler
 * @param src_path      [<b>const char *</b>]   source file path
 * @return One of @ref errno value.
 */
#define API_FS_INIT(fsname, fs_handle, src_path)
#else
#define API_FS_INIT(fsname, ...)        _FS_EXTERN_C int _##fsname##_init(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of release function.
 *
 * Function created by this macro is called by system when file system has to
 * be released. If it is possible, all resources shall be released, if not then
 * error code shall be returned.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @return One of @ref errno value.
 */
#define API_FS_RELEASE(fsname, fs_handle)
#else
#define API_FS_RELEASE(fsname, ...)     _FS_EXTERN_C int _##fsname##_release(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file open function.
 *
 * Function created by this macro is called by system when file system has to
 * open selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void **</b>]        file extra data (user defined)
 * @param fd            [<b>fd_t *</b>]         file descriptor (user defined)
 * @param fpos          [<b>fpos_t *</b>]       file position indicator
 * @param path          [<b>const char *</b>]   file path
 * @param flags         [<b>u32_t</b>]          file flags (O_*)
 * @return One of @ref errno value.
 *
 * @see Flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_EXCL, O_TRUNC, O_APPEND
 */
#define API_FS_OPEN(fsname, fs_handle, extra, fd, fpos, path, flags)
#else
#define API_FS_OPEN(fsname, ...)        _FS_EXTERN_C int _##fsname##_open(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file close function.
 *
 * Function created by this macro is called by system when file system has to
 * close selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param force         [<b>bool</b>]           force file close (system request)
 * @return One of @ref errno value.
 */
#define API_FS_CLOSE(fsname, fs_handle, extra, fd, force)
#else
#define API_FS_CLOSE(fsname, ...)       _FS_EXTERN_C int _##fsname##_close(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file write function.
 *
 * Function created by this macro is called by system when file system has to
 * write to selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param src           [<b>const u8_t *</b>]   source buffer
 * @param count         [<b>size_t</b>]         bytes to write
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param wrcnt         [<b>size_t *</b>]       number of wrote bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#define API_FS_WRITE(fsname, fs_handle, extra, fd, src, count, fpos, wrcnt, fattr)
#else
#define API_FS_WRITE(fsname, ...)       _FS_EXTERN_C int _##fsname##_write(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file read function.
 *
 * Function created by this macro is called by system when file system has to
 * read from selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param dst           [<b>u8_t *</b>]         destination buffer
 * @param count         [<b>size_t</b>]         bytes to read
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param rdcnt         [<b>size_t *</b>]       number of read bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#define API_FS_READ(fsname, fs_handle, extra, fd, dst, count, fpos, rdcnt, fattr)
#else
#define API_FS_READ(fsname, ...)        _FS_EXTERN_C int _##fsname##_read(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file ioctl function.
 *
 * Function created by this macro is called by system when file system has to
 * do non-standard operation on selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param request       [<b>int</b>]            ioctl request
 * @param arg           [<b>void *</b>]         ioctl argument
 * @return One of @ref errno value.
 *
 * @see ioctl()
 */
#define API_FS_IOCTL(fsname, fs_handle, extra, fd, request, arg)
#else
#define API_FS_IOCTL(fsname, ...)       _FS_EXTERN_C int _##fsname##_ioctl(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file buffer flush function.
 *
 * Function created by this macro is called by system when file system has to
 * flush buffers of selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 *
 * @return One of @ref errno value.
 *
 * @see sync()
 */
#define API_FS_FLUSH(fsname, fs_handle, extra, fd)
#else
#define API_FS_FLUSH(fsname, ...)       _FS_EXTERN_C int _##fsname##_flush(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of directory create function.
 *
 * Function created by this macro is called by system when file system has to
 * create a new directory.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   directory path
 * @param mode          [<b>mode_t</b>]         directory mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#define API_FS_MKDIR(fsname, fs_handle, path, mode)
#else
#define API_FS_MKDIR(fsname, ...)       _FS_EXTERN_C int _##fsname##_mkdir(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of FIFO create function.
 *
 * Function created by this macro is called by system when file system has to
 * create a new FIFO file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   FIFO path
 * @param mode          [<b>mode_t</b>]         FIFO mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#define API_FS_MKFIFO(fsname, fs_handle, path, mode)
#else
#define API_FS_MKFIFO(fsname, ...)      _FS_EXTERN_C int _##fsname##_mkfifo(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of device node create function.
 *
 * Function created by this macro is called by system when file system has to
 * create a new device node file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   device node path
 * @param dev           [<b>dev_t</b>]          device ID
 * @return One of @ref errno value.
 *
 * @see dev_t
 */
#define API_FS_MKNOD(fsname, fs_handle, path, dev)
#else
#define API_FS_MKNOD(fsname, ...)       _FS_EXTERN_C int _##fsname##_mknod(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of open directory function.
 *
 * Function created by this macro is called by system when file system has to
 * open selected directory.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   directory path
 * @param dir           [<b>DIR *</b>]          directory object (already created)
 * @return One of @ref errno value.
 *
 * @see DIR
 */
#define API_FS_OPENDIR(fsname, fs_handle, path, dir)
#else
#define API_FS_OPENDIR(fsname, ...)     _FS_EXTERN_C int _##fsname##_opendir(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file remove function.
 *
 * Function created by this macro is called by system when file system has to
 * remove selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @return One of @ref errno value.
 */
#define API_FS_REMOVE(fsname, fs_handle, path)
#else
#define API_FS_REMOVE(fsname, ...)      _FS_EXTERN_C int _##fsname##_remove(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file rename function.
 *
 * Function created by this macro is called by system when file system has to
 * rename selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param old_name      [<b>const char *</b>]   old file path (name)
 * @param new_name      [<b>const char *</b>]   new file path (name)
 * @return One of @ref errno value.
 */
#define API_FS_RENAME(fsname, fs_handle, old_name, new_name)
#else
#define API_FS_RENAME(fsname, ...)      _FS_EXTERN_C int _##fsname##_rename(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file change mode function.
 *
 * Function created by this macro is called by system when file system has to
 * change permissions of selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @param mode          [<b>mode_t</b>]         new file mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#define API_FS_CHMOD(fsname, fs_handle, path, mode)
#else
#define API_FS_CHMOD(fsname, ...)       _FS_EXTERN_C int _##fsname##_chmod(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file change owner function.
 *
 * Function created by this macro is called by system when file system has to
 * change owner of selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @param owner         [<b>uid_t</b>]          owner ID
 * @param group         [<b>gid_t</b>]          group ID
 * @return One of @ref errno value.
 *
 * @see uid_t, gid_t
 */
#define API_FS_CHOWN(fsname, fs_handle, path, owner, group)
#else
#define API_FS_CHOWN(fsname, ...)       _FS_EXTERN_C int _##fsname##_chown(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file statistics function.
 *
 * Function created by this macro is called by system when file system has to
 * get statistics of selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param stat          [<b>struct stat *</b>]  file information
 * @return One of @ref errno value.
 *
 * @see struct stat
 */
#define API_FS_FSTAT(fsname, fs_handle, extra, fd, stat)
#else
#define API_FS_FSTAT(fsname, ...)       _FS_EXTERN_C int _##fsname##_fstat(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file statistics function.
 *
 * Function created by this macro is called by system when file system has to
 * get statistics selected file.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param path          [<b>const char *</b>]   file path
 * @param stat          [<b>struct stat *</b>]  file information
 * @return One of @ref errno value.
 *
 * @see struct stat
 */
#define API_FS_STAT(fsname, fs_handle, extra, path, stat)
#else
#define API_FS_STAT(fsname, ...)        _FS_EXTERN_C int _##fsname##_stat(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file system statistics function.
 *
 * Function created by this macro is called by system when file system has to
 * get statistics.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]          file system memory handler
 * @param statfs        [<b>struct statfs *</b>] file system information
 * @return One of @ref errno value.
 *
 * @see struct statfs
 */
#define API_FS_STATFS(fsname, fs_handle, statfs)
#define API_FS_STATFS(fsname, ...)      _FS_EXTERN_C int _##fsname##_statfs(__VA_ARGS__)
#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of file system synchronize function.
 *
 * Function created by this macro is called by system when file system has to
 * synchronize  with storage all buffers of opened files.
 *
 * @note Macro can be used only by file system code.
 *
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @return One of @ref errno value.
 */
#define API_FS_SYNC(fsname, fs_handle)
#else
#define API_FS_SYNC(fsname, ...)        _FS_EXTERN_C int _##fsname##_sync(__VA_ARGS__)
#endif
#ifdef DOXYGEN
/**
 * @brief Structure indicate file access mode.
 */
struct vfs_fattr {
        bool non_blocking_rd;         //!< Non-blocking file read access
        bool non_blocking_wr;         //!< Non-blocking file write access
};

/**
 * @brief Structure describe built-in program data.
 * @see   sys_get_programs_table()
 */
 struct _prog_data {
        const char     *name;           //!< Program name
        const size_t   *globals_size;   //!< Size of program global variables
        const size_t   *stack_depth;    //!< Stack depth
        process_func_t  main;           //!< Program main function
};
#endif
#ifndef DOXYGEN /* functions described in sysfunc.h header */
 * @param size             object size
 * @param mem              pointer to memory block pointer
 * @return One of @ref errno value.
static inline int sys_malloc(size_t size, void **mem)
 * @param size             object size
 * @param mem              pointer to memory block pointer
 * @return One of @ref errno value.
static inline int sys_zalloc(size_t size, void **mem)
 * @param mem           pointer to memory block to free
 * @return One of @ref errno value.
static inline int sys_free(void **mem)
//==============================================================================
/**
 * @brief  List constructor (for FS only)
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_llist_create(llist_cmp_functor_t functor, llist_obj_dtor_t obj_dtor, llist_t **list)
{
        return _llist_create_krn(_MM_FS, functor, obj_dtor, list);
}

#endif /* DOXYGEN */

 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_open(dev_t id, u32_t flags)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_close(dev_t id, bool force)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_write(dev_t            id,
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_read(dev_t            id,
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_ioctl(dev_t id, int request, void *arg)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_flush(dev_t id)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_driver_stat(dev_t id, struct vfs_dev_stat *stat)
 * @note Function can be used only by file system code.
 * @param pipe     pointer to pointer of pipe handle
 *
 * @return One of @ref errno value.
static inline int sys_pipe_create(pipe_t **pipe)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_destroy(pipe_t *pipe)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_get_length(pipe_t *pipe, size_t *len)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_read(pipe_t *pipe, u8_t *buf, size_t count, size_t *rdcnt, bool non_blocking)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_write(pipe_t *pipe, const u8_t *buf, size_t count, size_t *wrcnt, bool non_blocking)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_close(pipe_t *pipe)
 * @note Function can be used only by file system code.
 *
 * @return One of @ref errno value.
static inline int sys_pipe_clear(pipe_t *pipe)
 * @note Function can be used only by file system code.
static inline int sys_get_programs_table_size()
 * @note Function can be used only by file system code.
static inline const struct _prog_data *sys_get_programs_table()
/**@}*/