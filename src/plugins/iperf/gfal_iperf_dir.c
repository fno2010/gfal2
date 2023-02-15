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
#include <string.h>


typedef struct {
    struct stat st;
    struct dirent de;
} IperfPluginDirEntry;


typedef struct {
    GSList *list;
    GSList *item;
} IperfPluginDirectory;


gfal_file_handle gfal_plugin_iperf_opendir(plugin_handle plugin_data,
    const char *url, GError **err)
{
    struct stat st;
    gfal_plugin_iperf_stat(plugin_data, url, &st, err);
    if (*err) {
        return NULL;
    }
    if (!S_ISDIR(st.st_mode)) {
        gfal_plugin_iperf_report_error(strerror(ENOTDIR), ENOTDIR, err);
        return NULL;
    }

    char file_list[1024];
    gfal_plugin_iperf_get_value(url, "list", file_list, sizeof(file_list));

    IperfPluginDirectory *dir = g_malloc0(sizeof(IperfPluginDirectory));
    dir->list = NULL;

    // Populate list
    char *saveptr = NULL, *p;
    p = strtok_r(file_list, ",", &saveptr);
    while (p) {
        IperfPluginDirEntry *entry = g_malloc0(sizeof(IperfPluginDirEntry));

        char *sep = strchr(p, ':');
        if (!sep) {
            g_strlcpy(entry->de.d_name, p, sizeof(entry->de.d_name));
        }
        else {
            g_strlcpy(entry->de.d_name, p, sep - p + 1);
            entry->st.st_mode = strtol(sep + 1, &sep, 8);
            if (!(entry->st.st_mode & S_IFMT)) {
                entry->st.st_mode |= S_IFREG;
            }
            if (sep) {
                entry->st.st_size = strtol(sep + 1, &sep, 10);
            }
        }
        entry->de.d_reclen = strnlen(entry->de.d_name, 256);

        dir->list = g_slist_append(dir->list, entry);
        p = strtok_r(NULL, ",", &saveptr);
    }

    dir->item = dir->list;
    return gfal_file_handle_new2(gfal_iperf_plugin_getName(), dir, NULL, url);
}


int gfal_plugin_iperf_closedir(plugin_handle plugin_data,
    gfal_file_handle dir_desc, GError **err)
{
    IperfPluginDirectory *dir = gfal_file_handle_get_fdesc(dir_desc);
    g_slist_foreach(dir->list, (GFunc) g_free, NULL);
    g_slist_free(dir->list);
    g_free(dir);
    gfal_file_handle_delete(dir_desc);
    return 0;
}


struct dirent *gfal_plugin_iperf_readdirpp(plugin_handle plugin_data,
    gfal_file_handle dir_desc, struct stat *st, GError **err)
{
    IperfPluginDirectory *dir = gfal_file_handle_get_fdesc(dir_desc);
    if (!dir->item) {
        return NULL;
    }

    IperfPluginDirEntry *entry = (IperfPluginDirEntry *) (dir->item->data);
    dir->item = g_slist_next(dir->item);

    memcpy(st, &entry->st, sizeof(struct stat));
    return &entry->de;
}


struct dirent *gfal_plugin_iperf_readdir(plugin_handle plugin_data,
    gfal_file_handle dir_desc, GError **err)
{
    struct stat st;
    return gfal_plugin_iperf_readdirpp(plugin_data, dir_desc, &st, err);
}
