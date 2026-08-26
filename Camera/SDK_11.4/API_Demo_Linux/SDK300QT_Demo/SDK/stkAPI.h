/*
============================================================================
Name: stkAPI.h
Version: 28-December-2025 10:00
Copyright: MicroCon LTD
Description: API for Micro Family Units
============================================================================
*/

#ifndef _STK_API
#define _STK_API

#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#define STK_VERSION 0
#define SEND_ON_SET // Send Data To Unit Each Set Function Call
//#define DEBUG_MODE // Set printing debug info on


/* Communication Related Variables */
extern int COMMUNICATION_PORT;
extern int LOCAL_PORT;
extern char *REMOTE_IP;
extern char *HOST_IP;


/* Buffers for UDP RX/TX */
extern uint8_t RXBuff[];
extern uint8_t TXBuff[];




enum SyncLocalErrorEnum
{
	ERR_NO_ERROR = 0,
	ERR_UDP_RX = -1,
	ERR_HEADER_KEY = -2,
	ERR_LEN_MISSMATCH = -3,
	ERR_CRC_MISSMATCH = -4,
};

enum Camera
{
	TI = 1,
	DTV = 2,
	SeeSpot = 4
};
 
enum PIP
{
	On = 1,
	Off = 2
};
 
enum TOperationMode_Micro
{
	IDLE = 0,
	Init = 1,
	Rate = 2,
	Position_RelativeAngles = 3,
	DriftCalibration = 4,
	BIT = 5,
	Safe = 6,
	Center = 7,
	Track = 10,
	GimbalCalibration = 11,
	Rate_Pos = 12,
	SystemLoading = 13
};
 
enum NavigationType
{
	Inertialnavigation = 0,
	VerticalGyro = 1,
	AddgimbalanglestoexternalINSdata = 2,
	Nonavigation = 7
};
 
enum ZoomChange
{
	Nochange = 0,
	Zoomout = 1,
	Zoomin = 2
};
 
enum TrackerType
{
	Autogatesize20pix = 0,
	StructureSmall = 1,
	StructureMedium = 2,
	StructureLarge = 3,
	Human = 4,
	Vehicle = 5,
	Truck = 6
};
 
enum TrackerStatus
{
	NoTrack = 0,
	TrackerOK = 1,
	Prediction = 2,
	TrackLoss = 3
};
 
enum GenericIncDecVal
{
	NoChange = 0,
	DecreaseValue = 1,
	IncreaseValue = 2
};
 
enum DTVExposureMode
{
	ManualExposureMode = 0,
	AutomaticGainMode = 9,
	AutomaticExposureMode = 12
};
 
enum DTVExposureModeSet
{
	SetAutomaticExposureMode = 0,
	SetAutomaticGainMode = 1,
	SetManualExposureMode = 2
};
 
enum ThermalCameraColorPalette
{
	SetColorPaletteNC = 0,
	SetColorPaletteWhiteHot = 1,
	SetColorPaletteBlackHot = 2,
	SetColorPaletteRainbow = 3,
	SetColorPaletteRainbowHC = 4,
	SetColorPaletteIronBow = 5
};
 
enum ThermalCameraGainMode
{
	GainModeNC = 0,
	GainModeHigh = 1,
	GainModeLow = 2,
	GainModeAuto = 3
};
 
enum ThermalCameraSyncMode
{
	SyncModeNC = 0,
	SyncModeDisabled = 1,
	SyncModeMaster = 2
};
 
enum ThermalCameraColorPalleteMode
{
	ColorPaletteNC = 0,
	ColorPaletteWhiteHot = 1,
	ColorPaletteBlackHot = 2,
	ColorPaletteRainbow = 3,
	ColorPaletteRainbowHC = 4,
	ColorPaletteIronBow = 5
};
 
enum NUCMode
{
	NucModeManual = 0,
	NucModeAuto = 1,
	NucModeExternal = 2,
	NucModeShutterTest = 3
};
 
enum ThermalCameraFFCMode
{
	FFCModeNC = 0,
	FFCModeManual = 1,
	FFCModeAuto = 2
};
 
enum NUCForceActivate
{
	NoForceNUC = 0,
	SetForceNuc = 1
};
 
enum PowerStatesRequest
{
	PowerRequestNC = 0,
	PowerRequestOn = 1,
	PowerRequestOff = 2
};
 
enum PowerStatesReport
{
	PowerReportOff = 0,
	PowerReportOn = 1
};
 
enum RetreiveDataType
{
	StopRetreive = 0,
	CyclicRetreive = 1,
	OnceRetreive = 2,
	OnArrival = 4
};
 
enum FontColor
{
	BLACK = 0,
	RED = 1,
	GREEN = 2,
	YELLOW = 3,
	BLUE = 4,
	MAGENTA = 5,
	CYAN = 6,
	LIGHT_GREY = 7,
	GREY = 8,
	BRIGHT_RED = 9,
	BRIGHT_GREEN = 10,
	BRIGHT_YELLOW = 11,
	BRIGHT_BLUE = 12,
	BRIGHT_MAGENTA = 13,
	BRIGHT_CYAN = 14,
	WHITE = 15
};
 
enum DTVAutoWhiteBalanceType
{
	AWBNC = 0,
	AWBOn = 1,
	AWBManual = 2
};
 
enum NavAltituteType
{
	Ellipsoid = 0,
	Meansealevel = 1
};
 
struct MessagesTransmitEnableStruct 
{
	bool EnableTXSystemInit;
	bool EnableTXIDLE;
	bool EnableTXDriftCalibration;
	bool EnableTXRateCommand;
	bool EnableTXPositionCommand;
	bool EnableTXCenterMode;
	bool EnableTXSafeMode;
	bool EnableTXGroundReferenceCommand;
	bool EnableTXDayCameraCommand;
	bool EnableTXDayCameraSetFOV;
	bool EnableTXThermalCameraCommand;
	bool EnableTXTICameraSetFOV;
	bool EnableTXSeeSpotControlCommand;
	bool EnableTXClearStickyBIT;
	bool EnableTXVideoChannelCommand;
	bool EnableTXVideoChannelRunTimeConfig;
	bool EnableTXRecordVideoCommand;
	bool EnableTXTrack;
	bool EnableTXTrackerParameters;
	bool EnableTXTrackerAdjustControls;
	bool EnableTXTrackerOffsetControls;
	bool EnableTXVMDControls;
	bool EnableTXATRControls;
	bool EnableTXNavigationInit;
	bool EnableTXNavigationProperties;
	bool EnableTXPlatformNavigationDataAccuracy;
	bool EnableTXPlatformNavigationData;
	bool EnableTXNavigationRecord;
	bool EnableTXDesignator;
	bool EnableTXDesignator;
	bool EnableTXDesignatorNATO;
	bool EnableTXIlluminator;
	bool EnableTXIlluminator;
	bool EnableTXKeepAlive;
	bool EnableTXGetDATA;
	bool EnableTXLogRecordCommand;
	bool EnableTXCameraPowerControl;
	bool EnableTXSetSystemClock;
};

/*Default Is Set all Fields To True, So SyncRemote() Will Allways Send All Data*/
extern struct MessagesTransmitEnableStruct MessagesTransmitEnable;


/*----- SystemInit Command -----*/

/*Function Name       :    SetSystemInit
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Initialize the system - Gimbals and Sensors. This process takes few seconds*/
int SetSystemInit();


/*----- IDLE Command -----*/

/*Function Name       :    SetIDLE
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Enter to Idle Mode. Shutdown gimbals' motors and releases gimbal's current loop.*/
int SetIDLE();


/*----- DriftCalibration Command -----*/

/*Function Name       :    SetDriftCalibration
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Performs calibration to gimbal. This process takes few seconds.*/
int SetDriftCalibration();


/*----- RateCommand Command -----*/

/*Function Name       :    SetRateCommandAngularVelocities_X
* Function Arguments  :    float AngularVelocities_X
* Function Returns    :    int Success
* Function Description:    Set Angular Velocities for both axes, in units [°/Sec] On X Axis*/
int SetRateCommandAngularVelocities_X(float AngularVelocities_X);

/*Function Name       :    SetRateCommandAngularVelocities_Y
* Function Arguments  :    float AngularVelocities_Y
* Function Returns    :    int Success
* Function Description:    Set Angular Velocities for both axes, in units [°/Sec] On Y Axis*/
int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);


/*----- PositionCommand Command -----*/

/*Function Name       :    SetPositionCommandLOSAngles_X
* Function Arguments  :    float LOSAngles_X
* Function Returns    :    int Success
* Function Description:    Set Line Of Sight Command in angels [°] On X Axis*/
int SetPositionCommandLOSAngles_X(float LOSAngles_X);

/*Function Name       :    SetPositionCommandLOSAngles_Y
* Function Arguments  :    float LOSAngles_Y
* Function Returns    :    int Success
* Function Description:    Set Line Of Sight Command in angels [°] On Y Axis*/
int SetPositionCommandLOSAngles_Y(float LOSAngles_Y);


/*----- CenterMode Command -----*/

/*Function Name       :    SetCenterMode
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Set system as Center Mode. Sets predefined gimbal angles and sensors FOV*/
int SetCenterMode();


/*----- SafeMode Command -----*/

/*Function Name       :    SetSafeMode
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Set system to Safe Mode. Set predefined gimbal angles and sensors FOV*/
int SetSafeMode();


/*----- PositionReport Report -----*/

/*Function Name       :    GetPositionReportRelativeGimbalAngles_X
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets Line Of Sight in angels, in units [°] On X Axis */
float GetPositionReportRelativeGimbalAngles_X();

/*Function Name       :    GetPositionReportRelativeGimbalAngles_Y
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets Line Of Sight in angels, in units [°] On Y Axis */
float GetPositionReportRelativeGimbalAngles_Y();


/*----- GyroRawReport Report -----*/

/*Function Name       :    GetGyroRawReportControlGyroTimeTag
* Function Arguments  :    None
* Function Returns    :    uint64_t 
* Function Description:    Gets Control Gyro Message Time Tag */
uint64_t GetGyroRawReportControlGyroTimeTag();

/*Function Name       :    GetGyroRawReportGyroYaw
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Gyro Yaw Value */
float GetGyroRawReportGyroYaw();

