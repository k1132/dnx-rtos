#include "system/dnxfs.h"
#include "core/printx.h"
#include "core/conv.h"
#include "core/progman.h"
#include "system/thread.h"
#define DIR_BIN_STR                       "bin"
      u32_t    ID_counter;
static stdret_t    procfs_closedir_freedd  (void *fs_handle, DIR *dir);
static stdret_t    procfs_closedir_generic (void *fs_handle, DIR *dir);
static dirent_t    procfs_readdir_root     (void *fs_handle, DIR *dir);
static dirent_t    procfs_readdir_taskname (void *fs_handle, DIR *dir);
static dirent_t    procfs_readdir_bin      (void *fs_handle, DIR *dir);
static dirent_t    procfs_readdir_taskid   (void *fs_handle, DIR *dir);
static dirent_t    procfs_readdir_taskid_n (void *fs_handle, DIR *dir);
static inline void mutex_force_lock        (mutex_t *mtx);
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
API_FS_INIT(procfs, void **fs_handle, const char *src_path)
        struct procfs *procfs    = calloc(1, sizeof(struct procfs));
        list_t        *file_list = list_new();
        mutex_t       *mtx       = mutex_new(MUTEX_NORMAL);
        if (procfs && file_list && mtx) {
                procfs->file_list    = file_list;
                procfs->resource_mtx = mtx;
                procfs->ID_counter   = 0;
                *fs_handle = procfs;
                return STD_RET_OK;
        }
        if (file_list) {
                list_delete(file_list);
        }
        if (mtx) {
                mutex_delete(mtx);

        if (procfs) {
                free(procfs);
        }

        return STD_RET_ERROR;
 * @brief Release file system
 * @param[in ]          *fs_handle              file system allocated memory
API_FS_RELEASE(procfs, void *fs_handle)
        struct procfs *procfs = fs_handle;

        if (mutex_lock(procfs->resource_mtx, 100)) {
                if (list_get_item_count(procfs->file_list) != 0) {
                        mutex_unlock(procfs->resource_mtx);
                        errno = EBUSY;
                        return STD_RET_ERROR;
                }
                critical_section_begin();
                mutex_unlock(procfs->resource_mtx);
                mutex_delete(procfs->resource_mtx);
                list_delete(procfs->file_list);
                free(procfs);
                critical_section_end();
                return STD_RET_OK;
        } else {
                errno = EBUSY;
        }
        return STD_RET_ERROR;
 * @brief Open file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags (see vfs.h)
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
API_FS_OPEN(procfs, void *fs_handle, void **extra, fd_t *fd, u64_t *lseek, const char *path, int flags)
        if (flags != O_RDONLY) {
                errno = EROFS;
                        errno = ENOENT;
                path = sys_strtoi((char*)path, 16, (i32_t*)&taskHdl);
                        errno = ENOENT;
                        errno = ENOENT;
                mutex_force_lock(procmem->resource_mtx);
                        mutex_unlock(procmem->resource_mtx);
                mutex_unlock(procmem->resource_mtx);
                        errno = ENOENT;
                        mutex_force_lock(procmem->resource_mtx);
                                mutex_unlock(procmem->resource_mtx);
                        mutex_unlock(procmem->resource_mtx);
        errno = ENOENT;

 * @brief Close file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 * @param[in ]          *file_owner             task which opened file (valid if force is true)
API_FS_CLOSE(procfs, void *fs_handle, void *extra, fd_t fd, bool force, task_t *file_owner)
        UNUSED_ARG(force);
        UNUSED_ARG(file_owner);
                mutex_force_lock(procmem->resource_mtx);
                        mutex_unlock(procmem->resource_mtx);
                mutex_unlock(procmem->resource_mtx);
 * @brief Write data to the file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file

 * @return number of written bytes, -1 if error
API_FS_WRITE(procfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos)
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        errno = EROFS;
 * @brief Read data from file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file

 * @return number of read bytes, -1 if error
API_FS_READ(procfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, u64_t *fpos)
        STOP_IF(!count);
        STOP_IF(!fpos);
        mutex_force_lock(procmem->resource_mtx);
        mutex_unlock(procmem->resource_mtx);
                errno = ENOENT;
                errno = ENOENT;
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.free_stack);
                dataSize = sys_snprintf(data, dataSize, "%s\n", taskInfo.task_name);
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.opened_files);
                dataSize = sys_snprintf(data, dataSize, "%d\n", taskInfo.priority);
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.memory_usage);
        size_t seek = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;
                if (dataSize - seek <= count) {
                        strncpy((char*)dst, data + seek, n);
                        n = count;
                        strncpy((char *)dst, data + seek, n);
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
API_FS_IOCTL(procfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
        UNUSED_ARG(request);
        UNUSED_ARG(arg);

        errno = EPERM;
 * @brief Flush file data
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
API_FS_FLUSH(procfs, void *fs_handle, void *extra, fd_t fd)
        errno = EROFS;

 * @brief Return file status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
API_FS_FSTAT(procfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
        mutex_force_lock(procmem->resource_mtx);
        mutex_unlock(procmem->resource_mtx);
                errno = ENOENT;
                errno = ENOENT;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_type  = FILE_TYPE_REGULAR;
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.free_stack);
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.opened_files);
                stat->st_size = sys_snprintf(data, sizeof(data), "%d\n", taskInfo.priority);
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.memory_usage);
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
API_FS_MKDIR(procfs, void *fs_handle, const char *path, mode_t mode)
        UNUSED_ARG(mode);

        errno = EROFS;
 * @brief Create pipe
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
API_FS_MKFIFO(procfs, void *fs_handle, const char *path, mode_t mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        UNUSED_ARG(mode);

        /* not supported by this file system */
        errno = EPERM;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]          *drv_if                 driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(procfs, void *fs_handle, const char *path, const struct vfs_drv_interface *drv_if)
        errno = EROFS;

 * @brief Open directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
