/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QPushButton *Connect_To_Remote_pushButton;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *Azimuth_Neg_pushButton;
    QVBoxLayout *verticalLayout_5;
    QPushButton *Elevation_Pos_pushButton;
    QGroupBox *groupBox_5;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLabel *txt_Elevation_Rate;
    QLabel *label_4;
    QLabel *txt_Azimuth_Rate;
    QGroupBox *groupBox_6;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *txt_Elevation_position;
    QLabel *txt_Azimuth_position;
    QPushButton *Elevation_Neg_pushButton;
    QPushButton *Azimuth_Pos_pushButton;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_6;
    QPushButton *CenterMode_pushButton;
    QPushButton *Track_pushButton;
    QFormLayout *formLayout_3;
    QLabel *label_10;
    QLabel *txt_OperationMode;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *Toggle_Primar_Cam_pushButton;
    QFormLayout *formLayout_4;
    QLabel *label_12;
    QLabel *txt_ActiveCam;
    QVBoxLayout *verticalLayout_4;
    QPushButton *SyncLocal_pushButton;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout;
    QLabel *txt_FOV;
    QPushButton *Saturation_Neg_pushButton;
    QPushButton *ExpTime_Neg_pushButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_5;
    QPushButton *Zoom_out_pushButton;
    QPushButton *ExpTime_Pos_pushButton;
    QLabel *txt_Brigtness;
    QSpacerItem *horizontalSpacer;
    QLabel *label_27;
    QLabel *txt_Sharpness;
    QLabel *label_30;
    QPushButton *Zoom_In_pushButton;
    QPushButton *Sharpness_Pos_pushButton;
    QLabel *txt_Contrast;
    QLabel *txt_Saturation;
    QLabel *label_22;
    QLabel *label_25;
    QLabel *txt_Exp_Mode;
    QPushButton *AutoExpMode_pushButton;
    QPushButton *Contrast_Neg_pushButton;
    QPushButton *Saturation_Pos_pushButton;
    QPushButton *Gamma_Neg_pushButton;
    QLabel *txt_Gamma;
    QPushButton *Gamma_Pos_pushButton;
    QLabel *txt_Exp_Time;
    QLabel *label_24;
    QPushButton *ManExpMode_pushButton;
    QLabel *label_23;
    QPushButton *Brigtness_Pos_pushButton;
    QLabel *label_26;
    QPushButton *Contrast_Pos_pushButton;
    QPushButton *Brigtness_Neg_pushButton;
    QPushButton *Sharpness_Neg_pushButton;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_2;
    QLabel *label_28;
    QPushButton *Zoom_In_pushButton_TI;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label;
    QLabel *txt_Contrast_TI;
    QLabel *label_29;
    QPushButton *Zoom_out_pushButton_TI;
    QLabel *txt_Brigtness_TI;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_3;
    QLabel *txt_FOV_TI;
    QLabel *txt_ACE_TI;
    QPushButton *Brigtness_Neg_pushButton_TI;
    QPushButton *Contrast_Neg_pushButton_TI;
    QPushButton *ACE_Pos_pushButton_TI;
    QPushButton *ACE_Neg_pushButton_TI;
    QPushButton *Contrast_Pos_pushButton_TI;
    QPushButton *Brigtness_Pos_pushButton_TI;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLabel *txt_ToolTip;
    QMenuBar *menubar;
    QMenu *menuUnmastered;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1033, 782);
        MainWindow->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #2b2b2b;\n"
