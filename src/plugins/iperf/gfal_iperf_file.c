/*
 * Copyright (c) CERN 2013-2017
 *
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 *  See  http://www.eu-emi.eu/partners for details on the copyright
 *  holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gfal_iperf_plugin.h"
#include <stdio.h>
#include <string.h>

#ifndef EBADFD
#define EBADFD EBADF
#endif

typedef struct {
    const char *url;
    int fd;
    off_t size;
    off_t offset;
} IperfFile;


gfal_file_handle gfal_plugin_iperf_open(plugin_handle plugin_data, const char *url, int flag, mode_t mode, GError **err)
{
    struct stat st;
    int ret = gfal_plugin_iperf_stat(plugin_data, url, &st, err);
    if (ret < 0) {
        return NULL;
    }

    char arg_buffer[64] = {0};
    gfal_plugin_iperf_get_value(url, "open_errno", arg_buffer, sizeof(arg_buffer));
    int errcode = gfal_plugin_iperf_get_int_from_str(arg_buffer);
    if (errcode > 0) {
        gfal_plugin_iperf_report_error(strerror(errcode), errcode, err);
        return NULL;
    }

    IperfFile *fd = g_malloc(sizeof(IperfFile));
    fd->url = url;
    fd->size = st.st_size;
    fd->offset = 0;
    if (flag == O_RDONLY) {
        fd->fd = open("/dev/urandom", O_RDONLY);
    }
    else if (flag == O_WRONLY) {
        fd->fd = open("/dev/null", O_WRONLY);
    }
    else {
        gfal_plugin_iperf_report_error("Iperf plugin does not support read and write", ENOSYS, err);
        return NULL;
    }

    if (fd->fd < 0) {
        gfal_plugin_iperf_report_error("Could not open the file!", errno, err);
        return NULL;
    }

    return gfal_file_handle_new2(gfal_iperf_plugin_getName(), fd, NULL, url);
}


ssize_t gfal_plugin_iperf_read(plugin_handle plugin_data, gfal_file_handle fd, void *buff, size_t count, GError **err)
{
    IperfFile *mfd = gfal_file_handle_get_fdesc(fd);
    char arg_buffer[64] = {0};

    gfal_plugin_iperf_get_value(mfd->url, "read_wait", arg_buffer, sizeof(arg_buffer));
    int wait = gfal_plugin_iperf_get_int_from_str(arg_buffer);
    if (wait > 0) {
        sleep(wait);
    }

    gfal_plugin_iperf_get_value(mfd->url, "read_errno", arg_buffer, sizeof(arg_buffer));
    int errcode = gfal_plugin_iperf_get_int_from_str(arg_buffer);
    if (errcode > 0) {
        gfal_plugin_iperf_report_error(strerror(errcode), errcode, err);
        return -1;
    }

    off_t remaining = mfd->size - mfd->offset;
    if (count > remaining) {
        count = remaining;
    }

    if (count < 0) {
        gfal_plugin_iperf_report_error("Reading passed end of file", EBADFD, err);
        return -1;
    }

    off_t nread = read(mfd->fd, buff, count);
    if (nread < 0) {
        gfal_plugin_iperf_report_error("Failed to read file", errno, err);
        return -1;
    }

    mfd->offset += nread;
    return nread;
}

ssize_t gfal_plugin_iperf_write(plugin_handle plugin_data, gfal_file_handle fd, const void *buff, size_t count,
    GError **err)
{
    IperfFile *mfd = gfal_file_handle_get_fdesc(fd);

    off_t nwrite = write(mfd->fd, buff, count);
    if (nwrite < 0) {
        gfal_plugin_iperf_report_error("Failed to write file", errno, err);
    }

    mfd->offset += nwrite;
    return nwrite;
}


int gfal_plugin_iperf_close(plugin_handle plugin_data, gfal_file_handle fd, GError **err)
{
    IperfFile *mfd = gfal_file_handle_get_fdesc(fd);
    close(mfd->fd);
    g_free(mfd);
    return 0;
}


off_t gfal_plugin_iperf_seek(plugin_handle plugin_data, gfal_file_handle fd, off_t offset, int whence, GError **err)
{
    IperfFile *mfd = gfal_file_handle_get_fdesc(fd);
    switch (whence) {
        case SEEK_SET:
            mfd->offset = offset;
            break;
        case SEEK_END:
            mfd->offset = mfd->size + offset;
            break;
        case SEEK_CUR:
            mfd->offset += offset;
    }
    return mfd->offset;
}