/*Function Name       :    GetGyroRawReportGyroPitch
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Gyro Pitch Value */
float GetGyroRawReportGyroPitch();

/*Function Name       :    GetGyroRawReportGyroRoll
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Gyro Roll Value */
float GetGyroRawReportGyroRoll();


/*----- GroundReferenceCommand Command -----*/

/*Function Name       :    SetGroundReferenceCommandAngularVelocities_X
* Function Arguments  :    float AngularVelocities_X
* Function Returns    :    int Success
* Function Description:    Sets GRR (Ground Reference Rate) Angular Velocities, in units [°/Sec] On X Axis*/
int SetGroundReferenceCommandAngularVelocities_X(float AngularVelocities_X);

/*Function Name       :    SetGroundReferenceCommandAngularVelocities_Y
* Function Arguments  :    float AngularVelocities_Y
* Function Returns    :    int Success
* Function Description:    Sets GRR (Ground Reference Rate) Angular Velocities, in units [°/Sec] On Y Axis*/
int SetGroundReferenceCommandAngularVelocities_Y(float AngularVelocities_Y);

/*Function Name       :    SetGroundReferenceCommandSlantRange
* Function Arguments  :    float SlantRange
* Function Returns    :    int Success
* Function Description:    Sets Slant Range, in units [meters]*/
int SetGroundReferenceCommandSlantRange(float SlantRange);


/*----- GroundReferenceReport Report -----*/

/*Function Name       :    GetGroundReferenceReportAngularVelocities_X
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets GRR (Ground Reference Rate) Angular Velocities, in units [°/Sec] On X Axis */
float GetGroundReferenceReportAngularVelocities_X();

/*Function Name       :    GetGroundReferenceReportAngularVelocities_Y
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets GRR (Ground Reference Rate) Angular Velocities, in units [°/Sec] On Y Axis */
float GetGroundReferenceReportAngularVelocities_Y();

/*Function Name       :    GetGroundReferenceReportSlantRange
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets Slant Range, in units [meters] */
float GetGroundReferenceReportSlantRange();


/*----- Rate_GRR Report -----*/

/*Function Name       :    GetRate_GRRAngularVelocities_X
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets Angular Velocities + GRR, in units [°/Sec] On X Axis */
float GetRate_GRRAngularVelocities_X();

/*Function Name       :    GetRate_GRRAngularVelocities_Y
* Function Arguments  :    None
* Function Returns    :     
* Function Description:    Gets Angular Velocities + GRR, in units [°/Sec] On Y Axis */
float GetRate_GRRAngularVelocities_Y();


/*----- StabilizationError Report -----*/

/*Function Name       :    GetStabilizationErrorAngularVelocitiesX
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets Angular Velocities Error On X Axis */
int16_t GetStabilizationErrorAngularVelocitiesX();

/*Function Name       :    GetStabilizationErrorAngularVelocitiesY
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets Angular Velocities Error On Y Axis */
int16_t GetStabilizationErrorAngularVelocitiesY();


/*----- GimbalControlData Report -----*/

/*Function Name       :    GetGimbalControlDataAxisXCurrentConsumption
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Current Consumption Of X Axis Motor */
float GetGimbalControlDataAxisXCurrentConsumption();

/*Function Name       :    GetGimbalControlDataAxisYCurrentConsumption
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Current Consumption Of Y Axis Motor */
float GetGimbalControlDataAxisYCurrentConsumption();


/*----- GimbalMasterBITStatus Report -----*/

/*Function Name       :    GetGimbalMasterBITStatusSystemBusy
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Busy Flag Mode */
bool GetGimbalMasterBITStatusSystemBusy();

/*Function Name       :    GetGimbalMasterBITStatusGimbalnotinitialized
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Control Operational Failure */
bool GetGimbalMasterBITStatusGimbalnotinitialized();

/*Function Name       :    GetGimbalMasterBITStatusGimbalInitFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets HW + Sensors */
bool GetGimbalMasterBITStatusGimbalInitFail();

/*Function Name       :    GetGimbalMasterBITStatusGimbalFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Azimuth Gimbal General Bit */
bool GetGimbalMasterBITStatusGimbalFail();

/*Function Name       :    GetGimbalMasterBITStatusGimbalHigh_LowTempWarning
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusGimbalHigh_LowTempWarning();

/*Function Name       :    GetGimbalMasterBITStatusTrackInitFlag
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Track Init Flag */
bool GetGimbalMasterBITStatusTrackInitFlag();

/*Function Name       :    GetGimbalMasterBITStatusPayloadcommfail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusPayloadcommfail();

/*Function Name       :    GetGimbalMasterBITStatusLowstoragewarning
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusLowstoragewarning();

/*Function Name       :    GetGimbalMasterBITStatusHiveTemperaturewarning
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusHiveTemperaturewarning();

/*Function Name       :    GetGimbalMasterBITStatusHivesoftwarefail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusHivesoftwarefail();

/*Function Name       :    GetGimbalMasterBITStatusHostCommfail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Elevation Gimbal General Bit */
bool GetGimbalMasterBITStatusHostCommfail();

/*Function Name       :    GetGimbalMasterBITStatusVISWideFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Wide Visable Camera fail Bit */
bool GetGimbalMasterBITStatusVISWideFail();

/*Function Name       :    GetGimbalMasterBITStatusVISNarrowFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Narrow Visable Camera fail Bit */
bool GetGimbalMasterBITStatusVISNarrowFail();

/*Function Name       :    GetGimbalMasterBITStatusTIFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets TI Camera fail Bit */
bool GetGimbalMasterBITStatusTIFail();


/*----- DayCameraCommand Command -----*/

/*Function Name       :    SetDayCameraCommandZoomCommand
* Function Arguments  :    enum ZoomChange ZoomCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Zoom*/
int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand);

/*Function Name       :    SetDayCameraCommandSharpnessCommand
* Function Arguments  :    enum GenericIncDecVal SharpnessCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Sharpness*/
int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand);

/*Function Name       :    SetDayCameraCommandGammaCommand
* Function Arguments  :    enum GenericIncDecVal GammaCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Gamma*/
int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand);

/*Function Name       :    SetDayCameraCommandSaturationCommand
* Function Arguments  :    enum GenericIncDecVal SaturationCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Saturation*/
int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand);

/*Function Name       :    SetDayCameraCommandGainCommand
* Function Arguments  :    enum GenericIncDecVal GainCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Gain*/
int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand);

/*Function Name       :    SetDayCameraCommandLevelCommand
* Function Arguments  :    enum GenericIncDecVal LevelCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Level*/
int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand);

/*Function Name       :    SetDayCameraCommandExposureMode
* Function Arguments  :    enum DTVExposureModeSet ExposureMode
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Exposure Mode*/
int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode);

/*Function Name       :    SetDayCameraCommandExposureTime
* Function Arguments  :    enum GenericIncDecVal ExposureTime
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Exposure Time*/
int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime);

/*Function Name       :    SetDayCameraCommandExposureGain
* Function Arguments  :    enum GenericIncDecVal ExposureGain
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Exposure Gain*/
int SetDayCameraCommandExposureGain(enum GenericIncDecVal ExposureGain);

/*Function Name       :    SetDayCameraCommandAutoWhiteBalance
* Function Arguments  :    enum DTVAutoWhiteBalanceType AutoWhiteBalance
* Function Returns    :    int Success
* Function Description:    Sets Auto White Balance*/
int SetDayCameraCommandAutoWhiteBalance(enum DTVAutoWhiteBalanceType AutoWhiteBalance);

/*Function Name       :    SetDayCameraCommandTemperature
* Function Arguments  :    uint8_t Temperature
* Function Returns    :    int Success
* Function Description:    Sets Temperature*/
int SetDayCameraCommandTemperature(uint8_t Temperature);

/*Function Name       :    SetDayCameraCommandFlickerMode
* Function Arguments  :    int8_t FlickerMode
* Function Returns    :    int Success
* Function Description:    Sets Flicker Mode*/
int SetDayCameraCommandFlickerMode(int8_t FlickerMode);

/*Function Name       :    SetDayCameraCommandRestoreToDefault
* Function Arguments  :    bool RestoreToDefault
* Function Returns    :    int Success
* Function Description:    Restores All Parameters And Disables All Changes Excpet Zoom While Active*/
int SetDayCameraCommandRestoreToDefault(bool RestoreToDefault);


/*----- DayCameraSetFOV Command -----*/

/*Function Name       :    SetDayCameraSetFOVSetFOVCommand
* Function Arguments  :    float SetFOVCommand
* Function Returns    :    int Success
* Function Description:    Sets DayLightCamera Field Of View*/
int SetDayCameraSetFOVSetFOVCommand(float SetFOVCommand);


/*----- DayCameraReport Report -----*/

/*Function Name       :    GetDayCameraReportExposureMode
* Function Arguments  :    None
* Function Returns    :    DTVExposureMode 
* Function Description:    Gets DayLightCamera Exposure Mode */
enum DTVExposureMode GetDayCameraReportExposureMode();

/*Function Name       :    GetDayCameraReportExposureTime
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets DayLightCamera Exposure Time */
int32_t GetDayCameraReportExposureTime();

/*Function Name       :    GetDayCameraReportExposureGain
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets DayLightCamera Exposure Gain */
int32_t GetDayCameraReportExposureGain();

/*Function Name       :    GetDayCameraReportGammaValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Gamma Value */
float GetDayCameraReportGammaValue();

/*Function Name       :    GetDayCameraReportSaturationValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Saturation Value */
float GetDayCameraReportSaturationValue();

/*Function Name       :    GetDayCameraReportContrastValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Contrast Value */
float GetDayCameraReportContrastValue();

/*Function Name       :    GetDayCameraReportSharpnessValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Sharpness Value */
float GetDayCameraReportSharpnessValue();

/*Function Name       :    GetDayCameraReportFOV
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera FOV */
float GetDayCameraReportFOV();

/*Function Name       :    GetDayCameraReportZoomXAxisCenter
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Zoom X Axis Center */
float GetDayCameraReportZoomXAxisCenter();

/*Function Name       :    GetDayCameraReportZoomYAxisCenter
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Zoom Y Axis Center */
float GetDayCameraReportZoomYAxisCenter();

