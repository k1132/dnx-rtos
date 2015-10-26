#include "fs/fs.h"
static int closedir(void *fs_handle, DIR *dir);
static int readdir (void *fs_handle, DIR *dir, dirent_t **dirent);
 * @return One of errno value (errno.h)
API_FS_INIT(<!fs_name!>, void **fs_handle, const char *src_path)
        return EINVAL;
 * @return One of errno value (errno.h)
API_FS_RELEASE(<!fs_name!>, void *fs_handle)
        return EINVAL;
 * @param[in]            flags                  file open flags (O_...)
 * @return One of errno value (errno.h)
API_FS_OPEN(<!fs_name!>, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_CLOSE(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, bool force)
        return EINVAL;
 * @param[out]          *wrcnt                  number of written bytes
 * @return One of errno value (errno.h)
API_FS_WRITE(<!fs_name!>,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
        return EIO;
 * @param[out]          *rdcnt                  number of read bytes
 * @return One of errno value (errno.h)
API_FS_READ(<!fs_name!>,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
        return EIO;
 * @return One of errno value (errno.h)
API_FS_IOCTL(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
        return ESUCC;
 * @return One of errno value (errno.h)
API_FS_FLUSH(<!fs_name!>, void *fs_handle, void *extra, fd_t fd)
        return EINVAL;
 * @return One of errno value (errno.h)
API_FS_FSTAT(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
        return EINVAL;
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKDIR(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
API_FS_MKFIFO(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_MKNOD(<!fs_name!>, void *fs_handle, const char *path, const dev_t dev)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_OPENDIR(<!fs_name!>, void *fs_handle, const char *path, DIR *dir)
        return ENOENT;
 * @return One of errno value (errno.h)
static int closedir(void *fs_handle, DIR *dir)
        return EINVAL;
 * @param[out]          **dirent                directory entry
 * @return One of errno value (errno.h)
static int readdir(void *fs_handle, DIR *dir, dirent_t **dirent)
        dir->dirent.name = "Example";
        dir->dirent.size = 10;
        dir->dirent.filetype = FILE_TYPE_REGULAR;
        *dirent = &dir->dirent;
        return ESUCC;
 * @return One of errno value (errno.h)
API_FS_REMOVE(<!fs_name!>, void *fs_handle, const char *path)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_RENAME(<!fs_name!>, void *fs_handle, const char *old_name, const char *new_name)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_CHMOD(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_CHOWN(<!fs_name!>, void *fs_handle, const char *path, uid_t owner, gid_t group)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_STAT(<!fs_name!>, void *fs_handle, const char *path, struct stat *stat)
        return ENOENT;
 * @return One of errno value (errno.h)
API_FS_STATFS(<!fs_name!>, void *fs_handle, struct statfs *statfs)
        statfs->f_fsname = "<!fs_name!>";
        return EINVAL;
 * @return One of errno value (errno.h)
API_FS_SYNC(<!fs_name!>, void *fs_handle)
        return ESUCC;