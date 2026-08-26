#include "mainwindow.h"
#include "math.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>
#include "readmanager.h"
#include "syncdata.h"

extern "C"
{
#include "SDK/stkAPI.h"
}

GlobEventFilter::GlobEventFilter(MainWindow *mainWindow) : m_mainWindow(mainWindow) {}

bool GlobEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter) {
        if (m_mainWindow && m_mainWindow->ui->txt_ToolTip) {
            QVariant hoverTextProperty = obj->property("hoverText");
            if (hoverTextProperty.isValid() && hoverTextProperty.canConvert<QString>()) {
                m_mainWindow->ui->txt_ToolTip->setText(hoverTextProperty.toString());
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_globEventFilter(this) // Initialize GlobEventFilter with 'this'
    , m_readMgr()
{
    ui->setupUi(this);
    qApp->installEventFilter(&m_globEventFilter);

    /* Manually added connections for API control */
    /* Connect to remote Push Button */

    connect(ui->Connect_To_Remote_pushButton, &QPushButton::clicked, this, &MainWindow::Connect_To_Remote);
    ui->Connect_To_Remote_pushButton->setToolTip("int STKInitializeCommunication(struct MessagesTransmitEnableStruct data)");
    ui->Connect_To_Remote_pushButton->setAttribute(Qt::WA_Hover);
    ui->Connect_To_Remote_pushButton->installEventFilter(&m_globEventFilter);
    ui->Connect_To_Remote_pushButton->setProperty("hoverText", "int STKInitializeCommunication(struct MessagesTransmitEnableStruct data)");

    /* Sync remote Push Button */
    //connect(ui->SyncRemote_pushButton, &QPushButton::clicked, this, &MainWindow::SyncRemote);
    //ui->SyncRemote_pushButton->setToolTip("int STKUpdateRemote()");
    //ui->SyncRemote_pushButton->setAttribute(Qt::WA_Hover);
    //ui->SyncRemote_pushButton->installEventFilter(&m_globEventFilter);
    //ui->SyncRemote_pushButton->setProperty("hoverText", "int STKUpdateRemote()");

    /* Sync Local Push Button */
    connect(ui->SyncLocal_pushButton, &QPushButton::clicked, this, &MainWindow::SyncLocal);
    ui->SyncLocal_pushButton->setToolTip("int STKSyncLocal(uint16_t timeout)");
    ui->SyncLocal_pushButton->setAttribute(Qt::WA_Hover);
    ui->SyncLocal_pushButton->installEventFilter(&m_globEventFilter);
    ui->SyncLocal_pushButton->setProperty("hoverText", "int STKSyncLocal(uint16_t timeout)");

    /* ZoomIn PushButton */
    connect(ui->Zoom_In_pushButton, &QPushButton::pressed, this, &MainWindow::Zoom_In_Clicked);
    connect(ui->Zoom_In_pushButton, &QPushButton::released, this, &MainWindow::Zoom_In_Released);
    ui->Zoom_In_pushButton->setToolTip("int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
    ui->Zoom_In_pushButton->setAttribute(Qt::WA_Hover);
    ui->Zoom_In_pushButton->installEventFilter(&m_globEventFilter);
    ui->Zoom_In_pushButton->setProperty("hoverText", "int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");

    /* ZoomOut PushButton */
    connect(ui->Zoom_out_pushButton, &QPushButton::pressed, this, &MainWindow::Zoom_Out_Clicked);
    connect(ui->Zoom_out_pushButton, &QPushButton::released, this, &MainWindow::Zoom_Out_Released);
    ui->Zoom_out_pushButton->setToolTip("int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
    ui->Zoom_out_pushButton->setAttribute(Qt::WA_Hover);
    ui->Zoom_out_pushButton->installEventFilter(&m_globEventFilter);
    ui->Zoom_out_pushButton->setProperty("hoverText", "int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");

    /* GammaPos PushButton */
    connect(ui->Gamma_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::GammaPosClicked);
    connect(ui->Gamma_Pos_pushButton, &QPushButton::released, this, &MainWindow::GammaPosReleased);
    ui->Gamma_Pos_pushButton->setToolTip("int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");
    ui->Gamma_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Gamma_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Gamma_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");

    /* GammaNeg PushButton */
    connect(ui->Gamma_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::GammaNegClicked);
    connect(ui->Gamma_Neg_pushButton, &QPushButton::released, this, &MainWindow::GammaNegReleased);
    ui->Gamma_Neg_pushButton->setToolTip("int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");
    ui->Gamma_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Gamma_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Gamma_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");

    /* SatPos PushButton */
    connect(ui->Saturation_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::SaturationPosClicked);
    connect(ui->Saturation_Pos_pushButton, &QPushButton::released, this, &MainWindow::SaturationPosReleased);
    ui->Saturation_Pos_pushButton->setToolTip("int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)");
    ui->Saturation_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Saturation_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Saturation_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)");

    /* SatNeg PushButton */
    connect(ui->Saturation_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::SaturationNegClicked);
    connect(ui->Saturation_Neg_pushButton, &QPushButton::released, this, &MainWindow::SaturationNegReleased);
    ui->Saturation_Neg_pushButton->setToolTip("int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)");
    ui->Saturation_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Saturation_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Saturation_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)");

    /* Brightness Pos PushButton */
    connect(ui->Brigtness_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::BrigtnessPosClicked);
    connect(ui->Brigtness_Pos_pushButton, &QPushButton::released, this, &MainWindow::BrigtnessPosReleased);
    ui->Brigtness_Pos_pushButton->setToolTip("int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");
    ui->Brigtness_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Brigtness_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Brigtness_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");

    /* Brightness Neg PushButton */
    connect(ui->Brigtness_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::BrigtnessNegClicked);
    connect(ui->Brigtness_Neg_pushButton, &QPushButton::released, this, &MainWindow::BrigtnessNegReleased);
    ui->Brigtness_Neg_pushButton->setToolTip("int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");
    ui->Brigtness_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Brigtness_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Brigtness_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");

    /* Sharpness Pos PushButton */
    connect(ui->Sharpness_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::SharpnessPosClicked);
    connect(ui->Sharpness_Pos_pushButton, &QPushButton::released, this, &MainWindow::SharpnessPosReleased);
    ui->Sharpness_Pos_pushButton->setToolTip("int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)");
    ui->Sharpness_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Sharpness_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Sharpness_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)");

    /* Sharpness Neg PushButton */
    connect(ui->Sharpness_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::SharpnessNegClicked);
    connect(ui->Sharpness_Neg_pushButton, &QPushButton::released, this, &MainWindow::SharpnessNegReleased);
    ui->Sharpness_Neg_pushButton->setToolTip("int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)");
    ui->Sharpness_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Sharpness_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Sharpness_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandSharpnessCommand(enum GenericIncDecVal SharpnessCommand)");

    /* Contrast Pos PushButton */
    connect(ui->Contrast_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::ContrastPosClicked);
    connect(ui->Contrast_Pos_pushButton, &QPushButton::released, this, &MainWindow::ContrastPosReleased);
    ui->Contrast_Pos_pushButton->setToolTip("int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand)");
    ui->Contrast_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Contrast_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Contrast_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand)");

    /* Contrast Neg PushButton */
    connect(ui->Contrast_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::ContrastNegClicked);
    connect(ui->Contrast_Neg_pushButton, &QPushButton::released, this, &MainWindow::ContrastNegReleased);
    ui->Contrast_Neg_pushButton->setToolTip("int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand)");
    ui->Contrast_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Contrast_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Contrast_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandGainCommand(enum GenericIncDecVal GainCommand)");

    /*Auto Exposure Mode PushButton */
    connect(ui->AutoExpMode_pushButton, &QPushButton::pressed, this, &MainWindow::AutoExpModePosClicked);
    ui->AutoExpMode_pushButton->setToolTip("int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");
    ui->AutoExpMode_pushButton->setAttribute(Qt::WA_Hover);
    ui->AutoExpMode_pushButton->installEventFilter(&m_globEventFilter);
    ui->AutoExpMode_pushButton->setProperty("hoverText", "int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");

    /*Manual Exposure Mode PushButton */
    connect(ui->ManExpMode_pushButton, &QPushButton::pressed, this, &MainWindow::ManExpModePosClicked);
    ui->ManExpMode_pushButton->setToolTip("int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");
    ui->ManExpMode_pushButton->setAttribute(Qt::WA_Hover);
    ui->ManExpMode_pushButton->installEventFilter(&m_globEventFilter);
    ui->ManExpMode_pushButton->setProperty("hoverText", "int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");

    /* Exposure Time Neg PushButton */
    connect(ui->ExpTime_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::ExpTimePosClicked);
    connect(ui->ExpTime_Pos_pushButton, &QPushButton::released, this, &MainWindow::ExpTimePosReleased);
    ui->ExpTime_Pos_pushButton->setToolTip("int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");
    ui->ExpTime_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->ExpTime_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->ExpTime_Pos_pushButton->setProperty("hoverText", "int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");

    /* Exposure Time Neg PushButton */
    connect(ui->ExpTime_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::ExpTimeNegClicked);
    connect(ui->ExpTime_Neg_pushButton, &QPushButton::released, this, &MainWindow::ExpTimeNegReleased);
    ui->ExpTime_Neg_pushButton->setToolTip("int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");
    ui->ExpTime_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->ExpTime_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->ExpTime_Neg_pushButton->setProperty("hoverText", "int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");

    /* TI ZoomIn PushButton */
    connect(ui->Zoom_In_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_Zoom_In_Clicked);
    connect(ui->Zoom_In_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_Zoom_In_Released);
    ui->Zoom_In_pushButton_TI->setToolTip("int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
    ui->Zoom_In_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Zoom_In_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Zoom_In_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");

    /* TI ZoomOut PushButton */
    connect(ui->Zoom_out_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_Zoom_Out_Clicked);
    connect(ui->Zoom_out_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_Zoom_Out_Released);
    ui->Zoom_out_pushButton_TI->setToolTip("int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
    ui->Zoom_out_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Zoom_out_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Zoom_out_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");

    /* TI ACE Pos PushButton */
    connect(ui->ACE_Pos_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_ACEPosClicked);
    connect(ui->ACE_Pos_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_ACEPosReleased);
    ui->ACE_Pos_pushButton_TI->setToolTip("int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand)");
    ui->ACE_Pos_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->ACE_Pos_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->ACE_Pos_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand)");

    /* TI ACE Neg PushButton */
    connect(ui->ACE_Neg_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_ACENegClicked);
    connect(ui->ACE_Neg_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_ACENegReleased);
    ui->ACE_Neg_pushButton_TI->setToolTip("int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand)");
    ui->ACE_Neg_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->ACE_Neg_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->ACE_Neg_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandACECommand(enum GenericIncDecVal ACECommand)");

    /* TI Brightness Pos PushButton */
    connect(ui->Brigtness_Pos_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_BrigtnessPosClicked);
    connect(ui->Brigtness_Pos_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_BrigtnessPosReleased);
    ui->Brigtness_Pos_pushButton_TI->setToolTip("int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");
    ui->Brigtness_Pos_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Brigtness_Pos_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Brigtness_Pos_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");

    /* TI Brightnes Neg PushButton */
    connect(ui->Brigtness_Neg_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_BrigtnessNegClicked);
    connect(ui->Brigtness_Neg_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_BrigtnessNegReleased);
    ui->Brigtness_Neg_pushButton_TI->setToolTip("int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");
    ui->Brigtness_Neg_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Brigtness_Neg_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Brigtness_Neg_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandLevelCommand(enum GenericIncDecVal LevelCommand)");

    /* TI Contrast Pos PushButton */
    connect(ui->Contrast_Pos_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_ContrastPosClicked);
    connect(ui->Contrast_Pos_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_ContrastPosReleased);
    ui->Contrast_Pos_pushButton_TI->setToolTip("int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand)");
    ui->Contrast_Pos_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Contrast_Pos_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Contrast_Pos_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand)");

    /* TI Contrast Neg PushButton */
    connect(ui->Contrast_Neg_pushButton_TI, &QPushButton::pressed, this, &MainWindow::TI_ContrastNegClicked);
    connect(ui->Contrast_Neg_pushButton_TI, &QPushButton::released, this, &MainWindow::TI_ContrastNegReleased);
    ui->Contrast_Neg_pushButton_TI->setToolTip("int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand)");
    ui->Contrast_Neg_pushButton_TI->setAttribute(Qt::WA_Hover);
    ui->Contrast_Neg_pushButton_TI->installEventFilter(&m_globEventFilter);
    ui->Contrast_Neg_pushButton_TI->setProperty("hoverText", "int SetThermalCameraCommandGainModeCommand(enum ThermalCameraGainMode GainModeCommand)");

    /* Primary Camera Toggle*/
    connect(ui->Toggle_Primar_Cam_pushButton, &QPushButton::pressed, this, &MainWindow::Toggle_Primar_Cam_pushButtonClicked);
    ui->Toggle_Primar_Cam_pushButton->setToolTip("int SetVideoChannelCommandPrimaryVideoChannel(enum Camera PrimaryVideoChannel)");
    ui->Toggle_Primar_Cam_pushButton->setAttribute(Qt::WA_Hover);
    ui->Toggle_Primar_Cam_pushButton->installEventFilter(&m_globEventFilter);
    ui->Toggle_Primar_Cam_pushButton->setProperty("hoverText", "int SetVideoChannelCommandPrimaryVideoChannel(enum Camera PrimaryVideoChannel)");

    /* Elevation Pos PushButtons*/
    connect(ui->Elevation_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::ElevationPosClicked);
    connect(ui->Elevation_Pos_pushButton, &QPushButton::released, this, &MainWindow::ElevationPosReleased );
    ui->Elevation_Pos_pushButton->setToolTip("int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");
    ui->Elevation_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Elevation_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Elevation_Pos_pushButton->setProperty("hoverText", "int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");

    /* Elevation Neg PushButtons*/
    connect(ui->Elevation_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::ElevationNegClicked);
    connect(ui->Elevation_Neg_pushButton, &QPushButton::released, this, &MainWindow::ElevationNegReleased );
    ui->Elevation_Neg_pushButton->setToolTip("int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");
    ui->Elevation_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Elevation_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Elevation_Neg_pushButton->setProperty("hoverText", "int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");


    /* Azimuth Pos PushButtons*/
    connect(ui->Azimuth_Pos_pushButton, &QPushButton::pressed, this, &MainWindow::AzimuthPosClicked);
    connect(ui->Azimuth_Pos_pushButton, &QPushButton::released, this, &MainWindow::AzimuthPosReleased );
    ui->Azimuth_Pos_pushButton->setToolTip("int SetRateCommandAngularVelocities_X(float AngularVelocities_X);");
    ui->Azimuth_Pos_pushButton->setAttribute(Qt::WA_Hover);
    ui->Azimuth_Pos_pushButton->installEventFilter(&m_globEventFilter);
    ui->Azimuth_Pos_pushButton->setProperty("hoverText", "int SetRateCommandAngularVelocities_X(float AngularVelocities_X);");

    /* Azimuth Neg PushButtons*/
    connect(ui->Azimuth_Neg_pushButton, &QPushButton::pressed, this, &MainWindow::AzimuthNegClicked);
    connect(ui->Azimuth_Neg_pushButton, &QPushButton::released, this, &MainWindow::AzimuthNegReleased );
    ui->Azimuth_Neg_pushButton->setToolTip("int SetRateCommandAngularVelocities_X(float AngularVelocities_X);");
    ui->Azimuth_Neg_pushButton->setAttribute(Qt::WA_Hover);
    ui->Azimuth_Neg_pushButton->installEventFilter(&m_globEventFilter);
    ui->Azimuth_Neg_pushButton->setProperty("hoverText", "int SetRateCommandAngularVelocities_X(float AngularVelocities_X);");

    /* Center Mode PushButtons*/
    connect(ui->CenterMode_pushButton, &QPushButton::pressed, this, &MainWindow::SetCenterModePressed);
    ui->CenterMode_pushButton->setToolTip("int SetCenterMode();");
    ui->CenterMode_pushButton->setAttribute(Qt::WA_Hover);
    ui->CenterMode_pushButton->installEventFilter(&m_globEventFilter);
    ui->CenterMode_pushButton->setProperty("hoverText", "int SetCenterMode();");

    /* Track Mode PushButtons*/
    connect(ui->Track_pushButton, &QPushButton::pressed, this, &MainWindow::SetTrackMode);
    ui->Track_pushButton->setToolTip("int SetTrackTargetXCoordinate(int16_t TargetXCoordinate); int SetTrackTargetYCoordinate(int16_t TargetYCoordinate);");
    ui->Track_pushButton->setAttribute(Qt::WA_Hover);
    ui->Track_pushButton->installEventFilter(&m_globEventFilter);
    ui->Track_pushButton->setProperty("hoverText", "int SetTrackTargetXCoordinate(int16_t TargetXCoordinate); int SetTrackTargetYCoordinate(int16_t TargetYCoordinate);");

    /* Connect ToolTips For All TextBoxes */
    ui->txt_FOV->setToolTip("float GetDayCameraReportFOV()");
    ui->txt_FOV->setAttribute(Qt::WA_Hover);
    ui->txt_FOV->installEventFilter(&m_globEventFilter);
    ui->txt_FOV->setProperty("hoverText", "float GetDayCameraReportFOV()");

    ui->txt_Gamma->setToolTip("float GetDayCameraReportGammaValue()");
    ui->txt_Gamma->setAttribute(Qt::WA_Hover);
    ui->txt_Gamma->installEventFilter(&m_globEventFilter);
    ui->txt_Gamma->setProperty("hoverText", "float GetDayCameraReportGammaValue()");

    ui->txt_Saturation->setToolTip("float GetDayCameraReportSaturationValue()");
    ui->txt_Saturation->setAttribute(Qt::WA_Hover);
    ui->txt_Saturation->installEventFilter(&m_globEventFilter);
    ui->txt_Saturation->setProperty("hoverText", "float GetDayCameraReportSaturationValue()");

    ui->txt_Contrast->setToolTip("float GetDayCameraReportContrastValue()");
    ui->txt_Contrast->setAttribute(Qt::WA_Hover);
    ui->txt_Contrast->installEventFilter(&m_globEventFilter);
    ui->txt_Contrast->setProperty("hoverText", "float GetDayCameraReportContrastValue()");

    ui->txt_Brigtness->setToolTip("float GetDayCameraReportBrigthnessValue()");
    ui->txt_Brigtness->setAttribute(Qt::WA_Hover);
    ui->txt_Brigtness->installEventFilter(&m_globEventFilter);
    ui->txt_Brigtness->setProperty("hoverText", "float GetDayCameraReportBrigthnessValue()");

    ui->txt_Sharpness->setToolTip("float GetDayCameraReportSharpnessValue()");
    ui->txt_Sharpness->setAttribute(Qt::WA_Hover);
    ui->txt_Sharpness->installEventFilter(&m_globEventFilter);
    ui->txt_Sharpness->setProperty("hoverText", "float GetDayCameraReportSharpnessValue()");

    ui->txt_Exp_Mode->setToolTip("enum DTVExposureMode GetDayCameraReportExposureMode()");
    ui->txt_Exp_Mode->setAttribute(Qt::WA_Hover);
    ui->txt_Exp_Mode->installEventFilter(&m_globEventFilter);
    ui->txt_Exp_Mode->setProperty("hoverText", "enum DTVExposureMode GetDayCameraReportExposureMode()");

    ui->txt_Exp_Time->setToolTip("uint32_t GetDayCameraReportExposureTime()");
    ui->txt_Exp_Time->setAttribute(Qt::WA_Hover);
    ui->txt_Exp_Time->installEventFilter(&m_globEventFilter);
    ui->txt_Exp_Time->setProperty("hoverText", "uint32_t GetDayCameraReportExposureTime()");

    ui->txt_FOV_TI->setToolTip("float GetThermalCameraReportFOV()");
    ui->txt_FOV_TI->setAttribute(Qt::WA_Hover);
    ui->txt_FOV_TI->installEventFilter(&m_globEventFilter);
    ui->txt_FOV_TI->setProperty("hoverText", "float GetThermalCameraReportFOV()");

    ui->txt_ACE_TI->setToolTip("float GetThermalCameraReportACEValue()");
    ui->txt_ACE_TI->setAttribute(Qt::WA_Hover);
    ui->txt_ACE_TI->installEventFilter(&m_globEventFilter);
    ui->txt_ACE_TI->setProperty("hoverText", "float GetThermalCameraReportACEValue()");

    ui->txt_Brigtness_TI->setToolTip("int32_t GetThermalCameraReportLevelValue()");
    ui->txt_Brigtness_TI->setAttribute(Qt::WA_Hover);
    ui->txt_Brigtness_TI->installEventFilter(&m_globEventFilter);
    ui->txt_Brigtness_TI->setProperty("hoverText", "int32_t GetThermalCameraReportLevelValue()");

    ui->txt_Contrast_TI->setToolTip("int32_t GetThermalCameraReportGainValue()");
    ui->txt_Contrast_TI->setAttribute(Qt::WA_Hover);
    ui->txt_Contrast_TI->installEventFilter(&m_globEventFilter);
    ui->txt_Contrast_TI->setProperty("hoverText", "int32_t GetThermalCameraReportGainValue()");

    ui->txt_ActiveCam->setToolTip("enum Camera GetVideoChannelReportPrimaryVideoChannel()");
    ui->txt_ActiveCam->setAttribute(Qt::WA_Hover);
    ui->txt_ActiveCam->installEventFilter(&m_globEventFilter);
    ui->txt_ActiveCam->setProperty("hoverText", "enum Camera GetVideoChannelReportPrimaryVideoChannel()");

    ui->txt_Elevation_Rate->setToolTip("float GetRate_GRRAngularVelocities_Y()");
    ui->txt_Elevation_Rate->setAttribute(Qt::WA_Hover);
    ui->txt_Elevation_Rate->installEventFilter(&m_globEventFilter);
    ui->txt_Elevation_Rate->setProperty("hoverText", "float GetRate_GRRAngularVelocities_Y()");

    ui->txt_Azimuth_Rate->setToolTip("float GetRate_GRRAngularVelocities_X()");
    ui->txt_Azimuth_Rate->setAttribute(Qt::WA_Hover);
    ui->txt_Azimuth_Rate->installEventFilter(&m_globEventFilter);
    ui->txt_Azimuth_Rate->setProperty("hoverText", "float GetRate_GRRAngularVelocities_X()");

    ui->txt_Elevation_position->setToolTip("float GetPositionReportRelativeGimbalAngles_Y()");
    ui->txt_Elevation_position->setAttribute(Qt::WA_Hover);
    ui->txt_Elevation_position->installEventFilter(&m_globEventFilter);
    ui->txt_Elevation_position->setProperty("hoverText", "float GetPositionReportRelativeGimbalAngles_Y()");

    ui->txt_Azimuth_position->setToolTip("float GetPositionReportRelativeGimbalAngles_X()");
    ui->txt_Azimuth_position->setAttribute(Qt::WA_Hover);
    ui->txt_Azimuth_position->installEventFilter(&m_globEventFilter);
    ui->txt_Azimuth_position->setProperty("hoverText", "float GetPositionReportRelativeGimbalAngles_X()");

    ui->txt_OperationMode->setToolTip("enum TOperationMode_Micro GetGimbalOperationReportOperationMode()");
    ui->txt_OperationMode->setAttribute(Qt::WA_Hover);
    ui->txt_OperationMode->installEventFilter(&m_globEventFilter);
    ui->txt_OperationMode->setProperty("hoverText", "enum TOperationMode_Micro GetGimbalOperationReportOperationMode()");

    /* run the syncData */
    m_future = QtConcurrent::run(SyncData::process);

    /* set m_updateDataTimer to periodically update data */
    m_updateDataTimer = new QTimer(this);
    QObject::connect(m_updateDataTimer, &QTimer::timeout, this, &MainWindow::m_updateDataTimer_onTimeout);
    // initiates when communication is succeeded.

    /* set m_keepAliveTimer */
    m_keepAliveTimer = new QTimer(this);
    QObject::connect(m_keepAliveTimer, &QTimer::timeout, this, &MainWindow::m_keepAliveTimer_onTimeout);
    m_keepAliveTimer->start(400);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
}


MainWindow::~MainWindow()
{
    delete ui;
    shouldRun.store(0);
    SetRecordVideoCommandEnable(false);
    m_updateDataTimer->stop();
}


/* Manually added methods for API control */
/* Connect To host Method */
void MainWindow::Connect_To_Remote()
{
    MessagesTransmitEnableStruct MessagesTransmitInitialize;

    int ret = STKInitializeCommunication(MessagesTransmitInitialize);
    if (ret == 0)
    {
        ui->Connect_To_Remote_pushButton->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n""background-color:  rgb(125, 125, 255);"));
        ui->Connect_To_Remote_pushButton->setText(QApplication::translate("MainWindow", "Connected To Device", nullptr));
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // delay for half a second before raising the flag (optional)
        SetCenterMode();
        SetGetDATA(50999, CyclicRetreive);
        SetGetDATA(50000, CyclicRetreive);

        //char myNewName[] = "MyNewDir";
        //SetRecordVideoCommandFolderName(myNewName, sizeof(myNewName));
        //SetRecordVideoCommandEnable(true);

        //char mynewlogfile[] = "TempLog3";
        //SetLogRecordCommandFreeText(mynewlogfile, sizeof(mynewlogfile));

        m_updateDataTimer->start(50);
        isConnectedToHive.store(1);
        isCommunicationInit.store(1);
    }
    else
    {
        ui->Connect_To_Remote_pushButton->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n""background-color:  rgb(255, 0, 0);"));
        ui->Connect_To_Remote_pushButton->setText(QApplication::translate("MainWindow", "Failed connecting To Device", nullptr));
    }
}

/* SyncRemote To host Method */
void MainWindow::SyncRemote()
{
    STKUpdateRemote();
}

/* SyncLocal To host Method */
void MainWindow::SyncLocal()
{
    STKSyncLocal(20);
    DisplayData();
}

/* ZoomIn PushButton Method*/
void MainWindow::Zoom_In_Clicked()
{
    SetDayCameraCommandZoomCommand(Zoomin);
}

void MainWindow::Zoom_In_Released()
{
    SetDayCameraCommandZoomCommand(Nochange);
}


/* ZoomOut PushButton Method*/
void MainWindow::Zoom_Out_Clicked()
{
    SetDayCameraCommandZoomCommand(Zoomout);
}

void MainWindow::Zoom_Out_Released()
{
    SetDayCameraCommandZoomCommand(Nochange);
}

void MainWindow::DisplayData()
{
    static bool disconnected;
    if (!isConnectedToHive.load())
    {
        if (!disconnected)
        {
            QMessageBox::information(nullptr, "Info", "Lost connection.");
            disconnected = true;
        }
        ui->Connect_To_Remote_pushButton->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n""background-color:  rgb(255, 0, 0);"));
        ui->Connect_To_Remote_pushButton->setText(QApplication::translate("MainWindow", "Communication Lost", nullptr));
        return;
    } else {
        if (disconnected) disconnected = true;
    }

    if (!isFirstSuccessCommunication.load())
        return;

    m_readMgr.SyncFields();
    ui->Connect_To_Remote_pushButton->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);\n""background-color:  rgb(125, 125, 255);"));
    ui->Connect_To_Remote_pushButton->setText(QApplication::translate("MainWindow", "Connected To Device", nullptr));

    ui->txt_FOV->setText(QString::number(m_readMgr.DaylightFOV, 'f', 2));
    ui->txt_Gamma->setText(QString::number(m_readMgr.DayLightGamma, 'f', 2));
    ui->txt_Saturation->setText(QString::number(m_readMgr.DayLightSaturation, 'f', 2));
    ui->txt_Contrast->setText(QString::number(m_readMgr.DayLightContrast, 'f', 2));
    ui->txt_Sharpness->setText(QString::number(m_readMgr.DaylightSharpness, 'f', 2));
    ui->txt_Brigtness->setText(QString::number(m_readMgr.DayLightBrightness, 'f', 2));
    if (m_readMgr.DayLightExpMode == AutomaticExposureMode)
    {
        ui->txt_Exp_Mode->setText(QString("Auto"));
    }
    else
    {
        ui->txt_Exp_Mode->setText(QString("Manual"));
    }
    ui->txt_Exp_Time->setText(QString::number(m_readMgr.DayLightExpTime, 'i', 0));

    ui->txt_FOV_TI->setText(QString::number(m_readMgr.ThermalFov, 'f', 2));
    ui->txt_ACE_TI->setText(QString::number(m_readMgr.ThermalACE, 'f', 2));
    ui->txt_Brigtness_TI->setText(QString::number(m_readMgr.ThermalBrightness, 'f', 2));
    ui->txt_Contrast_TI->setText(QString::number(m_readMgr.ThermalContrast, 'f', 2));

    if (m_readMgr.MainCameraReadBack == Camera::TI)
    {
        ui->txt_ActiveCam->setText(QString("TI"));
    }
    else
    {
        ui->txt_ActiveCam->setText(QString("DTV"));
    }

    ui->txt_Azimuth_Rate->setText(QString::number(m_readMgr.Azimuth_Rate, 'f', 2));
    ui->txt_Elevation_Rate->setText(QString::number(m_readMgr.Elevation_Rate, 'f', 2));
    ui->txt_Azimuth_position->setText(QString::number(m_readMgr.Azimuth_Pos, 'f', 2));
    ui->txt_Elevation_position->setText(QString::number(m_readMgr.Elevation_Pos, 'f', 2));

    ui->txt_OperationMode->setText(QString::number(m_readMgr.OperationMode, 'i', 0));

    // std::cout << "CPU Version is: " << m_readMgr.SystemVersionCPU << std::endl;
    // std::cout << "is: " << m_readMgr.RecordVideoFolderName << std::endl;
}

