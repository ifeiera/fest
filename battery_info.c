#include "battery_info.h"
#include <stdio.h>

BatteryInfo *getBatteryInfo(void)
{
    BatteryInfo *info = (BatteryInfo *)malloc(sizeof(BatteryInfo));
    if (!info)
        return NULL;

    // Inisialisasi nilai default untuk sistem desktop
    info->percent = 100;
    info->powerPlugged = TRUE;
    info->isDesktop = TRUE;

    // Dapatkan status power sistem
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus))
    {
        // Dapatkan persentase baterai jika tersedia
        if (powerStatus.BatteryLifePercent != 255)
        {
            info->percent = (int)powerStatus.BatteryLifePercent;
            info->isDesktop = FALSE; // Memiliki baterai valid, bukan desktop
        }

        // Cek apakah tersambung listrik
        info->powerPlugged = (powerStatus.ACLineStatus == 1);

        // Deteksi apakah sistem adalah desktop berdasarkan flag baterai
        if (powerStatus.BatteryFlag == 128 ||
            powerStatus.BatteryFlag == 255 ||
            (powerStatus.BatteryLifePercent == 255 && powerStatus.BatteryFlag == 1))
        {
            info->isDesktop = TRUE;
            info->percent = 100;
            info->powerPlugged = TRUE;
        }
    }

    return info;
}

void freeBatteryInfo(BatteryInfo *info)
{
    if (info)
    {
        free(info);
    }
}

int getBatteryPercent(const BatteryInfo *info)
{
    return info ? info->percent : 100;
}

BOOL isPowerPlugged(const BatteryInfo *info)
{
    return info ? info->powerPlugged : TRUE;
}

BOOL isDesktopSystem(const BatteryInfo *info)
{
    return info ? info->isDesktop : TRUE;
}
