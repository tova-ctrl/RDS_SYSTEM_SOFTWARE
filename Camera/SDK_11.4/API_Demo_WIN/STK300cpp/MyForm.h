#pragma once
extern "C"
{
	#include "stkAPI.h"
}

#include <thread>
#include <xtimec.h>
#include "Globals.h"
#include "iostream"
#include "msgBoxForm.h"

#include <iostream>
#include <string>

namespace STK300cpp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace std;


	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ bInitCommunication;
	private: System::Windows::Forms::GroupBox^ gbGimbalMovment;
	private: System::Windows::Forms::Button^ bElevationPlus;
	private: System::Windows::Forms::ToolTip^ toolTip1;
	private: System::Windows::Forms::Button^ bElevationMinus;
	private: System::Windows::Forms::Button^ bAzimuthPlus;
	private: System::Windows::Forms::Button^ bAzimuthMinus;
	private: System::Windows::Forms::GroupBox^ gbRate;
	private: System::Windows::Forms::GroupBox^ gbPosition;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::TextBox^ tbPositionAzimuth;

	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::TextBox^ tbPositionElevation;

	private: System::Windows::Forms::Label^ lAzimuth;
	private: System::Windows::Forms::TextBox^ tbRateAzimuth;

	private: System::Windows::Forms::Label^ lElevation;
	private: System::Windows::Forms::TextBox^ tbRateElevation;
	private: System::Windows::Forms::GroupBox^ gbThermalChannel;
	private: System::Windows::Forms::Label^ lThermalChannelFOV;
	private: System::Windows::Forms::Button^ bThermalChannelZoomOut;
	private: System::Windows::Forms::TextBox^ tbThermalChannelFOV;
	private: System::Windows::Forms::Button^ bThermalChannelZoomIn;
	private: System::Windows::Forms::GroupBox^ gbVideoControlCommands;
	private: System::Windows::Forms::Label^ lActiveCamera;
	private: System::Windows::Forms::TextBox^ tbActiveCamera;
	private: System::Windows::Forms::Button^ bToggleActiveCamera;
	private: System::Windows::Forms::Button^ bManualSyncLocal;
	private: System::Windows::Forms::GroupBox^ gbDayLightChannel;








	private: System::Windows::Forms::Label^ lDayLightChannelExpTime;
	private: System::Windows::Forms::Button^ bDayLightChannelExpTimePluse;


	private: System::Windows::Forms::TextBox^ tbDayLightChannelExpTime;

	private: System::Windows::Forms::Button^ bDayLightChannelExpTimeMinus;
	private: System::Windows::Forms::Label^ lDayLightChannelExposure;


	private: System::Windows::Forms::Button^ bDayLightChannelAutoExposure;


	private: System::Windows::Forms::TextBox^ tbDayLightChannelExposure;

	private: System::Windows::Forms::Button^ bDayLightChannelManualExposure;

	private: System::Windows::Forms::Label^ lDayLightChannelBrightness;

	private: System::Windows::Forms::Button^ bDayLightChannelbrightnessPlus;
	private: System::Windows::Forms::TextBox^ tbDayLightChannelBrightness;



	private: System::Windows::Forms::Button^ bDayLightChannelBrightnessMinus;



	private: System::Windows::Forms::Label^ lDayLightChannelContrast;
	private: System::Windows::Forms::Button^ bDayLightChannelContrastPlus;
	private: System::Windows::Forms::TextBox^ tbDayLightChannelContrast;
	private: System::Windows::Forms::Button^ bDayLightChannelContrastMinus;


	private: System::Windows::Forms::Label^ lDayLightChannelSaturation;
	private: System::Windows::Forms::Button^ bDayLightChannelSaturationPlus;
	private: System::Windows::Forms::TextBox^ tbDayLightChannelSaturation;
	private: System::Windows::Forms::Button^ bDayLightChannelSaturationMinus;
	private: System::Windows::Forms::Label^ lDayLightChannelGamma;
	private: System::Windows::Forms::Button^ bDayLightChannelGammaPlus;
	private: System::Windows::Forms::TextBox^ tbDayLightChannelGamma;
	private: System::Windows::Forms::Button^ bDayLightChannelGammaMinus;
	private: System::Windows::Forms::Label^ lDayLightChannelFOV;
	private: System::Windows::Forms::Button^ bDayLightChannelZoomOut;
	private: System::Windows::Forms::TextBox^ tbDayLightChannelFOV;
	private: System::Windows::Forms::Button^ bDayLightChannelZoomIn;
private: System::Windows::Forms::Timer^ timer1;

private: System::Windows::Forms::Label^ label4;
private: System::Windows::Forms::GroupBox^ groupBox1;
private: System::Windows::Forms::Label^ label5;
private: System::Windows::Forms::Button^ btTrack;
private: System::Windows::Forms::TextBox^ tbOperationMode;