/*Function Name       :    GetDayCameraReportBrigthnessValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Brigthness Value */
float GetDayCameraReportBrigthnessValue();

/*Function Name       :    GetDayCameraReportAutoWhiteBalance
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets DayLightCamera Auto White Balance */
int32_t GetDayCameraReportAutoWhiteBalance();

/*Function Name       :    GetDayCameraReportTemperatureValue
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets DayLightCamera Temperature Value */
int32_t GetDayCameraReportTemperatureValue();

/*Function Name       :    GetDayCameraReportFlickerMode
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets DayLightCamera Flicker Mode */
float GetDayCameraReportFlickerMode();


/*----- ThermalCameraCommand Command -----*/

/*Function Name       :    SetThermalCameraCommandZoomCommand
* Function Arguments  :    enum ZoomChange ZoomCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Zoom Command*/
int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand);

/*Function Name       :    SetThermalCameraCommandGainCommand
* Function Arguments  :    enum GenericIncDecVal GainCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Gain Command*/
int SetThermalCameraCommandGainCommand(enum GenericIncDecVal GainCommand);

/*Function Name       :    SetThermalCameraCommandLevelCommand
* Function Arguments  :    enum GenericIncDecVal LevelCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Level Command*/
int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand);

/*Function Name       :    SetThermalCameraCommandGainModeCommand
* Function Arguments  :    enum ThermalCameraGainMode GainModeCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Gain Mode Command*/
int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand);

/*Function Name       :    SetThermalCameraCommandLinearPercentCommand
* Function Arguments  :    enum GenericIncDecVal LinearPercentCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Linear Percent Command*/
int SetThermalCameraCommandLinearPercentCommand(enum GenericIncDecVal LinearPercentCommand);

/*Function Name       :    SetThermalCameraCommandACECommand
* Function Arguments  :    enum GenericIncDecVal ACECommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Gamma Corective Value (ACE) Command*/
int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand);

/*Function Name       :    SetThermalCameraCommandSharpnessCommand
* Function Arguments  :    enum GenericIncDecVal SharpnessCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Sharpness Command*/
int SetThermalCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand);

/*Function Name       :    SetThermalCameraCommandNUCMode
* Function Arguments  :    enum ThermalCameraFFCMode NUCMode
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera NUC Mode*/
int SetThermalCameraCommandNUCMode(enum ThermalCameraFFCMode NUCMode);

/*Function Name       :    SetThermalCameraCommandColorPalette
* Function Arguments  :    enum ThermalCameraColorPalette ColorPalette
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Color Palette Command*/
int SetThermalCameraCommandColorPalette(enum ThermalCameraColorPalette ColorPalette);

/*Function Name       :    SetThermalCameraCommandNUC
* Function Arguments  :    enum NUCForceActivate NUC
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera NUC Force Set*/
int SetThermalCameraCommandNUC(enum NUCForceActivate NUC);

/*Function Name       :    SetThermalCameraCommandRestoreToDefault
* Function Arguments  :    bool RestoreToDefault
* Function Returns    :    int Success
* Function Description:    Restores All Parameters And Disables All Changes Excpet Zoom While Active*/
int SetThermalCameraCommandRestoreToDefault(bool RestoreToDefault);

/*Function Name       :    SetThermalCameraCommandSyncModeCommand
* Function Arguments  :    enum ThermalCameraSyncMode SyncModeCommand
* Function Returns    :    int Success
* Function Description:    Sets Thremal Camera Sync Mode Command*/
int SetThermalCameraCommandSyncModeCommand(enum ThermalCameraSyncMode SyncModeCommand);


/*----- TICameraSetFOV Command -----*/

/*Function Name       :    SetTICameraSetFOVSetFOVCommand
* Function Arguments  :    float SetFOVCommand
* Function Returns    :    int Success
* Function Description:    Sets TI Field Of View*/
int SetTICameraSetFOVSetFOVCommand(float SetFOVCommand);


/*----- ThermalCameraReport Report -----*/

/*Function Name       :    GetThermalCameraReportGainMode
* Function Arguments  :    None
* Function Returns    :    ThermalCameraGainMode 
* Function Description:    Gets Thermal Camera Gain Mode */
enum ThermalCameraGainMode GetThermalCameraReportGainMode();

/*Function Name       :    GetThermalCameraReportSyncMode
* Function Arguments  :    None
* Function Returns    :    ThermalCameraSyncMode 
* Function Description:    Gets Thermal Camera Sync Mode */
enum ThermalCameraSyncMode GetThermalCameraReportSyncMode();

/*Function Name       :    GetThermalCameraReportLinearPercent
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Thermal Camera Linear Percent */
float GetThermalCameraReportLinearPercent();

/*Function Name       :    GetThermalCameraReportACEValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Thermal Camera Gamma Corective Value (ACE) */
float GetThermalCameraReportACEValue();

/*Function Name       :    GetThermalCameraReportDDEValue
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Thermal Camera Headroom Detail (DDE) */
float GetThermalCameraReportDDEValue();

/*Function Name       :    GetThermalCameraReportColorPalette
* Function Arguments  :    None
* Function Returns    :    ThermalCameraColorPalleteMode 
* Function Description:    Gets Thermal Camera Color Palette Mode */
enum ThermalCameraColorPalleteMode GetThermalCameraReportColorPalette();

/*Function Name       :    GetThermalCameraReportNUCMode
* Function Arguments  :    None
* Function Returns    :    NUCMode 
* Function Description:    Gets Noc Enable Mode */
enum NUCMode GetThermalCameraReportNUCMode();

/*Function Name       :    GetThermalCameraReportFOV
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets Thermal Camera Field Of View */
float GetThermalCameraReportFOV();

/*Function Name       :    GetThermalCameraReportGainValue
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets Thremal Camera Gain Value */
int32_t GetThermalCameraReportGainValue();

/*Function Name       :    GetThermalCameraReportLevelValue
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets Thremal Camera Level Value */
int32_t GetThermalCameraReportLevelValue();


/*----- SeeSpotControlCommand Command -----*/

/*Function Name       :    SetSeeSpotControlCommandPower
* Function Arguments  :    bool Power
* Function Returns    :    int Success
* Function Description:    Sets SeeSpot camera 0 - Off. 1 - On*/
int SetSeeSpotControlCommandPower(bool Power);

/*Function Name       :    SetSeeSpotControlCommandFreeze
* Function Arguments  :    bool Freeze
* Function Returns    :    int Success
* Function Description:    Sets video freeze frame 0 - Off. 1 - On*/
int SetSeeSpotControlCommandFreeze(bool Freeze);


/*----- SeeSpotControlCommand Report -----*/

/*Function Name       :    GetSeeSpotControlCommandPower
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets SeeSpot camera 0 - Off. 1 - On */
bool GetSeeSpotControlCommandPower();

/*Function Name       :    GetSeeSpotControlCommandFreeze
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets video freeze frame 0 - Off. 1 - On */
bool GetSeeSpotControlCommandFreeze();


/*----- ClearStickyBIT Command -----*/

/*Function Name       :    SetClearStickyBIT
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Clear Cameras Sticky BITs*/
int SetClearStickyBIT();


/*----- CamerasBITStatus Report -----*/

/*Function Name       :    GetCamerasBITStatusDTVNarrowActiveError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Narrow Active Error BIT */
bool GetCamerasBITStatusDTVNarrowActiveError();

/*Function Name       :    GetCamerasBITStatusDTVNarrowStickyError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Narrow Sticky Error BIT */
bool GetCamerasBITStatusDTVNarrowStickyError();

/*Function Name       :    GetCamerasBITStatusDTVNarrowInitError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Narrow Init Error BIT */
bool GetCamerasBITStatusDTVNarrowInitError();

/*Function Name       :    GetCamerasBITStatusDTVNarrowVideoErrorsCounter
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets DTV Narrow Video Errors Counter BIT */
uint8_t GetCamerasBITStatusDTVNarrowVideoErrorsCounter();

/*Function Name       :    GetCamerasBITStatusDTVWideActiveError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Wide Active Error BIT */
bool GetCamerasBITStatusDTVWideActiveError();

/*Function Name       :    GetCamerasBITStatusDTVWideStickyError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Wide Sticky Error BIT */
bool GetCamerasBITStatusDTVWideStickyError();

/*Function Name       :    GetCamerasBITStatusDTVWideInitError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets DTV Wide Init Error BIT */
bool GetCamerasBITStatusDTVWideInitError();

/*Function Name       :    GetCamerasBITStatusDTVWideVideoErrorsCounter
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets DTV Wide Video Errors Counter BIT */
uint8_t GetCamerasBITStatusDTVWideVideoErrorsCounter();

/*Function Name       :    GetCamerasBITStatusTIActiveError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets TI Active Error BIT */
bool GetCamerasBITStatusTIActiveError();

/*Function Name       :    GetCamerasBITStatusTIStickyError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets TI Sticky Error BIT */
bool GetCamerasBITStatusTIStickyError();

/*Function Name       :    GetCamerasBITStatusTIInitError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets TI Init Error BIT */
bool GetCamerasBITStatusTIInitError();

/*Function Name       :    GetCamerasBITStatusTIVideoErrorsCounter
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets TI Video Errors Counter BIT */
uint8_t GetCamerasBITStatusTIVideoErrorsCounter();

/*Function Name       :    GetCamerasBITStatusUSBHubActiveError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets USB Hub Active Error BIT */
bool GetCamerasBITStatusUSBHubActiveError();

/*Function Name       :    GetCamerasBITStatusUSBHubStickyError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets USB Hub Sticky Error BIT */
bool GetCamerasBITStatusUSBHubStickyError();

/*Function Name       :    GetCamerasBITStatusUSBHubInitError
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets USB Hub Init Error BIT */
bool GetCamerasBITStatusUSBHubInitError();


/*----- VideoChannelCommand Command -----*/

/*Function Name       :    SetVideoChannelCommandPrimaryVideoChannel
* Function Arguments  :    enum Camera PrimaryVideoChannel
* Function Returns    :    int Success
* Function Description:    Selecet primary sensors DTV/TI*/
int SetVideoChannelCommandPrimaryVideoChannel(enum Camera PrimaryVideoChannel);