void MainWindow::AssertKeepAlive()
{
    SetKeepAlive();
}

void MainWindow::GammaPosClicked()
{
    SetDayCameraCommandGammaCommand(IncreaseValue);
}

void MainWindow::GammaNegClicked()
{
    SetDayCameraCommandGammaCommand(DecreaseValue);
}

void MainWindow::GammaPosReleased()
{
    SetDayCameraCommandGammaCommand(NoChange);
}

void MainWindow::GammaNegReleased()
{
    SetDayCameraCommandGammaCommand(NoChange);
}

void MainWindow::SaturationPosClicked()
{
    SetDayCameraCommandSaturationCommand(IncreaseValue);
}

void MainWindow::SaturationNegClicked()
{
    SetDayCameraCommandSaturationCommand(DecreaseValue);
}

void MainWindow::SaturationPosReleased()
{
    SetDayCameraCommandSaturationCommand(NoChange);
}

void MainWindow::SaturationNegReleased()
{
    SetDayCameraCommandSaturationCommand(NoChange);
}

void MainWindow::ContrastPosClicked()
{
    SetDayCameraCommandGainCommand(IncreaseValue);
}

void MainWindow::ContrastNegClicked()
{
    SetDayCameraCommandGainCommand(DecreaseValue);
}

void MainWindow::ContrastPosReleased()
{
    SetDayCameraCommandGainCommand(NoChange);
}