private: System::Windows::Forms::Button^ btCEnterMode;
private: System::Windows::Forms::TextBox^ tbToolTip;
private: System::Windows::Forms::TextBox^ textBox1;
private: System::Windows::Forms::Button^ btTrackX;
private: System::Windows::Forms::TextBox^ textBox2;
private: System::Windows::Forms::Button^ btTrackY;











	private: System::ComponentModel::IContainer^ components;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->bInitCommunication = (gcnew System::Windows::Forms::Button());
			this->gbGimbalMovment = (gcnew System::Windows::Forms::GroupBox());
			this->gbPosition = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tbPositionAzimuth = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbPositionElevation = (gcnew System::Windows::Forms::TextBox());
			this->gbRate = (gcnew System::Windows::Forms::GroupBox());
			this->lAzimuth = (gcnew System::Windows::Forms::Label());
			this->tbRateAzimuth = (gcnew System::Windows::Forms::TextBox());
			this->lElevation = (gcnew System::Windows::Forms::Label());
			this->tbRateElevation = (gcnew System::Windows::Forms::TextBox());
			this->bAzimuthPlus = (gcnew System::Windows::Forms::Button());
			this->bAzimuthMinus = (gcnew System::Windows::Forms::Button());
			this->bElevationMinus = (gcnew System::Windows::Forms::Button());
			this->bElevationPlus = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->bThermalChannelZoomIn = (gcnew System::Windows::Forms::Button());
			this->bThermalChannelZoomOut = (gcnew System::Windows::Forms::Button());
			this->tbThermalChannelFOV = (gcnew System::Windows::Forms::TextBox());
			this->tbActiveCamera = (gcnew System::Windows::Forms::TextBox());
			this->bToggleActiveCamera = (gcnew System::Windows::Forms::Button());
			this->bManualSyncLocal = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelZoomOut = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelFOV = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelZoomIn = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelGammaPlus = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelGamma = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelGammaMinus = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelSaturationPlus = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelSaturation = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelSaturationMinus = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelContrastPlus = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelContrast = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelContrastMinus = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelbrightnessPlus = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelBrightness = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelBrightnessMinus = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelAutoExposure = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelExposure = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelManualExposure = (gcnew System::Windows::Forms::Button());
			this->bDayLightChannelExpTimePluse = (gcnew System::Windows::Forms::Button());
			this->tbDayLightChannelExpTime = (gcnew System::Windows::Forms::TextBox());
			this->bDayLightChannelExpTimeMinus = (gcnew System::Windows::Forms::Button());
			this->btTrack = (gcnew System::Windows::Forms::Button());
			this->tbOperationMode = (gcnew System::Windows::Forms::TextBox());
			this->btCEnterMode = (gcnew System::Windows::Forms::Button());
			this->btTrackX = (gcnew System::Windows::Forms::Button());
			this->gbThermalChannel = (gcnew System::Windows::Forms::GroupBox());
			this->lThermalChannelFOV = (gcnew System::Windows::Forms::Label());
			this->gbVideoControlCommands = (gcnew System::Windows::Forms::GroupBox());
			this->lActiveCamera = (gcnew System::Windows::Forms::Label());
			this->gbDayLightChannel = (gcnew System::Windows::Forms::GroupBox());
			this->lDayLightChannelExpTime = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelExposure = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelBrightness = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelContrast = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelSaturation = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelGamma = (gcnew System::Windows::Forms::Label());
			this->lDayLightChannelFOV = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->tbToolTip = (gcnew System::Windows::Forms::TextBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->btTrackY = (gcnew System::Windows::Forms::Button());
			this->gbGimbalMovment->SuspendLayout();
			this->gbPosition->SuspendLayout();
			this->gbRate->SuspendLayout();
			this->gbThermalChannel->SuspendLayout();
			this->gbVideoControlCommands->SuspendLayout();
			this->gbDayLightChannel->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// bInitCommunication
			// 
			this->bInitCommunication->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bInitCommunication->Location = System::Drawing::Point(12, 12);
			this->bInitCommunication->Name = L"bInitCommunication";
			this->bInitCommunication->Size = System::Drawing::Size(331, 23);
			this->bInitCommunication->TabIndex = 0;
			this->bInitCommunication->Text = L"Initiate Communication";
			this->toolTip1->SetToolTip(this->bInitCommunication, L"int STKInitializeCommunication(struct MessagesTransmitEnableStruct data)");
			this->bInitCommunication->UseVisualStyleBackColor = false;
			this->bInitCommunication->Click += gcnew System::EventHandler(this, &MyForm::bInitCommunication_Click);
			this->bInitCommunication->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// gbGimbalMovment
			// 
			this->gbGimbalMovment->BackColor = System::Drawing::SystemColors::ControlLight;
			this->gbGimbalMovment->Controls->Add(this->gbPosition);
			this->gbGimbalMovment->Controls->Add(this->gbRate);
			this->gbGimbalMovment->Controls->Add(this->bAzimuthPlus);
			this->gbGimbalMovment->Controls->Add(this->bAzimuthMinus);
			this->gbGimbalMovment->Controls->Add(this->bElevationMinus);
			this->gbGimbalMovment->Controls->Add(this->bElevationPlus);
			this->gbGimbalMovment->Location = System::Drawing::Point(12, 41);
			this->gbGimbalMovment->Name = L"gbGimbalMovment";
			this->gbGimbalMovment->Size = System::Drawing::Size(331, 273);
			this->gbGimbalMovment->TabIndex = 1;
			this->gbGimbalMovment->TabStop = false;
			this->gbGimbalMovment->Text = L"Gimbal Movment";
			// 
			// gbPosition
			// 
			this->gbPosition->BackColor = System::Drawing::Color::Gainsboro;
			this->gbPosition->Controls->Add(this->label1);
			this->gbPosition->Controls->Add(this->tbPositionAzimuth);
			this->gbPosition->Controls->Add(this->label2);
			this->gbPosition->Controls->Add(this->tbPositionElevation);
			this->gbPosition->Location = System::Drawing::Point(94, 148);
			this->gbPosition->Name = L"gbPosition";
			this->gbPosition->Size = System::Drawing::Size(139, 73);
			this->gbPosition->TabIndex = 6;
			this->gbPosition->TabStop = false;
			this->gbPosition->Text = L"Position";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(14, 49);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Azimuth";
			// 
			// tbPositionAzimuth
			// 
			this->tbPositionAzimuth->Location = System::Drawing::Point(71, 46);
			this->tbPositionAzimuth->Name = L"tbPositionAzimuth";
			this->tbPositionAzimuth->Size = System::Drawing::Size(61, 20);
			this->tbPositionAzimuth->TabIndex = 2;
			this->toolTip1->SetToolTip(this->tbPositionAzimuth, L"float GetPositionReportRelativeGimbalAngles_X()");
			this->tbPositionAzimuth->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(14, 23);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(51, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Elevation";
			// 
			// tbPositionElevation
			// 
			this->tbPositionElevation->Location = System::Drawing::Point(71, 20);
			this->tbPositionElevation->Name = L"tbPositionElevation";
			this->tbPositionElevation->Size = System::Drawing::Size(61, 20);
			this->tbPositionElevation->TabIndex = 0;
			this->toolTip1->SetToolTip(this->tbPositionElevation, L"float GetPositionReportRelativeGimbalAngles_Y()");
			this->tbPositionElevation->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// gbRate
			// 
			this->gbRate->BackColor = System::Drawing::Color::Gainsboro;
			this->gbRate->Controls->Add(this->lAzimuth);
			this->gbRate->Controls->Add(this->tbRateAzimuth);
			this->gbRate->Controls->Add(this->lElevation);
			this->gbRate->Controls->Add(this->tbRateElevation);
			this->gbRate->Location = System::Drawing::Point(95, 69);
			this->gbRate->Name = L"gbRate";
			this->gbRate->Size = System::Drawing::Size(139, 73);
			this->gbRate->TabIndex = 5;
			this->gbRate->TabStop = false;
			this->gbRate->Text = L"Rate";
			// 
			// lAzimuth
			// 
			this->lAzimuth->AutoSize = true;
			this->lAzimuth->Location = System::Drawing::Point(14, 48);
			this->lAzimuth->Name = L"lAzimuth";
			this->lAzimuth->Size = System::Drawing::Size(44, 13);
			this->lAzimuth->TabIndex = 3;
			this->lAzimuth->Text = L"Azimuth";
			// 
			// tbRateAzimuth
			// 
			this->tbRateAzimuth->Location = System::Drawing::Point(71, 45);
			this->tbRateAzimuth->Name = L"tbRateAzimuth";
			this->tbRateAzimuth->Size = System::Drawing::Size(61, 20);
			this->tbRateAzimuth->TabIndex = 2;
			this->toolTip1->SetToolTip(this->tbRateAzimuth, L"float GetRate_GRRAngularVelocities_X()");
			this->tbRateAzimuth->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// lElevation
			// 
			this->lElevation->AutoSize = true;
			this->lElevation->Location = System::Drawing::Point(14, 22);
			this->lElevation->Name = L"lElevation";
			this->lElevation->Size = System::Drawing::Size(51, 13);
			this->lElevation->TabIndex = 1;
			this->lElevation->Text = L"Elevation";
			// 
			// tbRateElevation
			// 
			this->tbRateElevation->Location = System::Drawing::Point(71, 19);
			this->tbRateElevation->Name = L"tbRateElevation";
			this->tbRateElevation->Size = System::Drawing::Size(61, 20);
			this->tbRateElevation->TabIndex = 0;
			this->toolTip1->SetToolTip(this->tbRateElevation, L"float GetRate_GRRAngularVelocities_Y()");
			this->tbRateElevation->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bAzimuthPlus
			// 
			this->bAzimuthPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bAzimuthPlus->Location = System::Drawing::Point(241, 29);
			this->bAzimuthPlus->Name = L"bAzimuthPlus";
			this->bAzimuthPlus->Size = System::Drawing::Size(84, 232);
			this->bAzimuthPlus->TabIndex = 4;
			this->bAzimuthPlus->Text = L"Azimuth +";
			this->toolTip1->SetToolTip(this->bAzimuthPlus, L"int SetRateCommandAngularVelocities_X(float AngularVelocities_X)");
			this->bAzimuthPlus->UseVisualStyleBackColor = false;
			this->bAzimuthPlus->Click += gcnew System::EventHandler(this, &MyForm::bAzimuthPlus_Click);
			this->bAzimuthPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bAzimuthPlus_MouseDown);
			this->bAzimuthPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bAzimuthPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bAzimuthPlus_MouseUp);
			// 
			// bAzimuthMinus
			// 
			this->bAzimuthMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bAzimuthMinus->Location = System::Drawing::Point(4, 29);
			this->bAzimuthMinus->Name = L"bAzimuthMinus";
			this->bAzimuthMinus->Size = System::Drawing::Size(84, 232);
			this->bAzimuthMinus->TabIndex = 3;
			this->bAzimuthMinus->Text = L"Azimuth -";
			this->toolTip1->SetToolTip(this->bAzimuthMinus, L"int SetRateCommandAngularVelocities_X(float AngularVelocities_X)");
			this->bAzimuthMinus->UseVisualStyleBackColor = false;
			this->bAzimuthMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bAzimuthMinus_MouseDown);
			this->bAzimuthMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bAzimuthMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bAzimuthMinus_MouseUp);
			// 
			// bElevationMinus
			// 
			this->bElevationMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bElevationMinus->Location = System::Drawing::Point(94, 227);
			this->bElevationMinus->Name = L"bElevationMinus";
			this->bElevationMinus->Size = System::Drawing::Size(141, 34);
			this->bElevationMinus->TabIndex = 1;
			this->bElevationMinus->Text = L"Elevation-";
			this->toolTip1->SetToolTip(this->bElevationMinus, L"int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y)");
			this->bElevationMinus->UseVisualStyleBackColor = false;
			this->bElevationMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bElevationMinus_MouseDown);
			this->bElevationMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bElevationMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bElevationMinus_MouseUp);
			// 
			// bElevationPlus
			// 
			this->bElevationPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bElevationPlus->Location = System::Drawing::Point(94, 29);
			this->bElevationPlus->Name = L"bElevationPlus";
			this->bElevationPlus->Size = System::Drawing::Size(141, 34);
			this->bElevationPlus->TabIndex = 0;
			this->bElevationPlus->Text = L"Elevation +";
			this->toolTip1->SetToolTip(this->bElevationPlus, L"int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");
			this->bElevationPlus->UseVisualStyleBackColor = false;
			this->bElevationPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bElevationPlus_MouseDown);
			this->bElevationPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bElevationPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bElevationPlus_MouseUp);
			// 
			// toolTip1
			// 
			this->toolTip1->AutoPopDelay = 5000;
			this->toolTip1->InitialDelay = 500;
			this->toolTip1->ReshowDelay = 1;
			// 
			// bThermalChannelZoomIn
			// 
			this->bThermalChannelZoomIn->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bThermalChannelZoomIn->Location = System::Drawing::Point(4, 28);
			this->bThermalChannelZoomIn->Name = L"bThermalChannelZoomIn";
			this->bThermalChannelZoomIn->Size = System::Drawing::Size(84, 41);
			this->bThermalChannelZoomIn->TabIndex = 8;
			this->bThermalChannelZoomIn->Text = L"Zoom In";
			this->toolTip1->SetToolTip(this->bThermalChannelZoomIn, L"int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
			this->bThermalChannelZoomIn->UseVisualStyleBackColor = false;
			this->bThermalChannelZoomIn->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bThermalChannelZoomIn_MouseDown);
			this->bThermalChannelZoomIn->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bThermalChannelZoomIn->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bThermalChannelZoomIn_MouseUp);
			// 
			// bThermalChannelZoomOut
			// 
			this->bThermalChannelZoomOut->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bThermalChannelZoomOut->Location = System::Drawing::Point(241, 28);
			this->bThermalChannelZoomOut->Name = L"bThermalChannelZoomOut";
			this->bThermalChannelZoomOut->Size = System::Drawing::Size(84, 41);
			this->bThermalChannelZoomOut->TabIndex = 9;
			this->bThermalChannelZoomOut->Text = L"Zoom Out";
			this->toolTip1->SetToolTip(this->bThermalChannelZoomOut, L"int SetThermalCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
			this->bThermalChannelZoomOut->UseVisualStyleBackColor = false;
			this->bThermalChannelZoomOut->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bThermalChannelZoomOut_MouseDown);
			this->bThermalChannelZoomOut->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bThermalChannelZoomOut->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bThermalChannelZoomOut_MouseUp);
			// 
			// tbThermalChannelFOV
			// 
			this->tbThermalChannelFOV->Location = System::Drawing::Point(166, 39);
			this->tbThermalChannelFOV->Name = L"tbThermalChannelFOV";
			this->tbThermalChannelFOV->Size = System::Drawing::Size(61, 20);
			this->tbThermalChannelFOV->TabIndex = 8;
			this->toolTip1->SetToolTip(this->tbThermalChannelFOV, L"float GetThermalCameraReportFOV()");
			this->tbThermalChannelFOV->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// tbActiveCamera
			// 
			this->tbActiveCamera->Location = System::Drawing::Point(166, 39);
			this->tbActiveCamera->Name = L"tbActiveCamera";
			this->tbActiveCamera->Size = System::Drawing::Size(61, 20);
			this->tbActiveCamera->TabIndex = 8;
			this->toolTip1->SetToolTip(this->tbActiveCamera, L"enum Camera GetVideoChannelReportPrimaryVideoChannel()");
			this->tbActiveCamera->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bToggleActiveCamera
			// 
			this->bToggleActiveCamera->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bToggleActiveCamera->Location = System::Drawing::Point(4, 28);
			this->bToggleActiveCamera->Name = L"bToggleActiveCamera";
			this->bToggleActiveCamera->Size = System::Drawing::Size(84, 41);
			this->bToggleActiveCamera->TabIndex = 8;
			this->bToggleActiveCamera->Text = L"Toggle Active Camera";
			this->toolTip1->SetToolTip(this->bToggleActiveCamera, L"int SetVideoChannelCommandPrimaryVideoChannel(enum Camera PrimaryVideoChannel)");
			this->bToggleActiveCamera->UseVisualStyleBackColor = false;
			this->bToggleActiveCamera->Click += gcnew System::EventHandler(this, &MyForm::bToggleActiveCamera_Click);
			this->bToggleActiveCamera->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bManualSyncLocal
			// 
			this->bManualSyncLocal->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bManualSyncLocal->Location = System::Drawing::Point(349, 12);
			this->bManualSyncLocal->Name = L"bManualSyncLocal";
			this->bManualSyncLocal->Size = System::Drawing::Size(331, 23);
			this->bManualSyncLocal->TabIndex = 9;
			this->bManualSyncLocal->Text = L"Manual Sync Local";
			this->toolTip1->SetToolTip(this->bManualSyncLocal, L"int STKSyncLocal(uint16_t timeout)");
			this->bManualSyncLocal->UseVisualStyleBackColor = false;
			this->bManualSyncLocal->Click += gcnew System::EventHandler(this, &MyForm::bManualSyncLocal_Click);
			this->bManualSyncLocal->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelZoomOut
			// 
			this->bDayLightChannelZoomOut->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelZoomOut->Location = System::Drawing::Point(241, 28);
			this->bDayLightChannelZoomOut->Name = L"bDayLightChannelZoomOut";
			this->bDayLightChannelZoomOut->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelZoomOut->TabIndex = 9;
			this->bDayLightChannelZoomOut->Text = L"Zoom Out";
			this->toolTip1->SetToolTip(this->bDayLightChannelZoomOut, L"int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
			this->bDayLightChannelZoomOut->UseVisualStyleBackColor = false;
			this->bDayLightChannelZoomOut->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelZoomOut_MouseDown);
			this->bDayLightChannelZoomOut->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelZoomOut->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelZoomOut_MouseUp);
			// 
			// tbDayLightChannelFOV
			// 
			this->tbDayLightChannelFOV->Location = System::Drawing::Point(166, 39);
			this->tbDayLightChannelFOV->Name = L"tbDayLightChannelFOV";
			this->tbDayLightChannelFOV->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelFOV->TabIndex = 8;
			this->toolTip1->SetToolTip(this->tbDayLightChannelFOV, L"float GetDayCameraReportFOV()");
			this->tbDayLightChannelFOV->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelZoomIn
			// 
			this->bDayLightChannelZoomIn->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelZoomIn->Location = System::Drawing::Point(4, 28);
			this->bDayLightChannelZoomIn->Name = L"bDayLightChannelZoomIn";
			this->bDayLightChannelZoomIn->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelZoomIn->TabIndex = 8;
			this->bDayLightChannelZoomIn->Text = L"Zoom In";
			this->toolTip1->SetToolTip(this->bDayLightChannelZoomIn, L"int SetDayCameraCommandZoomCommand(enum ZoomChange ZoomCommand)");
			this->bDayLightChannelZoomIn->UseVisualStyleBackColor = false;
			this->bDayLightChannelZoomIn->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelZoomIn_MouseDown);
			this->bDayLightChannelZoomIn->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelZoomIn->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelZoomIn_MouseUp);
			// 
			// bDayLightChannelGammaPlus
			// 
			this->bDayLightChannelGammaPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelGammaPlus->Location = System::Drawing::Point(241, 80);
			this->bDayLightChannelGammaPlus->Name = L"bDayLightChannelGammaPlus";
			this->bDayLightChannelGammaPlus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelGammaPlus->TabIndex = 13;
			this->bDayLightChannelGammaPlus->Text = L"Gamma +";
			this->toolTip1->SetToolTip(this->bDayLightChannelGammaPlus, L"int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");
			this->bDayLightChannelGammaPlus->UseVisualStyleBackColor = false;
			this->bDayLightChannelGammaPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelGammaPlus_MouseDown);
			this->bDayLightChannelGammaPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelGammaPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelGammaPlus_MouseUp);
			// 
			// tbDayLightChannelGamma
			// 
			this->tbDayLightChannelGamma->Location = System::Drawing::Point(166, 91);
			this->tbDayLightChannelGamma->Name = L"tbDayLightChannelGamma";
			this->tbDayLightChannelGamma->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelGamma->TabIndex = 10;
			this->toolTip1->SetToolTip(this->tbDayLightChannelGamma, L"float GetDayCameraReportGammaValue()");
			this->tbDayLightChannelGamma->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelGammaMinus
			// 
			this->bDayLightChannelGammaMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelGammaMinus->Location = System::Drawing::Point(4, 80);
			this->bDayLightChannelGammaMinus->Name = L"bDayLightChannelGammaMinus";
			this->bDayLightChannelGammaMinus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelGammaMinus->TabIndex = 11;
			this->bDayLightChannelGammaMinus->Text = L"Gamma -";
			this->toolTip1->SetToolTip(this->bDayLightChannelGammaMinus, L"int SetDayCameraCommandGammaCommand(enum GenericIncDecVal GammaCommand)");
			this->bDayLightChannelGammaMinus->UseVisualStyleBackColor = false;
			this->bDayLightChannelGammaMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelGammaMinus_MouseDown);
			this->bDayLightChannelGammaMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelGammaMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelGammaMinus_MouseUp);
			// 
			// bDayLightChannelSaturationPlus
			// 
			this->bDayLightChannelSaturationPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelSaturationPlus->Location = System::Drawing::Point(241, 132);
			this->bDayLightChannelSaturationPlus->Name = L"bDayLightChannelSaturationPlus";
			this->bDayLightChannelSaturationPlus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelSaturationPlus->TabIndex = 17;
			this->bDayLightChannelSaturationPlus->Text = L"Saturation +";
			this->toolTip1->SetToolTip(this->bDayLightChannelSaturationPlus, L"int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)"
				L"");
			this->bDayLightChannelSaturationPlus->UseVisualStyleBackColor = false;
			this->bDayLightChannelSaturationPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelSaturationPlus_MouseDown);
			this->bDayLightChannelSaturationPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelSaturationPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelSaturationPlus_MouseUp);
			// 
			// tbDayLightChannelSaturation
			// 
			this->tbDayLightChannelSaturation->Location = System::Drawing::Point(166, 143);
			this->tbDayLightChannelSaturation->Name = L"tbDayLightChannelSaturation";
			this->tbDayLightChannelSaturation->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelSaturation->TabIndex = 14;
			this->toolTip1->SetToolTip(this->tbDayLightChannelSaturation, L"float GetDayCameraReportSaturationValue()");
			this->tbDayLightChannelSaturation->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelSaturationMinus
			// 
			this->bDayLightChannelSaturationMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelSaturationMinus->Location = System::Drawing::Point(4, 132);
			this->bDayLightChannelSaturationMinus->Name = L"bDayLightChannelSaturationMinus";
			this->bDayLightChannelSaturationMinus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelSaturationMinus->TabIndex = 15;
			this->bDayLightChannelSaturationMinus->Text = L"Saturation -";
			this->toolTip1->SetToolTip(this->bDayLightChannelSaturationMinus, L"int SetDayCameraCommandSaturationCommand(enum GenericIncDecVal SaturationCommand)"
				L"");
			this->bDayLightChannelSaturationMinus->UseVisualStyleBackColor = false;
			this->bDayLightChannelSaturationMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelSaturationMinus_MouseDown);
			this->bDayLightChannelSaturationMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelSaturationMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelSaturationMinus_MouseUp);
			// 
			// bDayLightChannelContrastPlus
			// 
			this->bDayLightChannelContrastPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelContrastPlus->Location = System::Drawing::Point(241, 185);
			this->bDayLightChannelContrastPlus->Name = L"bDayLightChannelContrastPlus";
			this->bDayLightChannelContrastPlus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelContrastPlus->TabIndex = 21;
			this->bDayLightChannelContrastPlus->Text = L"Level +";
			this->toolTip1->SetToolTip(this->bDayLightChannelContrastPlus, L"int SetDayCameraCommandContrstCommand(enum GenericIncDecVal ContrstCommand)");
			this->bDayLightChannelContrastPlus->UseVisualStyleBackColor = false;
			this->bDayLightChannelContrastPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelContrastPlus_MouseDown);
			this->bDayLightChannelContrastPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelContrastPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelContrastPlus_MouseUp);
			// 
			// tbDayLightChannelContrast
			// 
			this->tbDayLightChannelContrast->Location = System::Drawing::Point(166, 196);
			this->tbDayLightChannelContrast->Name = L"tbDayLightChannelContrast";
			this->tbDayLightChannelContrast->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelContrast->TabIndex = 18;
			this->toolTip1->SetToolTip(this->tbDayLightChannelContrast, L"float GetDayCameraReportContrastValue()");
			this->tbDayLightChannelContrast->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelContrastMinus
			// 
			this->bDayLightChannelContrastMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelContrastMinus->Location = System::Drawing::Point(4, 185);
			this->bDayLightChannelContrastMinus->Name = L"bDayLightChannelContrastMinus";
			this->bDayLightChannelContrastMinus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelContrastMinus->TabIndex = 19;
			this->bDayLightChannelContrastMinus->Text = L"Level -";
			this->toolTip1->SetToolTip(this->bDayLightChannelContrastMinus, L"int SetDayCameraCommandContrstCommand(enum GenericIncDecVal ContrstCommand)");
			this->bDayLightChannelContrastMinus->UseVisualStyleBackColor = false;
			this->bDayLightChannelContrastMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelContrastMinus_MouseDown);
			this->bDayLightChannelContrastMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelContrastMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelContrastMinus_MouseUp);
			// 
			// bDayLightChannelbrightnessPlus
			// 
			this->bDayLightChannelbrightnessPlus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelbrightnessPlus->Location = System::Drawing::Point(241, 238);
			this->bDayLightChannelbrightnessPlus->Name = L"bDayLightChannelbrightnessPlus";
			this->bDayLightChannelbrightnessPlus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelbrightnessPlus->TabIndex = 25;
			this->bDayLightChannelbrightnessPlus->Text = L"Gain +";
			this->toolTip1->SetToolTip(this->bDayLightChannelbrightnessPlus, L"int SetDayCameraCommandBrightnessCommand(enum GenericIncDecVal BrightnessCommand)"
				L"");
			this->bDayLightChannelbrightnessPlus->UseVisualStyleBackColor = false;
			this->bDayLightChannelbrightnessPlus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelbrightnessPlus_MouseDown);
			this->bDayLightChannelbrightnessPlus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelbrightnessPlus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelbrightnessPlus_MouseUp);
			// 
			// tbDayLightChannelBrightness
			// 
			this->tbDayLightChannelBrightness->Location = System::Drawing::Point(166, 249);
			this->tbDayLightChannelBrightness->Name = L"tbDayLightChannelBrightness";
			this->tbDayLightChannelBrightness->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelBrightness->TabIndex = 22;
			this->toolTip1->SetToolTip(this->tbDayLightChannelBrightness, L"float GetDayCameraReportBrigthnessValue()");
			this->tbDayLightChannelBrightness->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelBrightnessMinus
			// 
			this->bDayLightChannelBrightnessMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelBrightnessMinus->Location = System::Drawing::Point(4, 238);
			this->bDayLightChannelBrightnessMinus->Name = L"bDayLightChannelBrightnessMinus";
			this->bDayLightChannelBrightnessMinus->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelBrightnessMinus->TabIndex = 23;
			this->bDayLightChannelBrightnessMinus->Text = L"Gain -";
			this->toolTip1->SetToolTip(this->bDayLightChannelBrightnessMinus, L"int SetDayCameraCommandBrightnessCommand(enum GenericIncDecVal BrightnessCommand)"
				L"");
			this->bDayLightChannelBrightnessMinus->UseVisualStyleBackColor = false;
			this->bDayLightChannelBrightnessMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelBrightnessMinus_MouseDown);
			this->bDayLightChannelBrightnessMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelBrightnessMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelBrightnessMinus_MouseUp);
			// 
			// bDayLightChannelAutoExposure
			// 
			this->bDayLightChannelAutoExposure->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelAutoExposure->Location = System::Drawing::Point(241, 291);
			this->bDayLightChannelAutoExposure->Name = L"bDayLightChannelAutoExposure";
			this->bDayLightChannelAutoExposure->Size = System::Drawing::Size(86, 41);
			this->bDayLightChannelAutoExposure->TabIndex = 29;
			this->bDayLightChannelAutoExposure->Text = L"Auto Exposure";
			this->toolTip1->SetToolTip(this->bDayLightChannelAutoExposure, L"int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");
			this->bDayLightChannelAutoExposure->UseVisualStyleBackColor = false;
			this->bDayLightChannelAutoExposure->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelAutoExposure_MouseClick);
			this->bDayLightChannelAutoExposure->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// tbDayLightChannelExposure
			// 
			this->tbDayLightChannelExposure->Location = System::Drawing::Point(168, 302);
			this->tbDayLightChannelExposure->Name = L"tbDayLightChannelExposure";
			this->tbDayLightChannelExposure->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelExposure->TabIndex = 26;
			this->toolTip1->SetToolTip(this->tbDayLightChannelExposure, L"enum DTVExposureMode GetDayCameraReportExposureMode()");
			this->tbDayLightChannelExposure->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelManualExposure
			// 
			this->bDayLightChannelManualExposure->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelManualExposure->Location = System::Drawing::Point(4, 291);
			this->bDayLightChannelManualExposure->Name = L"bDayLightChannelManualExposure";
			this->bDayLightChannelManualExposure->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelManualExposure->TabIndex = 27;
			this->bDayLightChannelManualExposure->Text = L"Auto Gain";
			this->toolTip1->SetToolTip(this->bDayLightChannelManualExposure, L"int SetDayCameraCommandExposureMode(enum DTVExposureModeSet ExposureMode)");
			this->bDayLightChannelManualExposure->UseVisualStyleBackColor = false;
			this->bDayLightChannelManualExposure->Click += gcnew System::EventHandler(this, &MyForm::bDayLightChannelManualExposure_Click);
			this->bDayLightChannelManualExposure->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelExpTimePluse
			// 
			this->bDayLightChannelExpTimePluse->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelExpTimePluse->Location = System::Drawing::Point(241, 344);
			this->bDayLightChannelExpTimePluse->Name = L"bDayLightChannelExpTimePluse";
			this->bDayLightChannelExpTimePluse->Size = System::Drawing::Size(84, 41);
			this->bDayLightChannelExpTimePluse->TabIndex = 33;
			this->bDayLightChannelExpTimePluse->Text = L"Exp Time +";
			this->toolTip1->SetToolTip(this->bDayLightChannelExpTimePluse, L"int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");
			this->bDayLightChannelExpTimePluse->UseVisualStyleBackColor = false;
			this->bDayLightChannelExpTimePluse->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelExpTimePluse_MouseDown);
			this->bDayLightChannelExpTimePluse->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelExpTimePluse->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelExpTimePluse_MouseUp);
			// 
			// tbDayLightChannelExpTime
			// 
			this->tbDayLightChannelExpTime->Location = System::Drawing::Point(168, 355);
			this->tbDayLightChannelExpTime->Name = L"tbDayLightChannelExpTime";
			this->tbDayLightChannelExpTime->Size = System::Drawing::Size(61, 20);
			this->tbDayLightChannelExpTime->TabIndex = 30;
			this->toolTip1->SetToolTip(this->tbDayLightChannelExpTime, L"uint32_t GetDayCameraReportExposureTime()");
			this->tbDayLightChannelExpTime->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// bDayLightChannelExpTimeMinus
			// 
			this->bDayLightChannelExpTimeMinus->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->bDayLightChannelExpTimeMinus->Location = System::Drawing::Point(4, 344);
			this->bDayLightChannelExpTimeMinus->Name = L"bDayLightChannelExpTimeMinus";
			this->bDayLightChannelExpTimeMinus->Size = System::Drawing::Size(86, 41);
			this->bDayLightChannelExpTimeMinus->TabIndex = 31;
			this->bDayLightChannelExpTimeMinus->Text = L"Exp Time -";
			this->toolTip1->SetToolTip(this->bDayLightChannelExpTimeMinus, L"int SetDayCameraCommandExposureTime(enum GenericIncDecVal ExposureTime)");
			this->bDayLightChannelExpTimeMinus->UseVisualStyleBackColor = false;
			this->bDayLightChannelExpTimeMinus->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelExpTimeMinus_MouseDown);
			this->bDayLightChannelExpTimeMinus->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			this->bDayLightChannelExpTimeMinus->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::bDayLightChannelExpTimeMinus_MouseUp);
			// 
			// btTrack
			// 
			this->btTrack->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->btTrack->Location = System::Drawing::Point(6, 65);
			this->btTrack->Name = L"btTrack";
			this->btTrack->Size = System::Drawing::Size(84, 40);
			this->btTrack->TabIndex = 9;
			this->btTrack->Text = L"Track";
			this->toolTip1->SetToolTip(this->btTrack, L"int SetTrackTargetXCoordinate(int16_t TargetXCoordinate); int SetTrackTargetYCoor"
				L"dinate(int16_t TargetYCoordinate)");
			this->btTrack->UseVisualStyleBackColor = false;
			this->btTrack->Click += gcnew System::EventHandler(this, &MyForm::btTrack_Click);
			this->btTrack->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// tbOperationMode
			// 
			this->tbOperationMode->Location = System::Drawing::Point(198, 23);
			this->tbOperationMode->Name = L"tbOperationMode";
			this->tbOperationMode->Size = System::Drawing::Size(76, 20);
			this->tbOperationMode->TabIndex = 8;
			this->toolTip1->SetToolTip(this->tbOperationMode, L"enum TOperationMode_Micro GetGimbalOperationReportOperationMode()");
			this->tbOperationMode->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// btCEnterMode
			// 
			this->btCEnterMode->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->btCEnterMode->Location = System::Drawing::Point(4, 19);
			this->btCEnterMode->Name = L"btCEnterMode";
			this->btCEnterMode->Size = System::Drawing::Size(84, 40);
			this->btCEnterMode->TabIndex = 8;
			this->btCEnterMode->Text = L"Center Mode";
			this->toolTip1->SetToolTip(this->btCEnterMode, L"int SetCenterMode()");
			this->btCEnterMode->UseVisualStyleBackColor = false;
			this->btCEnterMode->Click += gcnew System::EventHandler(this, &MyForm::btCEnterMode_Click);
			this->btCEnterMode->MouseHover += gcnew System::EventHandler(this, &MyForm::OnButtonMouseHover);
			// 
			// btTrackX
			// 
			this->btTrackX->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->btTrackX->Location = System::Drawing::Point(182, 65);
			this->btTrackX->Name = L"btTrackX";
			this->btTrackX->Size = System::Drawing::Size(61, 20);
			this->btTrackX->TabIndex = 10;
			this->btTrackX->Text = L"Track X";
			this->toolTip1->SetToolTip(this->btTrackX, L"int SetTrackTargetXCoordinate(int16_t TargetXCoordinate); int SetTrackTargetYCoor"
				L"dinate(int16_t TargetYCoordinate)");
			this->btTrackX->UseVisualStyleBackColor = false;
			this->btTrackX->Click += gcnew System::EventHandler(this, &MyForm::btTrackX_Click);
			// 
			// gbThermalChannel
			// 
			this->gbThermalChannel->BackColor = System::Drawing::SystemColors::ControlLight;
			this->gbThermalChannel->Controls->Add(this->lThermalChannelFOV);
			this->gbThermalChannel->Controls->Add(this->bThermalChannelZoomOut);
			this->gbThermalChannel->Controls->Add(this->tbThermalChannelFOV);
			this->gbThermalChannel->Controls->Add(this->bThermalChannelZoomIn);
			this->gbThermalChannel->Location = System::Drawing::Point(349, 437);
			this->gbThermalChannel->Name = L"gbThermalChannel";
			this->gbThermalChannel->Size = System::Drawing::Size(331, 84);
			this->gbThermalChannel->TabIndex = 7;
			this->gbThermalChannel->TabStop = false;
			this->gbThermalChannel->Text = L"Thermal Channel";
			// 
			// lThermalChannelFOV
			// 
			this->lThermalChannelFOV->AutoSize = true;
			this->lThermalChannelFOV->Location = System::Drawing::Point(109, 42);
			this->lThermalChannelFOV->Name = L"lThermalChannelFOV";
			this->lThermalChannelFOV->Size = System::Drawing::Size(28, 13);
			this->lThermalChannelFOV->TabIndex = 9;
			this->lThermalChannelFOV->Text = L"FOV";
			// 
			// gbVideoControlCommands
			// 
			this->gbVideoControlCommands->BackColor = System::Drawing::SystemColors::ControlLight;
			this->gbVideoControlCommands->Controls->Add(this->lActiveCamera);
			this->gbVideoControlCommands->Controls->Add(this->tbActiveCamera);
			this->gbVideoControlCommands->Controls->Add(this->bToggleActiveCamera);
			this->gbVideoControlCommands->Location = System::Drawing::Point(12, 437);
			this->gbVideoControlCommands->Name = L"gbVideoControlCommands";
			this->gbVideoControlCommands->Size = System::Drawing::Size(331, 84);
			this->gbVideoControlCommands->TabIndex = 8;
			this->gbVideoControlCommands->TabStop = false;
			this->gbVideoControlCommands->Text = L"Video Control Commands";
			// 
			// lActiveCamera
			// 
			this->lActiveCamera->AutoSize = true;
			this->lActiveCamera->Location = System::Drawing::Point(108, 42);
			this->lActiveCamera->Name = L"lActiveCamera";
			this->lActiveCamera->Size = System::Drawing::Size(43, 13);
			this->lActiveCamera->TabIndex = 9;
			this->lActiveCamera->Text = L"Camera";
			// 
			// gbDayLightChannel
			// 
			this->gbDayLightChannel->BackColor = System::Drawing::SystemColors::ControlLight;
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelExpTime);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelExpTimePluse);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelExpTime);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelExpTimeMinus);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelExposure);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelAutoExposure);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelExposure);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelManualExposure);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelBrightness);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelbrightnessPlus);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelBrightness);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelBrightnessMinus);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelContrast);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelContrastPlus);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelContrast);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelContrastMinus);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelSaturation);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelSaturationPlus);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelSaturation);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelSaturationMinus);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelGamma);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelGammaPlus);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelGamma);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelGammaMinus);
			this->gbDayLightChannel->Controls->Add(this->lDayLightChannelFOV);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelZoomOut);
			this->gbDayLightChannel->Controls->Add(this->tbDayLightChannelFOV);
			this->gbDayLightChannel->Controls->Add(this->bDayLightChannelZoomIn);
			this->gbDayLightChannel->Location = System::Drawing::Point(349, 41);
			this->gbDayLightChannel->Name = L"gbDayLightChannel";
			this->gbDayLightChannel->Size = System::Drawing::Size(331, 390);
			this->gbDayLightChannel->TabIndex = 10;
			this->gbDayLightChannel->TabStop = false;
			this->gbDayLightChannel->Text = L"Day Light Channel";
			// 
			// lDayLightChannelExpTime
			// 
			this->lDayLightChannelExpTime->AutoSize = true;
			this->lDayLightChannelExpTime->Location = System::Drawing::Point(101, 358);
			this->lDayLightChannelExpTime->Name = L"lDayLightChannelExpTime";
			this->lDayLightChannelExpTime->Size = System::Drawing::Size(51, 13);
			this->lDayLightChannelExpTime->TabIndex = 32;
			this->lDayLightChannelExpTime->Text = L"Exp Time";
			// 
			// lDayLightChannelExposure
			// 
			this->lDayLightChannelExposure->AutoSize = true;
			this->lDayLightChannelExposure->Location = System::Drawing::Point(101, 305);
			this->lDayLightChannelExposure->Name = L"lDayLightChannelExposure";
			this->lDayLightChannelExposure->Size = System::Drawing::Size(51, 13);
			this->lDayLightChannelExposure->TabIndex = 28;
			this->lDayLightChannelExposure->Text = L"Exposure";
			// 
			// lDayLightChannelBrightness
			// 
			this->lDayLightChannelBrightness->AutoSize = true;
			this->lDayLightChannelBrightness->Location = System::Drawing::Point(99, 252);
			this->lDayLightChannelBrightness->Name = L"lDayLightChannelBrightness";
			this->lDayLightChannelBrightness->Size = System::Drawing::Size(29, 13);
			this->lDayLightChannelBrightness->TabIndex = 24;
			this->lDayLightChannelBrightness->Text = L"Gain";
			// 
			// lDayLightChannelContrast
			// 
			this->lDayLightChannelContrast->AutoSize = true;
			this->lDayLightChannelContrast->Location = System::Drawing::Point(99, 199);
			this->lDayLightChannelContrast->Name = L"lDayLightChannelContrast";
			this->lDayLightChannelContrast->Size = System::Drawing::Size(33, 13);
			this->lDayLightChannelContrast->TabIndex = 20;
			this->lDayLightChannelContrast->Text = L"Level";
			// 
			// lDayLightChannelSaturation
			// 
			this->lDayLightChannelSaturation->AutoSize = true;
			this->lDayLightChannelSaturation->Location = System::Drawing::Point(99, 146);
			this->lDayLightChannelSaturation->Name = L"lDayLightChannelSaturation";
			this->lDayLightChannelSaturation->Size = System::Drawing::Size(55, 13);
			this->lDayLightChannelSaturation->TabIndex = 16;
			this->lDayLightChannelSaturation->Text = L"Saturation";
			// 
			// lDayLightChannelGamma
			// 
			this->lDayLightChannelGamma->AutoSize = true;
			this->lDayLightChannelGamma->Location = System::Drawing::Point(99, 94);
			this->lDayLightChannelGamma->Name = L"lDayLightChannelGamma";
			this->lDayLightChannelGamma->Size = System::Drawing::Size(43, 13);
			this->lDayLightChannelGamma->TabIndex = 12;
			this->lDayLightChannelGamma->Text = L"Gamma";
			// 
			// lDayLightChannelFOV
			// 
			this->lDayLightChannelFOV->AutoSize = true;
			this->lDayLightChannelFOV->Location = System::Drawing::Point(99, 42);
			this->lDayLightChannelFOV->Name = L"lDayLightChannelFOV";
			this->lDayLightChannelFOV->Size = System::Drawing::Size(28, 13);
			this->lDayLightChannelFOV->TabIndex = 9;
			this->lDayLightChannelFOV->Text = L"FOV";
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 200;
			this->timer1->Tick += gcnew System::EventHandler(this, &MyForm::timer1_Tick);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(15, 524);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(46, 13);
			this->label4->TabIndex = 12;
			this->label4->Text = L"ToolTip:";
			// 
			// groupBox1
			// 
			this->groupBox1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->groupBox1->Controls->Add(this->textBox2);
			this->groupBox1->Controls->Add(this->btTrackY);
			this->groupBox1->Controls->Add(this->textBox1);
			this->groupBox1->Controls->Add(this->btTrackX);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->btTrack);
			this->groupBox1->Controls->Add(this->tbOperationMode);
			this->groupBox1->Controls->Add(this->btCEnterMode);
			this->groupBox1->Location = System::Drawing::Point(12, 320);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(331, 111);
			this->groupBox1->TabIndex = 10;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Operation Mode";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(111, 65);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(65, 20);
			this->textBox1->TabIndex = 11;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(109, 26);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(83, 13);
			this->label5->TabIndex = 9;
			this->label5->Text = L"Operation Mode";
			// 
			// tbToolTip
			// 
			this->tbToolTip->Font = (gcnew System::Drawing::Font(L"Book Antiqua", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tbToolTip->Location = System::Drawing::Point(12, 540);
			this->tbToolTip->Multiline = true;
			this->tbToolTip->Name = L"tbToolTip";
			this->tbToolTip->Size = System::Drawing::Size(668, 40);
			this->tbToolTip->TabIndex = 13;
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(111, 86);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(65, 20);
			this->textBox2->TabIndex = 13;
			// 
			// btTrackY
			// 
			this->btTrackY->BackColor = System::Drawing::SystemColors::InactiveCaption;
			this->btTrackY->Location = System::Drawing::Point(182, 86);
			this->btTrackY->Name = L"btTrackY";
			this->btTrackY->Size = System::Drawing::Size(61, 20);
			this->btTrackY->TabIndex = 12;
			this->btTrackY->Text = L"Track Y";
			this->toolTip1->SetToolTip(this->btTrackY, L"int SetTrackTargetXCoordinate(int16_t TargetXCoordinate); int SetTrackTargetYCoor"
				L"dinate(int16_t TargetYCoordinate)");
			this->btTrackY->UseVisualStyleBackColor = false;
			this->btTrackY->Click += gcnew System::EventHandler(this, &MyForm::btTrackY_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(694, 583);
			this->Controls->Add(this->tbToolTip);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->gbDayLightChannel);
			this->Controls->Add(this->bManualSyncLocal);
			this->Controls->Add(this->gbVideoControlCommands);
			this->Controls->Add(this->gbThermalChannel);
			this->Controls->Add(this->gbGimbalMovment);
			this->Controls->Add(this->bInitCommunication);
			this->Name = L"MyForm";
			this->Text = L"STK Demo App";
			this->toolTip1->SetToolTip(this, L"int SetRateCommandAngularVelocities_Y(float AngularVelocities_Y);");
			this->gbGimbalMovment->ResumeLayout(false);
			this->gbPosition->ResumeLayout(false);
			this->gbPosition->PerformLayout();
			this->gbRate->ResumeLayout(false);
			this->gbRate->PerformLayout();
			this->gbThermalChannel->ResumeLayout(false);
			this->gbThermalChannel->PerformLayout();
			this->gbVideoControlCommands->ResumeLayout(false);
			this->gbVideoControlCommands->PerformLayout();
			this->gbDayLightChannel->ResumeLayout(false);
			this->gbDayLightChannel->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void bInitCommunication_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		MessagesTransmitEnableStruct MessagesTransmitInitialize;
		MessagesTransmitInitialize.EnableTXATRControls = true;
		MessagesTransmitInitialize.EnableTXCameraPowerControl = true;
		MessagesTransmitInitialize.EnableTXCenterMode = true;
		MessagesTransmitInitialize.EnableTXClearStickyBIT = true;
		MessagesTransmitInitialize.EnableTXDayCameraCommand = true;
		MessagesTransmitInitialize.EnableTXDayCameraCommand = true;
		MessagesTransmitInitialize.EnableTXDriftCalibration = true;
		MessagesTransmitInitialize.EnableTXGroundReferenceCommand = true;
		MessagesTransmitInitialize.EnableTXIDLE = true;
		MessagesTransmitInitialize.EnableTXKeepAlive = true;
		MessagesTransmitInitialize.EnableTXLogRecordCommand = true;
		MessagesTransmitInitialize.EnableTXPositionCommand = true;
		MessagesTransmitInitialize.EnableTXRateCommand = true;
		MessagesTransmitInitialize.EnableTXRecordVideoCommand = true;
		MessagesTransmitInitialize.EnableTXSafeMode = true;
		MessagesTransmitInitialize.EnableTXSetSystemClock = true;
		MessagesTransmitInitialize.EnableTXSystemInit = true;
		MessagesTransmitInitialize.EnableTXThermalCameraCommand = true;
		MessagesTransmitInitialize.EnableTXTrack = true;
		MessagesTransmitInitialize.EnableTXTrackerAdjustControls = true;
		MessagesTransmitInitialize.EnableTXTrackerParameters = true;
		MessagesTransmitInitialize.EnableTXVideoChannelCommand = true;
		MessagesTransmitInitialize.EnableTXVMDControls = true;


		msgBoxForm msgForm;
		int res = STKInitializeCommunication(MessagesTransmitInitialize);
		if (res == 0)
		{
			bInitCommunication->BackColor = Color::LightSkyBlue;
			isCommunicationInit = 1;
		}
		else
		{
			// Initialize Winsock was not successful.
			// OR Create a UDP socket was not successful.
			// OR Bind the socket to a port was not successful.
			msgForm.change_Lable_text_to_error();
			msgForm.ShowDialog();
		}
	}

	private: System::Void bManualSyncLocal_Click(System::Object^ sender, System::EventArgs^ e)
	{
		// Update all text boxes.
		int temp = STKSyncLocal(10);
		if (0 != temp)
		{
			MessageBox::Show(temp.ToString());
		}
		tbDayLightChannelFOV->Text = GetDayCameraReportFOV().ToString();
		tbDayLightChannelGamma->Text = GetDayCameraReportGammaValue().ToString();
		tbDayLightChannelSaturation->Text = GetDayCameraReportSaturationValue().ToString();
		tbDayLightChannelContrast->Text = GetDayCameraReportContrastValue().ToString();
		tbDayLightChannelBrightness->Text = GetDayCameraReportBrigthnessValue().ToString();
		if (GetDayCameraReportExposureMode() == DTVExposureMode::AutomaticExposureMode)
		{
			tbDayLightChannelExposure->Text = "Auto";
		}
		if (GetDayCameraReportExposureMode() == DTVExposureMode::ManualExposureMode)
		{
			tbDayLightChannelExposure->Text = "Manual";
		}
		tbDayLightChannelExpTime->Text = GetDayCameraReportExposureTime().ToString();		
		tbThermalChannelFOV->Text = GetThermalCameraReportFOV().ToString();
		if (GetVideoChannelReportPrimaryVideoChannel() == Camera::DTV)
		{
			tbThermalChannelFOV->Text = "DTV";
		}
		if (GetVideoChannelReportPrimaryVideoChannel() == Camera::TI)
		{
			tbThermalChannelFOV->Text = "TI";
		}
		tbPositionElevation->Text = GetPositionReportRelativeGimbalAngles_Y().ToString();
		tbPositionAzimuth->Text = GetPositionReportRelativeGimbalAngles_X().ToString();
		tbRateElevation->Text = GetRate_GRRAngularVelocities_Y().ToString();
		tbRateAzimuth->Text = GetRate_GRRAngularVelocities_X().ToString();
	}

	private: System::Void bDayLightChannelZoomIn_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{	
		SetDayCameraCommandZoomCommand(ZoomChange::Zoomin);
	}
	private: System::Void bDayLightChannelZoomIn_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{	
		SetDayCameraCommandZoomCommand(ZoomChange::Nochange);
	}
	private: System::Void bDayLightChannelZoomOut_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{	
		SetDayCameraCommandZoomCommand(ZoomChange::Zoomout);
	}
	private: System::Void bDayLightChannelZoomOut_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandZoomCommand(ZoomChange::Nochange);
	}
	private: System::Void bDayLightChannelGammaMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGammaCommand(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelGammaMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGammaCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelGammaPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGammaCommand(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelGammaPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGammaCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelSaturationMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandSaturationCommand(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelSaturationMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandSaturationCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelSaturationPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandSaturationCommand(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelSaturationPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandSaturationCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelContrastMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandLevelCommand(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelContrastMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandLevelCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelContrastPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandLevelCommand(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelContrastPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandLevelCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelBrightnessMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandGainCommand(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelBrightnessMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGainCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelbrightnessPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGainCommand(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelbrightnessPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandGainCommand(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelManualExposure_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SetDayCameraCommandExposureMode(DTVExposureModeSet::SetManualExposureMode);
	}
	private: System::Void bDayLightChannelAutoExposure_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureMode(DTVExposureModeSet::SetAutomaticExposureMode);
	}
	private: System::Void bDayLightChannelExpTimeMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureTime(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelExpTimeMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandExposureTime(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelExpTimePluse_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureTime(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelExpTimePluse_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureTime(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelExpGainMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureGain(GenericIncDecVal::DecreaseValue);
	}
	private: System::Void bDayLightChannelExpGainMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureGain(GenericIncDecVal::NoChange);
	}
	private: System::Void bDayLightChannelExpGainPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetDayCameraCommandExposureGain(GenericIncDecVal::IncreaseValue);
	}
	private: System::Void bDayLightChannelExpGainPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetDayCameraCommandExposureGain(GenericIncDecVal::NoChange);
	}
	private: System::Void bThermalChannelZoomIn_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetThermalCameraCommandZoomCommand(ZoomChange::Zoomin);
	}
	private: System::Void bThermalChannelZoomIn_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetThermalCameraCommandZoomCommand(ZoomChange::Nochange);
	}
	private: System::Void bThermalChannelZoomOut_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) 
	{
		SetThermalCameraCommandZoomCommand(ZoomChange::Zoomout);
	}
	private: System::Void bThermalChannelZoomOut_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetThermalCameraCommandZoomCommand(ZoomChange::Nochange);
	}
	private: System::Void bToggleActiveCamera_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		// Enum parameter.
		if (Camera::DTV == GetVideoChannelReportPrimaryVideoChannel())
			SetVideoChannelCommandPrimaryVideoChannel(Camera::TI);
		else
			SetVideoChannelCommandPrimaryVideoChannel(Camera::DTV);
	}
	private: System::Void bElevationPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_Y(5);
	}
	private: System::Void bElevationPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_Y(0);
	}
	private: System::Void bElevationMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_Y(-5);
	}
	private: System::Void bElevationMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_Y(0);
	}
	private: System::Void bAzimuthPlus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_X(5);
	}
	private: System::Void bAzimuthPlus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_X(0);
	}
	private: System::Void bAzimuthMinus_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_X(-5);
	}
	private: System::Void bAzimuthMinus_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SetRateCommandAngularVelocities_X(0);
	}
	private: System::Void btCEnterMode_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		SetCenterMode();
	}
	private: System::Void btTrack_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		SetTrack(0, 0);
	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) 
	{
		if (delayCounter > 400000)
		{
			if (isConnectedToHive == 1)										// Connection successful - popup a message.			
			{
				isConnectedToHive = 2;										// Step here only once.
				isNotConnectedToHive = 2;									// Don't Step in second if.
				isUnSuccessCommunication = 101;

				// Popup the message (popup a new form).
				msgBoxForm msgForm1;
				msgForm1.change_Lable_text_to_connect();					// Change the text in the form.
				msgForm1.ShowDialog();
			}
			if (isNotConnectedToHive == 1)									// Connection unsuccessful - popup a message.	
			{
				isNotConnectedToHive = 2;									// Step here only once.
				isConnectedToHive = 2;										// Don't Step in first if.

				// Popup the message (popup a new form).
				msgBoxForm msgForm2;
				msgForm2.change_Lable_text_to_error();						// Change the text in the form.
				msgForm2.ShowDialog();
			}
		}

		//System::Diagnostics::Debug::WriteLine(tbDayLightChannelFOV->Text);

		// Update all text boxes every 200ms.
		//
		// STKSyncLocal() is called in 'system_structs_update' thread (My.Form.cpp).
		// STKSyncLocal() update the structs that are defined in 'stkAPI.c' and this
		// timer update the text boxes.
		tbDayLightChannelFOV->Text = GetDayCameraReportFOV().ToString();
		tbDayLightChannelGamma->Text = GetDayCameraReportGammaValue().ToString();
		tbDayLightChannelSaturation->Text = GetDayCameraReportSaturationValue().ToString();
		tbDayLightChannelContrast->Text = GetDayCameraReportBrigthnessValue().ToString();
		tbDayLightChannelBrightness->Text = GetDayCameraReportContrastValue().ToString();
		if (GetDayCameraReportExposureMode() == DTVExposureMode::AutomaticExposureMode)
		{
			tbDayLightChannelExposure->Text = "Auto";
		}
		if (GetDayCameraReportExposureMode() == DTVExposureMode::ManualExposureMode)
		{
			tbDayLightChannelExposure->Text = "Manual";
		}
		tbDayLightChannelExpTime->Text = GetDayCameraReportExposureTime().ToString();
	
		tbThermalChannelFOV->Text = GetThermalCameraReportFOV().ToString();
		if (GetVideoChannelReportPrimaryVideoChannel() == Camera::DTV)
		{
			tbActiveCamera->Text = "DTV";
		}
		if (GetVideoChannelReportPrimaryVideoChannel() == Camera::TI)
		{
			tbActiveCamera->Text = "TI";
		}
		tbPositionElevation->Text = GetPositionReportRelativeGimbalAngles_Y().ToString();
		tbPositionAzimuth->Text = GetPositionReportRelativeGimbalAngles_X().ToString();
		tbRateElevation->Text = GetRate_GRRAngularVelocities_Y().ToString();
		tbRateAzimuth->Text = GetRate_GRRAngularVelocities_X().ToString();
		tbOperationMode->Text = int(GetSystemGeneralReportOperationMode()).ToString();
	}


	   void OnButtonMouseHover(System::Object^ sender, System::EventArgs^ e) {
		   Button^ button = dynamic_cast<Button^>(sender);
		   TextBox^ textBox = dynamic_cast<TextBox^>(sender);
		   if (button != nullptr) {
			   // Assuming each button has a tooltip set
			   ToolTip^ tooltip = gcnew ToolTip();
			   String^ tooltipText = toolTip1->GetToolTip(button);  //tooltip->GetToolTip(button);
			   tbToolTip->Text = tooltipText;
		   }
		   else if (textBox != nullptr) {
			   // Assuming each button has a tooltip set
			   ToolTip^ tooltip = gcnew ToolTip();
			   String^ tooltipText = toolTip1->GetToolTip(textBox);  //tooltip->GetToolTip(button);
			   tbToolTip->Text = tooltipText;
		   }
	   }


private: System::Void bAzimuthPlus_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void btTrackX_Click(System::Object^ sender, System::EventArgs^ e) {
	SetTrackerParametersGateXSize(System::Convert::ToInt16(textBox1->Text));
	SetTrackerParametersGateYSize(System::Convert::ToInt16(textBox2->Text));
}
private: System::Void btTrackY_Click(System::Object^ sender, System::EventArgs^ e) {
	
}
};
}

