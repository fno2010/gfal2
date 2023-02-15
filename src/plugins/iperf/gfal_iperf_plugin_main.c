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

#include <glib.h>
#include <string.h>
#include <sys/stat.h>

#include "gfal_iperf_plugin.h"


GQuark gfal2_get_plugin_iperf_quark()
{
    return g_quark_from_static_string(GFAL2_QUARK_PLUGINS "::FILE");
}


static gboolean is_iperf_uri(const char *src)
{
    return strncmp(src, "iperf:", 6) == 0;
}


const char *gfal_iperf_plugin_getName()
{
    return GFAL2_PLUGIN_VERSIONED("iperf", VERSION);
}


static gboolean gfal_iperf_check_url(plugin_handle handle, const char *url, plugin_mode mode, GError **err)
{
    g_return_val_err_if_fail(url != NULL, EINVAL, err, "[gfal_lfile_path_checker] Invalid url ");

    switch (mode) {
        //case GFAL_PLUGIN_ACCESS:
        //case GFAL_PLUGIN_MKDIR:
        case GFAL_PLUGIN_STAT:
        case GFAL_PLUGIN_LSTAT:
            //case GFAL_PLUGIN_RMDIR:
        case GFAL_PLUGIN_OPENDIR:
            //case GFAL_PLUGIN_OPEN:
            //case GFAL_PLUGIN_CHMOD:
        case GFAL_PLUGIN_UNLINK:
        case GFAL_PLUGIN_GETXATTR:
            //case GFAL_PLUGIN_LISTXATTR:
            //case GFAL_PLUGIN_SETXATTR:
            //case GFAL_PLUGIN_RENAME:
            //case GFAL_PLUGIN_SYMLINK:
        case GFAL_PLUGIN_CHECKSUM:
        case GFAL_PLUGIN_BRING_ONLINE:
        case GFAL_PLUGIN_ARCHIVE:
        case GFAL_PLUGIN_OPEN:
            return is_iperf_uri(url);
        default:
            return FALSE;
    }
}

void gfal_plugin_iperf_report_error(const char *msg, int errn, GError **err)
{
    g_set_error(err, gfal2_get_plugin_iperf_quark(), errn, "%s", msg);
}


void gfal_plugin_iperf_get_hostname(const char *url, char *value, size_t val_size)
{
    value[0] = '\0';

    if (!is_iperf_uri(url) || (strlen(url) < 8)) {
        return;
    }

    char **args = g_strsplit(url + 8, "/", 2);
    if (args[0]) {
        g_strlcpy(value, args[0], val_size);
    }

    g_strfreev(args);
}


void gfal_plugin_iperf_get_value(const char *url, const char *key, char *value, size_t val_size)
{
    // make sure it's an empty C-string
    value[0] = '\0';

    char *str = strchr(url, '?');
    if (str == NULL) {
        return;
    }

    size_t key_len = strlen(key);
    char **args = g_strsplit(str + 1, "&", 0);
    int i;
    for (i = 0; args[i] != NULL; ++i) {
        if (strncmp(args[i], key, key_len) == 0) {
            char *p = strchr(args[i], '=');
            if (p) {
                g_strlcpy(value, p + 1, val_size);
                break;
            }
        }
    }

    g_strfreev(args);
}


long long gfal_plugin_iperf_get_int_from_str(const char *buff)
{
    if (buff == 0 || buff[0] == '\0')
        return 0;
    return atoll(buff);
}


gboolean gfal_plugin_iperf_check_url_transfer(plugin_handle handle, gfal2_context_t ctx, const char *src,
    const char *dst, gfal_url2_check type)
{
    gboolean res = FALSE;
    if (src != NULL && dst != NULL) {
        if (type == GFAL_FILE_COPY && is_iperf_uri(src) && is_iperf_uri(dst)) {
            res = TRUE;
        }
    }
    return res;
}


void gfal_plugin_iperf_delete(plugin_handle plugin_data)
{
    free(plugin_data);
}

/*
 * This is a super ugly hack to allow gfal2 iperf plugin to kill self
 * on instantiation time. It peeks the arguments looking for a "magic"
 * string
 */