"	color: #e0e0e0\n"
"}\n"
"\n"
"QGroupBox {\n"
"    background-color: #3a3a3a;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: #6c6c6c;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #7d7d7d;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #555555\n"
"}\n"
"\n"
"\n"
"\n"
"QToolTip {\n"
"    background-color: #444444;   /* Dark gray, softer than black */\n"
"    color: #dddddd;             /* Softer white text */\n"
"    border: 1px solid #666666;  /* Subtle border */\n"
"    padding: 2px 6px;           /* Slim padding */\n"
"    font-size: 10pt;            /* Smaller font */\n"
"    border-radius: 3px;         /* Slightly rounded corners */\n"
"}\n"
"\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        Connect_To_Remote_pushButton = new QPushButton(centralwidget);
        Connect_To_Remote_pushButton->setObjectName(QString::fromUtf8("Connect_To_Remote_pushButton"));

        verticalLayout_3->addWidget(Connect_To_Remote_pushButton);

        verticalSpacer = new QSpacerItem(20, 4, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Azimuth_Neg_pushButton = new QPushButton(groupBox_2);
        Azimuth_Neg_pushButton->setObjectName(QString::fromUtf8("Azimuth_Neg_pushButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Azimuth_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Azimuth_Neg_pushButton->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(Azimuth_Neg_pushButton);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        Elevation_Pos_pushButton = new QPushButton(groupBox_2);
        Elevation_Pos_pushButton->setObjectName(QString::fromUtf8("Elevation_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Elevation_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Elevation_Pos_pushButton->setSizePolicy(sizePolicy);

        verticalLayout_5->addWidget(Elevation_Pos_pushButton);

        groupBox_5 = new QGroupBox(groupBox_2);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"        background-color: #3c3c3c;\n"
"}\n"
"\n"
"QLabel {\n"
"        background-color: #3c3c3c;\n"
"}"));
        formLayout = new QFormLayout(groupBox_5);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(10);
        formLayout->setVerticalSpacing(10);
        formLayout->setContentsMargins(20, -1, -1, -1);
        label_2 = new QLabel(groupBox_5);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAutoFillBackground(false);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        txt_Elevation_Rate = new QLabel(groupBox_5);
        txt_Elevation_Rate->setObjectName(QString::fromUtf8("txt_Elevation_Rate"));
        txt_Elevation_Rate->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout->setWidget(0, QFormLayout::FieldRole, txt_Elevation_Rate);

        label_4 = new QLabel(groupBox_5);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        txt_Azimuth_Rate = new QLabel(groupBox_5);
        txt_Azimuth_Rate->setObjectName(QString::fromUtf8("txt_Azimuth_Rate"));
        txt_Azimuth_Rate->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout->setWidget(1, QFormLayout::FieldRole, txt_Azimuth_Rate);


        verticalLayout_5->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(groupBox_2);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"        background-color: #3c3c3c;\n"
"}\n"
"\n"
"QLabel {\n"
"        background-color: #3c3c3c;\n"
"}"));
        formLayout_2 = new QFormLayout(groupBox_6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setHorizontalSpacing(10);
        formLayout_2->setVerticalSpacing(10);
        formLayout_2->setContentsMargins(20, -1, -1, -1);
        label_6 = new QLabel(groupBox_6);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(groupBox_6);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_7);

        txt_Elevation_position = new QLabel(groupBox_6);
        txt_Elevation_position->setObjectName(QString::fromUtf8("txt_Elevation_position"));
        txt_Elevation_position->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, txt_Elevation_position);

        txt_Azimuth_position = new QLabel(groupBox_6);
        txt_Azimuth_position->setObjectName(QString::fromUtf8("txt_Azimuth_position"));
        txt_Azimuth_position->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, txt_Azimuth_position);


        verticalLayout_5->addWidget(groupBox_6);

        Elevation_Neg_pushButton = new QPushButton(groupBox_2);
        Elevation_Neg_pushButton->setObjectName(QString::fromUtf8("Elevation_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Elevation_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Elevation_Neg_pushButton->setSizePolicy(sizePolicy);

        verticalLayout_5->addWidget(Elevation_Neg_pushButton);

        verticalLayout_5->setStretch(0, 1);
        verticalLayout_5->setStretch(1, 2);
        verticalLayout_5->setStretch(2, 2);
        verticalLayout_5->setStretch(3, 1);

        horizontalLayout_2->addLayout(verticalLayout_5);

        Azimuth_Pos_pushButton = new QPushButton(groupBox_2);
        Azimuth_Pos_pushButton->setObjectName(QString::fromUtf8("Azimuth_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Azimuth_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Azimuth_Pos_pushButton->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(Azimuth_Pos_pushButton);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 2);
        horizontalLayout_2->setStretch(2, 1);

        verticalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setSpacing(30);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, -1, -1, 9);
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        CenterMode_pushButton = new QPushButton(groupBox_3);
        CenterMode_pushButton->setObjectName(QString::fromUtf8("CenterMode_pushButton"));
        sizePolicy.setHeightForWidth(CenterMode_pushButton->sizePolicy().hasHeightForWidth());
        CenterMode_pushButton->setSizePolicy(sizePolicy);

        verticalLayout_6->addWidget(CenterMode_pushButton);

        Track_pushButton = new QPushButton(groupBox_3);
        Track_pushButton->setObjectName(QString::fromUtf8("Track_pushButton"));
        sizePolicy.setHeightForWidth(Track_pushButton->sizePolicy().hasHeightForWidth());
        Track_pushButton->setSizePolicy(sizePolicy);

        verticalLayout_6->addWidget(Track_pushButton);

        verticalLayout_6->setStretch(0, 1);
        verticalLayout_6->setStretch(1, 1);

        horizontalLayout_3->addLayout(verticalLayout_6);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        formLayout_3->setContentsMargins(20, -1, 10, -1);
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setStyleSheet(QString::fromUtf8("background-color: #3a3a3a"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_10);

        txt_OperationMode = new QLabel(groupBox_3);
        txt_OperationMode->setObjectName(QString::fromUtf8("txt_OperationMode"));
        txt_OperationMode->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, txt_OperationMode);


        horizontalLayout_3->addLayout(formLayout_3);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 2);

        verticalLayout_3->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        horizontalLayout_4 = new QHBoxLayout(groupBox_4);
        horizontalLayout_4->setSpacing(30);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        Toggle_Primar_Cam_pushButton = new QPushButton(groupBox_4);
        Toggle_Primar_Cam_pushButton->setObjectName(QString::fromUtf8("Toggle_Primar_Cam_pushButton"));
        sizePolicy.setHeightForWidth(Toggle_Primar_Cam_pushButton->sizePolicy().hasHeightForWidth());
        Toggle_Primar_Cam_pushButton->setSizePolicy(sizePolicy);
        Toggle_Primar_Cam_pushButton->setMinimumSize(QSize(0, 0));

        horizontalLayout_4->addWidget(Toggle_Primar_Cam_pushButton);

        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        formLayout_4->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        formLayout_4->setContentsMargins(20, -1, 10, -1);
        label_12 = new QLabel(groupBox_4);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setStyleSheet(QString::fromUtf8("background-color: #3a3a3a"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_12);

        txt_ActiveCam = new QLabel(groupBox_4);
        txt_ActiveCam->setObjectName(QString::fromUtf8("txt_ActiveCam"));
        txt_ActiveCam->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, txt_ActiveCam);


        horizontalLayout_4->addLayout(formLayout_4);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 2);

        verticalLayout_3->addWidget(groupBox_4);

        verticalLayout_3->setStretch(0, 1);
        verticalLayout_3->setStretch(2, 7);
        verticalLayout_3->setStretch(3, 3);
        verticalLayout_3->setStretch(4, 2);

        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        SyncLocal_pushButton = new QPushButton(centralwidget);
        SyncLocal_pushButton->setObjectName(QString::fromUtf8("SyncLocal_pushButton"));

        verticalLayout_4->addWidget(SyncLocal_pushButton);

        verticalSpacer_2 = new QSpacerItem(20, 4, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        groupBox_7 = new QGroupBox(centralwidget);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setStyleSheet(QString::fromUtf8("QLabel {\n"
"	background-color: #3a3a3a;\n"
"}"));
        gridLayout = new QGridLayout(groupBox_7);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(5);
        gridLayout->setVerticalSpacing(10);
        txt_FOV = new QLabel(groupBox_7);
        txt_FOV->setObjectName(QString::fromUtf8("txt_FOV"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(txt_FOV->sizePolicy().hasHeightForWidth());
        txt_FOV->setSizePolicy(sizePolicy1);
        txt_FOV->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_FOV, 0, 3, 1, 1);

        Saturation_Neg_pushButton = new QPushButton(groupBox_7);
        Saturation_Neg_pushButton->setObjectName(QString::fromUtf8("Saturation_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Saturation_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Saturation_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Saturation_Neg_pushButton, 2, 0, 1, 1);

        ExpTime_Neg_pushButton = new QPushButton(groupBox_7);
        ExpTime_Neg_pushButton->setObjectName(QString::fromUtf8("ExpTime_Neg_pushButton"));
        sizePolicy.setHeightForWidth(ExpTime_Neg_pushButton->sizePolicy().hasHeightForWidth());
        ExpTime_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(ExpTime_Neg_pushButton, 7, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 4, 1, 1);

        label_5 = new QLabel(groupBox_7);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_5, 4, 2, 1, 1);

        Zoom_out_pushButton = new QPushButton(groupBox_7);
        Zoom_out_pushButton->setObjectName(QString::fromUtf8("Zoom_out_pushButton"));
        sizePolicy.setHeightForWidth(Zoom_out_pushButton->sizePolicy().hasHeightForWidth());
        Zoom_out_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Zoom_out_pushButton, 0, 5, 1, 1);

        ExpTime_Pos_pushButton = new QPushButton(groupBox_7);
        ExpTime_Pos_pushButton->setObjectName(QString::fromUtf8("ExpTime_Pos_pushButton"));
        sizePolicy.setHeightForWidth(ExpTime_Pos_pushButton->sizePolicy().hasHeightForWidth());
        ExpTime_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(ExpTime_Pos_pushButton, 7, 5, 1, 1);

        txt_Brigtness = new QLabel(groupBox_7);
        txt_Brigtness->setObjectName(QString::fromUtf8("txt_Brigtness"));
        sizePolicy1.setHeightForWidth(txt_Brigtness->sizePolicy().hasHeightForWidth());
        txt_Brigtness->setSizePolicy(sizePolicy1);
        txt_Brigtness->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Brigtness, 3, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(10, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 1, 1, 1);

        label_27 = new QLabel(groupBox_7);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        sizePolicy1.setHeightForWidth(label_27->sizePolicy().hasHeightForWidth());
        label_27->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_27, 6, 2, 1, 1);

        txt_Sharpness = new QLabel(groupBox_7);
        txt_Sharpness->setObjectName(QString::fromUtf8("txt_Sharpness"));
        sizePolicy1.setHeightForWidth(txt_Sharpness->sizePolicy().hasHeightForWidth());
        txt_Sharpness->setSizePolicy(sizePolicy1);
        txt_Sharpness->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Sharpness, 4, 3, 1, 1);

        label_30 = new QLabel(groupBox_7);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        sizePolicy1.setHeightForWidth(label_30->sizePolicy().hasHeightForWidth());
        label_30->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_30, 7, 2, 1, 1);

        Zoom_In_pushButton = new QPushButton(groupBox_7);
        Zoom_In_pushButton->setObjectName(QString::fromUtf8("Zoom_In_pushButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Zoom_In_pushButton->sizePolicy().hasHeightForWidth());
        Zoom_In_pushButton->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(Zoom_In_pushButton, 0, 0, 1, 1);

        Sharpness_Pos_pushButton = new QPushButton(groupBox_7);
        Sharpness_Pos_pushButton->setObjectName(QString::fromUtf8("Sharpness_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Sharpness_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Sharpness_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Sharpness_Pos_pushButton, 4, 5, 1, 1);

        txt_Contrast = new QLabel(groupBox_7);
        txt_Contrast->setObjectName(QString::fromUtf8("txt_Contrast"));
        sizePolicy1.setHeightForWidth(txt_Contrast->sizePolicy().hasHeightForWidth());
        txt_Contrast->setSizePolicy(sizePolicy1);
        txt_Contrast->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Contrast, 5, 3, 1, 1);

        txt_Saturation = new QLabel(groupBox_7);
        txt_Saturation->setObjectName(QString::fromUtf8("txt_Saturation"));
        sizePolicy1.setHeightForWidth(txt_Saturation->sizePolicy().hasHeightForWidth());
        txt_Saturation->setSizePolicy(sizePolicy1);
        txt_Saturation->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Saturation, 2, 3, 1, 1);

        label_22 = new QLabel(groupBox_7);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        sizePolicy1.setHeightForWidth(label_22->sizePolicy().hasHeightForWidth());
        label_22->setSizePolicy(sizePolicy1);
        label_22->setAutoFillBackground(false);

        gridLayout->addWidget(label_22, 0, 2, 1, 1);

        label_25 = new QLabel(groupBox_7);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        sizePolicy1.setHeightForWidth(label_25->sizePolicy().hasHeightForWidth());
        label_25->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_25, 3, 2, 1, 1);

        txt_Exp_Mode = new QLabel(groupBox_7);
        txt_Exp_Mode->setObjectName(QString::fromUtf8("txt_Exp_Mode"));
        sizePolicy1.setHeightForWidth(txt_Exp_Mode->sizePolicy().hasHeightForWidth());
        txt_Exp_Mode->setSizePolicy(sizePolicy1);
        txt_Exp_Mode->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Exp_Mode, 6, 3, 1, 1);

        AutoExpMode_pushButton = new QPushButton(groupBox_7);
        AutoExpMode_pushButton->setObjectName(QString::fromUtf8("AutoExpMode_pushButton"));
        sizePolicy.setHeightForWidth(AutoExpMode_pushButton->sizePolicy().hasHeightForWidth());
        AutoExpMode_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(AutoExpMode_pushButton, 6, 5, 1, 1);

        Contrast_Neg_pushButton = new QPushButton(groupBox_7);
        Contrast_Neg_pushButton->setObjectName(QString::fromUtf8("Contrast_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Contrast_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Contrast_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Contrast_Neg_pushButton, 5, 0, 1, 1);

        Saturation_Pos_pushButton = new QPushButton(groupBox_7);
        Saturation_Pos_pushButton->setObjectName(QString::fromUtf8("Saturation_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Saturation_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Saturation_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Saturation_Pos_pushButton, 2, 5, 1, 1);

        Gamma_Neg_pushButton = new QPushButton(groupBox_7);
        Gamma_Neg_pushButton->setObjectName(QString::fromUtf8("Gamma_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Gamma_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Gamma_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Gamma_Neg_pushButton, 1, 0, 1, 1);

        txt_Gamma = new QLabel(groupBox_7);
        txt_Gamma->setObjectName(QString::fromUtf8("txt_Gamma"));
        sizePolicy1.setHeightForWidth(txt_Gamma->sizePolicy().hasHeightForWidth());
        txt_Gamma->setSizePolicy(sizePolicy1);
        txt_Gamma->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Gamma, 1, 3, 1, 1);

        Gamma_Pos_pushButton = new QPushButton(groupBox_7);
        Gamma_Pos_pushButton->setObjectName(QString::fromUtf8("Gamma_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Gamma_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Gamma_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Gamma_Pos_pushButton, 1, 5, 1, 1);

        txt_Exp_Time = new QLabel(groupBox_7);
        txt_Exp_Time->setObjectName(QString::fromUtf8("txt_Exp_Time"));
        sizePolicy1.setHeightForWidth(txt_Exp_Time->sizePolicy().hasHeightForWidth());
        txt_Exp_Time->setSizePolicy(sizePolicy1);
        txt_Exp_Time->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout->addWidget(txt_Exp_Time, 7, 3, 1, 1);

        label_24 = new QLabel(groupBox_7);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        sizePolicy1.setHeightForWidth(label_24->sizePolicy().hasHeightForWidth());
        label_24->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_24, 2, 2, 1, 1);

        ManExpMode_pushButton = new QPushButton(groupBox_7);
        ManExpMode_pushButton->setObjectName(QString::fromUtf8("ManExpMode_pushButton"));
        sizePolicy.setHeightForWidth(ManExpMode_pushButton->sizePolicy().hasHeightForWidth());
        ManExpMode_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(ManExpMode_pushButton, 6, 0, 1, 1);

        label_23 = new QLabel(groupBox_7);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        sizePolicy1.setHeightForWidth(label_23->sizePolicy().hasHeightForWidth());
        label_23->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_23, 1, 2, 1, 1);

        Brigtness_Pos_pushButton = new QPushButton(groupBox_7);
        Brigtness_Pos_pushButton->setObjectName(QString::fromUtf8("Brigtness_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Brigtness_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Brigtness_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Brigtness_Pos_pushButton, 3, 5, 1, 1);

        label_26 = new QLabel(groupBox_7);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        sizePolicy1.setHeightForWidth(label_26->sizePolicy().hasHeightForWidth());
        label_26->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_26, 5, 2, 1, 1);

        Contrast_Pos_pushButton = new QPushButton(groupBox_7);
        Contrast_Pos_pushButton->setObjectName(QString::fromUtf8("Contrast_Pos_pushButton"));
        sizePolicy.setHeightForWidth(Contrast_Pos_pushButton->sizePolicy().hasHeightForWidth());
        Contrast_Pos_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Contrast_Pos_pushButton, 5, 5, 1, 1);

        Brigtness_Neg_pushButton = new QPushButton(groupBox_7);
        Brigtness_Neg_pushButton->setObjectName(QString::fromUtf8("Brigtness_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Brigtness_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Brigtness_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Brigtness_Neg_pushButton, 3, 0, 1, 1);

        Sharpness_Neg_pushButton = new QPushButton(groupBox_7);
        Sharpness_Neg_pushButton->setObjectName(QString::fromUtf8("Sharpness_Neg_pushButton"));
        sizePolicy.setHeightForWidth(Sharpness_Neg_pushButton->sizePolicy().hasHeightForWidth());
        Sharpness_Neg_pushButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Sharpness_Neg_pushButton, 4, 0, 1, 1);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 1);
        gridLayout->setRowStretch(2, 1);
        gridLayout->setRowStretch(3, 1);
        gridLayout->setRowStretch(4, 1);
        gridLayout->setRowStretch(5, 1);
        gridLayout->setRowStretch(6, 1);
        gridLayout->setRowStretch(7, 1);
        gridLayout->setColumnStretch(0, 3);
        gridLayout->setColumnStretch(2, 2);
        gridLayout->setColumnStretch(3, 2);
        gridLayout->setColumnStretch(5, 3);

        verticalLayout_4->addWidget(groupBox_7);

        groupBox_8 = new QGroupBox(centralwidget);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setStyleSheet(QString::fromUtf8("QLabel {\n"
"	background-color: #3a3a3a;\n"
"}"));
        gridLayout_2 = new QGridLayout(groupBox_8);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(5);
        gridLayout_2->setVerticalSpacing(10);
        label_28 = new QLabel(groupBox_8);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        sizePolicy1.setHeightForWidth(label_28->sizePolicy().hasHeightForWidth());
        label_28->setSizePolicy(sizePolicy1);
        label_28->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_28, 0, 2, 1, 1);

        Zoom_In_pushButton_TI = new QPushButton(groupBox_8);
        Zoom_In_pushButton_TI->setObjectName(QString::fromUtf8("Zoom_In_pushButton_TI"));
        sizePolicy.setHeightForWidth(Zoom_In_pushButton_TI->sizePolicy().hasHeightForWidth());
        Zoom_In_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(Zoom_In_pushButton_TI, 0, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 1, 1, 1);

        label = new QLabel(groupBox_8);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label, 2, 2, 1, 1);

        txt_Contrast_TI = new QLabel(groupBox_8);
        txt_Contrast_TI->setObjectName(QString::fromUtf8("txt_Contrast_TI"));
        sizePolicy1.setHeightForWidth(txt_Contrast_TI->sizePolicy().hasHeightForWidth());
        txt_Contrast_TI->setSizePolicy(sizePolicy1);
        txt_Contrast_TI->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout_2->addWidget(txt_Contrast_TI, 3, 3, 1, 1);

        label_29 = new QLabel(groupBox_8);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        sizePolicy1.setHeightForWidth(label_29->sizePolicy().hasHeightForWidth());
        label_29->setSizePolicy(sizePolicy1);
        label_29->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_29, 1, 2, 1, 1);

        Zoom_out_pushButton_TI = new QPushButton(groupBox_8);
        Zoom_out_pushButton_TI->setObjectName(QString::fromUtf8("Zoom_out_pushButton_TI"));
        sizePolicy2.setHeightForWidth(Zoom_out_pushButton_TI->sizePolicy().hasHeightForWidth());
        Zoom_out_pushButton_TI->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(Zoom_out_pushButton_TI, 0, 5, 1, 1);

        txt_Brigtness_TI = new QLabel(groupBox_8);
        txt_Brigtness_TI->setObjectName(QString::fromUtf8("txt_Brigtness_TI"));
        sizePolicy1.setHeightForWidth(txt_Brigtness_TI->sizePolicy().hasHeightForWidth());
        txt_Brigtness_TI->setSizePolicy(sizePolicy1);
        txt_Brigtness_TI->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout_2->addWidget(txt_Brigtness_TI, 2, 3, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(10, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 0, 4, 1, 1);

        label_3 = new QLabel(groupBox_8);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_3, 3, 2, 1, 1);

        txt_FOV_TI = new QLabel(groupBox_8);
        txt_FOV_TI->setObjectName(QString::fromUtf8("txt_FOV_TI"));
        sizePolicy1.setHeightForWidth(txt_FOV_TI->sizePolicy().hasHeightForWidth());
        txt_FOV_TI->setSizePolicy(sizePolicy1);
        txt_FOV_TI->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout_2->addWidget(txt_FOV_TI, 0, 3, 1, 1);

        txt_ACE_TI = new QLabel(groupBox_8);
        txt_ACE_TI->setObjectName(QString::fromUtf8("txt_ACE_TI"));
        sizePolicy1.setHeightForWidth(txt_ACE_TI->sizePolicy().hasHeightForWidth());
        txt_ACE_TI->setSizePolicy(sizePolicy1);
        txt_ACE_TI->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));

        gridLayout_2->addWidget(txt_ACE_TI, 1, 3, 1, 1);

        Brigtness_Neg_pushButton_TI = new QPushButton(groupBox_8);
        Brigtness_Neg_pushButton_TI->setObjectName(QString::fromUtf8("Brigtness_Neg_pushButton_TI"));
        sizePolicy.setHeightForWidth(Brigtness_Neg_pushButton_TI->sizePolicy().hasHeightForWidth());
        Brigtness_Neg_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(Brigtness_Neg_pushButton_TI, 2, 0, 1, 1);

        Contrast_Neg_pushButton_TI = new QPushButton(groupBox_8);
        Contrast_Neg_pushButton_TI->setObjectName(QString::fromUtf8("Contrast_Neg_pushButton_TI"));
        sizePolicy.setHeightForWidth(Contrast_Neg_pushButton_TI->sizePolicy().hasHeightForWidth());
        Contrast_Neg_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(Contrast_Neg_pushButton_TI, 3, 0, 1, 1);

        ACE_Pos_pushButton_TI = new QPushButton(groupBox_8);
        ACE_Pos_pushButton_TI->setObjectName(QString::fromUtf8("ACE_Pos_pushButton_TI"));
        sizePolicy.setHeightForWidth(ACE_Pos_pushButton_TI->sizePolicy().hasHeightForWidth());
        ACE_Pos_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(ACE_Pos_pushButton_TI, 1, 5, 1, 1);

        ACE_Neg_pushButton_TI = new QPushButton(groupBox_8);
        ACE_Neg_pushButton_TI->setObjectName(QString::fromUtf8("ACE_Neg_pushButton_TI"));
        sizePolicy.setHeightForWidth(ACE_Neg_pushButton_TI->sizePolicy().hasHeightForWidth());
        ACE_Neg_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(ACE_Neg_pushButton_TI, 1, 0, 1, 1);

        Contrast_Pos_pushButton_TI = new QPushButton(groupBox_8);
        Contrast_Pos_pushButton_TI->setObjectName(QString::fromUtf8("Contrast_Pos_pushButton_TI"));
        sizePolicy.setHeightForWidth(Contrast_Pos_pushButton_TI->sizePolicy().hasHeightForWidth());
        Contrast_Pos_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(Contrast_Pos_pushButton_TI, 3, 5, 1, 1);

        Brigtness_Pos_pushButton_TI = new QPushButton(groupBox_8);
        Brigtness_Pos_pushButton_TI->setObjectName(QString::fromUtf8("Brigtness_Pos_pushButton_TI"));
        sizePolicy.setHeightForWidth(Brigtness_Pos_pushButton_TI->sizePolicy().hasHeightForWidth());
        Brigtness_Pos_pushButton_TI->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(Brigtness_Pos_pushButton_TI, 2, 5, 1, 1);

        gridLayout_2->setRowStretch(0, 1);
        gridLayout_2->setRowStretch(1, 1);
        gridLayout_2->setRowStretch(2, 1);
        gridLayout_2->setRowStretch(3, 1);
        gridLayout_2->setColumnStretch(0, 3);
        gridLayout_2->setColumnStretch(2, 2);
        gridLayout_2->setColumnStretch(3, 2);
        gridLayout_2->setColumnStretch(5, 3);

        verticalLayout_4->addWidget(groupBox_8);

        verticalLayout_4->setStretch(0, 1);
        verticalLayout_4->setStretch(2, 3);
        verticalLayout_4->setStretch(3, 2);

        horizontalLayout->addLayout(verticalLayout_4);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setStyleSheet(QString::fromUtf8("background-color: #3a3a3a"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        txt_ToolTip = new QLabel(groupBox);
        txt_ToolTip->setObjectName(QString::fromUtf8("txt_ToolTip"));
        txt_ToolTip->setStyleSheet(QString::fromUtf8("background-color: #4a4a4a"));
        txt_ToolTip->setFrameShape(QFrame::NoFrame);

        verticalLayout_2->addWidget(txt_ToolTip);


        horizontalLayout_5->addWidget(groupBox);

        horizontalLayout_5->setStretch(0, 10);

        verticalLayout->addLayout(horizontalLayout_5);

        verticalLayout->setStretch(0, 9);
        verticalLayout->setStretch(1, 1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1033, 22));
        menuUnmastered = new QMenu(menubar);
        menuUnmastered->setObjectName(QString::fromUtf8("menuUnmastered"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuUnmastered->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        Connect_To_Remote_pushButton->setText(QApplication::translate("MainWindow", "Initiate Communication", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Gimbal Movement", nullptr));
        Azimuth_Neg_pushButton->setText(QApplication::translate("MainWindow", "Azimuth -", nullptr));
        Elevation_Pos_pushButton->setText(QApplication::translate("MainWindow", "Elevation +", nullptr));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Rate", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Elevation:", nullptr));
        txt_Elevation_Rate->setText(QString());
        label_4->setText(QApplication::translate("MainWindow", "Azimuth:", nullptr));
        txt_Azimuth_Rate->setText(QString());
        groupBox_6->setTitle(QApplication::translate("MainWindow", "Position", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Elevation:", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Azimuth:", nullptr));
        txt_Elevation_position->setText(QString());
        txt_Azimuth_position->setText(QString());
        Elevation_Neg_pushButton->setText(QApplication::translate("MainWindow", "Elevation -", nullptr));
        Azimuth_Pos_pushButton->setText(QApplication::translate("MainWindow", "Azimuth +", nullptr));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Operation Mode", nullptr));
        CenterMode_pushButton->setText(QApplication::translate("MainWindow", "Center", nullptr));
        Track_pushButton->setText(QApplication::translate("MainWindow", "Track", nullptr));
        label_10->setText(QApplication::translate("MainWindow", " Mode:", nullptr));
        txt_OperationMode->setText(QString());
        groupBox_4->setTitle(QApplication::translate("MainWindow", "GroupBox", nullptr));
        Toggle_Primar_Cam_pushButton->setText(QApplication::translate("MainWindow", "Toggle Active\n"
"Camera", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "Active Camera:", nullptr));
        txt_ActiveCam->setText(QString());
        SyncLocal_pushButton->setText(QApplication::translate("MainWindow", "Manual Sync Local", nullptr));
        groupBox_7->setTitle(QApplication::translate("MainWindow", "Daylight Channel", nullptr));
        txt_FOV->setText(QString());
        Saturation_Neg_pushButton->setText(QApplication::translate("MainWindow", "Saturation -", nullptr));
        ExpTime_Neg_pushButton->setText(QApplication::translate("MainWindow", "Exp Time -", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Sharpness:", nullptr));
        Zoom_out_pushButton->setText(QApplication::translate("MainWindow", "Zoom Out", nullptr));
        ExpTime_Pos_pushButton->setText(QApplication::translate("MainWindow", "Exp Time +", nullptr));
        txt_Brigtness->setText(QString());
        label_27->setText(QApplication::translate("MainWindow", "Exposure:", nullptr));
        txt_Sharpness->setText(QString());
        label_30->setText(QApplication::translate("MainWindow", "Exp Time:", nullptr));
        Zoom_In_pushButton->setText(QApplication::translate("MainWindow", "Zoom In", nullptr));
        Sharpness_Pos_pushButton->setText(QApplication::translate("MainWindow", "Sharpness +", nullptr));
        txt_Contrast->setText(QString());
        txt_Saturation->setText(QString());
        label_22->setText(QApplication::translate("MainWindow", "FOV:", nullptr));
        label_25->setText(QApplication::translate("MainWindow", "Level:", nullptr));
        txt_Exp_Mode->setText(QString());
        AutoExpMode_pushButton->setText(QApplication::translate("MainWindow", "Auto Exposure", nullptr));
        Contrast_Neg_pushButton->setText(QApplication::translate("MainWindow", "Gain -", nullptr));
        Saturation_Pos_pushButton->setText(QApplication::translate("MainWindow", "Saturation +", nullptr));
        Gamma_Neg_pushButton->setText(QApplication::translate("MainWindow", "Gamma -", nullptr));
        txt_Gamma->setText(QString());
        Gamma_Pos_pushButton->setText(QApplication::translate("MainWindow", "Gamma +", nullptr));
        txt_Exp_Time->setText(QString());
        label_24->setText(QApplication::translate("MainWindow", "Saturation:", nullptr));
        ManExpMode_pushButton->setText(QApplication::translate("MainWindow", "Manual Exposure", nullptr));
        label_23->setText(QApplication::translate("MainWindow", "Gamma:", nullptr));
        Brigtness_Pos_pushButton->setText(QApplication::translate("MainWindow", "Level +", nullptr));
        label_26->setText(QApplication::translate("MainWindow", "Gain:", nullptr));
        Contrast_Pos_pushButton->setText(QApplication::translate("MainWindow", "Gain +", nullptr));
        Brigtness_Neg_pushButton->setText(QApplication::translate("MainWindow", "Level -", nullptr));
        Sharpness_Neg_pushButton->setText(QApplication::translate("MainWindow", "Sharpness -", nullptr));
        groupBox_8->setTitle(QApplication::translate("MainWindow", "Thermal Channel", nullptr));
        label_28->setText(QApplication::translate("MainWindow", "FOV:", nullptr));
        Zoom_In_pushButton_TI->setText(QApplication::translate("MainWindow", "Zoom In", nullptr));
        label->setText(QApplication::translate("MainWindow", "Level:", nullptr));
        txt_Contrast_TI->setText(QString());
        label_29->setText(QApplication::translate("MainWindow", "Gamma:", nullptr));
        Zoom_out_pushButton_TI->setText(QApplication::translate("MainWindow", "Zoom Out", nullptr));
        txt_Brigtness_TI->setText(QString());
        label_3->setText(QApplication::translate("MainWindow", "Gain:", nullptr));
        txt_FOV_TI->setText(QString());
        txt_ACE_TI->setText(QString());
        Brigtness_Neg_pushButton_TI->setText(QApplication::translate("MainWindow", "Level -", nullptr));
        Contrast_Neg_pushButton_TI->setText(QApplication::translate("MainWindow", "Gain -", nullptr));
        ACE_Pos_pushButton_TI->setText(QApplication::translate("MainWindow", "ACE +", nullptr));
        ACE_Neg_pushButton_TI->setText(QApplication::translate("MainWindow", "ACE -", nullptr));
        Contrast_Pos_pushButton_TI->setText(QApplication::translate("MainWindow", "Gain +", nullptr));
        Brigtness_Pos_pushButton_TI->setText(QApplication::translate("MainWindow", "Level +", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "ToolTip", nullptr));
        txt_ToolTip->setText(QString());
        menuUnmastered->setTitle(QApplication::translate("MainWindow", "SDK Demo App", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
