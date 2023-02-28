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
#include "gfal_iperf_plugin.h"
#include <string.h>


static gboolean gfal_plugin_iperf_checksum_verify(const char *chk1, const char *chk2)
{
    // if no checksum was defined return
    if (!chk1 || !chk2 || !*chk1 || !*chk2)
        return TRUE;

    return strcmp(chk1, chk2) == 0;
}


static void gfal_iperf_cancel_transfer(gfal2_context_t context, void *userdata)
{
    int *seconds = (int *) userdata;
    *seconds = -10;
}


int gfal_plugin_iperf_filecopy(plugin_handle plugin_data,
    gfal2_context_t context, gfalt_params_t params, const char *src,
    const char *dst, GError **err)
{
    char checksum_type[GFAL_URL_MAX_LEN] = {0};
    char checksum_usr[GFAL_URL_MAX_LEN] = {0};
    char checksum_src[GFAL_URL_MAX_LEN] = {0};

    gfalt_checksum_mode_t checksum_method = gfalt_get_checksum(params,
        checksum_type, sizeof(checksum_type),
        checksum_usr, sizeof(checksum_usr),
        NULL);

    // validate source checksum
    if (checksum_method & GFALT_CHECKSUM_SOURCE) {
        gfal_plugin_iperf_get_value(src, "checksum", checksum_src, sizeof(checksum_src));
        if (!gfal_plugin_iperf_checksum_verify(checksum_usr, checksum_src)) {
            gfal_plugin_iperf_report_error("User and source checksums do not match", EIO, err);
            return -1;
        }
    }

    gfal2_log(G_LOG_LEVEL_DEBUG, "Get checksum method");

    // transferred file size
    long long file_size = 0;

    // check if the file size is specified in source
    char size_src[GFAL_URL_MAX_LEN] = {0};
    gfal_plugin_iperf_get_value(src, "size", size_src, sizeof(size_src));

    gfal2_log(G_LOG_LEVEL_DEBUG, "Read source file size");

    if (size_src[0] != '\0') {
        // get the value from source
        file_size = gfal_plugin_iperf_get_int_from_str(size_src);
    }
    else {
        file_size = gfal2_get_opt_integer_with_default(context, "IPERF PLUGIN", "DEFAULT_TRANSFER_SIZE", 100000000);
    }

    gfal2_log(G_LOG_LEVEL_DEBUG, "Parse source file size: %lld", file_size);

    plugin_trigger_event(params, gfal2_get_plugin_iperf_quark(), GFAL_EVENT_NONE,
        GFAL_EVENT_TRANSFER_ENTER, "Iperf: %s => %s", src, dst);
    plugin_trigger_event(params, gfal2_get_plugin_iperf_quark(),
        GFAL_EVENT_NONE, GFAL_EVENT_TRANSFER_TYPE, "iperf");

    gfal2_log(G_LOG_LEVEL_DEBUG, "Generating transfer commandline");

    // call iperf to generate traffic
    char src_se[GFAL_URL_MAX_LEN] = {0};
    gfal_plugin_iperf_get_hostname(src, src_se, sizeof(src_se));

    gfal2_log(G_LOG_LEVEL_DEBUG, "Parse source hostname: %s", src_se);

    char dst_se[GFAL_URL_MAX_LEN] = {0};
    gfal_plugin_iperf_get_hostname(dst, dst_se, sizeof(dst_se));

    gfal2_log(G_LOG_LEVEL_DEBUG, "Parse destination hostname: %s", dst_se);

    char iperf_cmd[200] = {0};
    sprintf(iperf_cmd, "ssh %s iperf -c %s -n %lld -i 1 -y C", src_se, dst_se, file_size);

    gfal2_log(G_LOG_LEVEL_DEBUG, "Transfer command arguments: %s", iperf_cmd);

    gfalt_transfer_status_t tr_status;
    memset(tr_status, 0x00, sizeof(*tr_status));

    FILE *tr;
    tr = popen(iperf_cmd, "r");

    char tr_progress[256];
    int seconds = 0;
    while (fgets(tr_progress, sizeof(tr_progress), tr) != NULL) {
        seconds++;

        gfal2_log(G_LOG_LEVEL_DEBUG, "progress line: %s", tr_progress);
        // get iperf progress
        char **tr_args = g_strsplit(tr_progress, ",", 0);

        if (tr_args != NULL && g_strv_length(tr_args) > 7) {
            tr_status->status = 0;
            tr_status->bytes_transfered += gfal_plugin_iperf_get_int_from_str(tr_args[7]);
            gfal2_log(G_LOG_LEVEL_DEBUG, "raw data: %s, transferred window: %lld", tr_args[7], tr_status->bytes_transfered);
            if (tr_status->bytes_transfered > file_size) {
                tr_status->bytes_transfered = file_size;
            }
            tr_status->transfer_time = seconds;
            if (seconds > 0)
                tr_status->average_baudrate = tr_status->bytes_transfered / seconds;
            tr_status->instant_baudrate = tr_status->average_baudrate;

            plugin_trigger_monitor(params, tr_status, src, dst);
            gfal2_log(G_LOG_LEVEL_DEBUG, "bytes: %lld, avg: %lld, inst: %lld, elapsed: %d", tr_status->bytes_transfered, tr_status->average_baudrate, tr_status->instant_baudrate, tr_status->transfer_time);
        }

        g_strfreev(tr_args);
    }

    int status = pclose(tr);
    if (status == -1) {
        gfal_plugin_iperf_report_error("Iperf test failed", EIO, err);
    }

    // Transfer finished
    plugin_trigger_event(params, gfal2_get_plugin_iperf_quark(), GFAL_EVENT_NONE,
        GFAL_EVENT_TRANSFER_EXIT, "Iperf finished in %d seconds", seconds);

    // Jump over to the destination stat
    IperfPluginData *mdata = plugin_data;
    mdata->stat_stage = STAT_DESTINATION_AFTER_TRANSFER;

    // validate destination checksum
    if (!*err && (checksum_method & GFALT_CHECKSUM_TARGET)) {
        char checksum_dst[GFAL_URL_MAX_LEN] = {0};
        gfal_plugin_iperf_get_value(dst, "checksum", checksum_dst, sizeof(checksum_dst));

        if (checksum_method & GFALT_CHECKSUM_SOURCE) {
            if (!gfal_plugin_iperf_checksum_verify(checksum_src, checksum_dst)) {
                gfal_plugin_iperf_report_error("Source and destination checksums do not match", EIO, err);
            }
        }
        else {
            if (!gfal_plugin_iperf_checksum_verify(checksum_usr, checksum_dst)) {
                gfal_plugin_iperf_report_error("User and destination checksums do not match", EIO, err);
            }
        }
    }

    if (*err)
        return -1;
    return 0;
}
