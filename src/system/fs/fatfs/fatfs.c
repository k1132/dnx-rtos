#include "system/dnxfs.h"
static stdret_t fatfs_closedir  (void *fs_handle, DIR *dir);
static dirent_t fatfs_readdir   (void *fs_handle, DIR *dir);
static int      handle_error    (FRESULT fresult);
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
API_FS_INIT(fatfs, void **fs_handle, const char *src_path)
                        if (handle_error(libfat_mount(hdl->fsfile, &hdl->fatfs)) == 0) {
 * @brief Release file system
 * @param[in ]          *fs_handle              file system allocated memory
API_FS_RELEASE(fatfs, void *fs_handle)
                if (handle_error(libfat_umount(&hdl->fatfs)) == 0) {
                        vfs_fclose(hdl->fsfile);
                        free(hdl);
                        return STD_RET_OK;
                }
        } else {
                errno = EBUSY;
 * @brief Open file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags (see vfs.h)
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
API_FS_OPEN(fatfs, void *fs_handle, void **extra, fd_t *fd, u64_t *fpos, const char *path, int flags)
        STOP_IF(!fpos);
        if (flags == O_RDONLY) {
        } else if (flags == O_RDWR) {
        } else if (flags == (O_WRONLY | O_CREAT)) {
        } else if (flags == (O_RDWR | O_CREAT)) {
        } else if (flags == (O_WRONLY | O_APPEND | O_CREAT)) {
        } else if (flags == (O_RDWR | O_APPEND | O_CREAT)) {
        } else {
                free(fat_file);
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        if (handle_error(libfat_open(&hdl->fatfs, fat_file, path, fat_mode)) != 0) {
                free(fat_file);
                return STD_RET_ERROR;
        if (handle_error(libfat_open(&hdl->fatfs, fat_file, path, fat_mode)) != 0) {
        if (flags & O_APPEND) {
                if (handle_error(libfat_lseek(fat_file, libfat_size(fat_file))) != 0) {
                        free(fat_file);
                        return STD_RET_ERROR;
                }
                *fpos = libfat_size(fat_file);
                *fpos = 0;
 * @brief Close file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 * @param[in ]          *file_owner             task which opened file (valid if force is true)
API_FS_CLOSE(fatfs, void *fs_handle, void *extra, fd_t fd, bool force, task_t *file_owner)
        UNUSED_ARG(force);
        UNUSED_ARG(file_owner);
        if (handle_error(libfat_close(fat_file)) == 0) {
 * @brief Write data to the file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file

 * @return number of written bytes, -1 if error
API_FS_WRITE(fatfs, void *fs_handle, void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos)
        STOP_IF(!count);
        STOP_IF(!fpos);
        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                if (handle_error(libfat_lseek(fat_file, (u32_t)*fpos)) != 0) {
                        return -1;
                }
        if (handle_error(libfat_write(fat_file, src, count, &n)) != 0) {
                return -1;
        } else {
                return n;
        }
 * @brief Read data from file
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file

 * @return number of read bytes, -1 if error
API_FS_READ(fatfs, void *fs_handle, void *extra, fd_t fd, void *dst, size_t count, u64_t *fpos)
        STOP_IF(!count);
        STOP_IF(!fpos);
        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                if (handle_error(libfat_lseek(fat_file, (u32_t)*fpos)) != 0) {
                        return -1;
                }
        if (handle_error(libfat_read(fat_file, dst, count, &n)) != 0) {
                return -1;
        } else {
                return n;
        }
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
API_FS_IOCTL(fatfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
        UNUSED_ARG(request);
        UNUSED_ARG(arg);
        errno = EBADRQC;
 * @brief Flush file data
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
API_FS_FLUSH(fatfs, void *fs_handle, void *extra, fd_t fd)
        if (handle_error(libfat_sync(fat_file)) == 0)
 * @brief Return file status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
API_FS_FSTAT(fatfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
        stat->st_type  = FILE_TYPE_REGULAR;
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
API_FS_MKDIR(fatfs, void *fs_handle, const char *path, mode_t mode)
        if (handle_error(libfat_mkdir(&hdl->fatfs, path)) == 0) {
                uint8_t dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
                if (handle_error(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO)) == 0) {
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
 * @brief Create pipe
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKFIFO(fatfs, void *fs_handle, const char *path, mode_t mode)
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
API_FS_MKNOD(fatfs, void *fs_handle, const char *path, const struct vfs_drv_interface *drv_if)
        errno = EPERM;
 * @brief Open directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
API_FS_OPENDIR(fatfs, void *fs_handle, const char *path, DIR *dir)
        if (handle_error(libfat_opendir(&hdl->fatfs, &fatdir->dir, dospath)) == 0) {
static stdret_t fatfs_closedir(void *fs_handle, DIR *dir)
static dirent_t fatfs_readdir(void *fs_handle, DIR *dir)
        if (handle_error(libfat_readdir(&fatdir->dir, &fat_file_info)) == 0) {
 * @brief Remove file/directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
API_FS_REMOVE(fatfs, void *fs_handle, const char *path)
        if (handle_error(libfat_unlink(&hdl->fatfs, path)) == 0)
 * @brief Rename file/directory
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
API_FS_RENAME(fatfs, void *fs_handle, const char *old_name, const char *new_name)
        if (handle_error(libfat_rename(&hdl->fatfs, old_name, new_name)) == 0)
 * @brief Change file's mode
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
API_FS_CHMOD(fatfs, void *fs_handle, const char *path, int mode)
        uint8_t dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
        if (handle_error(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO)) == 0) {
 * @brief Change file's owner and group
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
API_FS_CHOWN(fatfs, void *fs_handle, const char *path, int owner, int group)
        errno = EPERM;
 * @brief Return file/dir status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
API_FS_STAT(fatfs, void *fs_handle, const char *path, struct stat *stat)
        if (handle_error(libfat_stat(&hdl->fatfs, path, &file_info)) == 0) {
                stat->st_type  = FILE_TYPE_REGULAR;
 * @brief Return file system status
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
API_FS_STATFS(fatfs, void *fs_handle, struct vfs_statfs *statfs)
        struct fatfs *hdl    = fs_handle;
        struct stat fstat;
        if (vfs_fstat(hdl->fsfile, &fstat) != 0)
                return STD_RET_ERROR;
        if (handle_error(libfat_getfree(&hdl->fatfs, &free_clusters)) == 0) {
                        statfs->f_fsname = "fatfs (FAT12)";
                        statfs->f_fsname = "fatfs (FAT16)";
                        statfs->f_fsname = "fatfs (FAT32)";
                        statfs->f_fsname = "fatfs (FAT\?\?)";
//==============================================================================
/**
 * @brief Function handle libfat errors and translate to errno
 *
 * @param fresult       libfat result
 *
 * @return 0 if no error, 1 if error
 */
//==============================================================================
static int handle_error(FRESULT fresult)
{
        int errn = 0;

        switch (fresult) {
        case FR_OK                 : errn = 0;           break;
        case FR_DISK_ERR           : errn = EIO;         break;
        case FR_INT_ERR            : errn = EAGAIN;      break;
        case FR_NOT_READY          : errn = EBUSY;       break;
        case FR_NO_FILE            : errn = ENOENT;      break;
        case FR_NO_PATH            : errn = ENOENT;      break;
        case FR_INVALID_NAME       : errn = ENOENT;      break;
        case FR_DENIED             : errn = EACCES;      break;
        case FR_EXIST              : errn = EEXIST;      break;
        case FR_INVALID_OBJECT     : errn = ENOENT;      break;
        case FR_WRITE_PROTECTED    : errn = EROFS;       break;
        case FR_INVALID_DRIVE      : errn = EMEDIUMTYPE; break;
        case FR_NOT_ENABLED        : errn = ENOSPC;      break;
        case FR_NO_FILESYSTEM      : errn = EMEDIUMTYPE; break;
        case FR_MKFS_ABORTED       : errn = ECANCELED;   break;
        case FR_TIMEOUT            : errn = ETIME;       break;
        case FR_LOCKED             : errn = ECANCELED;   break;
        case FR_NOT_ENOUGH_CORE    : errn = ENOMEM;      break;
        case FR_TOO_MANY_OPEN_FILES: errn = EMFILE;      break;
        case FR_INVALID_PARAMETER  : errn = EINVAL;      break;
        }

        if (errn) {
                errno = errn;
                return 1;
        } else {
                return 0;
        }
}