void MainWindow::ContrastNegReleased()
{
    SetDayCameraCommandGainCommand(NoChange);
}

void MainWindow::BrigtnessPosClicked()
{
    SetDayCameraCommandLevelCommand(IncreaseValue);
}

void MainWindow::BrigtnessNegClicked()
{
    SetDayCameraCommandLevelCommand(DecreaseValue);
}

void MainWindow::BrigtnessPosReleased()
{
    SetDayCameraCommandLevelCommand(NoChange);
}

void MainWindow::BrigtnessNegReleased()
{
    SetDayCameraCommandLevelCommand(NoChange);
}

void MainWindow::SharpnessPosClicked()
{
    SetDayCameraCommandSharpnessCommand(IncreaseValue);
}

void MainWindow::SharpnessNegClicked()
{
    SetDayCameraCommandSharpnessCommand(DecreaseValue);
}

void MainWindow::SharpnessPosReleased()
{
    SetDayCameraCommandSharpnessCommand(NoChange);
}

void MainWindow::SharpnessNegReleased()
{
    SetDayCameraCommandSharpnessCommand(NoChange);
}

void MainWindow::AutoExpModePosClicked()
{
    SetDayCameraCommandExposureMode(SetAutomaticExposureMode);
}

void MainWindow::ManExpModePosClicked()
{
    SetDayCameraCommandExposureMode(SetManualExposureMode);
}

