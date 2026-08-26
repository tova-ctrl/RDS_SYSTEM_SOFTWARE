#pragma once

#include "Globals.h"

namespace STK300cpp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for msgBoxForm
	/// </summary>
	public ref class msgBoxForm : public System::Windows::Forms::Form
	{
	public:
		msgBoxForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			STK300cpp::msgBoxForm::ControlBox = false;
		}

		void change_Lable_text_to_error()
		{
			this->label1->Text = "Error During Hive Connection!";
		}
		void change_Lable_text_to_connect()
		{
			this->label1->Text = "Hive is connected!";
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~msgBoxForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ bMsgBoxFormOKButton;
	private: System::Windows::Forms::Label^ label1;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->bMsgBoxFormOKButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// bMsgBoxFormOKButton
			// 
			this->bMsgBoxFormOKButton->Location = System::Drawing::Point(80, 45);
			this->bMsgBoxFormOKButton->Name = L"bMsgBoxFormOKButton";
			this->bMsgBoxFormOKButton->Size = System::Drawing::Size(102, 23);
			this->bMsgBoxFormOKButton->TabIndex = 0;
			this->bMsgBoxFormOKButton->Text = L"OK";
			this->bMsgBoxFormOKButton->UseVisualStyleBackColor = true;
			this->bMsgBoxFormOKButton->Click += gcnew System::EventHandler(this, &msgBoxForm::bMsgBoxFormOKButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(177)));
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(154, 18);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Hive is Connected !";
			// 
			// msgBoxForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(256, 80);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->bMsgBoxFormOKButton);
			this->Name = L"msgBoxForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Hive connection";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void bMsgBoxFormOKButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			STK300cpp::msgBoxForm::Close();
		}
	};
}