/*Function Name       :    SetVideoChannelCommandFOVSynchronizationMode
* Function Arguments  :    bool FOVSynchronizationMode
* Function Returns    :    int Success
* Function Description:    Enables/Disables sensors sync - secondary sensor changes its FOV according primary's FOV*/
int SetVideoChannelCommandFOVSynchronizationMode(bool FOVSynchronizationMode);

/*Function Name       :    SetVideoChannelCommandImageStabilizationEnable
* Function Arguments  :    bool ImageStabilizationEnable
* Function Returns    :    int Success
* Function Description:    Enables/Disables Image Stabilization*/
int SetVideoChannelCommandImageStabilizationEnable(bool ImageStabilizationEnable);

/*Function Name       :    SetVideoChannelCommandImageStabilizationMethod
* Function Arguments  :    bool ImageStabilizationMethod
* Function Returns    :    int Success
* Function Description:    Sets image stabilization method. 0 - Image based. 1 - IMU Based*/
int SetVideoChannelCommandImageStabilizationMethod(bool ImageStabilizationMethod);

/*Function Name       :    SetVideoChannelCommandOSDDisabled
* Function Arguments  :    bool OSDDisabled
* Function Returns    :    int Success
* Function Description:    Disables/Enables OSD layer (Set '1' to disable)*/
int SetVideoChannelCommandOSDDisabled(bool OSDDisabled);

/*Function Name       :    SetVideoChannelCommandPIPEnable
* Function Arguments  :    enum PIP PIPEnable
* Function Returns    :    int Success
* Function Description:    Enables/Disables streaming secondary sensor as Picture-In-Picture on video stream*/
int SetVideoChannelCommandPIPEnable(enum PIP PIPEnable);

/*Function Name       :    SetVideoChannelCommandHistogramEnable
* Function Arguments  :    bool HistogramEnable
* Function Returns    :    int Success
* Function Description:    Enables/Disables Histogram on video stream*/
int SetVideoChannelCommandHistogramEnable(bool HistogramEnable);

/*Function Name       :    SetVideoChannelCommandFontColor
* Function Arguments  :    enum FontColor FontColor
* Function Returns    :    int Success
* Function Description:    Sets OSD font color*/
int SetVideoChannelCommandFontColor(enum FontColor FontColor);


/*----- VideoChannelReport Report -----*/

/*Function Name       :    GetVideoChannelReportPrimaryVideoChannel
* Function Arguments  :    None
* Function Returns    :    Camera 
* Function Description:    Selecet primary sensors DTV/TI */
enum Camera GetVideoChannelReportPrimaryVideoChannel();

/*Function Name       :    GetVideoChannelReportFOVSynchronizationMode
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Enables/Disables sensors sync - secondary sensor changes its FOV according primary's FOV */
bool GetVideoChannelReportFOVSynchronizationMode();

/*Function Name       :    GetVideoChannelReportImageStabilizationEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Enables/Disables Image Stabilization */
bool GetVideoChannelReportImageStabilizationEnable();

/*Function Name       :    GetVideoChannelReportImageStabilizationMethod
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets image stabilization method. 0 - Image based. 1 - IMU Based */
bool GetVideoChannelReportImageStabilizationMethod();

/*Function Name       :    GetVideoChannelReportOSDDisabled
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Disables/Enables OSD layer (Set '1' to disable) */
bool GetVideoChannelReportOSDDisabled();

/*Function Name       :    GetVideoChannelReportPIPEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Enables/Disables streaming secondary sensor as Picture-In-Picture on video stream */
bool GetVideoChannelReportPIPEnable();

/*Function Name       :    GetVideoChannelReportHistogramEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Enables/Disables Histogram on video stream */
bool GetVideoChannelReportHistogramEnable();

/*Function Name       :    GetVideoChannelReportFontColor
* Function Arguments  :    None
* Function Returns    :    FontColor 
* Function Description:    Sets OSD font color */
enum FontColor GetVideoChannelReportFontColor();


/*----- VideoChannelRunTimeConfig Command -----*/

/*Function Name       :    SetVideoChannelRunTimeConfigStopChannel0Stream
* Function Arguments  :    bool StopChannel0Stream
* Function Returns    :    int Success
* Function Description:    Stop Channel 0 Stream*/
int SetVideoChannelRunTimeConfigStopChannel0Stream(bool StopChannel0Stream);

/*Function Name       :    SetVideoChannelRunTimeConfigStartChannel0Stream
* Function Arguments  :    bool StartChannel0Stream
* Function Returns    :    int Success
* Function Description:    Start Channel 0 Stream*/
int SetVideoChannelRunTimeConfigStartChannel0Stream(bool StartChannel0Stream);

/*Function Name       :    SetVideoChannelRunTimeConfigH264Channel0Stream
* Function Arguments  :    bool H264Channel0Stream
* Function Returns    :    int Success
* Function Description:    Set Channel 0 Stream to H264 encoding*/
int SetVideoChannelRunTimeConfigH264Channel0Stream(bool H264Channel0Stream);

/*Function Name       :    SetVideoChannelRunTimeConfigH265Channel0Stream
* Function Arguments  :    bool H265Channel0Stream
* Function Returns    :    int Success
* Function Description:    Set Channel 0 Stream to H265 encoding*/
int SetVideoChannelRunTimeConfigH265Channel0Stream(bool H265Channel0Stream);

/*Function Name       :    SetVideoChannelRunTimeConfigBandwidthChannel0Stream
* Function Arguments  :    int32_t BandwidthChannel0Stream
* Function Returns    :    int Success
* Function Description:    Set Channel 0 Bandwidth - leave 0 if no change required*/
int SetVideoChannelRunTimeConfigBandwidthChannel0Stream(int32_t BandwidthChannel0Stream);

/*Function Name       :    SetVideoChannelRunTimeConfigStopDTVStream
* Function Arguments  :    bool StopDTVStream
* Function Returns    :    int Success
* Function Description:    Stop DTV Wide and DTV Narrow pipeline*/
int SetVideoChannelRunTimeConfigStopDTVStream(bool StopDTVStream);

/*Function Name       :    SetVideoChannelRunTimeConfigStopTIStream
* Function Arguments  :    bool StopTIStream
* Function Returns    :    int Success
* Function Description:    Stop TI pipeline*/
int SetVideoChannelRunTimeConfigStopTIStream(bool StopTIStream);


/*----- RecordVideoCommand Command -----*/

/*Function Name       :    SetRecordVideoCommandEnable
* Function Arguments  :    bool Enable
* Function Returns    :    int Success
* Function Description:    Starts/Stops Recording Video*/
int SetRecordVideoCommandEnable(bool Enable);

/*Function Name       :    SetRecordVideoCommandEnableRaw
* Function Arguments  :    bool EnableRaw
* Function Returns    :    int Success
* Function Description:    Starts/Stops Recording Raw Video*/
int SetRecordVideoCommandEnableRaw(bool EnableRaw);

/*Function Name       :    SetRecordVideoCommandDeleteALLRecords
* Function Arguments  :    bool DeleteALLRecords
* Function Returns    :    int Success
* Function Description:    Deletes All Records*/
int SetRecordVideoCommandDeleteALLRecords(bool DeleteALLRecords);

/*Function Name       :    SetRecordVideoCommandFolderName
* Function Arguments  :    char* FolderName, size_t Size - C string should be at most 19 Bytes.
* Function Returns    :    int Success
* Function Description:    Sets Record Video Folder Name*/
int SetRecordVideoCommandFolderName(char* FolderName, size_t Size);


/*----- RecordVideoReport Report -----*/

/*Function Name       :    GetRecordVideoReportRecordVideoTimestamp_nSec_
* Function Arguments  :    None
* Function Returns    :    int64_t 
* Function Description:    Gets Record Video Timestamp [nSec] */
int64_t GetRecordVideoReportRecordVideoTimestamp_nSec_();

/*Function Name       :    GetRecordVideoReportRecordRawVideoTimestamp_nSec_
* Function Arguments  :    None
* Function Returns    :    int64_t 
* Function Description:    Gets Record Raw Video Timestamp [nSec] */
int64_t GetRecordVideoReportRecordRawVideoTimestamp_nSec_();

/*Function Name       :    GetRecordVideoReportDiskFreeSpace_GB_
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Disk Free Space [GB] */
float GetRecordVideoReportDiskFreeSpace_GB_();

/*Function Name       :    GetRecordVideoReportDiskFreeSpace_Percent_
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Disk Free Space [%] */
float GetRecordVideoReportDiskFreeSpace_Percent_();

/*Function Name       :    GetRecordVideoReportTotalDiskSize_GB_
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Total Disk Size [GB] */
float GetRecordVideoReportTotalDiskSize_GB_();

/*Function Name       :    GetRecordVideoReportRecordVideoEnable
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Record Video Enable */
int32_t GetRecordVideoReportRecordVideoEnable();

/*Function Name       :    GetRecordVideoReportRecordRawVideoEnable
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Record Raw Video Enable */
int32_t GetRecordVideoReportRecordRawVideoEnable();

/*Function Name       :    GetRecordVideoReportVideoDelete
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    ALL Recorded video is in deletion process */
int32_t GetRecordVideoReportVideoDelete();

/*Function Name       :    GetRecordVideoReportDiskStatus
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Disk Status */
int32_t GetRecordVideoReportDiskStatus();

/*Function Name       :    GetRecordVideoReportFolderName
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 20
* Function Returns    :    void 
* Function Description:    Sets Record Video Folder Name */
void GetRecordVideoReportFolderName(char* Buffer, size_t Size);


/*----- Track Command -----*/

/*Function Name       :    SetTrack
* Function Arguments  :    int16_t TargetXCoordinate, int16_t TargetYCoordinate
* Function Returns    :    int Success
* Function Description:    Change X, Y coordination in pixels, relative to center of screen. Positive direction is Right, Down.*/
int SetTrack(int16_t TargetXCoordinate, int16_t TargetYCoordinate);


/*----- TrackerTargetErrorReport Report -----*/

/*Function Name       :    GetTrackerTargetErrorReportCamera
* Function Arguments  :    None
* Function Returns    :    Camera 
* Function Description:    Gets active sensor */
enum Camera GetTrackerTargetErrorReportCamera();

