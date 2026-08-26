#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include <QThread>
#include <iostream>
#include "readmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow; // Forward declaration

class GlobEventFilter : public QObject
{
public:
    GlobEventFilter(MainWindow *mainWindow); // Constructor takes MainWindow pointer

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    MainWindow *m_mainWindow; // Store MainWindow pointer
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Connect_To_Remote();
    void SyncLocal();
    void SyncRemote();
    void DisplayData();
    void AssertKeepAlive();
    void Zoom_In_Clicked();
    void Zoom_In_Released();
    void Zoom_Out_Clicked();
    void Zoom_Out_Released();

    void GammaPosClicked();
    void GammaNegClicked();
    void GammaPosReleased();
    void GammaNegReleased();

    void SaturationPosClicked();
    void SaturationNegClicked();
    void SaturationPosReleased();
    void SaturationNegReleased();

    void ContrastPosClicked();
    void ContrastNegClicked();
    void ContrastPosReleased();
    void ContrastNegReleased();

    void BrigtnessPosClicked();
    void BrigtnessNegClicked();
    void BrigtnessPosReleased();
    void BrigtnessNegReleased();

    void SharpnessPosClicked();
    void SharpnessNegClicked();
    void SharpnessPosReleased();
    void SharpnessNegReleased();

    void AutoExpModePosClicked();
    void ManExpModePosClicked();

    void ExpTimePosClicked();
    void ExpTimeNegClicked();
    void ExpTimePosReleased();
    void ExpTimeNegReleased();

    void ExpGainPosClicked();
    void ExpGainNegClicked();
    void ExpGainPosReleased();
    void ExpGainNegReleased();

    // Thermal commands
    void TI_Zoom_In_Clicked();
    void TI_Zoom_In_Released();
    void TI_Zoom_Out_Clicked();
    void TI_Zoom_Out_Released();

    void TI_ACEPosClicked();
    void TI_ACENegClicked();
    void TI_ACEPosReleased();
    void TI_ACENegReleased();

    void TI_ContrastPosClicked();
    void TI_ContrastNegClicked();
    void TI_ContrastPosReleased();
    void TI_ContrastNegReleased();

    void TI_BrigtnessPosClicked();
    void TI_BrigtnessNegClicked();
    void TI_BrigtnessPosReleased();
    void TI_BrigtnessNegReleased();

    void Toggle_Primar_Cam_pushButtonClicked();

    void ElevationPosClicked();
    void ElevationPosReleased();
    void ElevationNegClicked();
    void ElevationNegReleased();

    void AzimuthPosClicked();
    void AzimuthPosReleased();
    void AzimuthNegClicked();
    void AzimuthNegReleased();

    void SetCenterModePressed();
    void SetTrackMode();

    Ui::MainWindow *ui;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void m_updateDataTimer_onTimeout();
    void m_keepAliveTimer_onTimeout();

private:
    QTimer *m_updateDataTimer;
    QTimer *m_keepAliveTimer;
    QFuture<void> m_future;
    GlobEventFilter m_globEventFilter;
    ReadManager m_readMgr;

};

#endif // MAINWINDOW_H
