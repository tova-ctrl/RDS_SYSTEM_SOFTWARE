#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib
#else // __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#endif
#include "stkAPI.h"


#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifdef SEND_ON_SET
#define SendData(BuffPtr, BuffSize) STKSendMessage(BuffPtr , BuffSize)
#else
#define SendData(BuffPtr, BuffSize) 0
#endif
#define VelocityToDeg 349.06585039886585
#define AnglesToDeg 182.0388

#ifdef DEBUG_MODE
#define DBG_PRINT(...) printf(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif

#if defined(_MSC_VER)
    #define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__)
    #define PACKED_STRUCT(name) struct __attribute__((packed)) name
#else
    #error "Unknown compiler"
#endif
char *REMOTE_IP = "1.1.1.3";
char *HOST_IP = "1.1.1.2";
int COMMUNICATION_PORT = 1024;
int LOCAL_PORT = 1025;


PACKED_STRUCT(SystemInitStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(IDLEStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(DriftCalibrationStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(RateCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AngularVelocities_X;
	int16_t AngularVelocities_Y;
};

PACKED_STRUCT(PositionCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t LOSAngles_X;
	int16_t LOSAngles_Y;
};

PACKED_STRUCT(CenterModeStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(SafeModeStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(PositionReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t RelativeGimbalAngles_X;
	int16_t RelativeGimbalAngles_Y;
};

PACKED_STRUCT(GyroRawReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint64_t ControlGyroTimeTag;
	float GyroYaw;
	float GyroPitch;
	float GyroRoll;
};

PACKED_STRUCT(GroundReferenceCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AngularVelocities_X;
	int16_t AngularVelocities_Y;
	uint16_t SlantRange;
	uint8_t Spare0;
	uint8_t Spare1;
};

PACKED_STRUCT(GroundReferenceReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AngularVelocities_X;
	int16_t AngularVelocities_Y;
	uint16_t SlantRange;
	uint8_t Spare0;
	uint8_t Spare1;
};

PACKED_STRUCT(Rate_GRRStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AngularVelocities_X;
	int16_t AngularVelocities_Y;
};

PACKED_STRUCT(StabilizationErrorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AngularVelocitiesX;
	int16_t AngularVelocitiesY;
};

PACKED_STRUCT(GimbalControlDataStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t AxisXCurrentConsumption;
	int16_t AxisYCurrentConsumption;
	uint8_t Spare0;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
};

PACKED_STRUCT(GimbalMasterBITStatusStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool SystemBusy : 1;
	bool Gimbalnotinitialized : 1;
	bool GimbalInitFail : 1;
	bool GimbalFail : 1;
	bool GimbalHigh_LowTempWarning : 1;
	bool TrackInitFlag : 1;
	uint8_t Spare0 : 2;
	bool Payloadcommfail : 1;
	bool Lowstoragewarning : 1;
	bool HiveTemperaturewarning : 1;
	bool Hivesoftwarefail : 1;
	bool HostCommfail : 1;
	uint8_t Spare1 : 3;
	bool VISWideFail : 1;
	bool VISNarrowFail : 1;
	bool TIFail : 1;
	uint8_t Spare2 : 5;
};

PACKED_STRUCT(DayCameraCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t ZoomCommand : 2;
	uint8_t SharpnessCommand : 2;
	uint8_t Spare0 : 4;
	uint8_t GammaCommand : 2;
	uint8_t SaturationCommand : 2;
	uint8_t GainCommand : 2;
	uint8_t LevelCommand : 2;
	int8_t ExposureMode : 2;
	uint8_t ExposureTime : 2;
	uint8_t ExposureGain : 2;
	int8_t AutoWhiteBalance : 2;
	uint8_t Temperature : 2;
	int8_t FlickerMode : 2;
	bool RestoreToDefault : 1;
	uint8_t Spare1 : 3;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
	uint8_t Spare12;
	uint8_t Spare13;
};

PACKED_STRUCT(DayCameraSetFOVStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	float SetFOVCommand;
	uint8_t Spare0;
	uint8_t Spare1;
};

PACKED_STRUCT(DayCameraReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int32_t ExposureMode;
	int32_t ExposureTime;
	int32_t ExposureGain;
	float GammaValue;
	float SaturationValue;
	float ContrastValue;
	float SharpnessValue;
	float FOV;
	float ZoomXAxisCenter;
	float ZoomYAxisCenter;
	float BrigthnessValue;
	int32_t AutoWhiteBalance;
	int32_t TemperatureValue;
	float FlickerMode;
};

PACKED_STRUCT(ThermalCameraCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t ZoomCommand : 2;
	uint8_t GainCommand : 2;
	uint8_t LevelCommand : 2;
	uint8_t Spare0 : 2;
	uint8_t GainModeCommand : 2;
	uint8_t Spare1 : 2;
	uint8_t LinearPercentCommand : 2;
	uint8_t Spare2 : 2;
	uint8_t ACECommand : 2;
	uint8_t SharpnessCommand : 2;
	int8_t NUCMode : 2;
	uint8_t Spare3 : 2;
	int8_t ColorPalette;
	bool NUC : 1;
	bool RestoreToDefault : 1;
	uint8_t Spare4 : 6;
	uint8_t SyncModeCommand : 2;
	uint8_t Spare5 : 6;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
	uint8_t Spare12;
	uint8_t Spare13;
	uint8_t Spare14;
	uint8_t Spare15;
};

PACKED_STRUCT(TICameraSetFOVStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	float SetFOVCommand;
	uint8_t Spare0;
	uint8_t Spare1;
};

PACKED_STRUCT(ThermalCameraReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int32_t GainMode;
	int32_t SyncMode;
	float LinearPercent;
	float ACEValue;
	float DDEValue;
	int32_t ColorPalette;
	int32_t NUCMode;
	float FOV;
	int32_t GainValue;
	int32_t LevelValue;
};

PACKED_STRUCT(SeeSpotControlCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	bool Freeze : 1;
	uint8_t Spare0 : 6;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
	uint8_t Spare12;
	uint8_t Spare13;
	uint8_t Spare14;
	uint8_t Spare15;
};

PACKED_STRUCT(SeeSpotControlCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	bool Freeze : 1;
	uint8_t Spare0 : 6;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
	uint8_t Spare12;
	uint8_t Spare13;
	uint8_t Spare14;
	uint8_t Spare15;
};

PACKED_STRUCT(ClearStickyBITStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0;
};

PACKED_STRUCT(CamerasBITStatusStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool DTVNarrowActiveError : 1;
	bool DTVNarrowStickyError : 1;
	bool DTVNarrowInitError : 1;
	uint8_t Spare0 : 5;
	uint8_t DTVNarrowVideoErrorsCounter;
	bool DTVWideActiveError : 1;
	bool DTVWideStickyError : 1;
	bool DTVWideInitError : 1;
	uint8_t Spare1 : 5;
	uint8_t DTVWideVideoErrorsCounter;
	bool TIActiveError : 1;
	bool TIStickyError : 1;
	bool TIInitError : 1;
	uint8_t Spare2 : 5;
	uint8_t TIVideoErrorsCounter;
	bool USBHubActiveError : 1;
	bool USBHubStickyError : 1;
	bool USBHubInitError : 1;
	uint8_t Spare3 : 5;
	uint8_t Spare4;
};

PACKED_STRUCT(VideoChannelCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t PrimaryVideoChannel;
	bool FOVSynchronizationMode : 1;
	bool ImageStabilizationEnable : 1;
	bool ImageStabilizationMethod : 1;
	uint8_t Spare0 : 5;
	bool OSDDisabled : 1;
	bool PIPEnable : 1;
	bool HistogramEnable : 1;
	uint8_t Spare1 : 5;
	int8_t FontColor;
};

PACKED_STRUCT(VideoChannelReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t PrimaryVideoChannel;
	bool FOVSynchronizationMode : 1;
	bool ImageStabilizationEnable : 1;
	bool ImageStabilizationMethod : 1;
	uint8_t Spare0 : 5;
	bool OSDDisabled : 1;
	bool PIPEnable : 1;
	uint8_t Spare1 : 1;
	bool HistogramEnable : 1;
	uint8_t Spare2 : 4;
	int8_t FontColor;
};

PACKED_STRUCT(VideoChannelRunTimeConfigStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool StopChannel0Stream : 1;
	bool StartChannel0Stream : 1;
	bool H264Channel0Stream : 1;
	bool H265Channel0Stream : 1;
	uint8_t Spare0 : 4;
	int32_t BandwidthChannel0Stream;
	bool StopDTVStream : 1;
	bool StopTIStream : 1;
	uint8_t Spare1 : 6;
};

PACKED_STRUCT(RecordVideoCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Enable : 1;
	bool EnableRaw : 1;
	bool DeleteALLRecords : 1;
	uint8_t Spare0 : 5;
	char FolderName[20];
};

PACKED_STRUCT(RecordVideoReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int64_t RecordVideoTimestamp_nSec_;
	int64_t RecordRawVideoTimestamp_nSec_;
	float DiskFreeSpace_GB_;
	float DiskFreeSpace_Percent_;
	float TotalDiskSize_GB_;
	int32_t RecordVideoEnable;
	int32_t RecordRawVideoEnable;
	int32_t VideoDelete;
	int32_t DiskStatus;
	char FolderName[20];
};

PACKED_STRUCT(TrackStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t TargetXCoordinate;
	int16_t TargetYCoordinate;
};

PACKED_STRUCT(TrackerTargetErrorReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Camera;
	uint8_t TrackerGeneralStatus : 4;
	uint8_t TrackingQuality : 3;
	bool FirstTimeTrack : 1;
	uint16_t CurrFOV;
	float TrackerXError;
	float TrackerYError;
	int8_t GMC;
	uint16_t GateSizeX;
	uint16_t GateSizeY;
};

PACKED_STRUCT(TrackerParametersStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t TrackerType;
	uint8_t Spare0 : 8;
	uint16_t GateXSize;
	uint16_t GateYSize;
	bool GateSizeEnable : 1;
	uint8_t Spare1 : 7;
	uint8_t Spare2;
};

PACKED_STRUCT(TrackerParametersReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t TrackerType;
	uint8_t Spare0 : 8;
	uint16_t GateXSize;
	uint16_t GateYSize;
	bool GateSizeEnable : 1;
	uint8_t Spare1 : 7;
};

PACKED_STRUCT(TrackerAdjustControlsStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool AdjustEnable : 1;
	uint8_t Spare0;
	uint8_t Spare1 : 7;
	int16_t MovementCommand_X;
	int16_t MovementCommand_Y;
};

PACKED_STRUCT(TrackerAdjustControlsReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool AdjustEnable : 1;
	uint8_t Spare0 : 7;
	int16_t MovementX;
	int16_t MovementY;
};

PACKED_STRUCT(TrackerOffsetControlsStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool OffsetEnable : 1;
	uint8_t Spare0;
	uint8_t Spare1 : 7;
	int16_t Movementcommand_X;
	int16_t Movementcommand_Y;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
};

PACKED_STRUCT(TrackerOffsetControlsReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool OffsetEnable : 1;
	uint8_t Spare0 : 7;
	int16_t MovementX;
	int16_t MovementY;
};

PACKED_STRUCT(VMDControlsStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0 : 5;
	bool Enable : 1;
	uint8_t Spare1 : 2;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
};

PACKED_STRUCT(VMDReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool EnableVMD;
	uint8_t QuantityofDetections;
};

PACKED_STRUCT(ATRControlsStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0 : 5;
	bool DetectionsReport : 1;
	uint8_t Spare1 : 2;
	int8_t DetectionThreshold;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
};

PACKED_STRUCT(ATRReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Enabled : 1;
	uint8_t Spare0 : 7;
	uint8_t QuantityofDetections;
};

PACKED_STRUCT(NavigationInitStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t XMountingAngle;
	int16_t YMountingAngle;
	int16_t ZMountingAngle;
	int16_t LeverArmX;
	int16_t LeverArmY;
	int16_t LeverArmZ;
	int16_t PlatformBodyAzimuth;
	int16_t PlatformBodyPitch;
	int16_t PlatformBodyRoll;
};

PACKED_STRUCT(NavigationInitStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int16_t XMountingAngle;
	int16_t YMountingAngle;
	int16_t ZMountingAngle;
	int16_t LeverArmX;
	int16_t LeverArmY;
	int16_t LeverArmZ;
	int16_t PlatformBodyAzimuth;
	int16_t PlatformBodyPitch;
	int16_t PlatformBodyRoll;
};

PACKED_STRUCT(NavigationPropertiesStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t PlatformDataSource : 2;
	bool UsePlatformData : 1;
	bool NavigationType : 1;
	bool AltitudeType : 1;
	uint8_t Spare0 : 3;
};

PACKED_STRUCT(NavigationStatusStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t PlatformDataSource : 2;
	bool UsePlatformData : 1;
	bool NavigationType : 1;
	bool AltitudeType : 1;
	uint8_t Spare0 : 3;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint8_t Spare6;
	uint8_t Spare7;
	uint8_t Spare8;
	uint8_t Spare9;
	uint8_t Spare10;
	uint8_t Spare11;
	uint8_t Spare12;
	uint8_t Spare13;
	uint8_t Spare14;
	uint8_t Spare15;
	uint8_t Spare16;
	uint8_t Spare17;
	uint8_t Spare18;
	uint8_t Spare19;
	uint8_t Spare20;
	uint8_t Spare21;
	uint8_t Spare22;
	uint8_t Spare23;
	uint8_t Spare24;
	uint8_t Spare25;
	uint8_t Spare26;
	uint8_t Spare27;
	uint8_t Spare28;
	uint8_t Spare29;
};

PACKED_STRUCT(PlatformNavigationDataAccuracyStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t PositionAccuracy : 2;
	int8_t AltitudeAccuracy : 2;
	int8_t AttitudeAccuracy : 2;
	int8_t AzimuthAccuracy : 2;
	int8_t VelocityAccuracy : 2;
	int8_t RateofClimbAccuracy : 2;
	uint8_t Spare0 : 4;
};

PACKED_STRUCT(PlatformNavigationDataStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int32_t TimeTag;
	float PlatformLongitude;
	float PlatformLatitude;
	float PlatformAltitude;
	float PlatformBodyAzimuth;
	float PlatformBodyPitch;
	float PlatformBodyRoll;
	int8_t GroundSpeed;
	int8_t RateofClimb;
};

PACKED_STRUCT(PlatformNavigationDataStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int32_t TimeTag;
	float PlatformLongitude;
	float PlatformLatitude;
	float PlatformAltitude;
	float LOSAzimuth;
	float LOSPitch;
	float LOSRoll;
	float TargetLongitude;
	float TargetLatitude;
	float TargetAltitude;
	int8_t GroundSpeed;
	int8_t RateofClimb;
};

PACKED_STRUCT(NavigationIMUDataStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int64_t IMUTimeTag;
	int32_t IMUXRate;
	int32_t IMUYRate;
	int32_t IMUZRate;
	int32_t IMUXAcc;
	int32_t IMUYAcc;
	int32_t IMUZAcc;
	int8_t IMUTemperature;
	int16_t IMUStatus;
};

PACKED_STRUCT(GPSDataStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int32_t Longitude;
	int32_t Latitude;
	int32_t Altitude;
	int16_t Azimuth;
	uint32_t Second;
	uint32_t milliseconds;
	int8_t NumerOfSatellites;
	bool PositionValid : 1;
	uint8_t Spare0 : 7;
};

PACKED_STRUCT(NavigationRecordStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t RecordLevel;
};

PACKED_STRUCT(GPSRecordingStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t GPSRecordingLevel;
	int32_t GPSFreeMemory;
};

PACKED_STRUCT(NavigationBITStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool NavigationaccuracyWarning : 1;
	bool GPSCommFail : 1;
	bool GPSprecisionFail : 1;
	bool GPSantennaFail : 1;
	bool IMUFail : 1;
	bool PPSFail : 1;
	bool CalibrationFail : 1;
	uint8_t Spare0 : 1;
};

PACKED_STRUCT(DesignatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	uint8_t Spare0 : 7;
	bool Arm : 1;
	uint8_t Spare1 : 7;
	uint8_t Spare2;
	uint8_t Spare3;
};

PACKED_STRUCT(DesignatorStatusStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	bool Arm : 1;
	uint8_t Spare0 : 3;
	bool Ready : 1;
	bool SafetySwitch : 1;
	bool Enable : 1;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
};

PACKED_STRUCT(DesignatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0 : 4;
	bool Fire : 1;
	uint8_t Spare1 : 3;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
};

PACKED_STRUCT(DesignatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0 : 4;
	bool Fire : 1;
	uint8_t Spare1 : 2;
	bool FireBlocked : 1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
};

PACKED_STRUCT(DesignatorNATOStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint16_t CodeValue;
};

PACKED_STRUCT(DesignatorNATOStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t Spare0;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
	uint8_t Spare4;
	uint8_t Spare5;
	uint16_t CodeValue;
};

PACKED_STRUCT(IlluminatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	uint8_t Spare0 : 7;
	bool Arm : 1;
	uint8_t Spare1 : 7;
};

PACKED_STRUCT(IlluminatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Power : 1;
	uint8_t Spare0 : 7;
	bool Arm : 1;
	uint8_t Spare1 : 7;
};

PACKED_STRUCT(IlluminatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t Mode : 4;
	bool Fire : 1;
	uint8_t Spare0 : 3;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
};

PACKED_STRUCT(IlluminatorStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t Mode : 4;
	bool Fire : 1;
	uint8_t Spare0 : 3;
	uint8_t Spare1;
	uint8_t Spare2;
	uint8_t Spare3;
};

PACKED_STRUCT(KeepAliveStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
};

PACKED_STRUCT(GetDATAStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint16_t Data;
	int8_t RetreiveType;
};

PACKED_STRUCT(SystemGeneralReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t OperationMode;
	uint8_t Spare0 : 1;
	bool HybridMode : 1;
	uint8_t Spare1 : 6;
	int8_t HiveTemperature;
	uint8_t Spare2;
};

PACKED_STRUCT(LogRecordCommandStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Enable : 1;
	bool ErrorLevel : 1;
	bool InfoLevel : 1;
	bool DebugLevel : 1;
	uint8_t Spare0 : 4;
	char FreeText[20];
};

PACKED_STRUCT(LogRecordReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool Enable : 1;
	bool ErrorLevel : 1;
	bool InfoLevel : 1;
	bool DebugLevel : 1;
	uint8_t Spare0 : 4;
	char FileName[15];
};

PACKED_STRUCT(CameraPowerControlStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint8_t DayLightCameraPower : 2;
	uint8_t Spare0 : 6;
	uint8_t ThermalCameraPower : 2;
	uint8_t Spare1 : 6;
};

PACKED_STRUCT(CameraPowerReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	bool WideDTVState : 1;
	bool NarrowDTVState : 1;
	bool TIStateState : 1;
	uint8_t Spare0 : 5;
};

PACKED_STRUCT(TimeTagReportStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint64_t SystemTimeTag;
	uint64_t ControlTimeTag;
};

PACKED_STRUCT(SetSystemClockStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Spare0;
};

PACKED_STRUCT(CurrentSystemTimeStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
};

PACKED_STRUCT(SystemVersionStruct) 
{
	uint16_t KeyID;
	uint8_t Length;
	uint8_t SubMessageID;
	int8_t Bundle;
	char CPU[16];
	uint32_t Control;
	char DTVLibrary[16];
	char ECVML[16];
	char SensorsWide[16];
	char SensorsNarrow[16];
	char SensorsThermal[16];
	char Designator[16];
	char Illuminator[16];
	char SensorsSeeSpot[16];
};



typedef struct {
    unsigned short port;
    char ip[16];
} udp_addr_t;


/*Default Is Set all Fields To True, So SyncRemote() Will Allways Send All Data*/
struct MessagesTransmitEnableStruct MessagesTransmitEnable;

struct SystemInitStruct SystemInit_obj;
struct IDLEStruct IDLE_obj;
struct DriftCalibrationStruct DriftCalibration_obj;
struct RateCommandStruct RateCommand_obj;
struct PositionCommandStruct PositionCommand_obj;
struct CenterModeStruct CenterMode_obj;
struct SafeModeStruct SafeMode_obj;
struct PositionReportStruct PositionReport_obj;
struct GyroRawReportStruct GyroRawReport_obj;
struct GroundReferenceCommandStruct GroundReferenceCommand_obj;
struct GroundReferenceReportStruct GroundReferenceReport_obj;
struct Rate_GRRStruct Rate_GRR_obj;
struct StabilizationErrorStruct StabilizationError_obj;
struct GimbalControlDataStruct GimbalControlData_obj;
struct GimbalMasterBITStatusStruct GimbalMasterBITStatus_obj;
struct DayCameraCommandStruct DayCameraCommand_obj;
struct DayCameraSetFOVStruct DayCameraSetFOV_obj;
struct DayCameraReportStruct DayCameraReport_obj;
struct ThermalCameraCommandStruct ThermalCameraCommand_obj;
struct TICameraSetFOVStruct TICameraSetFOV_obj;
struct ThermalCameraReportStruct ThermalCameraReport_obj;
struct SeeSpotControlCommandStruct SeeSpotControlCommand_obj;
struct SeeSpotControlCommandStruct SeeSpotControlCommand_obj;
struct ClearStickyBITStruct ClearStickyBIT_obj;
struct CamerasBITStatusStruct CamerasBITStatus_obj;
struct VideoChannelCommandStruct VideoChannelCommand_obj;
struct VideoChannelReportStruct VideoChannelReport_obj;
struct VideoChannelRunTimeConfigStruct VideoChannelRunTimeConfig_obj;
struct RecordVideoCommandStruct RecordVideoCommand_obj;
struct RecordVideoReportStruct RecordVideoReport_obj;
struct TrackStruct Track_obj;
struct TrackerTargetErrorReportStruct TrackerTargetErrorReport_obj;
struct TrackerParametersStruct TrackerParameters_obj;
struct TrackerParametersReportStruct TrackerParametersReport_obj;
struct TrackerAdjustControlsStruct TrackerAdjustControls_obj;
struct TrackerAdjustControlsReportStruct TrackerAdjustControlsReport_obj;
struct TrackerOffsetControlsStruct TrackerOffsetControls_obj;
struct TrackerOffsetControlsReportStruct TrackerOffsetControlsReport_obj;
struct VMDControlsStruct VMDControls_obj;
struct VMDReportStruct VMDReport_obj;
struct ATRControlsStruct ATRControls_obj;
struct ATRReportStruct ATRReport_obj;
struct NavigationInitStruct NavigationInit_obj;
struct NavigationInitStruct NavigationInit_obj;
struct NavigationPropertiesStruct NavigationProperties_obj;
struct NavigationStatusStruct NavigationStatus_obj;
struct PlatformNavigationDataAccuracyStruct PlatformNavigationDataAccuracy_obj;
struct PlatformNavigationDataStruct PlatformNavigationData_obj;
struct PlatformNavigationDataStruct PlatformNavigationData_obj;
struct NavigationIMUDataStruct NavigationIMUData_obj;
struct GPSDataStruct GPSData_obj;
struct NavigationRecordStruct NavigationRecord_obj;
struct GPSRecordingStruct GPSRecording_obj;
struct NavigationBITStruct NavigationBIT_obj;
struct DesignatorStruct Designator_obj;
struct DesignatorStatusStruct DesignatorStatus_obj;
struct DesignatorStruct Designator_obj;
struct DesignatorStruct Designator_obj;
struct DesignatorNATOStruct DesignatorNATO_obj;
struct DesignatorNATOStruct DesignatorNATO_obj;
struct IlluminatorStruct Illuminator_obj;
struct IlluminatorStruct Illuminator_obj;
struct IlluminatorStruct Illuminator_obj;
struct IlluminatorStruct Illuminator_obj;
struct KeepAliveStruct KeepAlive_obj;
struct GetDATAStruct GetDATA_obj;
struct SystemGeneralReportStruct SystemGeneralReport_obj;
struct LogRecordCommandStruct LogRecordCommand_obj;
struct LogRecordReportStruct LogRecordReport_obj;
struct CameraPowerControlStruct CameraPowerControl_obj;
struct CameraPowerReportStruct CameraPowerReport_obj;
struct TimeTagReportStruct TimeTagReport_obj;
struct SetSystemClockStruct SetSystemClock_obj;
struct CurrentSystemTimeStruct CurrentSystemTime_obj;
struct SystemVersionStruct SystemVersion_obj;
uint8_t RXBuff[809*8] = {0};
uint8_t TXBuff[412*4] = {0};




/*----- GenericFunctions -----*/

void InitializeMessagesBuffers()
{
SystemInit_obj.KeyID = 1000;
SystemInit_obj.Length = 4;
SystemInit_obj.SubMessageID = 0;

IDLE_obj.KeyID = 1016;
IDLE_obj.Length = 4;
IDLE_obj.SubMessageID = 0;

DriftCalibration_obj.KeyID = 1006;
DriftCalibration_obj.Length = 4;
DriftCalibration_obj.SubMessageID = 0;

RateCommand_obj.KeyID = 1210;
RateCommand_obj.Length = 8;
RateCommand_obj.SubMessageID = 0;

PositionCommand_obj.KeyID = 1213;
PositionCommand_obj.Length = 8;
PositionCommand_obj.SubMessageID = 0;

CenterMode_obj.KeyID = 1222;
CenterMode_obj.Length = 4;
CenterMode_obj.SubMessageID = 0;

SafeMode_obj.KeyID = 1234;
SafeMode_obj.Length = 4;
SafeMode_obj.SubMessageID = 0;

PositionReport_obj.KeyID = 31213;
PositionReport_obj.Length = 8;
PositionReport_obj.SubMessageID = 0;

GyroRawReport_obj.KeyID = 31211;
GyroRawReport_obj.Length = 24;
GyroRawReport_obj.SubMessageID = 0;

GroundReferenceCommand_obj.KeyID = 7217;
GroundReferenceCommand_obj.Length = 12;
GroundReferenceCommand_obj.SubMessageID = 0;

GroundReferenceReport_obj.KeyID = 37217;
GroundReferenceReport_obj.Length = 12;
GroundReferenceReport_obj.SubMessageID = 0;

Rate_GRR_obj.KeyID = 34000;
Rate_GRR_obj.Length = 8;
Rate_GRR_obj.SubMessageID = 0;

StabilizationError_obj.KeyID = 34001;
StabilizationError_obj.Length = 8;
StabilizationError_obj.SubMessageID = 0;

GimbalControlData_obj.KeyID = 34002;
GimbalControlData_obj.Length = 20;
GimbalControlData_obj.SubMessageID = 0;

GimbalMasterBITStatus_obj.KeyID = 34996;
GimbalMasterBITStatus_obj.Length = 7;
GimbalMasterBITStatus_obj.SubMessageID = 0;

DayCameraCommand_obj.KeyID = 5600;
DayCameraCommand_obj.Length = 20;
DayCameraCommand_obj.SubMessageID = 0;

DayCameraSetFOV_obj.KeyID = 5601;
DayCameraSetFOV_obj.Length = 8;
DayCameraSetFOV_obj.SubMessageID = 0;

DayCameraReport_obj.KeyID = 35600;
DayCameraReport_obj.Length = 60;
DayCameraReport_obj.SubMessageID = 0;

ThermalCameraCommand_obj.KeyID = 5700;
ThermalCameraCommand_obj.Length = 20;
ThermalCameraCommand_obj.SubMessageID = 0;

TICameraSetFOV_obj.KeyID = 5701;
TICameraSetFOV_obj.Length = 8;
TICameraSetFOV_obj.SubMessageID = 0;

ThermalCameraReport_obj.KeyID = 35700;
ThermalCameraReport_obj.Length = 44;
ThermalCameraReport_obj.SubMessageID = 0;

SeeSpotControlCommand_obj.KeyID = 5400;
SeeSpotControlCommand_obj.Length = 20;
SeeSpotControlCommand_obj.SubMessageID = 0;

SeeSpotControlCommand_obj.KeyID = 35400;
SeeSpotControlCommand_obj.Length = 20;
SeeSpotControlCommand_obj.SubMessageID = 0;

ClearStickyBIT_obj.KeyID = 5999;
ClearStickyBIT_obj.Length = 5;
ClearStickyBIT_obj.SubMessageID = 0;

CamerasBITStatus_obj.KeyID = 35999;
CamerasBITStatus_obj.Length = 12;
CamerasBITStatus_obj.SubMessageID = 0;

VideoChannelCommand_obj.KeyID = 22851;
VideoChannelCommand_obj.Length = 8;
VideoChannelCommand_obj.SubMessageID = 0;

VideoChannelReport_obj.KeyID = 52851;
VideoChannelReport_obj.Length = 8;
VideoChannelReport_obj.SubMessageID = 0;

VideoChannelRunTimeConfig_obj.KeyID = 22800;
VideoChannelRunTimeConfig_obj.Length = 10;
VideoChannelRunTimeConfig_obj.SubMessageID = 0;

RecordVideoCommand_obj.KeyID = 21000;
RecordVideoCommand_obj.Length = 25;
RecordVideoCommand_obj.SubMessageID = 0;

RecordVideoReport_obj.KeyID = 51000;
RecordVideoReport_obj.Length = 68;
RecordVideoReport_obj.SubMessageID = 0;

Track_obj.KeyID = 6500;
Track_obj.Length = 8;
Track_obj.SubMessageID = 0;

TrackerTargetErrorReport_obj.KeyID = 36526;
TrackerTargetErrorReport_obj.Length = 21;
TrackerTargetErrorReport_obj.SubMessageID = 0;

TrackerParameters_obj.KeyID = 6510;
TrackerParameters_obj.Length = 12;
TrackerParameters_obj.SubMessageID = 0;

TrackerParametersReport_obj.KeyID = 36510;
TrackerParametersReport_obj.Length = 11;
TrackerParametersReport_obj.SubMessageID = 0;

TrackerAdjustControls_obj.KeyID = 6513;
TrackerAdjustControls_obj.Length = 9;
TrackerAdjustControls_obj.SubMessageID = 0;

TrackerAdjustControlsReport_obj.KeyID = 36513;
TrackerAdjustControlsReport_obj.Length = 9;
TrackerAdjustControlsReport_obj.SubMessageID = 0;

TrackerOffsetControls_obj.KeyID = 6514;
TrackerOffsetControls_obj.Length = 13;
TrackerOffsetControls_obj.SubMessageID = 0;

TrackerOffsetControlsReport_obj.KeyID = 36514;
TrackerOffsetControlsReport_obj.Length = 9;
TrackerOffsetControlsReport_obj.SubMessageID = 0;

VMDControls_obj.KeyID = 6551;
VMDControls_obj.Length = 12;
VMDControls_obj.SubMessageID = 0;

VMDReport_obj.KeyID = 36551;
VMDReport_obj.Length = 6;
VMDReport_obj.SubMessageID = 0;

ATRControls_obj.KeyID = 6561;
ATRControls_obj.Length = 12;
ATRControls_obj.SubMessageID = 0;

ATRReport_obj.KeyID = 36561;
ATRReport_obj.Length = 6;
ATRReport_obj.SubMessageID = 0;

NavigationInit_obj.KeyID = 7015;
NavigationInit_obj.Length = 22;
NavigationInit_obj.SubMessageID = 0;

NavigationInit_obj.KeyID = 37015;
NavigationInit_obj.Length = 22;
NavigationInit_obj.SubMessageID = 0;

NavigationProperties_obj.KeyID = 7000;
NavigationProperties_obj.Length = 5;
NavigationProperties_obj.SubMessageID = 0;

NavigationStatus_obj.KeyID = 37000;
NavigationStatus_obj.Length = 34;
NavigationStatus_obj.SubMessageID = 0;

PlatformNavigationDataAccuracy_obj.KeyID = 7010;
PlatformNavigationDataAccuracy_obj.Length = 6;
PlatformNavigationDataAccuracy_obj.SubMessageID = 0;

PlatformNavigationData_obj.KeyID = 7011;
PlatformNavigationData_obj.Length = 34;
PlatformNavigationData_obj.SubMessageID = 0;

PlatformNavigationData_obj.KeyID = 37011;
PlatformNavigationData_obj.Length = 46;
PlatformNavigationData_obj.SubMessageID = 0;

NavigationIMUData_obj.KeyID = 37050;
NavigationIMUData_obj.Length = 39;
NavigationIMUData_obj.SubMessageID = 0;

GPSData_obj.KeyID = 37801;
GPSData_obj.Length = 28;
GPSData_obj.SubMessageID = 0;

NavigationRecord_obj.KeyID = 7017;
NavigationRecord_obj.Length = 5;
NavigationRecord_obj.SubMessageID = 0;

GPSRecording_obj.KeyID = 37017;
GPSRecording_obj.Length = 9;
GPSRecording_obj.SubMessageID = 0;

NavigationBIT_obj.KeyID = 37010;
NavigationBIT_obj.Length = 5;
NavigationBIT_obj.SubMessageID = 0;

Designator_obj.KeyID = 8450;
Designator_obj.Length = 8;
Designator_obj.SubMessageID = 0;

DesignatorStatus_obj.KeyID = 38450;
DesignatorStatus_obj.Length = 8;
DesignatorStatus_obj.SubMessageID = 0;

Designator_obj.KeyID = 8451;
Designator_obj.Length = 8;
Designator_obj.SubMessageID = 0;

Designator_obj.KeyID = 38451;
Designator_obj.Length = 8;
Designator_obj.SubMessageID = 0;

DesignatorNATO_obj.KeyID = 8455;
DesignatorNATO_obj.Length = 6;
DesignatorNATO_obj.SubMessageID = 0;

DesignatorNATO_obj.KeyID = 38455;
DesignatorNATO_obj.Length = 6;
DesignatorNATO_obj.SubMessageID = 0;

Illuminator_obj.KeyID = 8500;
Illuminator_obj.Length = 6;
Illuminator_obj.SubMessageID = 0;

Illuminator_obj.KeyID = 38500;
Illuminator_obj.Length = 6;
Illuminator_obj.SubMessageID = 0;

Illuminator_obj.KeyID = 8501;
Illuminator_obj.Length = 8;
Illuminator_obj.SubMessageID = 0;

Illuminator_obj.KeyID = 38501;
Illuminator_obj.Length = 8;
Illuminator_obj.SubMessageID = 0;

KeepAlive_obj.KeyID = 0;
KeepAlive_obj.Length = 4;
KeepAlive_obj.SubMessageID = 0;

GetDATA_obj.KeyID = 144;
GetDATA_obj.Length = 7;
GetDATA_obj.SubMessageID = 0;

SystemGeneralReport_obj.KeyID = 50901;
SystemGeneralReport_obj.Length = 8;
SystemGeneralReport_obj.SubMessageID = 0;

LogRecordCommand_obj.KeyID = 20000;
LogRecordCommand_obj.Length = 25;
LogRecordCommand_obj.SubMessageID = 0;

LogRecordReport_obj.KeyID = 50000;
LogRecordReport_obj.Length = 20;
LogRecordReport_obj.SubMessageID = 0;

CameraPowerControl_obj.KeyID = 20500;
CameraPowerControl_obj.Length = 6;
CameraPowerControl_obj.SubMessageID = 0;

CameraPowerReport_obj.KeyID = 50500;
CameraPowerReport_obj.Length = 5;
CameraPowerReport_obj.SubMessageID = 0;

TimeTagReport_obj.KeyID = 50903;
TimeTagReport_obj.Length = 20;
TimeTagReport_obj.SubMessageID = 0;

SetSystemClock_obj.KeyID = 20005;
SetSystemClock_obj.Length = 12;
SetSystemClock_obj.SubMessageID = 0;

CurrentSystemTime_obj.KeyID = 50005;
CurrentSystemTime_obj.Length = 11;
CurrentSystemTime_obj.SubMessageID = 0;

SystemVersion_obj.KeyID = 50999;
SystemVersion_obj.Length = 153;
SystemVersion_obj.SubMessageID = 0;

}


uint16_t CheckSum16(uint8_t* ptr, int len)
{
    uint16_t CRC = 0;
    for(int i = 0; i < len ; i++)
    {
        CRC = CRC + *(ptr + i);
    }
    return CRC;
}
/*----- CommunicationProtocolFunctions -----*/

#ifdef _WIN32


SOCKET SockFD;

int udp_init(SOCKET *sockfd) {
    WSADATA wsaData;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }

    // Create a UDP socket
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        WSACleanup();
        return -1;
    }

    // Bind the socket to a port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
    server_addr.sin_port = htons(LOCAL_PORT);

    if (bind(*sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(*sockfd);
        WSACleanup();
        return -1;
    }

    return 0; // Success
}



int udp_send(SOCKET sockfd, const char *ip, int port, const void *buffer, size_t buflen) {
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0) {
        return -1;  // Invalid address error
    }

    // Send the buffer
    SSIZE_T sent_bytes = sendto(sockfd, buffer, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent_bytes == SOCKET_ERROR) {
        return -1;  // Error during sending
    }

    return sent_bytes;  // Number of bytes sent
}



int udp_receive(SOCKET sockfd, void *buffer, size_t buflen, udp_addr_t *addr, uint16_t timeout_ms) {
    // Set up the timeout for the socket
    DWORD timeout = (DWORD)timeout_ms; 
    // Set the socket receive timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        return -1;  // Error in setting option
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(sockfd, buffer, buflen, 0, (struct sockaddr *)&client_addr, &addr_len);
    
    if (bytes_received < 0) {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            return 0; // Timeout occurred
        }
        return -1;  // Error in receiving
    }

    // Populate the udp_addr_t structure
    addr->port = ntohs(client_addr.sin_port);
    inet_ntop(AF_INET, &(client_addr.sin_addr), addr->ip, sizeof(addr->ip));

    return bytes_received; // Return the number of bytes received
}


#else // __linux__


int SockFD;

int udp_init(int *sockfd) {
    // Create a UDP socket
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set the SO_REUSEADDR option
    int reuse = 1;
    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(*sockfd);
        return -1;
    }

    // Bind the socket to a port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
    server_addr.sin_port = htons(LOCAL_PORT); // Use LOCAL_PORT defined above

    if (bind(*sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(*sockfd);
        return -1;
    }

    return 0; // Success
}



int udp_send(int sockfd, const char *ip, int port, const void *buffer, size_t buflen) {
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0) {
        perror("Invalid address / address not supported");
        return -1;  // Invalid address error
    }

    // Send the buffer
    ssize_t sent_bytes = sendto(sockfd, buffer, buflen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent_bytes < 0) {
        perror("sendto failed");
        return -1;  // Error during sending
    }

    return sent_bytes;  // Number of bytes sent
}



int udp_receive(int sockfd, void *buffer, size_t buflen, udp_addr_t *addr, uint16_t timeout_ms) {
    fd_set readfds;
    struct timeval tv;

    // Clear the fd_set and set the socket
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    // Set the timeout value
    tv.tv_sec = timeout_ms / 1000;         // seconds
    tv.tv_usec = (timeout_ms % 1000) * 1000; // microseconds

    // Wait for data to be received or timeout
    int ret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    
    if (ret < 0) {
        perror("select error");
        return -1;  // Error in select
    } else if (ret == 0) {
        // Timeout
        return 0;   // No data received
    }

    // Data is available, receive it
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int bytes_received = recvfrom(sockfd, buffer, buflen, 0, (struct sockaddr *)&client_addr, &addr_len);
    
    if (bytes_received < 0) {
        perror("recvfrom error");
        return -1;  // Error in receiving
    }

    // Populate the udp_addr_t structure
    addr->port = ntohs(client_addr.sin_port);
    inet_ntop(AF_INET, &(client_addr.sin_addr), addr->ip, sizeof(addr->ip));

    return bytes_received; // Return the number of bytes received
}


#endif

int STKInitializeCommunication(struct MessagesTransmitEnableStruct data)
{
    if (udp_init(&SockFD) != 0)
    {
        return -1;
    }
    InitializeMessagesBuffers();
    MessagesTransmitEnable.EnableTXSystemInit = data.EnableTXSystemInit ;
    MessagesTransmitEnable.EnableTXIDLE = data.EnableTXIDLE ;
    MessagesTransmitEnable.EnableTXDriftCalibration = data.EnableTXDriftCalibration ;
    MessagesTransmitEnable.EnableTXRateCommand = data.EnableTXRateCommand ;
    MessagesTransmitEnable.EnableTXPositionCommand = data.EnableTXPositionCommand ;
    MessagesTransmitEnable.EnableTXCenterMode = data.EnableTXCenterMode ;
    MessagesTransmitEnable.EnableTXSafeMode = data.EnableTXSafeMode ;
    MessagesTransmitEnable.EnableTXGroundReferenceCommand = data.EnableTXGroundReferenceCommand ;
    MessagesTransmitEnable.EnableTXDayCameraCommand = data.EnableTXDayCameraCommand ;
    MessagesTransmitEnable.EnableTXDayCameraSetFOV = data.EnableTXDayCameraSetFOV ;
    MessagesTransmitEnable.EnableTXThermalCameraCommand = data.EnableTXThermalCameraCommand ;
    MessagesTransmitEnable.EnableTXTICameraSetFOV = data.EnableTXTICameraSetFOV ;
    MessagesTransmitEnable.EnableTXSeeSpotControlCommand = data.EnableTXSeeSpotControlCommand ;
    MessagesTransmitEnable.EnableTXClearStickyBIT = data.EnableTXClearStickyBIT ;
    MessagesTransmitEnable.EnableTXVideoChannelCommand = data.EnableTXVideoChannelCommand ;
    MessagesTransmitEnable.EnableTXVideoChannelRunTimeConfig = data.EnableTXVideoChannelRunTimeConfig ;
    MessagesTransmitEnable.EnableTXRecordVideoCommand = data.EnableTXRecordVideoCommand ;
    MessagesTransmitEnable.EnableTXTrack = data.EnableTXTrack ;
    MessagesTransmitEnable.EnableTXTrackerParameters = data.EnableTXTrackerParameters ;
    MessagesTransmitEnable.EnableTXTrackerAdjustControls = data.EnableTXTrackerAdjustControls ;
    MessagesTransmitEnable.EnableTXTrackerOffsetControls = data.EnableTXTrackerOffsetControls ;
    MessagesTransmitEnable.EnableTXVMDControls = data.EnableTXVMDControls ;
    MessagesTransmitEnable.EnableTXATRControls = data.EnableTXATRControls ;
    MessagesTransmitEnable.EnableTXNavigationInit = data.EnableTXNavigationInit ;
    MessagesTransmitEnable.EnableTXNavigationProperties = data.EnableTXNavigationProperties ;
    MessagesTransmitEnable.EnableTXPlatformNavigationDataAccuracy = data.EnableTXPlatformNavigationDataAccuracy ;
    MessagesTransmitEnable.EnableTXPlatformNavigationData = data.EnableTXPlatformNavigationData ;
    MessagesTransmitEnable.EnableTXNavigationRecord = data.EnableTXNavigationRecord ;
    MessagesTransmitEnable.EnableTXDesignator = data.EnableTXDesignator ;
    MessagesTransmitEnable.EnableTXDesignator = data.EnableTXDesignator ;
    MessagesTransmitEnable.EnableTXDesignatorNATO = data.EnableTXDesignatorNATO ;
    MessagesTransmitEnable.EnableTXIlluminator = data.EnableTXIlluminator ;
    MessagesTransmitEnable.EnableTXIlluminator = data.EnableTXIlluminator ;
    MessagesTransmitEnable.EnableTXKeepAlive = data.EnableTXKeepAlive ;
    MessagesTransmitEnable.EnableTXGetDATA = data.EnableTXGetDATA ;
    MessagesTransmitEnable.EnableTXLogRecordCommand = data.EnableTXLogRecordCommand ;
    MessagesTransmitEnable.EnableTXCameraPowerControl = data.EnableTXCameraPowerControl ;
    MessagesTransmitEnable.EnableTXSetSystemClock = data.EnableTXSetSystemClock ;
    return 0;
}


enum SyncLocalErrorEnum STKSyncLocal(uint16_t timeout)
{
    void *RxPtr = (void*)&RXBuff;
    udp_addr_t addr_recv;
    int BytesReceived = udp_receive(SockFD, RxPtr, sizeof(RXBuff), &addr_recv, timeout);
    if (BytesReceived <= 0) //Check For Errors On RX
    {
        return ERR_UDP_RX;
    }
    if (RXBuff[0] != 0x47 || RXBuff[1] != 0x52) //Check For Wrong Key
    {
        return ERR_HEADER_KEY;
    }
    uint16_t PacketLen = RXBuff[3] + (RXBuff[2] <<8);
    if (PacketLen != BytesReceived) //Check Wrong Packet Size
    {
        return ERR_LEN_MISSMATCH;
    } 
    uint16_t CRCReceived = RXBuff[5] + (RXBuff[4] <<8);
    RXBuff[4] = 0;
    RXBuff[5] = 0;
    uint16_t CRCCalculated =  CheckSum16(RxPtr, BytesReceived);
    if (CRCReceived != CRCCalculated) //Check For Wrong CRC
    {
        return ERR_CRC_MISSMATCH;
    }

    //All Tests For Packet Arrived Are OK, Lets Parse messaged
    
    
    uint32_t idx = 8;

    while (idx < PacketLen)
    {
        uint16_t TempMsgId = RXBuff[idx] + (RXBuff[idx + 1] << 8);
        uint16_t TempMsgSize = RXBuff[idx + 2] + (RXBuff[idx + 3] << 8);
        
        size_t BufSize = 0;

        if (TempMsgSize == 0)
        {
            break;
        }
        else
        {
            switch (TempMsgId)
            {
                case 31213:
                    memcpy(&PositionReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(PositionReport_obj);
                    break;
                case 31211:
                    memcpy(&GyroRawReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GyroRawReport_obj);
                    break;
                case 37217:
                    memcpy(&GroundReferenceReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GroundReferenceReport_obj);
                    break;
                case 34000:
                    memcpy(&Rate_GRR_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(Rate_GRR_obj);
                    break;
                case 34001:
                    memcpy(&StabilizationError_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(StabilizationError_obj);
                    break;
                case 34002:
                    memcpy(&GimbalControlData_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GimbalControlData_obj);
                    break;
                case 34996:
                    memcpy(&GimbalMasterBITStatus_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GimbalMasterBITStatus_obj);
                    break;
                case 35600:
                    memcpy(&DayCameraReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(DayCameraReport_obj);
                    break;
                case 35700:
                    memcpy(&ThermalCameraReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(ThermalCameraReport_obj);
                    break;
                case 35400:
                    memcpy(&SeeSpotControlCommand_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(SeeSpotControlCommand_obj);
                    break;
                case 35999:
                    memcpy(&CamerasBITStatus_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(CamerasBITStatus_obj);
                    break;
                case 52851:
                    memcpy(&VideoChannelReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(VideoChannelReport_obj);
                    break;
                case 51000:
                    memcpy(&RecordVideoReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(RecordVideoReport_obj);
                    break;
                case 36526:
                    memcpy(&TrackerTargetErrorReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(TrackerTargetErrorReport_obj);
                    break;
                case 36510:
                    memcpy(&TrackerParametersReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(TrackerParametersReport_obj);
                    break;
                case 36513:
                    memcpy(&TrackerAdjustControlsReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(TrackerAdjustControlsReport_obj);
                    break;
                case 36514:
                    memcpy(&TrackerOffsetControlsReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(TrackerOffsetControlsReport_obj);
                    break;
                case 36551:
                    memcpy(&VMDReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(VMDReport_obj);
                    break;
                case 36561:
                    memcpy(&ATRReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(ATRReport_obj);
                    break;
                case 37015:
                    memcpy(&NavigationInit_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(NavigationInit_obj);
                    break;
                case 37000:
                    memcpy(&NavigationStatus_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(NavigationStatus_obj);
                    break;
                case 37011:
                    memcpy(&PlatformNavigationData_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(PlatformNavigationData_obj);
                    break;
                case 37050:
                    memcpy(&NavigationIMUData_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(NavigationIMUData_obj);
                    break;
                case 37801:
                    memcpy(&GPSData_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GPSData_obj);
                    break;
                case 37017:
                    memcpy(&GPSRecording_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(GPSRecording_obj);
                    break;
                case 37010:
                    memcpy(&NavigationBIT_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(NavigationBIT_obj);
                    break;
                case 38450:
                    memcpy(&DesignatorStatus_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(DesignatorStatus_obj);
                    break;
                case 38451:
                    memcpy(&Designator_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(Designator_obj);
                    break;
                case 38455:
                    memcpy(&DesignatorNATO_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(DesignatorNATO_obj);
                    break;
                case 38500:
                    memcpy(&Illuminator_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(Illuminator_obj);
                    break;
                case 38501:
                    memcpy(&Illuminator_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(Illuminator_obj);
                    break;
                case 50901:
                    memcpy(&SystemGeneralReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(SystemGeneralReport_obj);
                    break;
                case 50000:
                    memcpy(&LogRecordReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(LogRecordReport_obj);
                    break;
                case 50500:
                    memcpy(&CameraPowerReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(CameraPowerReport_obj);
                    break;
                case 50903:
                    memcpy(&TimeTagReport_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(TimeTagReport_obj);
                    break;
                case 50005:
                    memcpy(&CurrentSystemTime_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(CurrentSystemTime_obj);
                    break;
                case 50999:
                    memcpy(&SystemVersion_obj, &RXBuff[idx], TempMsgSize );
                    BufSize=sizeof(SystemVersion_obj);
                    break;

            }
            idx += TempMsgSize;
            
            if (BufSize < TempMsgSize)
            {
                if (BufSize == 0) {
                    DBG_PRINT("Illegal code: %u\n", TempMsgId);
                } else {
                    DBG_PRINT("ERROR in %d: MessageSize: %u, BufferSize: %lu\n", TempMsgId, TempMsgSize, BufSize);
                }
                fflush(stdout);
            }
        }
    } 
        return ERR_NO_ERROR;
}
int STKUpdateRemote()
{

    int idx = 0;
    uint8_t* ptr = &TXBuff[0];
    *ptr = 0x52;
    *(ptr + ++idx) = 0x47;
    memset((ptr + ++idx), 0, 6);
    idx += 6;
    if (MessagesTransmitEnable.EnableTXSystemInit)
    {
        memcpy((ptr + idx), &SystemInit_obj, sizeof(SystemInit_obj));
        idx += sizeof(SystemInit_obj);
    }
if (MessagesTransmitEnable.EnableTXIDLE)
    {
        memcpy((ptr + idx), &IDLE_obj, sizeof(IDLE_obj));
        idx += sizeof(IDLE_obj);
    }
if (MessagesTransmitEnable.EnableTXDriftCalibration)
    {
        memcpy((ptr + idx), &DriftCalibration_obj, sizeof(DriftCalibration_obj));
        idx += sizeof(DriftCalibration_obj);
    }
if (MessagesTransmitEnable.EnableTXRateCommand)
    {
        memcpy((ptr + idx), &RateCommand_obj, sizeof(RateCommand_obj));
        idx += sizeof(RateCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXPositionCommand)
    {
        memcpy((ptr + idx), &PositionCommand_obj, sizeof(PositionCommand_obj));
        idx += sizeof(PositionCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXCenterMode)
    {
        memcpy((ptr + idx), &CenterMode_obj, sizeof(CenterMode_obj));
        idx += sizeof(CenterMode_obj);
    }
if (MessagesTransmitEnable.EnableTXSafeMode)
    {
        memcpy((ptr + idx), &SafeMode_obj, sizeof(SafeMode_obj));
        idx += sizeof(SafeMode_obj);
    }
if (MessagesTransmitEnable.EnableTXGroundReferenceCommand)
    {
        memcpy((ptr + idx), &GroundReferenceCommand_obj, sizeof(GroundReferenceCommand_obj));
        idx += sizeof(GroundReferenceCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXDayCameraCommand)
    {
        memcpy((ptr + idx), &DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
        idx += sizeof(DayCameraCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXDayCameraSetFOV)
    {
        memcpy((ptr + idx), &DayCameraSetFOV_obj, sizeof(DayCameraSetFOV_obj));
        idx += sizeof(DayCameraSetFOV_obj);
    }
if (MessagesTransmitEnable.EnableTXThermalCameraCommand)
    {
        memcpy((ptr + idx), &ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
        idx += sizeof(ThermalCameraCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXTICameraSetFOV)
    {
        memcpy((ptr + idx), &TICameraSetFOV_obj, sizeof(TICameraSetFOV_obj));
        idx += sizeof(TICameraSetFOV_obj);
    }
if (MessagesTransmitEnable.EnableTXSeeSpotControlCommand)
    {
        memcpy((ptr + idx), &SeeSpotControlCommand_obj, sizeof(SeeSpotControlCommand_obj));
        idx += sizeof(SeeSpotControlCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXClearStickyBIT)
    {
        memcpy((ptr + idx), &ClearStickyBIT_obj, sizeof(ClearStickyBIT_obj));
        idx += sizeof(ClearStickyBIT_obj);
    }
if (MessagesTransmitEnable.EnableTXVideoChannelCommand)
    {
        memcpy((ptr + idx), &VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
        idx += sizeof(VideoChannelCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXVideoChannelRunTimeConfig)
    {
        memcpy((ptr + idx), &VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
        idx += sizeof(VideoChannelRunTimeConfig_obj);
    }
if (MessagesTransmitEnable.EnableTXRecordVideoCommand)
    {
        memcpy((ptr + idx), &RecordVideoCommand_obj, sizeof(RecordVideoCommand_obj));
        idx += sizeof(RecordVideoCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXTrack)
    {
        memcpy((ptr + idx), &Track_obj, sizeof(Track_obj));
        idx += sizeof(Track_obj);
    }
if (MessagesTransmitEnable.EnableTXTrackerParameters)
    {
        memcpy((ptr + idx), &TrackerParameters_obj, sizeof(TrackerParameters_obj));
        idx += sizeof(TrackerParameters_obj);
    }
if (MessagesTransmitEnable.EnableTXTrackerAdjustControls)
    {
        memcpy((ptr + idx), &TrackerAdjustControls_obj, sizeof(TrackerAdjustControls_obj));
        idx += sizeof(TrackerAdjustControls_obj);
    }
if (MessagesTransmitEnable.EnableTXTrackerOffsetControls)
    {
        memcpy((ptr + idx), &TrackerOffsetControls_obj, sizeof(TrackerOffsetControls_obj));
        idx += sizeof(TrackerOffsetControls_obj);
    }
if (MessagesTransmitEnable.EnableTXVMDControls)
    {
        memcpy((ptr + idx), &VMDControls_obj, sizeof(VMDControls_obj));
        idx += sizeof(VMDControls_obj);
    }
if (MessagesTransmitEnable.EnableTXATRControls)
    {
        memcpy((ptr + idx), &ATRControls_obj, sizeof(ATRControls_obj));
        idx += sizeof(ATRControls_obj);
    }
if (MessagesTransmitEnable.EnableTXNavigationInit)
    {
        memcpy((ptr + idx), &NavigationInit_obj, sizeof(NavigationInit_obj));
        idx += sizeof(NavigationInit_obj);
    }
if (MessagesTransmitEnable.EnableTXNavigationProperties)
    {
        memcpy((ptr + idx), &NavigationProperties_obj, sizeof(NavigationProperties_obj));
        idx += sizeof(NavigationProperties_obj);
    }
if (MessagesTransmitEnable.EnableTXPlatformNavigationDataAccuracy)
    {
        memcpy((ptr + idx), &PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
        idx += sizeof(PlatformNavigationDataAccuracy_obj);
    }
if (MessagesTransmitEnable.EnableTXPlatformNavigationData)
    {
        memcpy((ptr + idx), &PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
        idx += sizeof(PlatformNavigationData_obj);
    }
if (MessagesTransmitEnable.EnableTXNavigationRecord)
    {
        memcpy((ptr + idx), &NavigationRecord_obj, sizeof(NavigationRecord_obj));
        idx += sizeof(NavigationRecord_obj);
    }
if (MessagesTransmitEnable.EnableTXDesignator)
    {
        memcpy((ptr + idx), &Designator_obj, sizeof(Designator_obj));
        idx += sizeof(Designator_obj);
    }
if (MessagesTransmitEnable.EnableTXDesignator)
    {
        memcpy((ptr + idx), &Designator_obj, sizeof(Designator_obj));
        idx += sizeof(Designator_obj);
    }
if (MessagesTransmitEnable.EnableTXDesignatorNATO)
    {
        memcpy((ptr + idx), &DesignatorNATO_obj, sizeof(DesignatorNATO_obj));
        idx += sizeof(DesignatorNATO_obj);
    }
if (MessagesTransmitEnable.EnableTXIlluminator)
    {
        memcpy((ptr + idx), &Illuminator_obj, sizeof(Illuminator_obj));
        idx += sizeof(Illuminator_obj);
    }
if (MessagesTransmitEnable.EnableTXIlluminator)
    {
        memcpy((ptr + idx), &Illuminator_obj, sizeof(Illuminator_obj));
        idx += sizeof(Illuminator_obj);
    }
if (MessagesTransmitEnable.EnableTXKeepAlive)
    {
        memcpy((ptr + idx), &KeepAlive_obj, sizeof(KeepAlive_obj));
        idx += sizeof(KeepAlive_obj);
    }
if (MessagesTransmitEnable.EnableTXGetDATA)
    {
        memcpy((ptr + idx), &GetDATA_obj, sizeof(GetDATA_obj));
        idx += sizeof(GetDATA_obj);
    }
if (MessagesTransmitEnable.EnableTXLogRecordCommand)
    {
        memcpy((ptr + idx), &LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
        idx += sizeof(LogRecordCommand_obj);
    }
if (MessagesTransmitEnable.EnableTXCameraPowerControl)
    {
        memcpy((ptr + idx), &CameraPowerControl_obj, sizeof(CameraPowerControl_obj));
        idx += sizeof(CameraPowerControl_obj);
    }
if (MessagesTransmitEnable.EnableTXSetSystemClock)
    {
        memcpy((ptr + idx), &SetSystemClock_obj, sizeof(SetSystemClock_obj));
        idx += sizeof(SetSystemClock_obj);
    }
    memset((ptr + idx), 0, 4);
    idx += 4;
    TXBuff[2] = idx & 0xff;
    TXBuff[3] = (idx & 0xff00) >> 8;
    uint16_t CRC = CheckSum16(ptr, idx);
    TXBuff[4] = CRC & 0xff;
    TXBuff[5] = (CRC & 0xff00) >> 8;
    if(udp_send(SockFD, REMOTE_IP, COMMUNICATION_PORT, ptr, idx) == idx)
    {
        return 0;
    }
    return -1;
}
    
int STKSendMessage(void *BuffPtr, size_t  BuffSize)
{
    int idx = 0;
    uint8_t* ptr = &TXBuff[0];
    *ptr = 0x52;
    *(ptr + ++idx) = 0x47;
    *(ptr + ++idx) = (BuffSize + 12) & 0xff;
    *(ptr + ++idx) = ((BuffSize + 12) & 0xff00) >> 8;
    memset(ptr + (++idx), 0, 4);
    idx += 4;
    memcpy((ptr + idx), BuffPtr, BuffSize);
    idx += BuffSize;
    memset(ptr + idx, 0, 4);
    idx += 4;
    uint16_t CRC = CheckSum16(ptr, idx);
    TXBuff[4] = CRC & 0xff;
    TXBuff[5] = (CRC & 0xff00) >> 8;
    if(udp_send(SockFD, REMOTE_IP, COMMUNICATION_PORT, ptr, idx) == idx)
    {
        return 0;
    }
    return -1;
}



/*----- SystemInit 1000 -----*/
int SetSystemInit()
{
	return SendData(&SystemInit_obj, sizeof(SystemInit_obj));
}


/*----- IDLE 1016 -----*/
int SetIDLE()
{
	return SendData(&IDLE_obj, sizeof(IDLE_obj));
}


/*----- DriftCalibration 1006 -----*/
int SetDriftCalibration()
{
	return SendData(&DriftCalibration_obj, sizeof(DriftCalibration_obj));
}


/*----- RateCommand 1210 -----*/
int SetRateCommandAngularVelocities_X(float AngularVelocities_X)
{
	RateCommand_obj.AngularVelocities_X = AngularVelocities_X*VelocityToDeg;
	return SendData(&RateCommand_obj, sizeof(RateCommand_obj));
}

int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y)
{
	RateCommand_obj.AngularVelocities_Y = AngularVelocities_Y*VelocityToDeg;
	return SendData(&RateCommand_obj, sizeof(RateCommand_obj));
}


/*----- PositionCommand 1213 -----*/
int SetPositionCommandLOSAngles_X(float LOSAngles_X)
{
	PositionCommand_obj.LOSAngles_X = LOSAngles_X*AnglesToDeg;
	return SendData(&PositionCommand_obj, sizeof(PositionCommand_obj));
}

int SetPositionCommandLOSAngles_Y(float LOSAngles_Y)
{
	PositionCommand_obj.LOSAngles_Y = LOSAngles_Y*AnglesToDeg;
	return SendData(&PositionCommand_obj, sizeof(PositionCommand_obj));
}


/*----- CenterMode 1222 -----*/
int SetCenterMode()
{
	return SendData(&CenterMode_obj, sizeof(CenterMode_obj));
}


/*----- SafeMode 1234 -----*/
int SetSafeMode()
{
	return SendData(&SafeMode_obj, sizeof(SafeMode_obj));
}


/*----- PositionReport 31213 -----*/
float GetPositionReportRelativeGimbalAngles_X()
{
	return (float)PositionReport_obj.RelativeGimbalAngles_X/AnglesToDeg;
}

float GetPositionReportRelativeGimbalAngles_Y()
{
	return (float)PositionReport_obj.RelativeGimbalAngles_Y/AnglesToDeg;
}


/*----- GyroRawReport 31211 -----*/
uint64_t GetGyroRawReportControlGyroTimeTag()
{
	return GyroRawReport_obj.ControlGyroTimeTag;
}

float GetGyroRawReportGyroYaw()
{
	return GyroRawReport_obj.GyroYaw;
}

float GetGyroRawReportGyroPitch()
{
	return GyroRawReport_obj.GyroPitch;
}

float GetGyroRawReportGyroRoll()
{
	return GyroRawReport_obj.GyroRoll;
}


/*----- GroundReferenceCommand 7217 -----*/
int SetGroundReferenceCommandAngularVelocities_X(float AngularVelocities_X)
{
	GroundReferenceCommand_obj.AngularVelocities_X = AngularVelocities_X*VelocityToDeg;
	return SendData(&GroundReferenceCommand_obj, sizeof(GroundReferenceCommand_obj));
}

int SetGroundReferenceCommandAngularVelocities_Y(float AngularVelocities_Y)
{
	GroundReferenceCommand_obj.AngularVelocities_Y = AngularVelocities_Y*VelocityToDeg;
	return SendData(&GroundReferenceCommand_obj, sizeof(GroundReferenceCommand_obj));
}

int SetGroundReferenceCommandSlantRange(float SlantRange)
{
	GroundReferenceCommand_obj.SlantRange = SlantRange*VelocityToDeg;
	return SendData(&GroundReferenceCommand_obj, sizeof(GroundReferenceCommand_obj));
}


/*----- GroundReferenceReport 37217 -----*/
float GetGroundReferenceReportAngularVelocities_X()
{
	return (float)GroundReferenceReport_obj.AngularVelocities_X/VelocityToDeg;
}

float GetGroundReferenceReportAngularVelocities_Y()
{
	return (float)GroundReferenceReport_obj.AngularVelocities_Y/VelocityToDeg;
}

float GetGroundReferenceReportSlantRange()
{
	return (float)GroundReferenceReport_obj.SlantRange/VelocityToDeg;
}


/*----- Rate_GRR 34000 -----*/
float GetRate_GRRAngularVelocities_X()
{
	return (float)Rate_GRR_obj.AngularVelocities_X/VelocityToDeg;
}

float GetRate_GRRAngularVelocities_Y()
{
	return (float)Rate_GRR_obj.AngularVelocities_Y/VelocityToDeg;
}


/*----- StabilizationError 34001 -----*/
int16_t GetStabilizationErrorAngularVelocitiesX()
{
	return StabilizationError_obj.AngularVelocitiesX;
}

int16_t GetStabilizationErrorAngularVelocitiesY()
{
	return StabilizationError_obj.AngularVelocitiesY;
}


/*----- GimbalControlData 34002 -----*/
float GetGimbalControlDataAxisXCurrentConsumption()
{
	return (float)GimbalControlData_obj.AxisXCurrentConsumption/1000;
}

float GetGimbalControlDataAxisYCurrentConsumption()
{
	return (float)GimbalControlData_obj.AxisYCurrentConsumption/1000;
}


/*----- GimbalMasterBITStatus 34996 -----*/
bool GetGimbalMasterBITStatusSystemBusy()
{
	return GimbalMasterBITStatus_obj.SystemBusy;
}

bool GetGimbalMasterBITStatusGimbalnotinitialized()
{
	return GimbalMasterBITStatus_obj.Gimbalnotinitialized;
}

bool GetGimbalMasterBITStatusGimbalInitFail()
{
	return GimbalMasterBITStatus_obj.GimbalInitFail;
}

bool GetGimbalMasterBITStatusGimbalFail()
{
	return GimbalMasterBITStatus_obj.GimbalFail;
}

bool GetGimbalMasterBITStatusGimbalHigh_LowTempWarning()
{
	return GimbalMasterBITStatus_obj.GimbalHigh_LowTempWarning;
}

bool GetGimbalMasterBITStatusTrackInitFlag()
{
	return GimbalMasterBITStatus_obj.TrackInitFlag;
}

bool GetGimbalMasterBITStatusPayloadcommfail()
{
	return GimbalMasterBITStatus_obj.Payloadcommfail;
}

bool GetGimbalMasterBITStatusLowstoragewarning()
{
	return GimbalMasterBITStatus_obj.Lowstoragewarning;
}

bool GetGimbalMasterBITStatusHiveTemperaturewarning()
{
	return GimbalMasterBITStatus_obj.HiveTemperaturewarning;
}

bool GetGimbalMasterBITStatusHivesoftwarefail()
{
	return GimbalMasterBITStatus_obj.Hivesoftwarefail;
}

bool GetGimbalMasterBITStatusHostCommfail()
{
	return GimbalMasterBITStatus_obj.HostCommfail;
}

bool GetGimbalMasterBITStatusVISWideFail()
{
	return GimbalMasterBITStatus_obj.VISWideFail;
}

bool GetGimbalMasterBITStatusVISNarrowFail()
{
	return GimbalMasterBITStatus_obj.VISNarrowFail;
}

bool GetGimbalMasterBITStatusTIFail()
{
	return GimbalMasterBITStatus_obj.TIFail;
}


/*----- DayCameraCommand 5600 -----*/
int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)
{
	DayCameraCommand_obj.ZoomCommand = ZoomCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)
{
	DayCameraCommand_obj.SharpnessCommand = SharpnessCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)
{
	DayCameraCommand_obj.GammaCommand = GammaCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)
{
	DayCameraCommand_obj.SaturationCommand = SaturationCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand)
{
	DayCameraCommand_obj.GainCommand = GainCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)
{
	DayCameraCommand_obj.LevelCommand = LevelCommand;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)
{
	DayCameraCommand_obj.ExposureMode = ExposureMode;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)
{
	DayCameraCommand_obj.ExposureTime = ExposureTime;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandExposureGain(enum GenericIncDecVal ExposureGain)
{
	DayCameraCommand_obj.ExposureGain = ExposureGain;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandAutoWhiteBalance(enum DTVAutoWhiteBalanceType AutoWhiteBalance)
{
	DayCameraCommand_obj.AutoWhiteBalance = AutoWhiteBalance;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandTemperature(uint8_t Temperature)
{
	DayCameraCommand_obj.Temperature = Temperature & 0x3;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandFlickerMode(int8_t FlickerMode)
{
	DayCameraCommand_obj.FlickerMode = FlickerMode & 0x3;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}

int SetDayCameraCommandRestoreToDefault(bool RestoreToDefault)
{
	DayCameraCommand_obj.RestoreToDefault = RestoreToDefault & 0x1;
	return SendData(&DayCameraCommand_obj, sizeof(DayCameraCommand_obj));
}


/*----- DayCameraSetFOV 5601 -----*/
int SetDayCameraSetFOVSetFOVCommand(float SetFOVCommand)
{
	DayCameraSetFOV_obj.SetFOVCommand = SetFOVCommand & 0xffff;
	return SendData(&DayCameraSetFOV_obj, sizeof(DayCameraSetFOV_obj));
}


/*----- DayCameraReport 35600 -----*/
enum DTVExposureMode GetDayCameraReportExposureMode()
{
	return DayCameraReport_obj.ExposureMode;
}

int32_t GetDayCameraReportExposureTime()
{
	return DayCameraReport_obj.ExposureTime;
}

int32_t GetDayCameraReportExposureGain()
{
	return DayCameraReport_obj.ExposureGain;
}

float GetDayCameraReportGammaValue()
{
	return DayCameraReport_obj.GammaValue;
}

float GetDayCameraReportSaturationValue()
{
	return DayCameraReport_obj.SaturationValue;
}

float GetDayCameraReportContrastValue()
{
	return DayCameraReport_obj.ContrastValue;
}

float GetDayCameraReportSharpnessValue()
{
	return DayCameraReport_obj.SharpnessValue;
}

float GetDayCameraReportFOV()
{
	return DayCameraReport_obj.FOV;
}

float GetDayCameraReportZoomXAxisCenter()
{
	return DayCameraReport_obj.ZoomXAxisCenter;
}

float GetDayCameraReportZoomYAxisCenter()
{
	return DayCameraReport_obj.ZoomYAxisCenter;
}

float GetDayCameraReportBrigthnessValue()
{
	return DayCameraReport_obj.BrigthnessValue;
}

int32_t GetDayCameraReportAutoWhiteBalance()
{
	return DayCameraReport_obj.AutoWhiteBalance;
}

int32_t GetDayCameraReportTemperatureValue()
{
	return DayCameraReport_obj.TemperatureValue;
}

float GetDayCameraReportFlickerMode()
{
	return DayCameraReport_obj.FlickerMode;
}


/*----- ThermalCameraCommand 5700 -----*/
int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)
{
	ThermalCameraCommand_obj.ZoomCommand = ZoomCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandGainCommand(enum GenericIncDecVal GainCommand)
{
	ThermalCameraCommand_obj.GainCommand = GainCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)
{
	ThermalCameraCommand_obj.LevelCommand = LevelCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand)
{
	ThermalCameraCommand_obj.GainModeCommand = GainModeCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandLinearPercentCommand(enum GenericIncDecVal LinearPercentCommand)
{
	ThermalCameraCommand_obj.LinearPercentCommand = LinearPercentCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand)
{
	ThermalCameraCommand_obj.ACECommand = ACECommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)
{
	ThermalCameraCommand_obj.SharpnessCommand = SharpnessCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandNUCMode(enum ThermalCameraFFCMode NUCMode)
{
	ThermalCameraCommand_obj.NUCMode = NUCMode;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandColorPalette(enum ThermalCameraColorPalette ColorPalette)
{
	ThermalCameraCommand_obj.ColorPalette = ColorPalette;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandNUC(enum NUCForceActivate NUC)
{
	ThermalCameraCommand_obj.NUC = NUC;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandRestoreToDefault(bool RestoreToDefault)
{
	ThermalCameraCommand_obj.RestoreToDefault = RestoreToDefault & 0x1;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}

int SetThermalCameraCommandSyncModeCommand(enum ThermalCameraSyncMode SyncModeCommand)
{
	ThermalCameraCommand_obj.SyncModeCommand = SyncModeCommand;
	return SendData(&ThermalCameraCommand_obj, sizeof(ThermalCameraCommand_obj));
}


/*----- TICameraSetFOV 5701 -----*/
int SetTICameraSetFOVSetFOVCommand(float SetFOVCommand)
{
	TICameraSetFOV_obj.SetFOVCommand = SetFOVCommand & 0xffff;
	return SendData(&TICameraSetFOV_obj, sizeof(TICameraSetFOV_obj));
}


/*----- ThermalCameraReport 35700 -----*/
enum ThermalCameraGainMode GetThermalCameraReportGainMode()
{
	return ThermalCameraReport_obj.GainMode;
}

enum ThermalCameraSyncMode GetThermalCameraReportSyncMode()
{
	return ThermalCameraReport_obj.SyncMode;
}

float GetThermalCameraReportLinearPercent()
{
	return ThermalCameraReport_obj.LinearPercent;
}

float GetThermalCameraReportACEValue()
{
	return ThermalCameraReport_obj.ACEValue;
}

float GetThermalCameraReportDDEValue()
{
	return ThermalCameraReport_obj.DDEValue;
}

enum ThermalCameraColorPalleteMode GetThermalCameraReportColorPalette()
{
	return ThermalCameraReport_obj.ColorPalette;
}

enum NUCMode GetThermalCameraReportNUCMode()
{
	return ThermalCameraReport_obj.NUCMode;
}

float GetThermalCameraReportFOV()
{
	return ThermalCameraReport_obj.FOV;
}

int32_t GetThermalCameraReportGainValue()
{
	return ThermalCameraReport_obj.GainValue;
}

int32_t GetThermalCameraReportLevelValue()
{
	return ThermalCameraReport_obj.LevelValue;
}


/*----- SeeSpotControlCommand 5400 -----*/
int SetSeeSpotControlCommandPower(bool Power)
{
	SeeSpotControlCommand_obj.Power = Power & 0x1;
	return SendData(&SeeSpotControlCommand_obj, sizeof(SeeSpotControlCommand_obj));
}

int SetSeeSpotControlCommandFreeze(bool Freeze)
{
	SeeSpotControlCommand_obj.Freeze = Freeze & 0x1;
	return SendData(&SeeSpotControlCommand_obj, sizeof(SeeSpotControlCommand_obj));
}


/*----- SeeSpotControlCommand 35400 -----*/
bool GetSeeSpotControlCommandPower()
{
	return SeeSpotControlCommand_obj.Power;
}

bool GetSeeSpotControlCommandFreeze()
{
	return SeeSpotControlCommand_obj.Freeze;
}


/*----- ClearStickyBIT 5999 -----*/
int SetClearStickyBIT()
{
	return SendData(&ClearStickyBIT_obj, sizeof(ClearStickyBIT_obj));
}


/*----- CamerasBITStatus 35999 -----*/
bool GetCamerasBITStatusDTVNarrowActiveError()
{
	return CamerasBITStatus_obj.DTVNarrowActiveError;
}

bool GetCamerasBITStatusDTVNarrowStickyError()
{
	return CamerasBITStatus_obj.DTVNarrowStickyError;
}

bool GetCamerasBITStatusDTVNarrowInitError()
{
	return CamerasBITStatus_obj.DTVNarrowInitError;
}

uint8_t GetCamerasBITStatusDTVNarrowVideoErrorsCounter()
{
	return CamerasBITStatus_obj.DTVNarrowVideoErrorsCounter;
}

bool GetCamerasBITStatusDTVWideActiveError()
{
	return CamerasBITStatus_obj.DTVWideActiveError;
}

bool GetCamerasBITStatusDTVWideStickyError()
{
	return CamerasBITStatus_obj.DTVWideStickyError;
}

bool GetCamerasBITStatusDTVWideInitError()
{
	return CamerasBITStatus_obj.DTVWideInitError;
}

uint8_t GetCamerasBITStatusDTVWideVideoErrorsCounter()
{
	return CamerasBITStatus_obj.DTVWideVideoErrorsCounter;
}

bool GetCamerasBITStatusTIActiveError()
{
	return CamerasBITStatus_obj.TIActiveError;
}

bool GetCamerasBITStatusTIStickyError()
{
	return CamerasBITStatus_obj.TIStickyError;
}

bool GetCamerasBITStatusTIInitError()
{
	return CamerasBITStatus_obj.TIInitError;
}

uint8_t GetCamerasBITStatusTIVideoErrorsCounter()
{
	return CamerasBITStatus_obj.TIVideoErrorsCounter;
}

bool GetCamerasBITStatusUSBHubActiveError()
{
	return CamerasBITStatus_obj.USBHubActiveError;
}

bool GetCamerasBITStatusUSBHubStickyError()
{
	return CamerasBITStatus_obj.USBHubStickyError;
}

bool GetCamerasBITStatusUSBHubInitError()
{
	return CamerasBITStatus_obj.USBHubInitError;
}


/*----- VideoChannelCommand 22851 -----*/
int SetVideoChannelCommandPrimaryVideoChannel(enum Camera PrimaryVideoChannel)
{
	VideoChannelCommand_obj.PrimaryVideoChannel = PrimaryVideoChannel;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandFOVSynchronizationMode(bool FOVSynchronizationMode)
{
	VideoChannelCommand_obj.FOVSynchronizationMode = FOVSynchronizationMode & 0x1;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandImageStabilizationEnable(bool ImageStabilizationEnable)
{
	VideoChannelCommand_obj.ImageStabilizationEnable = ImageStabilizationEnable & 0x1;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandImageStabilizationMethod(bool ImageStabilizationMethod)
{
	VideoChannelCommand_obj.ImageStabilizationMethod = ImageStabilizationMethod & 0x1;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandOSDDisabled(bool OSDDisabled)
{
	VideoChannelCommand_obj.OSDDisabled = OSDDisabled & 0x1;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandPIPEnable(enum PIP PIPEnable)
{
	VideoChannelCommand_obj.PIPEnable = PIPEnable;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandHistogramEnable(bool HistogramEnable)
{
	VideoChannelCommand_obj.HistogramEnable = HistogramEnable & 0x1;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}

int SetVideoChannelCommandFontColor(enum FontColor FontColor)
{
	VideoChannelCommand_obj.FontColor = FontColor;
	return SendData(&VideoChannelCommand_obj, sizeof(VideoChannelCommand_obj));
}


/*----- VideoChannelReport 52851 -----*/
enum Camera GetVideoChannelReportPrimaryVideoChannel()
{
	return VideoChannelReport_obj.PrimaryVideoChannel;
}

bool GetVideoChannelReportFOVSynchronizationMode()
{
	return VideoChannelReport_obj.FOVSynchronizationMode;
}

bool GetVideoChannelReportImageStabilizationEnable()
{
	return VideoChannelReport_obj.ImageStabilizationEnable;
}

bool GetVideoChannelReportImageStabilizationMethod()
{
	return VideoChannelReport_obj.ImageStabilizationMethod;
}

bool GetVideoChannelReportOSDDisabled()
{
	return VideoChannelReport_obj.OSDDisabled;
}

bool GetVideoChannelReportPIPEnable()
{
	return VideoChannelReport_obj.PIPEnable;
}

bool GetVideoChannelReportHistogramEnable()
{
	return VideoChannelReport_obj.HistogramEnable;
}

enum FontColor GetVideoChannelReportFontColor()
{
	return VideoChannelReport_obj.FontColor;
}


/*----- VideoChannelRunTimeConfig 22800 -----*/
int SetVideoChannelRunTimeConfigStopChannel0Stream(bool StopChannel0Stream)
{
	VideoChannelRunTimeConfig_obj.StopChannel0Stream = StopChannel0Stream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigStartChannel0Stream(bool StartChannel0Stream)
{
	VideoChannelRunTimeConfig_obj.StartChannel0Stream = StartChannel0Stream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigH264Channel0Stream(bool H264Channel0Stream)
{
	VideoChannelRunTimeConfig_obj.H264Channel0Stream = H264Channel0Stream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigH265Channel0Stream(bool H265Channel0Stream)
{
	VideoChannelRunTimeConfig_obj.H265Channel0Stream = H265Channel0Stream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigBandwidthChannel0Stream(int32_t BandwidthChannel0Stream)
{
	VideoChannelRunTimeConfig_obj.BandwidthChannel0Stream = BandwidthChannel0Stream & 0xffffffff;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigStopDTVStream(bool StopDTVStream)
{
	VideoChannelRunTimeConfig_obj.StopDTVStream = StopDTVStream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}

int SetVideoChannelRunTimeConfigStopTIStream(bool StopTIStream)
{
	VideoChannelRunTimeConfig_obj.StopTIStream = StopTIStream & 0x1;
	return SendData(&VideoChannelRunTimeConfig_obj, sizeof(VideoChannelRunTimeConfig_obj));
}


/*----- RecordVideoCommand 21000 -----*/
int SetRecordVideoCommandEnable(bool Enable)
{
	RecordVideoCommand_obj.Enable = Enable & 0x1;
	return SendData(&RecordVideoCommand_obj, sizeof(RecordVideoCommand_obj));
}

int SetRecordVideoCommandEnableRaw(bool EnableRaw)
{
	RecordVideoCommand_obj.EnableRaw = EnableRaw & 0x1;
	return SendData(&RecordVideoCommand_obj, sizeof(RecordVideoCommand_obj));
}

int SetRecordVideoCommandDeleteALLRecords(bool DeleteALLRecords)
{
	RecordVideoCommand_obj.DeleteALLRecords = DeleteALLRecords & 0x1;
	return SendData(&RecordVideoCommand_obj, sizeof(RecordVideoCommand_obj));
}

int SetRecordVideoCommandFolderName(char* FolderName, size_t Size)
{
	memset(RecordVideoCommand_obj.FolderName, 0, sizeof(RecordVideoCommand_obj.FolderName));
	size_t cpyLen = MIN(Size, sizeof(RecordVideoCommand_obj.FolderName - 1));
	strncpy(RecordVideoCommand_obj.FolderName, FolderName, cpyLen); // ensure null termination
	return SendData(&RecordVideoCommand_obj, sizeof(RecordVideoCommand_obj));
}


/*----- RecordVideoReport 51000 -----*/
int64_t GetRecordVideoReportRecordVideoTimestamp_nSec_()
{
	return RecordVideoReport_obj.RecordVideoTimestamp_nSec_;
}

int64_t GetRecordVideoReportRecordRawVideoTimestamp_nSec_()
{
	return RecordVideoReport_obj.RecordRawVideoTimestamp_nSec_;
}

float GetRecordVideoReportDiskFreeSpace_GB_()
{
	return RecordVideoReport_obj.DiskFreeSpace_GB_;
}

float GetRecordVideoReportDiskFreeSpace_Percent_()
{
	return RecordVideoReport_obj.DiskFreeSpace_Percent_;
}

float GetRecordVideoReportTotalDiskSize_GB_()
{
	return RecordVideoReport_obj.TotalDiskSize_GB_;
}

int32_t GetRecordVideoReportRecordVideoEnable()
{
	return RecordVideoReport_obj.RecordVideoEnable;
}

int32_t GetRecordVideoReportRecordRawVideoEnable()
{
	return RecordVideoReport_obj.RecordRawVideoEnable;
}

int32_t GetRecordVideoReportVideoDelete()
{
	return RecordVideoReport_obj.VideoDelete;
}

int32_t GetRecordVideoReportDiskStatus()
{
	return RecordVideoReport_obj.DiskStatus;
}

void GetRecordVideoReportFolderName(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(RecordVideoReport_obj.FolderName)); // ensure null termination either way
	strncpy(Buffer, RecordVideoReport_obj.FolderName, cpyLen);
	return;
}


/*----- Track 6500 -----*/
int SetTrack(int16_t TargetXCoordinate, int16_t TargetYCoordinate)
{
	Track_obj.TargetXCoordinate = TargetXCoordinate & 0xffff;
	Track_obj.TargetYCoordinate = TargetYCoordinate & 0xffff;
	return SendData(&Track_obj, sizeof(Track_obj));
}


/*----- TrackerTargetErrorReport 36526 -----*/
enum Camera GetTrackerTargetErrorReportCamera()
{
	return TrackerTargetErrorReport_obj.Camera;
}

enum TrackerStatus GetTrackerTargetErrorReportTrackerGeneralStatus()
{
	return TrackerTargetErrorReport_obj.TrackerGeneralStatus;
}

uint8_t GetTrackerTargetErrorReportTrackingQuality()
{
	return TrackerTargetErrorReport_obj.TrackingQuality;
}

bool GetTrackerTargetErrorReportFirstTimeTrack()
{
	return TrackerTargetErrorReport_obj.FirstTimeTrack;
}

uint16_t GetTrackerTargetErrorReportCurrFOV()
{
	return TrackerTargetErrorReport_obj.CurrFOV;
}

float GetTrackerTargetErrorReportTrackerXError()
{
	return TrackerTargetErrorReport_obj.TrackerXError;
}

float GetTrackerTargetErrorReportTrackerYError()
{
	return TrackerTargetErrorReport_obj.TrackerYError;
}

int8_t GetTrackerTargetErrorReportGMC()
{
	return TrackerTargetErrorReport_obj.GMC;
}

uint16_t GetTrackerTargetErrorReportGateSizeX()
{
	return TrackerTargetErrorReport_obj.GateSizeX;
}

uint16_t GetTrackerTargetErrorReportGateSizeY()
{
	return TrackerTargetErrorReport_obj.GateSizeY;
}


/*----- TrackerParameters 6510 -----*/
int SetTrackerParametersTrackerType(enum TrackerType TrackerType)
{
	TrackerParameters_obj.TrackerType = TrackerType;
	return SendData(&TrackerParameters_obj, sizeof(TrackerParameters_obj));
}

int SetTrackerParametersGateXSize(uint16_t GateXSize)
{
	TrackerParameters_obj.GateXSize = GateXSize & 0xffff;
	return SendData(&TrackerParameters_obj, sizeof(TrackerParameters_obj));
}

int SetTrackerParametersGateYSize(uint16_t GateYSize)
{
	TrackerParameters_obj.GateYSize = GateYSize & 0xffff;
	return SendData(&TrackerParameters_obj, sizeof(TrackerParameters_obj));
}

int SetTrackerParametersGateSizeEnable(bool GateSizeEnable)
{
	TrackerParameters_obj.GateSizeEnable = GateSizeEnable & 0x1;
	return SendData(&TrackerParameters_obj, sizeof(TrackerParameters_obj));
}


/*----- TrackerParametersReport 36510 -----*/
int8_t GetTrackerParametersReportTrackerType()
{
	return TrackerParametersReport_obj.TrackerType;
}

uint16_t GetTrackerParametersReportGateXSize()
{
	return TrackerParametersReport_obj.GateXSize;
}

uint16_t GetTrackerParametersReportGateYSize()
{
	return TrackerParametersReport_obj.GateYSize;
}

bool GetTrackerParametersReportGateSizeEnable()
{
	return TrackerParametersReport_obj.GateSizeEnable;
}


/*----- TrackerAdjustControls 6513 -----*/
int SetTrackerAdjustControls(bool AdjustEnable, int16_t MovementCommand_X, int16_t MovementCommand_Y)
{
	TrackerAdjustControls_obj.AdjustEnable = AdjustEnable & 0x1;
	TrackerAdjustControls_obj.MovementCommand_X = MovementCommand_X & 0xffff;
	TrackerAdjustControls_obj.MovementCommand_Y = MovementCommand_Y & 0xffff;
	return SendData(&TrackerAdjustControls_obj, sizeof(TrackerAdjustControls_obj));
}


/*----- TrackerAdjustControlsReport 36513 -----*/
bool GetTrackerAdjustControlsReportAdjustEnable()
{
	return TrackerAdjustControlsReport_obj.AdjustEnable;
}

int16_t GetTrackerAdjustControlsReportMovementX()
{
	return TrackerAdjustControlsReport_obj.MovementX;
}

int16_t GetTrackerAdjustControlsReportMovementY()
{
	return TrackerAdjustControlsReport_obj.MovementY;
}


/*----- TrackerOffsetControls 6514 -----*/
int SetTrackerOffsetControls(bool OffsetEnable, int16_t Movementcommand_X, int16_t Movementcommand_Y)
{
	TrackerOffsetControls_obj.OffsetEnable = OffsetEnable & 0x1;
	TrackerOffsetControls_obj.Movementcommand_X = Movementcommand_X & 0xffff;
	TrackerOffsetControls_obj.Movementcommand_Y = Movementcommand_Y & 0xffff;
	return SendData(&TrackerOffsetControls_obj, sizeof(TrackerOffsetControls_obj));
}


/*----- TrackerOffsetControlsReport 36514 -----*/
bool GetTrackerOffsetControlsReportOffsetEnable()
{
	return TrackerOffsetControlsReport_obj.OffsetEnable;
}

int16_t GetTrackerOffsetControlsReportMovementX()
{
	return TrackerOffsetControlsReport_obj.MovementX;
}

int16_t GetTrackerOffsetControlsReportMovementY()
{
	return TrackerOffsetControlsReport_obj.MovementY;
}


/*----- VMDControls 6551 -----*/
int SetVMDControlsEnable(bool Enable)
{
	VMDControls_obj.Enable = Enable & 0x1;
	return SendData(&VMDControls_obj, sizeof(VMDControls_obj));
}


/*----- VMDReport 36551 -----*/
bool GetVMDReportEnableVMD()
{
	return VMDReport_obj.EnableVMD;
}

uint8_t GetVMDReportQuantityofDetections()
{
	return VMDReport_obj.QuantityofDetections;
}


/*----- ATRControls 6561 -----*/
int SetATRControlsDetectionsReport(bool DetectionsReport)
{
	ATRControls_obj.DetectionsReport = DetectionsReport & 0x1;
	return SendData(&ATRControls_obj, sizeof(ATRControls_obj));
}

int SetATRControlsDetectionThreshold(int8_t DetectionThreshold)
{
	ATRControls_obj.DetectionThreshold = DetectionThreshold & 0xff;
	return SendData(&ATRControls_obj, sizeof(ATRControls_obj));
}


/*----- ATRReport 36561 -----*/
bool GetATRReportEnabled()
{
	return ATRReport_obj.Enabled;
}

uint8_t GetATRReportQuantityofDetections()
{
	return ATRReport_obj.QuantityofDetections;
}


/*----- NavigationInit 7015 -----*/
int SetNavigationInitXMountingAngle(int16_t XMountingAngle)
{
	NavigationInit_obj.XMountingAngle = XMountingAngle & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitYMountingAngle(int16_t YMountingAngle)
{
	NavigationInit_obj.YMountingAngle = YMountingAngle & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitZMountingAngle(int16_t ZMountingAngle)
{
	NavigationInit_obj.ZMountingAngle = ZMountingAngle & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitLeverArmX(int16_t LeverArmX)
{
	NavigationInit_obj.LeverArmX = LeverArmX & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitLeverArmY(int16_t LeverArmY)
{
	NavigationInit_obj.LeverArmY = LeverArmY & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitLeverArmZ(int16_t LeverArmZ)
{
	NavigationInit_obj.LeverArmZ = LeverArmZ & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitPlatformBodyAzimuth(int16_t PlatformBodyAzimuth)
{
	NavigationInit_obj.PlatformBodyAzimuth = PlatformBodyAzimuth & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitPlatformBodyPitch(int16_t PlatformBodyPitch)
{
	NavigationInit_obj.PlatformBodyPitch = PlatformBodyPitch & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}

int SetNavigationInitPlatformBodyRoll(int16_t PlatformBodyRoll)
{
	NavigationInit_obj.PlatformBodyRoll = PlatformBodyRoll & 0xffff;
	return SendData(&NavigationInit_obj, sizeof(NavigationInit_obj));
}


/*----- NavigationInit 37015 -----*/
int16_t GetNavigationInitXMountingAngle()
{
	return NavigationInit_obj.XMountingAngle;
}

int16_t GetNavigationInitYMountingAngle()
{
	return NavigationInit_obj.YMountingAngle;
}

int16_t GetNavigationInitZMountingAngle()
{
	return NavigationInit_obj.ZMountingAngle;
}

int16_t GetNavigationInitLeverArmX()
{
	return NavigationInit_obj.LeverArmX;
}

int16_t GetNavigationInitLeverArmY()
{
	return NavigationInit_obj.LeverArmY;
}

int16_t GetNavigationInitLeverArmZ()
{
	return NavigationInit_obj.LeverArmZ;
}

int16_t GetNavigationInitPlatformBodyAzimuth()
{
	return NavigationInit_obj.PlatformBodyAzimuth;
}

int16_t GetNavigationInitPlatformBodyPitch()
{
	return NavigationInit_obj.PlatformBodyPitch;
}

int16_t GetNavigationInitPlatformBodyRoll()
{
	return NavigationInit_obj.PlatformBodyRoll;
}


/*----- NavigationProperties 7000 -----*/
int SetNavigationPropertiesPlatformDataSource(int8_t PlatformDataSource)
{
	NavigationProperties_obj.PlatformDataSource = PlatformDataSource & 0x3;
	return SendData(&NavigationProperties_obj, sizeof(NavigationProperties_obj));
}

int SetNavigationPropertiesUsePlatformData(bool UsePlatformData)
{
	NavigationProperties_obj.UsePlatformData = UsePlatformData & 0x1;
	return SendData(&NavigationProperties_obj, sizeof(NavigationProperties_obj));
}

int SetNavigationPropertiesNavigationType(enum NavigationType NavigationType)
{
	NavigationProperties_obj.NavigationType = NavigationType;
	return SendData(&NavigationProperties_obj, sizeof(NavigationProperties_obj));
}

int SetNavigationPropertiesAltitudeType(enum NavAltituteType AltitudeType)
{
	NavigationProperties_obj.AltitudeType = AltitudeType;
	return SendData(&NavigationProperties_obj, sizeof(NavigationProperties_obj));
}


/*----- NavigationStatus 37000 -----*/
int8_t GetNavigationStatusPlatformDataSource()
{
	return NavigationStatus_obj.PlatformDataSource;
}

bool GetNavigationStatusUsePlatformData()
{
	return NavigationStatus_obj.UsePlatformData;
}

enum NavigationType GetNavigationStatusNavigationType()
{
	return NavigationStatus_obj.NavigationType;
}

enum NavAltituteType GetNavigationStatusAltitudeType()
{
	return NavigationStatus_obj.AltitudeType;
}


/*----- PlatformNavigationDataAccuracy 7010 -----*/
int SetPlatformNavigationDataAccuracyPositionAccuracy(int8_t PositionAccuracy)
{
	PlatformNavigationDataAccuracy_obj.PositionAccuracy = PositionAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}

int SetPlatformNavigationDataAccuracyAltitudeAccuracy(int8_t AltitudeAccuracy)
{
	PlatformNavigationDataAccuracy_obj.AltitudeAccuracy = AltitudeAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}

int SetPlatformNavigationDataAccuracyAttitudeAccuracy(int8_t AttitudeAccuracy)
{
	PlatformNavigationDataAccuracy_obj.AttitudeAccuracy = AttitudeAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}

int SetPlatformNavigationDataAccuracyAzimuthAccuracy(int8_t AzimuthAccuracy)
{
	PlatformNavigationDataAccuracy_obj.AzimuthAccuracy = AzimuthAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}

int SetPlatformNavigationDataAccuracyVelocityAccuracy(int8_t VelocityAccuracy)
{
	PlatformNavigationDataAccuracy_obj.VelocityAccuracy = VelocityAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}

int SetPlatformNavigationDataAccuracyRateofClimbAccuracy(int8_t RateofClimbAccuracy)
{
	PlatformNavigationDataAccuracy_obj.RateofClimbAccuracy = RateofClimbAccuracy & 0x3;
	return SendData(&PlatformNavigationDataAccuracy_obj, sizeof(PlatformNavigationDataAccuracy_obj));
}


/*----- PlatformNavigationData 7011 -----*/
int SetPlatformNavigationDataTimeTag(int32_t TimeTag)
{
	PlatformNavigationData_obj.TimeTag = TimeTag & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformLongitude(float PlatformLongitude)
{
	PlatformNavigationData_obj.PlatformLongitude = PlatformLongitude & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformLatitude(float PlatformLatitude)
{
	PlatformNavigationData_obj.PlatformLatitude = PlatformLatitude & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformAltitude(float PlatformAltitude)
{
	PlatformNavigationData_obj.PlatformAltitude = PlatformAltitude & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformBodyAzimuth(float PlatformBodyAzimuth)
{
	PlatformNavigationData_obj.PlatformBodyAzimuth = PlatformBodyAzimuth & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformBodyPitch(float PlatformBodyPitch)
{
	PlatformNavigationData_obj.PlatformBodyPitch = PlatformBodyPitch & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataPlatformBodyRoll(float PlatformBodyRoll)
{
	PlatformNavigationData_obj.PlatformBodyRoll = PlatformBodyRoll & 0xffffffff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataGroundSpeed(int8_t GroundSpeed)
{
	PlatformNavigationData_obj.GroundSpeed = GroundSpeed & 0xff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}

int SetPlatformNavigationDataRateofClimb(int8_t RateofClimb)
{
	PlatformNavigationData_obj.RateofClimb = RateofClimb & 0xff;
	return SendData(&PlatformNavigationData_obj, sizeof(PlatformNavigationData_obj));
}


/*----- PlatformNavigationData 37011 -----*/
int32_t GetPlatformNavigationDataTimeTag()
{
	return PlatformNavigationData_obj.TimeTag;
}

float GetPlatformNavigationDataPlatformLongitude()
{
	return PlatformNavigationData_obj.PlatformLongitude;
}

float GetPlatformNavigationDataPlatformLatitude()
{
	return PlatformNavigationData_obj.PlatformLatitude;
}

float GetPlatformNavigationDataPlatformAltitude()
{
	return PlatformNavigationData_obj.PlatformAltitude;
}

float GetPlatformNavigationDataLOSAzimuth()
{
	return PlatformNavigationData_obj.LOSAzimuth;
}

float GetPlatformNavigationDataLOSPitch()
{
	return PlatformNavigationData_obj.LOSPitch;
}

float GetPlatformNavigationDataLOSRoll()
{
	return PlatformNavigationData_obj.LOSRoll;
}

float GetPlatformNavigationDataTargetLongitude()
{
	return PlatformNavigationData_obj.TargetLongitude;
}

float GetPlatformNavigationDataTargetLatitude()
{
	return PlatformNavigationData_obj.TargetLatitude;
}

float GetPlatformNavigationDataTargetAltitude()
{
	return PlatformNavigationData_obj.TargetAltitude;
}

int8_t GetPlatformNavigationDataGroundSpeed()
{
	return PlatformNavigationData_obj.GroundSpeed;
}

int8_t GetPlatformNavigationDataRateofClimb()
{
	return PlatformNavigationData_obj.RateofClimb;
}


/*----- NavigationIMUData 37050 -----*/
int64_t GetNavigationIMUDataIMUTimeTag()
{
	return NavigationIMUData_obj.IMUTimeTag;
}

int32_t GetNavigationIMUDataIMUXRate()
{
	return NavigationIMUData_obj.IMUXRate;
}

int32_t GetNavigationIMUDataIMUYRate()
{
	return NavigationIMUData_obj.IMUYRate;
}

int32_t GetNavigationIMUDataIMUZRate()
{
	return NavigationIMUData_obj.IMUZRate;
}

int32_t GetNavigationIMUDataIMUXAcc()
{
	return NavigationIMUData_obj.IMUXAcc;
}

int32_t GetNavigationIMUDataIMUYAcc()
{
	return NavigationIMUData_obj.IMUYAcc;
}

int32_t GetNavigationIMUDataIMUZAcc()
{
	return NavigationIMUData_obj.IMUZAcc;
}

int8_t GetNavigationIMUDataIMUTemperature()
{
	return NavigationIMUData_obj.IMUTemperature;
}

int16_t GetNavigationIMUDataIMUStatus()
{
	return NavigationIMUData_obj.IMUStatus;
}


/*----- GPSData 37801 -----*/
int32_t GetGPSDataLongitude()
{
	return GPSData_obj.Longitude;
}

int32_t GetGPSDataLatitude()
{
	return GPSData_obj.Latitude;
}

int32_t GetGPSDataAltitude()
{
	return GPSData_obj.Altitude;
}

int16_t GetGPSDataAzimuth()
{
	return GPSData_obj.Azimuth;
}

uint32_t GetGPSDataSecond()
{
	return GPSData_obj.Second;
}

uint32_t GetGPSDatamilliseconds()
{
	return GPSData_obj.milliseconds;
}

int8_t GetGPSDataNumerOfSatellites()
{
	return GPSData_obj.NumerOfSatellites;
}

bool GetGPSDataPositionValid()
{
	return GPSData_obj.PositionValid;
}


/*----- NavigationRecord 7017 -----*/
int SetNavigationRecordRecordLevel(int8_t RecordLevel)
{
	NavigationRecord_obj.RecordLevel = RecordLevel & 0xff;
	return SendData(&NavigationRecord_obj, sizeof(NavigationRecord_obj));
}


/*----- GPSRecording 37017 -----*/
int8_t GetGPSRecordingGPSRecordingLevel()
{
	return GPSRecording_obj.GPSRecordingLevel;
}

int32_t GetGPSRecordingGPSFreeMemory()
{
	return GPSRecording_obj.GPSFreeMemory;
}


/*----- NavigationBIT 37010 -----*/
bool GetNavigationBITNavigationaccuracyWarning()
{
	return NavigationBIT_obj.NavigationaccuracyWarning;
}

bool GetNavigationBITGPSCommFail()
{
	return NavigationBIT_obj.GPSCommFail;
}

bool GetNavigationBITGPSprecisionFail()
{
	return NavigationBIT_obj.GPSprecisionFail;
}

bool GetNavigationBITGPSantennaFail()
{
	return NavigationBIT_obj.GPSantennaFail;
}

bool GetNavigationBITIMUFail()
{
	return NavigationBIT_obj.IMUFail;
}

bool GetNavigationBITPPSFail()
{
	return NavigationBIT_obj.PPSFail;
}

bool GetNavigationBITCalibrationFail()
{
	return NavigationBIT_obj.CalibrationFail;
}


/*----- Designator 8450 -----*/
int SetDesignatorPower(bool Power)
{
	Designator_obj.Power = Power & 0x1;
	return SendData(&Designator_obj, sizeof(Designator_obj));
}

int SetDesignatorArm(bool Arm)
{
	Designator_obj.Arm = Arm & 0x1;
	return SendData(&Designator_obj, sizeof(Designator_obj));
}


/*----- DesignatorStatus 38450 -----*/
bool GetDesignatorStatusPower()
{
	return DesignatorStatus_obj.Power;
}

bool GetDesignatorStatusArm()
{
	return DesignatorStatus_obj.Arm;
}

bool GetDesignatorStatusReady()
{
	return DesignatorStatus_obj.Ready;
}

bool GetDesignatorStatusSafetySwitch()
{
	return DesignatorStatus_obj.SafetySwitch;
}

bool GetDesignatorStatusEnable()
{
	return DesignatorStatus_obj.Enable;
}


/*----- Designator 8451 -----*/
int SetDesignatorFire(bool Fire)
{
	Designator_obj.Fire = Fire & 0x1;
	return SendData(&Designator_obj, sizeof(Designator_obj));
}


/*----- Designator 38451 -----*/
bool GetDesignatorFire()
{
	return Designator_obj.Fire;
}

bool GetDesignatorFireBlocked()
{
	return Designator_obj.FireBlocked;
}


/*----- DesignatorNATO 8455 -----*/
int SetDesignatorNATOCodeValue(uint16_t CodeValue)
{
	DesignatorNATO_obj.CodeValue = CodeValue & 0xffff;
	return SendData(&DesignatorNATO_obj, sizeof(DesignatorNATO_obj));
}


/*----- DesignatorNATO 38455 -----*/
uint16_t GetDesignatorNATOCodeValue()
{
	return DesignatorNATO_obj.CodeValue;
}


/*----- Illuminator 8500 -----*/
int SetIlluminatorPower(bool Power)
{
	Illuminator_obj.Power = Power & 0x1;
	return SendData(&Illuminator_obj, sizeof(Illuminator_obj));
}

int SetIlluminatorArm(bool Arm)
{
	Illuminator_obj.Arm = Arm & 0x1;
	return SendData(&Illuminator_obj, sizeof(Illuminator_obj));
}


/*----- Illuminator 38500 -----*/
bool GetIlluminatorPower()
{
	return Illuminator_obj.Power;
}

bool GetIlluminatorArm()
{
	return Illuminator_obj.Arm;
}


/*----- Illuminator 8501 -----*/
int SetIlluminatorMode(int8_t Mode)
{
	Illuminator_obj.Mode = Mode & 0xf;
	return SendData(&Illuminator_obj, sizeof(Illuminator_obj));
}

int SetIlluminatorFire(bool Fire)
{
	Illuminator_obj.Fire = Fire & 0x1;
	return SendData(&Illuminator_obj, sizeof(Illuminator_obj));
}


/*----- Illuminator 38501 -----*/
int8_t GetIlluminatorMode()
{
	return Illuminator_obj.Mode;
}

bool GetIlluminatorFire()
{
	return Illuminator_obj.Fire;
}


/*----- KeepAlive 0 -----*/
int SetKeepAlive()
{
	return SendData(&KeepAlive_obj, sizeof(KeepAlive_obj));
}


/*----- GetDATA 144 -----*/
int SetGetDATA(uint16_t Data, enum RetreiveDataType RetreiveType)
{
	GetDATA_obj.Data = Data & 0xffff;
	GetDATA_obj.RetreiveType = RetreiveType;
	return SendData(&GetDATA_obj, sizeof(GetDATA_obj));
}


/*----- SystemGeneralReport 50901 -----*/
enum TOperationMode_Micro GetSystemGeneralReportOperationMode()
{
	return SystemGeneralReport_obj.OperationMode;
}

bool GetSystemGeneralReportHybridMode()
{
	return SystemGeneralReport_obj.HybridMode;
}

int8_t GetSystemGeneralReportHiveTemperature()
{
	return SystemGeneralReport_obj.HiveTemperature;
}


/*----- LogRecordCommand 20000 -----*/
int SetLogRecordCommandEnable(bool Enable)
{
	LogRecordCommand_obj.Enable = Enable & 0x1;
	return SendData(&LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
}

int SetLogRecordCommandErrorLevel(bool ErrorLevel)
{
	LogRecordCommand_obj.ErrorLevel = ErrorLevel & 0x1;
	return SendData(&LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
}

int SetLogRecordCommandInfoLevel(bool InfoLevel)
{
	LogRecordCommand_obj.InfoLevel = InfoLevel & 0x1;
	return SendData(&LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
}

int SetLogRecordCommandDebugLevel(bool DebugLevel)
{
	LogRecordCommand_obj.DebugLevel = DebugLevel & 0x1;
	return SendData(&LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
}

int SetLogRecordCommandFreeText(char* FreeText, size_t Size)
{
	memset(LogRecordCommand_obj.FreeText, 0, sizeof(LogRecordCommand_obj.FreeText));
	size_t cpyLen = MIN(Size, sizeof(LogRecordCommand_obj.FreeText - 1));
	strncpy(LogRecordCommand_obj.FreeText, FreeText, cpyLen); // ensure null termination
	return SendData(&LogRecordCommand_obj, sizeof(LogRecordCommand_obj));
}


/*----- LogRecordReport 50000 -----*/
bool GetLogRecordReportEnable()
{
	return LogRecordReport_obj.Enable;
}

bool GetLogRecordReportErrorLevel()
{
	return LogRecordReport_obj.ErrorLevel;
}

bool GetLogRecordReportInfoLevel()
{
	return LogRecordReport_obj.InfoLevel;
}

bool GetLogRecordReportDebugLevel()
{
	return LogRecordReport_obj.DebugLevel;
}

void GetLogRecordReportFileName(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(LogRecordReport_obj.FileName)); // ensure null termination either way
	strncpy(Buffer, LogRecordReport_obj.FileName, cpyLen);
	return;
}


/*----- CameraPowerControl 20500 -----*/
int SetCameraPowerControlDayLightCameraPower(enum PowerStatesRequest DayLightCameraPower)
{
	CameraPowerControl_obj.DayLightCameraPower = DayLightCameraPower;
	return SendData(&CameraPowerControl_obj, sizeof(CameraPowerControl_obj));
}

int SetCameraPowerControlThermalCameraPower(enum PowerStatesRequest ThermalCameraPower)
{
	CameraPowerControl_obj.ThermalCameraPower = ThermalCameraPower;
	return SendData(&CameraPowerControl_obj, sizeof(CameraPowerControl_obj));
}


/*----- CameraPowerReport 50500 -----*/
enum PowerStatesReport GetCameraPowerReportWideDTVState()
{
	return CameraPowerReport_obj.WideDTVState;
}

enum PowerStatesReport GetCameraPowerReportNarrowDTVState()
{
	return CameraPowerReport_obj.NarrowDTVState;
}

enum PowerStatesReport GetCameraPowerReportTIStateState()
{
	return CameraPowerReport_obj.TIStateState;
}


/*----- TimeTagReport 50903 -----*/
uint64_t GetTimeTagReportSystemTimeTag()
{
	return TimeTagReport_obj.SystemTimeTag;
}

uint64_t GetTimeTagReportControlTimeTag()
{
	return TimeTagReport_obj.ControlTimeTag;
}


/*----- SetSystemClock 20005 -----*/
int SetSetSystemClockYear(uint16_t Year)
{
	SetSystemClock_obj.Year = Year & 0xffff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}

int SetSetSystemClockMonth(uint8_t Month)
{
	SetSystemClock_obj.Month = Month & 0xff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}

int SetSetSystemClockDay(uint8_t Day)
{
	SetSystemClock_obj.Day = Day & 0xff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}

int SetSetSystemClockHour(uint8_t Hour)
{
	SetSystemClock_obj.Hour = Hour & 0xff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}

int SetSetSystemClockMinute(uint8_t Minute)
{
	SetSystemClock_obj.Minute = Minute & 0xff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}

int SetSetSystemClockSecond(uint8_t Second)
{
	SetSystemClock_obj.Second = Second & 0xff;
	return SendData(&SetSystemClock_obj, sizeof(SetSystemClock_obj));
}


/*----- CurrentSystemTime 50005 -----*/
uint16_t GetCurrentSystemTimeYear()
{
	return CurrentSystemTime_obj.Year;
}

uint8_t GetCurrentSystemTimeMonth()
{
	return CurrentSystemTime_obj.Month;
}

uint8_t GetCurrentSystemTimeDay()
{
	return CurrentSystemTime_obj.Day;
}

uint8_t GetCurrentSystemTimeHour()
{
	return CurrentSystemTime_obj.Hour;
}

uint8_t GetCurrentSystemTimeMinute()
{
	return CurrentSystemTime_obj.Minute;
}

uint8_t GetCurrentSystemTimeSecond()
{
	return CurrentSystemTime_obj.Second;
}


/*----- SystemVersion 50999 -----*/
int8_t GetSystemVersionBundle()
{
	return SystemVersion_obj.Bundle;
}

void GetSystemVersionCPU(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.CPU)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.CPU, cpyLen);
	return;
}

uint32_t GetSystemVersionControl()
{
	return SystemVersion_obj.Control;
}

void GetSystemVersionDTVLibrary(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.DTVLibrary)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.DTVLibrary, cpyLen);
	return;
}

void GetSystemVersionECVML(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.ECVML)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.ECVML, cpyLen);
	return;
}

void GetSystemVersionSensorsWide(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.SensorsWide)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.SensorsWide, cpyLen);
	return;
}

void GetSystemVersionSensorsNarrow(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.SensorsNarrow)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.SensorsNarrow, cpyLen);
	return;
}

void GetSystemVersionSensorsThermal(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.SensorsThermal)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.SensorsThermal, cpyLen);
	return;
}

void GetSystemVersionDesignator(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.Designator)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.Designator, cpyLen);
	return;
}

void GetSystemVersionIlluminator(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.Illuminator)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.Illuminator, cpyLen);
	return;
}

void GetSystemVersionSensorsSeeSpot(char* Buffer, size_t Size)
{
	memset(Buffer, 0, Size);
	size_t cpyLen = MIN(Size-1, sizeof(SystemVersion_obj.SensorsSeeSpot)); // ensure null termination either way
	strncpy(Buffer, SystemVersion_obj.SensorsSeeSpot, cpyLen);
	return;
}