void MainWindow::ExpTimePosClicked()
{
    SetDayCameraCommandExposureTime(IncreaseValue);
}

void MainWindow::ExpTimeNegClicked()
{
    SetDayCameraCommandExposureTime(DecreaseValue);
}

void MainWindow::ExpTimePosReleased()
{
    SetDayCameraCommandExposureTime(NoChange);
}

void MainWindow::ExpTimeNegReleased()
{
    SetDayCameraCommandExposureTime(NoChange);
}

void MainWindow::ExpGainPosClicked()
{
    SetDayCameraCommandExposureGain(IncreaseValue);
}

void MainWindow::ExpGainNegClicked()
{
    SetDayCameraCommandExposureGain(DecreaseValue);
}

void MainWindow::ExpGainPosReleased()
{
    SetDayCameraCommandExposureGain(NoChange);
}

void MainWindow::ExpGainNegReleased()
{
    SetDayCameraCommandExposureGain(NoChange);
}

void MainWindow::TI_Zoom_In_Clicked()
{
    SetThermalCameraCommandZoomCommand(Zoomin);
}
void MainWindow::TI_Zoom_In_Released()
{
    SetThermalCameraCommandZoomCommand(Nochange);
}
void MainWindow::TI_Zoom_Out_Clicked()
{
    SetThermalCameraCommandZoomCommand(Zoomout);
}
void MainWindow::TI_Zoom_Out_Released()
{
    SetThermalCameraCommandZoomCommand(Nochange);
}

