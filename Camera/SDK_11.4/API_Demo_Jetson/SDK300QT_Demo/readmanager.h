#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QObject>
#include <QEvent>
#include <QVBoxLayout>
#include <QWidget>

extern "C" {
#include "SDK/stkAPI.h"
}

class ReadManager {
public:
    ReadManager();
    void SyncFields();

public:
    float DaylightFOV;
    float DayLightGamma;
    float DayLightSaturation;
    float DaylightSharpness;
    float DayLightBrightness;
    float DayLightContrast;
    enum DTVExposureMode DayLightExpMode;
    uint32_t DayLightExpTime;
    uint8_t DayLightExpGain;

    float ThermalFov;
    float ThermalACE;
    float ThermalBrightness;
    float ThermalContrast;

    float Azimuth_Rate;
    float Azimuth_Pos;
    float Elevation_Rate;
    float Elevation_Pos;

    enum TOperationMode_Micro OperationMode;
    enum Camera MainCameraReadBack;

    char SystemVersionCPU[16];
    char RecordVideoFolderName[20];
    char LogRecordFileName[15];
};

#endif // HELPERFUNCTIONS_H
