#include <iostream>
#include <chrono>
#include "readmanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern "C"
{
#include "SDK/stkAPI.h"
}

ReadManager::ReadManager() {}

void ReadManager::SyncFields()
{
    DaylightFOV = GetDayCameraReportFOV();
    DayLightGamma = GetDayCameraReportGammaValue();
    DayLightSaturation = GetDayCameraReportSaturationValue();
    DaylightSharpness = GetDayCameraReportSharpnessValue();
    DayLightBrightness = GetDayCameraReportBrigthnessValue();
    DayLightContrast = GetDayCameraReportContrastValue();
    DayLightExpMode = GetDayCameraReportExposureMode();
    DayLightExpTime = GetDayCameraReportExposureTime();
    DayLightExpGain = GetDayCameraReportExposureGain();

    ThermalFov = GetThermalCameraReportFOV();
    ThermalACE = GetThermalCameraReportACEValue();
    ThermalBrightness = GetThermalCameraReportLevelValue();
    ThermalContrast = GetThermalCameraReportGainValue();

    Azimuth_Rate = GetRate_GRRAngularVelocities_X();
    Azimuth_Pos  = GetPositionReportRelativeGimbalAngles_X();
    Elevation_Rate = GetRate_GRRAngularVelocities_Y();
    Elevation_Pos  = GetPositionReportRelativeGimbalAngles_Y();

    OperationMode = GetSystemGeneralReportOperationMode();
    MainCameraReadBack = GetVideoChannelReportPrimaryVideoChannel();

    GetSystemVersionCPU(SystemVersionCPU, sizeof(SystemVersionCPU));
    GetLogRecordReportFileName(LogRecordFileName, sizeof(LogRecordFileName));
    GetRecordVideoReportFolderName(RecordVideoFolderName, sizeof(RecordVideoFolderName));
}