void MainWindow::TI_ACEPosClicked()
{
    SetThermalCameraCommandACECommand(IncreaseValue);
}

void MainWindow::TI_ACENegClicked()
{
    SetThermalCameraCommandACECommand(DecreaseValue);
}

void MainWindow::TI_ACEPosReleased()
{
    SetThermalCameraCommandACECommand(NoChange);
}

void MainWindow::TI_ACENegReleased()
{
    SetThermalCameraCommandACECommand(NoChange);
}

void MainWindow::TI_ContrastPosClicked()
{
    SetThermalCameraCommandGainCommand(IncreaseValue);
}

void MainWindow::TI_ContrastNegClicked()
{
    SetThermalCameraCommandGainCommand(DecreaseValue);
}

void MainWindow::TI_ContrastPosReleased()
{
    SetThermalCameraCommandGainCommand(NoChange);
}

void MainWindow::TI_ContrastNegReleased()
{
    SetThermalCameraCommandGainCommand(NoChange);
}


void MainWindow::TI_BrigtnessPosClicked()
{
    SetThermalCameraCommandLevelCommand(IncreaseValue);
}

void MainWindow::TI_BrigtnessNegClicked()
{
    SetThermalCameraCommandLevelCommand(DecreaseValue);
}

void MainWindow::TI_BrigtnessPosReleased()
{
    SetThermalCameraCommandLevelCommand(NoChange);
}