/*Function Name       :    GetTrackerTargetErrorReportTrackerGeneralStatus
* Function Arguments  :    None
* Function Returns    :    TrackerStatus 
* Function Description:    Gets tracker status */
enum TrackerStatus GetTrackerTargetErrorReportTrackerGeneralStatus();

/*Function Name       :    GetTrackerTargetErrorReportTrackingQuality
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets tracker quality, in range [0 (worst) ÷ 7(best)] */
uint8_t GetTrackerTargetErrorReportTrackingQuality();

/*Function Name       :    GetTrackerTargetErrorReportFirstTimeTrack
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:     */
bool GetTrackerTargetErrorReportFirstTimeTrack();

/*Function Name       :    GetTrackerTargetErrorReportCurrFOV
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets current FOV of selected sensor */
uint16_t GetTrackerTargetErrorReportCurrFOV();

/*Function Name       :    GetTrackerTargetErrorReportTrackerXError
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets tracker error at X axis, in pixles */
float GetTrackerTargetErrorReportTrackerXError();

/*Function Name       :    GetTrackerTargetErrorReportTrackerYError
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets tracker error at Y axis, in pixles */
float GetTrackerTargetErrorReportTrackerYError();

/*Function Name       :    GetTrackerTargetErrorReportGMC
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets GMC value */
int8_t GetTrackerTargetErrorReportGMC();

/*Function Name       :    GetTrackerTargetErrorReportGateSizeX
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets tracker gate size width */
uint16_t GetTrackerTargetErrorReportGateSizeX();

/*Function Name       :    GetTrackerTargetErrorReportGateSizeY
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets tracker gate size heigh */
uint16_t GetTrackerTargetErrorReportGateSizeY();


/*----- TrackerParameters Command -----*/

/*Function Name       :    SetTrackerParametersTrackerType
* Function Arguments  :    enum TrackerType TrackerType
* Function Returns    :    int Success
* Function Description:    Sets target type accoording to TargetType Table. This field sets automatically the gate size.*/
int SetTrackerParametersTrackerType(enum TrackerType TrackerType);

/*Function Name       :    SetTrackerParametersGateXSize
* Function Arguments  :    uint16_t GateXSize
* Function Returns    :    int Success
* Function Description:    Sets gate width in case of manual gate size. The value range [5 ÷ (screen width / 2)]*/
int SetTrackerParametersGateXSize(uint16_t GateXSize);

/*Function Name       :    SetTrackerParametersGateYSize
* Function Arguments  :    uint16_t GateYSize
* Function Returns    :    int Success
* Function Description:    Sets gate heigh in case of manual gate size. The value range [5 ÷ (screen heigh / 2)]*/
int SetTrackerParametersGateYSize(uint16_t GateYSize);

/*Function Name       :    SetTrackerParametersGateSizeEnable
* Function Arguments  :    bool GateSizeEnable
* Function Returns    :    int Success
* Function Description:    Enables/Disables manual gate size according proviided X, Y sizes. When disabled, gate size will be accodring to TargetType*/
int SetTrackerParametersGateSizeEnable(bool GateSizeEnable);


/*----- TrackerParametersReport Report -----*/

/*Function Name       :    GetTrackerParametersReportTrackerType
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets selected target type. */
int8_t GetTrackerParametersReportTrackerType();

/*Function Name       :    GetTrackerParametersReportGateXSize
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets current gate X size */
uint16_t GetTrackerParametersReportGateXSize();

/*Function Name       :    GetTrackerParametersReportGateYSize
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets current gate Y size */
uint16_t GetTrackerParametersReportGateYSize();

/*Function Name       :    GetTrackerParametersReportGateSizeEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if manual gate size enabled/disabled */
bool GetTrackerParametersReportGateSizeEnable();


/*----- TrackerAdjustControls Command -----*/

/*Function Name       :    SetTrackerAdjustControls
* Function Arguments  :    bool AdjustEnable, int16_t MovementCommand_X, int16_t MovementCommand_Y
* Function Returns    :    int Success
* Function Description:    Sets the values how much to adjust the tracker gate, in units of pixels, for both axis X, Y. Positive direction is Right, Down. On Y Axis*/
int SetTrackerAdjustControls(bool AdjustEnable, int16_t MovementCommand_X, int16_t MovementCommand_Y);


/*----- TrackerAdjustControlsReport Report -----*/

/*Function Name       :    GetTrackerAdjustControlsReportAdjustEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if adjust function is enabled/disabled */
bool GetTrackerAdjustControlsReportAdjustEnable();

/*Function Name       :    GetTrackerAdjustControlsReportMovementX
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Returns provided adjust X movement */
int16_t GetTrackerAdjustControlsReportMovementX();

/*Function Name       :    GetTrackerAdjustControlsReportMovementY
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Returns provided adjust Y movement */
int16_t GetTrackerAdjustControlsReportMovementY();


/*----- TrackerOffsetControls Command -----*/

/*Function Name       :    SetTrackerOffsetControls
* Function Arguments  :    bool OffsetEnable, int16_t Movementcommand_X, int16_t Movementcommand_Y
* Function Returns    :    int Success
* Function Description:    Sets the values how much to Offset the tracker gate, in units of pixels, for both axis X, Y. Positive direction is Right, Down. On Y Axis*/
int SetTrackerOffsetControls(bool OffsetEnable, int16_t Movementcommand_X, int16_t Movementcommand_Y);


/*----- TrackerOffsetControlsReport Report -----*/

/*Function Name       :    GetTrackerOffsetControlsReportOffsetEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if offset function is enabled/disabled */
bool GetTrackerOffsetControlsReportOffsetEnable();

/*Function Name       :    GetTrackerOffsetControlsReportMovementX
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Returns provided offset X movement */
int16_t GetTrackerOffsetControlsReportMovementX();

/*Function Name       :    GetTrackerOffsetControlsReportMovementY
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Returns provided offset Y movement */
int16_t GetTrackerOffsetControlsReportMovementY();


/*----- VMDControls Command -----*/

/*Function Name       :    SetVMDControlsEnable
* Function Arguments  :    bool Enable
* Function Returns    :    int Success
* Function Description:    Enable/Disable VMD*/
int SetVMDControlsEnable(bool Enable);


/*----- VMDReport Report -----*/

/*Function Name       :    GetVMDReportEnableVMD
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:     */
bool GetVMDReportEnableVMD();

/*Function Name       :    GetVMDReportQuantityofDetections
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:     */
uint8_t GetVMDReportQuantityofDetections();


/*----- ATRControls Command -----*/

/*Function Name       :    SetATRControlsDetectionsReport
* Function Arguments  :    bool DetectionsReport
* Function Returns    :    int Success
* Function Description:    Enable/Disable ATR*/
int SetATRControlsDetectionsReport(bool DetectionsReport);

/*Function Name       :    SetATRControlsDetectionThreshold
* Function Arguments  :    int8_t DetectionThreshold
* Function Returns    :    int Success
* Function Description:    Set Detection Threshold [0-100]*/
int SetATRControlsDetectionThreshold(int8_t DetectionThreshold);


/*----- ATRReport Report -----*/

/*Function Name       :    GetATRReportEnabled
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets ATR Status Enabled/Disabled */
bool GetATRReportEnabled();

/*Function Name       :    GetATRReportQuantityofDetections
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Quantity of Detections */
uint8_t GetATRReportQuantityofDetections();


/*----- NavigationInit Command -----*/

/*Function Name       :    SetNavigationInitXMountingAngle
* Function Arguments  :    int16_t XMountingAngle
* Function Returns    :    int Success
* Function Description:    Sets platform X mounting angle installation [LSB:360º/65536]*/
int SetNavigationInitXMountingAngle(int16_t XMountingAngle);

/*Function Name       :    SetNavigationInitYMountingAngle
* Function Arguments  :    int16_t YMountingAngle
* Function Returns    :    int Success
* Function Description:    Sets platform Y mounting angle installation [LSB:360º/65536]*/
int SetNavigationInitYMountingAngle(int16_t YMountingAngle);

/*Function Name       :    SetNavigationInitZMountingAngle
* Function Arguments  :    int16_t ZMountingAngle
* Function Returns    :    int Success
* Function Description:    Sets platform Z mounting angle installation [LSB:360º/65536]*/
int SetNavigationInitZMountingAngle(int16_t ZMountingAngle);

/*Function Name       :    SetNavigationInitLeverArmX
* Function Arguments  :    int16_t LeverArmX
* Function Returns    :    int Success
* Function Description:    Sets platform lever arm X installation [LSB: 1CM]*/
int SetNavigationInitLeverArmX(int16_t LeverArmX);

/*Function Name       :    SetNavigationInitLeverArmY
* Function Arguments  :    int16_t LeverArmY
* Function Returns    :    int Success
* Function Description:    Sets platform lever arm Y installation [LSB: 1CM]*/
int SetNavigationInitLeverArmY(int16_t LeverArmY);

/*Function Name       :    SetNavigationInitLeverArmZ
* Function Arguments  :    int16_t LeverArmZ
* Function Returns    :    int Success
* Function Description:    Sets platform lever arm Z installation [LSB: 1CM]*/
int SetNavigationInitLeverArmZ(int16_t LeverArmZ);

/*Function Name       :    SetNavigationInitPlatformBodyAzimuth
* Function Arguments  :    int16_t PlatformBodyAzimuth
* Function Returns    :    int Success
* Function Description:    Sets platform body azimuth installation [LSB:360º/65536]*/
int SetNavigationInitPlatformBodyAzimuth(int16_t PlatformBodyAzimuth);

/*Function Name       :    SetNavigationInitPlatformBodyPitch
* Function Arguments  :    int16_t PlatformBodyPitch
* Function Returns    :    int Success
* Function Description:    Sets platform body pitch installation [LSB:360º/65536]*/
int SetNavigationInitPlatformBodyPitch(int16_t PlatformBodyPitch);

/*Function Name       :    SetNavigationInitPlatformBodyRoll
* Function Arguments  :    int16_t PlatformBodyRoll
* Function Returns    :    int Success
* Function Description:    Sets platform body roll installation [LSB:360º/65536]*/
int SetNavigationInitPlatformBodyRoll(int16_t PlatformBodyRoll);


/*----- NavigationInit Report -----*/

