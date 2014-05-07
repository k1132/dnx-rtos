#include "core/fs.h"
#include <dnx/misc.h>
#include <string.h>
 * @param[in]            flags                  file open flags
API_FS_OPEN(fatfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags)
        } else if (flags == (O_WRONLY | O_CREATE)) {
        } else if (flags == (O_RDWR | O_CREATE)) {
        } else if (flags == (O_WRONLY | O_APPEND | O_CREATE)) {
        } else if (flags == (O_RDWR | O_APPEND | O_CREATE)) {
API_FS_CLOSE(fatfs, void *fs_handle, void *extra, fd_t fd, bool force)
API_FS_WRITE(fatfs, void *fs_handle, void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos)
API_FS_READ(fatfs, void *fs_handle, void *extra, fd_t fd, void *dst, size_t count, fpos_t *fpos)
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
 * @param[in ]           dev                    driver number
API_FS_MKNOD(fatfs, void *fs_handle, const char *path, const dev_t dev)
        UNUSED_ARG(dev);
API_FS_CHMOD(fatfs, void *fs_handle, const char *path, mode_t mode)
API_FS_CHOWN(fatfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
API_FS_STATFS(fatfs, void *fs_handle, struct statfs *statfs)
        memset(statfs, 0, sizeof(struct statfs));
        statfs->f_fsname = "fatfs (FAT\?\?)";
        statfs->f_type   = hdl->fatfs.fs_type;

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(fatfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
}