static void gfal_iperf_seppuku_hook()
{
    static const char iperf_load_time_signal_str[] = "IPERF_LOAD_TIME_SIGNAL";

    FILE *fd = fopen("/proc/self/cmdline", "r");
    if (!fd) {
        return;
    }

    char *value = NULL;
    size_t len = 0;
    const char *signal_str = NULL;

    while (getdelim(&value, &len, '\0', fd) != -1) {
        signal_str = strstr(value, iperf_load_time_signal_str);
        if (signal_str) {
            break;
        }
    }

    fclose(fd);

    if (signal_str) {
        signal_str += sizeof(iperf_load_time_signal_str) - 1;
        int signal_number = gfal_plugin_iperf_get_int_from_str(signal_str);
        raise(signal_number);
    }

    free(value);
}

/*
 * Init function, called before all
 **/
gfal_plugin_interface gfal_plugin_init(gfal2_context_t handle, GError **err)
{
    gfal_plugin_interface iperf_plugin;
    memset(&iperf_plugin, 0, sizeof(gfal_plugin_interface));

    IperfPluginData *mdata = calloc(1, sizeof(IperfPluginData));
    mdata->handle = handle;
    mdata->enable_signals = gfal2_get_opt_boolean_with_default(handle, "IPERF PLUGIN", "SIGNALS", FALSE);

    if (mdata->enable_signals) {
        gfal_iperf_seppuku_hook();
    }

    iperf_plugin.plugin_data = mdata;
    iperf_plugin.plugin_delete = gfal_plugin_iperf_delete;
    iperf_plugin.check_plugin_url = &gfal_iperf_check_url;
    iperf_plugin.getName = &gfal_iperf_plugin_getName;

    iperf_plugin.statG = &gfal_plugin_iperf_stat;
    iperf_plugin.lstatG = &gfal_plugin_iperf_stat;
    iperf_plugin.unlinkG = &gfal_plugin_iperf_unlink;
    iperf_plugin.getxattrG = &gfal_iperf_getxattrG;
    iperf_plugin.checksum_calcG = &gfal_iperf_checksumG;

    iperf_plugin.bring_online = gfal_plugin_iperf_bring_online;
    iperf_plugin.bring_online_v2 = gfal_plugin_iperf_bring_online_v2;
    iperf_plugin.bring_online_poll = gfal_plugin_iperf_bring_online_poll;
    iperf_plugin.release_file = gfal_plugin_iperf_release_file;
    iperf_plugin.archive_poll = gfal_plugin_iperf_archive_poll;

    iperf_plugin.bring_online_list = gfal_plugin_iperf_bring_online_list;
    iperf_plugin.bring_online_list_v2 = gfal_plugin_iperf_bring_online_list_v2;
    iperf_plugin.bring_online_poll_list = gfal_plugin_iperf_bring_online_poll_list;
    iperf_plugin.release_file_list = gfal_plugin_iperf_release_file_list;
    iperf_plugin.abort_files = gfal_plugin_iperf_abort_file_list;
    iperf_plugin.archive_poll_list = &gfal_plugin_iperf_archive_poll_list;

    iperf_plugin.check_plugin_url_transfer = &gfal_plugin_iperf_check_url_transfer;
    iperf_plugin.copy_file = &gfal_plugin_iperf_filecopy;

    iperf_plugin.opendirG = gfal_plugin_iperf_opendir;
    iperf_plugin.readdirG = gfal_plugin_iperf_readdir;
    iperf_plugin.readdirppG = gfal_plugin_iperf_readdirpp;
    iperf_plugin.closedirG = gfal_plugin_iperf_closedir;

    iperf_plugin.openG = gfal_plugin_iperf_open;
    iperf_plugin.closeG = gfal_plugin_iperf_close;
    iperf_plugin.readG = gfal_plugin_iperf_read;
    iperf_plugin.writeG = gfal_plugin_iperf_write;
    iperf_plugin.lseekG = gfal_plugin_iperf_seek;

    return iperf_plugin;
}