/*Function Name       :    GetNavigationInitXMountingAngle
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform X mounting angle installation [LSB:360º/65536] */
int16_t GetNavigationInitXMountingAngle();

/*Function Name       :    GetNavigationInitYMountingAngle
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform Y mounting angle installation [LSB:360º/65536] */
int16_t GetNavigationInitYMountingAngle();

/*Function Name       :    GetNavigationInitZMountingAngle
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform Z mounting angle installation [LSB:360º/65536] */
int16_t GetNavigationInitZMountingAngle();

/*Function Name       :    GetNavigationInitLeverArmX
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform lever arm X installation [LSB: 1CM] */
int16_t GetNavigationInitLeverArmX();

/*Function Name       :    GetNavigationInitLeverArmY
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform lever arm Y installation [LSB: 1CM] */
int16_t GetNavigationInitLeverArmY();

/*Function Name       :    GetNavigationInitLeverArmZ
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform lever arm Z installation [LSB: 1CM] */
int16_t GetNavigationInitLeverArmZ();

/*Function Name       :    GetNavigationInitPlatformBodyAzimuth
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform body azimuth installation [LSB:360º/65536] */
int16_t GetNavigationInitPlatformBodyAzimuth();

/*Function Name       :    GetNavigationInitPlatformBodyPitch
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform body pitch installation [LSB:360º/65536] */
int16_t GetNavigationInitPlatformBodyPitch();

/*Function Name       :    GetNavigationInitPlatformBodyRoll
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets platform body roll installation [LSB:360º/65536] */
int16_t GetNavigationInitPlatformBodyRoll();


/*----- NavigationProperties Command -----*/

/*Function Name       :    SetNavigationPropertiesPlatformDataSource
* Function Arguments  :    int8_t PlatformDataSource
* Function Returns    :    int Success
* Function Description:    Enables data position and velocity from platform. 0 - Disable. 1 - Enable*/
int SetNavigationPropertiesPlatformDataSource(int8_t PlatformDataSource);

/*Function Name       :    SetNavigationPropertiesUsePlatformData
* Function Arguments  :    bool UsePlatformData
* Function Returns    :    int Success
* Function Description:    Sets either to use platform data (if Platfrom Data source enabled). 0 - Disable. 1 - Enable*/
int SetNavigationPropertiesUsePlatformData(bool UsePlatformData);

/*Function Name       :    SetNavigationPropertiesNavigationType
* Function Arguments  :    enum NavigationType NavigationType
* Function Returns    :    int Success
* Function Description:    Sets navigation type from. 0 - Payload IMU. 1 - Base IMU + Encoders*/
int SetNavigationPropertiesNavigationType(enum NavigationType NavigationType);

/*Function Name       :    SetNavigationPropertiesAltitudeType
* Function Arguments  :    enum NavAltituteType AltitudeType
* Function Returns    :    int Success
* Function Description:    Sets Altitude type. 0 - ellipsoid. 1 - mean sea level.*/
int SetNavigationPropertiesAltitudeType(enum NavAltituteType AltitudeType);


/*----- NavigationStatus Report -----*/

/*Function Name       :    GetNavigationStatusPlatformDataSource
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets data position and velocity from platform. 0 - Disable. 1 - Enable */
int8_t GetNavigationStatusPlatformDataSource();

/*Function Name       :    GetNavigationStatusUsePlatformData
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets either to use platform data (if Platfrom Data source enabled). 0 - Disable. 1 - Enable */
bool GetNavigationStatusUsePlatformData();

/*Function Name       :    GetNavigationStatusNavigationType
* Function Arguments  :    None
* Function Returns    :    NavigationType 
* Function Description:    Gets navigation type from. 0 - Payload IMU. 1 - Base IMU + Encoders */
enum NavigationType GetNavigationStatusNavigationType();

/*Function Name       :    GetNavigationStatusAltitudeType
* Function Arguments  :    None
* Function Returns    :    NavAltituteType 
* Function Description:    Gets Altitude type. 0 - ellipsoid. 1 - mean sea level. */
enum NavAltituteType GetNavigationStatusAltitudeType();


/*----- PlatformNavigationDataAccuracy Command -----*/

/*Function Name       :    SetPlatformNavigationDataAccuracyPositionAccuracy
* Function Arguments  :    int8_t PositionAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform horizontal position data (latitude, longitude)*/
int SetPlatformNavigationDataAccuracyPositionAccuracy(int8_t PositionAccuracy);

/*Function Name       :    SetPlatformNavigationDataAccuracyAltitudeAccuracy
* Function Arguments  :    int8_t AltitudeAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform altitude data*/
int SetPlatformNavigationDataAccuracyAltitudeAccuracy(int8_t AltitudeAccuracy);

/*Function Name       :    SetPlatformNavigationDataAccuracyAttitudeAccuracy
* Function Arguments  :    int8_t AttitudeAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform Attitude data*/
int SetPlatformNavigationDataAccuracyAttitudeAccuracy(int8_t AttitudeAccuracy);

/*Function Name       :    SetPlatformNavigationDataAccuracyAzimuthAccuracy
* Function Arguments  :    int8_t AzimuthAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform Azimuth data*/
int SetPlatformNavigationDataAccuracyAzimuthAccuracy(int8_t AzimuthAccuracy);

/*Function Name       :    SetPlatformNavigationDataAccuracyVelocityAccuracy
* Function Arguments  :    int8_t VelocityAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform  horizontal velocity data*/
int SetPlatformNavigationDataAccuracyVelocityAccuracy(int8_t VelocityAccuracy);

/*Function Name       :    SetPlatformNavigationDataAccuracyRateofClimbAccuracy
* Function Arguments  :    int8_t RateofClimbAccuracy
* Function Returns    :    int Success
* Function Description:    Estimated accuracy of platform  vertical velocity data*/
int SetPlatformNavigationDataAccuracyRateofClimbAccuracy(int8_t RateofClimbAccuracy);


/*----- PlatformNavigationData Command -----*/

/*Function Name       :    SetPlatformNavigationDataTimeTag
* Function Arguments  :    int32_t TimeTag
* Function Returns    :    int Success
* Function Description:    Sets platform Time Tag [GPS TOW uSec, cyclic data]*/
int SetPlatformNavigationDataTimeTag(int32_t TimeTag);

/*Function Name       :    SetPlatformNavigationDataPlatformLongitude
* Function Arguments  :    float PlatformLongitude
* Function Returns    :    int Success
* Function Description:    Sets platform longitude [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformLongitude(float PlatformLongitude);

/*Function Name       :    SetPlatformNavigationDataPlatformLatitude
* Function Arguments  :    float PlatformLatitude
* Function Returns    :    int Success
* Function Description:    Sets platform Latitude [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformLatitude(float PlatformLatitude);

/*Function Name       :    SetPlatformNavigationDataPlatformAltitude
* Function Arguments  :    float PlatformAltitude
* Function Returns    :    int Success
* Function Description:    Sets platform Altitude [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformAltitude(float PlatformAltitude);

/*Function Name       :    SetPlatformNavigationDataPlatformBodyAzimuth
* Function Arguments  :    float PlatformBodyAzimuth
* Function Returns    :    int Success
* Function Description:    Sets platform body azimuth [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformBodyAzimuth(float PlatformBodyAzimuth);

/*Function Name       :    SetPlatformNavigationDataPlatformBodyPitch
* Function Arguments  :    float PlatformBodyPitch
* Function Returns    :    int Success
* Function Description:    Sets platform body pitch [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformBodyPitch(float PlatformBodyPitch);

/*Function Name       :    SetPlatformNavigationDataPlatformBodyRoll
* Function Arguments  :    float PlatformBodyRoll
* Function Returns    :    int Success
* Function Description:    Sets platform body roll [rad, cyclic data]*/
int SetPlatformNavigationDataPlatformBodyRoll(float PlatformBodyRoll);

/*Function Name       :    SetPlatformNavigationDataGroundSpeed
* Function Arguments  :    int8_t GroundSpeed
* Function Returns    :    int Success
* Function Description:    Sets platform ground speed [1cm/sec, cyclic data]*/
int SetPlatformNavigationDataGroundSpeed(int8_t GroundSpeed);

/*Function Name       :    SetPlatformNavigationDataRateofClimb
* Function Arguments  :    int8_t RateofClimb
* Function Returns    :    int Success
* Function Description:    Sets platform rate of climb [1cm/sec, cyclic data]*/
int SetPlatformNavigationDataRateofClimb(int8_t RateofClimb);


/*----- PlatformNavigationData Report -----*/

/*Function Name       :    GetPlatformNavigationDataTimeTag
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets platform Time Tag [GPS TOW uSec, cyclic data] */
int32_t GetPlatformNavigationDataTimeTag();

/*Function Name       :    GetPlatformNavigationDataPlatformLongitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets platform longitude [rad, cyclic data] */
float GetPlatformNavigationDataPlatformLongitude();

/*Function Name       :    GetPlatformNavigationDataPlatformLatitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets platform Latitude [rad, cyclic data] */
float GetPlatformNavigationDataPlatformLatitude();

/*Function Name       :    GetPlatformNavigationDataPlatformAltitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets platform Altitude [rad, cyclic data] */
float GetPlatformNavigationDataPlatformAltitude();

/*Function Name       :    GetPlatformNavigationDataLOSAzimuth
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Azimuth [rad, cyclic data] */
float GetPlatformNavigationDataLOSAzimuth();

/*Function Name       :    GetPlatformNavigationDataLOSPitch
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Pitch [rad, cyclic data] */
float GetPlatformNavigationDataLOSPitch();

/*Function Name       :    GetPlatformNavigationDataLOSRoll
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Roll [rad, cyclic data] */
float GetPlatformNavigationDataLOSRoll();

/*Function Name       :    GetPlatformNavigationDataTargetLongitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Azimuth [rad, cyclic data] */
float GetPlatformNavigationDataTargetLongitude();

/*Function Name       :    GetPlatformNavigationDataTargetLatitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Pitch [rad, cyclic data] */
float GetPlatformNavigationDataTargetLatitude();

/*Function Name       :    GetPlatformNavigationDataTargetAltitude
* Function Arguments  :    None
* Function Returns    :    float 
* Function Description:    Gets LOS Roll [rad, cyclic data] */
float GetPlatformNavigationDataTargetAltitude();