void MainWindow::TI_BrigtnessNegReleased()
{
    SetThermalCameraCommandLevelCommand(NoChange);
}

void MainWindow::Toggle_Primar_Cam_pushButtonClicked()
{
    if (m_readMgr.MainCameraReadBack == TI)
    {
        SetVideoChannelCommandPrimaryVideoChannel(DTV);
    }
    else
    {
        SetVideoChannelCommandPrimaryVideoChannel(TI);
    }
}

void MainWindow::ElevationPosClicked()
{
    SetRateCommandAngularVelocities_Y(20);
}
void MainWindow::ElevationPosReleased()
{
    SetRateCommandAngularVelocities_Y(0);
}

void MainWindow::ElevationNegClicked()
{
    SetRateCommandAngularVelocities_Y(-20);
}

void MainWindow::ElevationNegReleased()
{
    SetRateCommandAngularVelocities_Y(0);
}


void MainWindow::AzimuthPosClicked()
{
    SetRateCommandAngularVelocities_X(20);
}

void MainWindow::AzimuthPosReleased()
{
    SetRateCommandAngularVelocities_X(0);
}

void MainWindow::AzimuthNegClicked()
{
    SetRateCommandAngularVelocities_X(-20);
}

void MainWindow::AzimuthNegReleased()
{
    SetRateCommandAngularVelocities_X(0);
}

void MainWindow::SetCenterModePressed()
{
    SetCenterMode();
}

void MainWindow::SetTrackMode()
{
    SetTrack(0,0);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::m_updateDataTimer_onTimeout()
{
    DisplayData();
}

void MainWindow::m_keepAliveTimer_onTimeout()
{
    if (isConnectedToHive.load() && isFirstSuccessCommunication.load())
        SetKeepAlive();
}
