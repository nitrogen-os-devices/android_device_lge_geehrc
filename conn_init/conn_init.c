/*
 * Copyright 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cutils/log.h>

extern int wfc_util_qcom_check_config(unsigned char *nv_mac_addr);
extern void wfc_util_atoh(char *pAsciiString, int szAsciiString, unsigned char *pHexaBuff, int szHexaBuff);

static int wifi_check_qcom_cfg_files()
{
    char macAddress[13];
    char hex[7];
    memset(macAddress, 0, sizeof(macAddress));
    memset(hex, 0, 7);

    // Read MAC String
    FILE *fp = NULL;
    int n = 0;
    fp = fopen("/persist/wifi/.macaddr", "r"); //Nexus 4 location
    if ( fp == NULL )
    {
        ALOGW("Failed to open /persist/wifi/.macaddr for reading. Failing over to misc partition.");
        fp = fopen("/dev/block/platform/msm_sdcc.1/by-name/misc", "r"); //Optimus G location
        if ( fp == NULL )
        {
            ALOGE("Failed to open misc partition file for wlan macaddr reading");
        
            wfc_util_qcom_check_config((unsigned char *)macAddress);
            return 0;
        }
        else
        {
            unsigned char macbuf[6];
            fseek(fp,0x3000,SEEK_SET);
            n = fread(macbuf, 6, 1, fp);
            sprintf(macAddress,"%02x%02x%02x%02x%02x%02x",
                    macbuf[0], macbuf[1], macbuf[2],
                    macbuf[3], macbuf[4], macbuf[5]);
            fclose(fp);
        }
    }
    else
    {
        n = fread(macAddress, 12, 1, fp);
        fclose(fp);
        if (n == 0) {
            // Buffer may be partially written. Reset.
            memset(macAddress, 0, sizeof(macAddress));
        }
    }

    // Write MAC String
    wfc_util_atoh( macAddress, 12, (unsigned char *)hex, 6);
    wfc_util_qcom_check_config((unsigned char *)hex);

    return 1;
}

int main(void)
{
    wifi_check_qcom_cfg_files();

    return 0;
}