/*Function Name       :    GetPlatformNavigationDataGroundSpeed
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets platform ground speed [1cm/sec, cyclic data] */
int8_t GetPlatformNavigationDataGroundSpeed();

/*Function Name       :    GetPlatformNavigationDataRateofClimb
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets platform rate of climb [1cm/sec, cyclic data] */
int8_t GetPlatformNavigationDataRateofClimb();


/*----- NavigationIMUData Report -----*/

/*Function Name       :    GetNavigationIMUDataIMUTimeTag
* Function Arguments  :    None
* Function Returns    :    int64_t 
* Function Description:    Gets IMU Time Tag [GPS TOW uSec] */
int64_t GetNavigationIMUDataIMUTimeTag();

/*Function Name       :    GetNavigationIMUDataIMUXRate
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU X Rate [LSB:1urad/sec] */
int32_t GetNavigationIMUDataIMUXRate();

/*Function Name       :    GetNavigationIMUDataIMUYRate
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU Y Rate [LSB:1urad/sec] */
int32_t GetNavigationIMUDataIMUYRate();

/*Function Name       :    GetNavigationIMUDataIMUZRate
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU Z Rate [LSB:1urad/sec] */
int32_t GetNavigationIMUDataIMUZRate();

/*Function Name       :    GetNavigationIMUDataIMUXAcc
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU X Acc [LSB:1uG] */
int32_t GetNavigationIMUDataIMUXAcc();

/*Function Name       :    GetNavigationIMUDataIMUYAcc
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU Y Acc [LSB:1uG] */
int32_t GetNavigationIMUDataIMUYAcc();

/*Function Name       :    GetNavigationIMUDataIMUZAcc
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets IMU Z Acc [LSB:1uG] */
int32_t GetNavigationIMUDataIMUZAcc();

/*Function Name       :    GetNavigationIMUDataIMUTemperature
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets IMU Temperature [LSB:°C] */
int8_t GetNavigationIMUDataIMUTemperature();

/*Function Name       :    GetNavigationIMUDataIMUStatus
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets IMU Status */
int16_t GetNavigationIMUDataIMUStatus();


/*----- GPSData Report -----*/

/*Function Name       :    GetGPSDataLongitude
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets GPS Longitude [rad] */
int32_t GetGPSDataLongitude();

/*Function Name       :    GetGPSDataLatitude
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets GPS Latitude [rad] */
int32_t GetGPSDataLatitude();

/*Function Name       :    GetGPSDataAltitude
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets GPS Altitude [rad] */
int32_t GetGPSDataAltitude();

/*Function Name       :    GetGPSDataAzimuth
* Function Arguments  :    None
* Function Returns    :    int16_t 
* Function Description:    Gets GPS Azimuth [LSB:360º/65536] */
int16_t GetGPSDataAzimuth();

/*Function Name       :    GetGPSDataSecond
* Function Arguments  :    None
* Function Returns    :    uint32_t 
* Function Description:    Gets GPS Time in seconds elapsed since Jan. 6, 1980. [LSB: 1 sec] */
uint32_t GetGPSDataSecond();

/*Function Name       :    GetGPSDatamilliseconds
* Function Arguments  :    None
* Function Returns    :    uint32_t 
* Function Description:    Gets GPS Time remainder in msec. [LSB: 1 msec] */
uint32_t GetGPSDatamilliseconds();

/*Function Name       :    GetGPSDataNumerOfSatellites
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets GPS Numer of Satellites */
int8_t GetGPSDataNumerOfSatellites();

/*Function Name       :    GetGPSDataPositionValid
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets GPS Position information valid. 0 - valid. 1 - not valid */
bool GetGPSDataPositionValid();


/*----- NavigationRecord Command -----*/

/*Function Name       :    SetNavigationRecordRecordLevel
* Function Arguments  :    int8_t RecordLevel
* Function Returns    :    int Success
* Function Description:    Enables navigation record and level*/
int SetNavigationRecordRecordLevel(int8_t RecordLevel);


/*----- GPSRecording Report -----*/

/*Function Name       :    GetGPSRecordingGPSRecordingLevel
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets GPS Recording level status */
int8_t GetGPSRecordingGPSRecordingLevel();

/*Function Name       :    GetGPSRecordingGPSFreeMemory
* Function Arguments  :    None
* Function Returns    :    int32_t 
* Function Description:    Gets free memory on recording device in kBytes */
int32_t GetGPSRecordingGPSFreeMemory();


/*----- NavigationBIT Report -----*/

/*Function Name       :    GetNavigationBITNavigationaccuracyWarning
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Navigation accuracy Warning Flag */
bool GetNavigationBITNavigationaccuracyWarning();

/*Function Name       :    GetNavigationBITGPSCommFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets GPS Comm Fail */
bool GetNavigationBITGPSCommFail();

/*Function Name       :    GetNavigationBITGPSprecisionFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets GPS precision Fail */
bool GetNavigationBITGPSprecisionFail();

/*Function Name       :    GetNavigationBITGPSantennaFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets GPS antenna Fail */
bool GetNavigationBITGPSantennaFail();

/*Function Name       :    GetNavigationBITIMUFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets IMU Fail */
bool GetNavigationBITIMUFail();

/*Function Name       :    GetNavigationBITPPSFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets PPS Fail */
bool GetNavigationBITPPSFail();

/*Function Name       :    GetNavigationBITCalibrationFail
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Calibration Fail */
bool GetNavigationBITCalibrationFail();


/*----- Designator Command -----*/

/*Function Name       :    SetDesignatorPower
* Function Arguments  :    bool Power
* Function Returns    :    int Success
* Function Description:    Sets Designator Power 0 - Off. 1 - On*/
int SetDesignatorPower(bool Power);

/*Function Name       :    SetDesignatorArm
* Function Arguments  :    bool Arm
* Function Returns    :    int Success
* Function Description:    Sets Designator Arm 0 - Off. 1 - On*/
int SetDesignatorArm(bool Arm);


/*----- DesignatorStatus Report -----*/

/*Function Name       :    GetDesignatorStatusPower
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator Power 0 - Off. 1 - On */
bool GetDesignatorStatusPower();

/*Function Name       :    GetDesignatorStatusArm
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator Arm 0 - Off. 1 - On */
bool GetDesignatorStatusArm();

/*Function Name       :    GetDesignatorStatusReady
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator Ready 0 - Not Ready for fire. 1 - Ready for fire */
bool GetDesignatorStatusReady();

/*Function Name       :    GetDesignatorStatusSafetySwitch
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator Safety Switch 0 - Off (Fire Disabled). 1 - On (Fire Enabled) */
bool GetDesignatorStatusSafetySwitch();

/*Function Name       :    GetDesignatorStatusEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator HW Input status 0 - Off (Designator Disabled). 1 - On (Designator Enabled) */
bool GetDesignatorStatusEnable();


/*----- Designator Command -----*/

/*Function Name       :    SetDesignatorFire
* Function Arguments  :    bool Fire
* Function Returns    :    int Success
* Function Description:    Sets Designator Fire. 0 - Fire Off. 1 - Fire On*/
int SetDesignatorFire(bool Fire);


/*----- Designator Report -----*/

/*Function Name       :    GetDesignatorFire
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Designator Fire. 0 - Fire Off. 1 - Fire On */
bool GetDesignatorFire();

/*Function Name       :    GetDesignatorFireBlocked
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Fire Blocked. 0 - Fire not blocked. 1 - Fire blocked */
bool GetDesignatorFireBlocked();


/*----- DesignatorNATO Command -----*/

/*Function Name       :    SetDesignatorNATOCodeValue
* Function Arguments  :    uint16_t CodeValue
* Function Returns    :    int Success
* Function Description:    Sets fire frequency. Set the value -50,000 [uSec]*/
int SetDesignatorNATOCodeValue(uint16_t CodeValue);


/*----- DesignatorNATO Report -----*/

/*Function Name       :    GetDesignatorNATOCodeValue
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Sets fire frequency. Set the value -50,000 [uSec] */
uint16_t GetDesignatorNATOCodeValue();


/*----- Illuminator Command -----*/

/*Function Name       :    SetIlluminatorPower
* Function Arguments  :    bool Power
* Function Returns    :    int Success
* Function Description:    Sets Illuminator Power . 0 - Off 1 - On*/
int SetIlluminatorPower(bool Power);

/*Function Name       :    SetIlluminatorArm
* Function Arguments  :    bool Arm
* Function Returns    :    int Success
* Function Description:    Sets Illuminator Arm . 0 - Off 1 - On*/
int SetIlluminatorArm(bool Arm);


/*----- Illuminator Report -----*/

/*Function Name       :    GetIlluminatorPower
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets Illuminator Power . 0 - Off 1 - On */
bool GetIlluminatorPower();

/*Function Name       :    GetIlluminatorArm
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets Illuminator Arm . 0 - Off 1 - On */
bool GetIlluminatorArm();


/*----- Illuminator Command -----*/

/*Function Name       :    SetIlluminatorMode
* Function Arguments  :    int8_t Mode
* Function Returns    :    int Success
* Function Description:    Sets Illuminator Mode*/
int SetIlluminatorMode(int8_t Mode);

/*Function Name       :    SetIlluminatorFire
* Function Arguments  :    bool Fire
* Function Returns    :    int Success
* Function Description:    Sets Illuminator Fire. 0 - Fire Off. 1 - Fire On*/
int SetIlluminatorFire(bool Fire);


/*----- Illuminator Report -----*/

/*Function Name       :    GetIlluminatorMode
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Sets Illuminator Mode */
int8_t GetIlluminatorMode();

/*Function Name       :    GetIlluminatorFire
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Sets Illuminator Fire. 0 - Fire Off. 1 - Fire On */
bool GetIlluminatorFire();


/*----- KeepAlive Command -----*/

/*Function Name       :    SetKeepAlive
* Function Arguments  :    None
* Function Returns    :    int Success
* Function Description:    Keep Alive signal. Need to send to system every ~500 mSec*/
int SetKeepAlive();


/*----- GetDATA Command -----*/

/*Function Name       :    SetGetDATA
* Function Arguments  :    uint16_t Data, enum RetreiveDataType RetreiveType
* Function Returns    :    int Success
* Function Description:    Set method to retreive data*/
int SetGetDATA(uint16_t Data, enum RetreiveDataType RetreiveType);


