#include "core/fs.h"
 * @param[in]            flags                  file open flags
API_FS_OPEN(genericfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags)
API_FS_CLOSE(genericfs, void *fs_handle, void *extra, fd_t fd, bool force)
 * @param[in ]           fattr                  file attributes
 *
API_FS_WRITE(genericfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
 * @param[in ]           fattr                  file attributes
 *
API_FS_READ(genericfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
 * @param[in ]           dev                    driver id
API_FS_MKNOD(genericfs, void *fs_handle, const char *path, const dev_t dev)
        dir->f_closedir = closedir;
        dir->f_readdir  = readdir;
        // ...
API_FS_CHMOD(genericfs, void *fs_handle, const char *path, mode_t mode)
API_FS_CHOWN(genericfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
API_FS_STATFS(genericfs, void *fs_handle, struct statfs *statfs)
//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(genericfs, void *fs_handle)
{
