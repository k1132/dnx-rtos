#include "core/fs.h"
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>
#include <string.h>
#include "core/pipe.h"
#define TIMEOUT_MS                      MAX_DELAY_MS
                        dev_t             drv;
                        pipe_t           *pipe;
                        void             *generic;
                } IF;
                gid_t                     gid;
                uid_t                     uid;
                mode_t                    mode;
        UNUSED_ARG(src_path);
 * @param[in]            flags                  file open flags
API_FS_OPEN(devfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags)
        UNUSED_ARG(fd);
                                open = driver_open(node->IF.drv, flags);
API_FS_CLOSE(devfs, void *fs_handle, void *extra, fd_t fd, bool force)
                close = driver_close(node->IF.drv, force);
 * @param[in ]           fattr                  file attributes
 *
API_FS_WRITE(devfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
        UNUSED_ARG(fs_handle);
                return driver_write(node->IF.drv, src, count, fpos, fattr);
                return pipe_write(node->IF.pipe, src, count, fattr.non_blocking_wr);
                errno = ENXIO;
 * @param[in ]           fattr                  file attributes
 *
API_FS_READ(devfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
        UNUSED_ARG(fs_handle);
                return driver_read(node->IF.drv, dst, count, fpos, fattr);
                return pipe_read(node->IF.pipe, dst, count, fattr.non_blocking_rd);
                errno = ENXIO;
        UNUSED_ARG(fs_handle);
                return driver_ioctl(node->IF.drv, request, arg);
        } else if (node->type == FILE_TYPE_PIPE && request == PIPE_CLOSE) {
                return pipe_close(node->IF.pipe) ? STD_RET_OK : STD_RET_ERROR;
                errno = EBADRQC;
                return STD_RET_ERROR;
        UNUSED_ARG(fs_handle);
                return driver_flush(node->IF.drv);
                return STD_RET_ERROR;
        UNUSED_ARG(fs_handle);
                getfstat = driver_stat(node->IF.drv, &devstat);
                pipelen = pipe_get_length(node->IF.pipe);
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
                                node->IF.pipe = pipe_new();
                                node->path    = malloc(strlen(path + 1) + 1);
                                if (node->IF.pipe && node->path) {
                                        if (node->IF.pipe) {
                                                pipe_delete(node->IF.pipe);
                                                node->IF.pipe = NULL;
 * @param[in ]           dev                    driver interface
API_FS_MKNOD(devfs, void *fs_handle, const char *path, const dev_t dev)
                                node->path = malloc(strlen(path + 1) + 1);
                                if (node->path) {
                                        node->IF.drv = dev;
                                        node->gid    = 0;
                                        node->uid    = 0;
                                        node->mode   = S_IRUSR | S_IWUSR | S_IRGRO | S_IWGRO | S_IROTH | S_IWOTH;
                                        node->type   = FILE_TYPE_DRV;
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);
                                if (driver_stat(node->IF.drv, &devstat) == STD_RET_OK) {
                                        dirent.size = devstat.st_size;
                                dirent.dev      = node->IF.drv;

                                int n = pipe_get_length(node->IF.pipe);
                                        dirent.size = n;

                                dirent.filetype = FILE_TYPE_PIPE;
                                if (node->type == FILE_TYPE_PIPE) {
                                        pipe_delete(node->IF.pipe);
                                node->IF.generic = NULL;
API_FS_CHMOD(devfs, void *fs_handle, const char *path, mode_t mode)
API_FS_CHOWN(devfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
API_FS_STATFS(devfs, void *fs_handle, struct statfs *statfs)
        statfs->f_bsize  = sizeof(struct devnode);
        statfs->f_blocks = devfs->number_of_chains * CHAIN_NUMBER_OF_NODES;
        statfs->f_bfree  = statfs->f_blocks - devfs->number_of_used_nodes;
        statfs->f_ffree  = statfs->f_blocks - devfs->number_of_used_nodes;
//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(devfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
}

                        if (nchain->devnode[i].path == NULL)
                        if (nchain->devnode[i].path == NULL)
                        if (nchain->devnode[i].path != NULL) {
                if (chain->devnode[i].IF.generic) {
                        free(chain->devnode[i].IF.generic);