/*----- SystemGeneralReport Report -----*/

/*Function Name       :    GetSystemGeneralReportOperationMode
* Function Arguments  :    None
* Function Returns    :    TOperationMode_Micro 
* Function Description:    Gets Operation Mode Status */
enum TOperationMode_Micro GetSystemGeneralReportOperationMode();

/*Function Name       :    GetSystemGeneralReportHybridMode
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets Hybrid Mode Status */
bool GetSystemGeneralReportHybridMode();

/*Function Name       :    GetSystemGeneralReportHiveTemperature
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Gets Hive Temperature */
int8_t GetSystemGeneralReportHiveTemperature();


/*----- LogRecordCommand Command -----*/

/*Function Name       :    SetLogRecordCommandEnable
* Function Arguments  :    bool Enable
* Function Returns    :    int Success
* Function Description:    Enables/Disables systerm logging*/
int SetLogRecordCommandEnable(bool Enable);

/*Function Name       :    SetLogRecordCommandErrorLevel
* Function Arguments  :    bool ErrorLevel
* Function Returns    :    int Success
* Function Description:    Enables/Disables Error Level*/
int SetLogRecordCommandErrorLevel(bool ErrorLevel);

/*Function Name       :    SetLogRecordCommandInfoLevel
* Function Arguments  :    bool InfoLevel
* Function Returns    :    int Success
* Function Description:    Enables/Disables Info Level*/
int SetLogRecordCommandInfoLevel(bool InfoLevel);

/*Function Name       :    SetLogRecordCommandDebugLevel
* Function Arguments  :    bool DebugLevel
* Function Returns    :    int Success
* Function Description:    Enables/Disables Debug Level*/
int SetLogRecordCommandDebugLevel(bool DebugLevel);

/*Function Name       :    SetLogRecordCommandFreeText
* Function Arguments  :    char* FreeText, size_t Size - C string should be at most 19 Bytes.
* Function Returns    :    int Success
* Function Description:    Sets folder name to save log file*/
int SetLogRecordCommandFreeText(char* FreeText, size_t Size);


/*----- LogRecordReport Report -----*/

/*Function Name       :    GetLogRecordReportEnable
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if log is Enabled/Disabled */
bool GetLogRecordReportEnable();

/*Function Name       :    GetLogRecordReportErrorLevel
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if Error Level is Enabled/Disabled */
bool GetLogRecordReportErrorLevel();

/*Function Name       :    GetLogRecordReportInfoLevel
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if Info Level is Enabled/Disabled */
bool GetLogRecordReportInfoLevel();

/*Function Name       :    GetLogRecordReportDebugLevel
* Function Arguments  :    None
* Function Returns    :    bool 
* Function Description:    Gets if Debug Level is Enabled/Disabled */
bool GetLogRecordReportDebugLevel();

/*Function Name       :    GetLogRecordReportFileName
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 15
* Function Returns    :    void 
* Function Description:    Gets log's folder name */
void GetLogRecordReportFileName(char* Buffer, size_t Size);


/*----- CameraPowerControl Command -----*/

/*Function Name       :    SetCameraPowerControlDayLightCameraPower
* Function Arguments  :    enum PowerStatesRequest DayLightCameraPower
* Function Returns    :    int Success
* Function Description:    Enables/Disables DTV Sensor*/
int SetCameraPowerControlDayLightCameraPower(enum PowerStatesRequest DayLightCameraPower);

/*Function Name       :    SetCameraPowerControlThermalCameraPower
* Function Arguments  :    enum PowerStatesRequest ThermalCameraPower
* Function Returns    :    int Success
* Function Description:    Enables/Disables TI Sensor*/
int SetCameraPowerControlThermalCameraPower(enum PowerStatesRequest ThermalCameraPower);


/*----- CameraPowerReport Report -----*/

/*Function Name       :    GetCameraPowerReportWideDTVState
* Function Arguments  :    None
* Function Returns    :    PowerStatesReport 
* Function Description:    Gets Enabled/Disabled Wide DTV */
enum PowerStatesReport GetCameraPowerReportWideDTVState();

/*Function Name       :    GetCameraPowerReportNarrowDTVState
* Function Arguments  :    None
* Function Returns    :    PowerStatesReport 
* Function Description:    Gets Enabled/Disabled Narrow DTV */
enum PowerStatesReport GetCameraPowerReportNarrowDTVState();

/*Function Name       :    GetCameraPowerReportTIStateState
* Function Arguments  :    None
* Function Returns    :    PowerStatesReport 
* Function Description:    Gets Enabled/Disabled TI */
enum PowerStatesReport GetCameraPowerReportTIStateState();


/*----- TimeTagReport Report -----*/

/*Function Name       :    GetTimeTagReportSystemTimeTag
* Function Arguments  :    None
* Function Returns    :    uint64_t 
* Function Description:    Gets System Message Time Tag */
uint64_t GetTimeTagReportSystemTimeTag();

/*Function Name       :    GetTimeTagReportControlTimeTag
* Function Arguments  :    None
* Function Returns    :    uint64_t 
* Function Description:    Gets Latest Control Message Time Tag */
uint64_t GetTimeTagReportControlTimeTag();


/*----- SetSystemClock Command -----*/

/*Function Name       :    SetSetSystemClockYear
* Function Arguments  :    uint16_t Year
* Function Returns    :    int Success
* Function Description:    Sets Year as decimal integer [YYYY]*/
int SetSetSystemClockYear(uint16_t Year);

/*Function Name       :    SetSetSystemClockMonth
* Function Arguments  :    uint8_t Month
* Function Returns    :    int Success
* Function Description:    Sets Month as decimal integer [1-12]*/
int SetSetSystemClockMonth(uint8_t Month);

/*Function Name       :    SetSetSystemClockDay
* Function Arguments  :    uint8_t Day
* Function Returns    :    int Success
* Function Description:    Sets Day as decimal integer [1-31]*/
int SetSetSystemClockDay(uint8_t Day);

/*Function Name       :    SetSetSystemClockHour
* Function Arguments  :    uint8_t Hour
* Function Returns    :    int Success
* Function Description:    Sets Hour as decimal integer in 24Hrs Pattern [0-23]*/
int SetSetSystemClockHour(uint8_t Hour);

/*Function Name       :    SetSetSystemClockMinute
* Function Arguments  :    uint8_t Minute
* Function Returns    :    int Success
* Function Description:    Sets Minute as decimal integer [0-59]*/
int SetSetSystemClockMinute(uint8_t Minute);

/*Function Name       :    SetSetSystemClockSecond
* Function Arguments  :    uint8_t Second
* Function Returns    :    int Success
* Function Description:    Sets Second as decimal integer [0-59]*/
int SetSetSystemClockSecond(uint8_t Second);


/*----- CurrentSystemTime Report -----*/

/*Function Name       :    GetCurrentSystemTimeYear
* Function Arguments  :    None
* Function Returns    :    uint16_t 
* Function Description:    Gets Year as decimal integer */
uint16_t GetCurrentSystemTimeYear();

/*Function Name       :    GetCurrentSystemTimeMonth
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Month as decimal integer */
uint8_t GetCurrentSystemTimeMonth();

/*Function Name       :    GetCurrentSystemTimeDay
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Day as decimal integer */
uint8_t GetCurrentSystemTimeDay();

/*Function Name       :    GetCurrentSystemTimeHour
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Hour as decimal integer in 24Hrs Pattern */
uint8_t GetCurrentSystemTimeHour();

/*Function Name       :    GetCurrentSystemTimeMinute
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Minute as decimal integer */
uint8_t GetCurrentSystemTimeMinute();

/*Function Name       :    GetCurrentSystemTimeSecond
* Function Arguments  :    None
* Function Returns    :    uint8_t 
* Function Description:    Gets Second as decimal integer */
uint8_t GetCurrentSystemTimeSecond();


/*----- SystemVersion Report -----*/

/*Function Name       :    GetSystemVersionBundle
* Function Arguments  :    None
* Function Returns    :    int8_t 
* Function Description:    Bundle Version */
int8_t GetSystemVersionBundle();

/*Function Name       :    GetSystemVersionCPU
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    CPU Version */
void GetSystemVersionCPU(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionControl
* Function Arguments  :    None
* Function Returns    :    uint32_t 
* Function Description:    Control Version */
uint32_t GetSystemVersionControl();

/*Function Name       :    GetSystemVersionDTVLibrary
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    DTV Library Version */
void GetSystemVersionDTVLibrary(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionECVML
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    ECVML Version */
void GetSystemVersionECVML(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionSensorsWide
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Sensors Wide Version */
void GetSystemVersionSensorsWide(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionSensorsNarrow
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Sensors Narrow Version */
void GetSystemVersionSensorsNarrow(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionSensorsThermal
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Sensors Thermal Version */
void GetSystemVersionSensorsThermal(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionDesignator
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Designator Version */
void GetSystemVersionDesignator(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionIlluminator
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Illuminator Version */
void GetSystemVersionIlluminator(char* Buffer, size_t Size);

/*Function Name       :    GetSystemVersionSensorsSeeSpot
* Function Arguments  :    char* Buffer,  size_t Size - Buffer size should be at least 16
* Function Returns    :    void 
* Function Description:    Sensors SeeSpot Version */
void GetSystemVersionSensorsSeeSpot(char* Buffer, size_t Size);


/*----- CommunicationProtocolFunctions -----*/

/* Function Name:           STKInitializeCommunication
* Function Arguments:       None
* Function Returns:         int Success
* Function Description:     Establish Communication Between Local Application And Remote Unit.*/
int STKInitializeCommunication(struct MessagesTransmitEnableStruct data);

/* Function Name:           STKSyncLocal
* Function Arguments:       uint16_t timeout
* Function Returns:         enum SyncLocalErrorEnum Success Code
* Function Description:     Sync Local Data From Remote Device.*/  
enum SyncLocalErrorEnum STKSyncLocal(uint16_t timeout);

/* Function Name:           STKUpdateRemote
* Function Arguments:       None
* Function Returns:         int Success
* Function Description:     Update Remote Device From Local Data.*/
int STKUpdateRemote();



#endif // _STK_API