API_FS_OPENDIR(procfs, void *fs_handle, const char *path, DIR *dir)
                dir->f_items    = 3;
                dir->f_closedir = procfs_closedir_generic;
                dir->f_closedir = procfs_closedir_generic;
        } else if (strcmp(path, "/"DIR_BIN_STR"/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = _get_programs_table_size();
                dir->f_readdir  = procfs_readdir_bin;
                dir->f_closedir = procfs_closedir_generic;
                return STD_RET_OK;
                        errno = ENOENT;
                path = sys_strtoi((char*)path, 16, (i32_t*)&taskHdl);
                        errno = ENOENT;
                        dir->f_closedir = procfs_closedir_generic;
        errno = ENOENT;

static stdret_t procfs_closedir_freedd(void *fs_handle, DIR *dir)
static stdret_t procfs_closedir_generic(void *fs_handle, DIR *dir)
 * @brief Remove file/directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
API_FS_REMOVE(procfs, void *fs_handle, const char *path)
        errno = EROFS;

 * @brief Rename file/directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
API_FS_RENAME(procfs, void *fs_handle, const char *old_name, const char *new_name)
        errno = EROFS;

 * @brief Change file's mode
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
API_FS_CHMOD(procfs, void *fs_handle, const char *path, int mode)
        errno = EROFS;

 * @brief Change file's owner and group
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
API_FS_CHOWN(procfs, void *fs_handle, const char *path, int owner, int group)
        errno = EROFS;

 * @brief Return file/dir status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
API_FS_STAT(procfs, void *fs_handle, const char *path, struct stat *stat)
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_type  = FILE_TYPE_REGULAR;
 * @brief Return file system status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
API_FS_STATFS(procfs, void *fs_handle, struct vfs_statfs *statfs)
        statfs->f_fsname = "procfs";
static dirent_t procfs_readdir_root(void *fs_handle, DIR *dir)
        case 2: dirent.name = DIR_BIN_STR;
                break;
static dirent_t procfs_readdir_taskname(void *fs_handle, DIR *dir)
static dirent_t procfs_readdir_bin(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_seek < (size_t)_get_programs_table_size()) {
                dirent.filetype = FILE_TYPE_PROGRAM;
                dirent.name     = _get_programs_table()[dir->f_seek].program_name;
                dirent.size     = 0;

                dir->f_seek++;
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid(void *fs_handle, DIR *dir)
                        sys_snprintf(dir->f_dd, TASK_ID_STR_LEN, "%x", (int)taskdata.task_handle);
static dirent_t procfs_readdir_taskid_n(void *fs_handle, DIR *dir)
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.priority);
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%u\n", taskdata.free_stack);
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.memory_usage);
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.opened_files);
//==============================================================================
/**
 * @brief Force lock mutex
 *
 * @param mtx           mutex
 */
//==============================================================================
static inline void mutex_force_lock(mutex_t *mtx)
{
        while (mutex_lock(mtx, MTX_BLOCK_TIME) != true);
}
