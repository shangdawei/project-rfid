/***********************************************************************
 MainForm.cpp - Defines the dialog box behavior for the MySQL++ C++/CLI
	Windows Forms example.

 Copyright (c) 2007 by Educational Technology Resources, Inc.  Others 
 may also hold copyrights on code in this file.  See the CREDITS.txt
 file in the top directory of the distribution for details.

 This file is part of MySQL++.

 MySQL++ is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 MySQL++ is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with MySQL++; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#pragma once

#include <mysql++.h>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>


namespace wforms {

	using namespace Microsoft::Win32;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;
	using namespace std;
	using namespace boost::algorithm;

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm()
		{
			InitializeComponent();
			LoadDefaults();

			findPorts();
		}

	protected:
		~MainForm()
		{
			if (components) {
				delete components;
			}
		}

	/* private: System::Void Data_Received(System::Object ^ sender, System::IO::Ports::SerialDataReceivedEventArgs ^e){
        String ^text = this->_serialPort->ReadLine();
        this->RFID->Text = text;
       }*/
	
	

	private:
	System::Void DataReceivedHandler(System::Object^ sender,SerialDataReceivedEventArgs^ e)
    {
		this->lblPortMsg->Text = "";

	if (RFID->InvokeRequired){
		RFID->Invoke(
			gcnew System::IO::Ports::SerialDataReceivedEventHandler(this,&wforms::MainForm::DataReceivedHandler),
			gcnew array<System::Object^> { sender, e }
		);
		
	}
	else {
		try {
			System::String^ indata = this->_serialPort->ReadLine();
			this->RFID->Text = indata;
		}
		catch (TimeoutException ^) {
			this->lblPortMsg->Text = "TIMEOUT";
		}

	} 
}
	// Insert a text string into the output list control
		Void AddMessage(String^ msg)
		{
			resultsList_->Items->Add(msg);
		}

		Void ProcessAccess(String^ access){
			if(access == "Access Granted"){
				this->_serialPort->WriteLine("1");
			}
			else {
				// DO NOTIFY ADMIN
				this->_serialPort->WriteLine("0");
			}
		}

		// Handle Close button click by shutting down application
		Void CloseButton_Click(Object^ sender, EventArgs^ e)
		{
			Application::Exit();
		}
		
		
		private: System::Void RFID_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			// Clear out the results list, in case this isn't the first time
			// we've come in here.
			resultsList_->Items->Clear();
			lblDBConnection->Text = "";
			mysqlpp::Connection con(false);
			const int kInputBufSize = 100;
			char acServerAddress[kInputBufSize];
			char acUserName[kInputBufSize];
			char acPassword[kInputBufSize];
			char tmpRFID[kInputBufSize];
			ToUTF8(acServerAddress, kInputBufSize, serverAddress_->Text);
			ToUTF8(acUserName, kInputBufSize, userName_->Text);
			ToUTF8(acPassword, kInputBufSize, password_->Text);
			ToUTF8(tmpRFID, kInputBufSize, RFID->Text);
			if (!con.connect("test", acServerAddress, acUserName, acPassword)) {
				lblDBConnection->Text = gcnew String(con.error());
				return;
			}

			if(con.connected()){				
				lblDBConnection->Text = "Connected";

				//char buff[100];

				string str1(tmpRFID);
				trim(str1);

				//sprintf(buff, "SELECT * FROM login WHERE cardnum = '%s';", str1);
				//std::string buffAsStdStr = buff;
				//String^ temp = gcnew String(buffAsStdStr.c_str());
				 
				 
				//textBox1->Text = temp;
				//const char* myQuery = buffAsStdStr.c_str();

				mysqlpp::Query query = con.query("SELECT * FROM login WHERE cardnum = %0q");				
				query.parse();

				//mysqlpp::SQLQueryParms sqp;
				//sqp << tmpRFID;
			
				if (mysqlpp::StoreQueryResult res = query.store(str1)) {					
					for (size_t i = 0; i < res.num_rows(); ++i) {
							AddMessage("Firstname: " + ToUCS2(res[i]["firstname"]));
							AddMessage("Lastname: " + ToUCS2(res[i]["lastname"]));
							AddMessage("Last Access: " + ToUCS2(res[i]["lastlogin"]));
							String^ isBanned = ToUCS2(res[i]["banned"]);
							if(isBanned == "1"){
								AddMessage("Banned : Yes");

								ProcessAccess("Access Denied");
							}
							else {
								AddMessage("Banned : No");
								ProcessAccess("Access Granted");
							}


					}
					SaveInputs();
				}
				else {
					// Retreive failed
					AddMessage("Failed to get item list:");
					AddMessage(ToUCS2(query.error()));
				}

			} else lblDBConnection->Text = "Not Connected";
			
		 }


		// Load the default input field values, if there are any
		Void LoadDefaults()
		{
			RegistryKey^ settings = OpenSettingsRegistryKey();
			if (settings) {
				userName_->Text = LoadSetting(settings, L"user");
				serverAddress_->Text = LoadSetting(settings, L"server");
			}
			
			if (String::IsNullOrEmpty(userName_->Text)) {
				userName_->Text = Environment::UserName;
			}
			if (String::IsNullOrEmpty(serverAddress_->Text)) {
				serverAddress_->Text = L"localhost";
			}
		}

		// Returns a setting from underneath the given registry key.
		// Assumes that it's a string value under the MySQL++ examples' 
		// settings area.
		String^ LoadSetting(RegistryKey^ key, String^ name)
		{
			return (String^)key->GetValue(name);
		}

		// Returns a reference to the MySQL++ examples' settings area in the
		// registry.
		RegistryKey^ OpenSettingsRegistryKey()
		{
			RegistryKey^ key = Registry::CurrentUser->OpenSubKey(L"Software",
					true);
			return key ? key->CreateSubKey(L"MySQL++ Examples") : nullptr;
		}

		// Saves the input fields' values to the registry, except for the
		// password field.
		Void SaveInputs()
		{
			RegistryKey^ settings = OpenSettingsRegistryKey();
			if (settings) {
				SaveSetting(settings, "user", userName_->Text);
				SaveSetting(settings, "server", serverAddress_->Text);
			}			
		}

		// Saves the given value as a named entry under the given registry
		// key.
		Void SaveSetting(RegistryKey^ key, String^ name, String^ value)
		{
			key->SetValue(name, value);
		}

		// Takes a string in the .NET platform's native Unicode format and
		// copies it to the given C string buffer in UTF-8 encoding.
		Void ToUTF8(char* pcOut, int nOutLen, String^ sIn)
		{
			array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(sIn);
			nOutLen = Math::Min(nOutLen - 1, bytes->Length);
			System::Runtime::InteropServices::Marshal::Copy(bytes, 0, 
				IntPtr(pcOut), nOutLen);
			pcOut[nOutLen] = '\0';
		}

		// Takes the given C string encoded in UTF-8 and converts it to a
		// Unicode string in the .NET platform's native Unicode encoding.
		String^ ToUCS2(const char* utf8)
		{
			return gcnew String(utf8, 0, strlen(utf8), 
					System::Text::Encoding::UTF8);
		}

	private: System::Windows::Forms::TextBox^ serverAddress_;
	private: System::Windows::Forms::TextBox^ password_;
	private: System::Windows::Forms::TextBox^ userName_;
	private: System::Windows::Forms::ListBox^ resultsList_;

	private: System::Windows::Forms::Button^ closeButton_;
	private: System::Windows::Forms::TextBox^  RFID;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Button^  btnOpen;
	private: System::Windows::Forms::GroupBox^  grpPorts;
	private: System::Windows::Forms::Button^  btnClosePort;
	private: System::Windows::Forms::Label^  lblPortMsg;
	private: System::Windows::Forms::ComboBox^  comboBox2;
	private: System::IO::Ports::SerialPort^  _serialPort;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  lblDBConnection;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox4;



private: System::ComponentModel::IContainer^  components;



#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::Label^  label1;
			System::Windows::Forms::Label^  label2;
			System::Windows::Forms::Label^  label3;
			this->serverAddress_ = (gcnew System::Windows::Forms::TextBox());
			this->password_ = (gcnew System::Windows::Forms::TextBox());
			this->userName_ = (gcnew System::Windows::Forms::TextBox());
			this->resultsList_ = (gcnew System::Windows::Forms::ListBox());
			this->closeButton_ = (gcnew System::Windows::Forms::Button());
			this->RFID = (gcnew System::Windows::Forms::TextBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->btnOpen = (gcnew System::Windows::Forms::Button());
			this->grpPorts = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->lblPortMsg = (gcnew System::Windows::Forms::Label());
			this->btnClosePort = (gcnew System::Windows::Forms::Button());
			this->_serialPort = (gcnew System::IO::Ports::SerialPort(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->lblDBConnection = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			label1 = (gcnew System::Windows::Forms::Label());
			label2 = (gcnew System::Windows::Forms::Label());
			label3 = (gcnew System::Windows::Forms::Label());
			this->grpPorts->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			label1->AutoSize = true;
			label1->Location = System::Drawing::Point(7, 18);
			label1->Name = L"label1";
			label1->Size = System::Drawing::Size(41, 13);
			label1->TabIndex = 6;
			label1->Text = L"Server:";
			label1->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label2
			// 
			label2->AutoSize = true;
			label2->Location = System::Drawing::Point(197, 17);
			label2->Name = L"label2";
			label2->Size = System::Drawing::Size(61, 13);
			label2->TabIndex = 7;
			label2->Text = L"User name:";
			label2->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label3
			// 
			label3->AutoSize = true;
			label3->Location = System::Drawing::Point(403, 17);
			label3->Name = L"label3";
			label3->Size = System::Drawing::Size(56, 13);
			label3->TabIndex = 8;
			label3->Text = L"Password:";
			label3->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// serverAddress_
			// 
			this->serverAddress_->Location = System::Drawing::Point(48, 16);
			this->serverAddress_->Name = L"serverAddress_";
			this->serverAddress_->Size = System::Drawing::Size(139, 20);
			this->serverAddress_->TabIndex = 0;
			// 
			// password_
			// 
			this->password_->Location = System::Drawing::Point(460, 14);
			this->password_->Name = L"password_";
			this->password_->Size = System::Drawing::Size(139, 20);
			this->password_->TabIndex = 2;
			this->password_->UseSystemPasswordChar = true;
			// 
			// userName_
			// 
			this->userName_->Location = System::Drawing::Point(258, 15);
			this->userName_->Name = L"userName_";
			this->userName_->Size = System::Drawing::Size(139, 20);
			this->userName_->TabIndex = 1;
			// 
			// resultsList_
			// 
			this->resultsList_->Enabled = false;
			this->resultsList_->FormattingEnabled = true;
			this->resultsList_->Location = System::Drawing::Point(5, 15);
			this->resultsList_->Name = L"resultsList_";
			this->resultsList_->Size = System::Drawing::Size(188, 121);
			this->resultsList_->TabIndex = 3;
			this->resultsList_->TabStop = false;
			// 
			// closeButton_
			// 
			this->closeButton_->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->closeButton_->Location = System::Drawing::Point(694, 5);
			this->closeButton_->Name = L"closeButton_";
			this->closeButton_->Size = System::Drawing::Size(75, 23);
			this->closeButton_->TabIndex = 4;
			this->closeButton_->Text = L"Exit";
			this->closeButton_->UseVisualStyleBackColor = true;
			this->closeButton_->Click += gcnew System::EventHandler(this, &MainForm::CloseButton_Click);
			// 
			// RFID
			// 
			this->RFID->Location = System::Drawing::Point(17, 19);
			this->RFID->Name = L"RFID";
			this->RFID->Size = System::Drawing::Size(161, 20);
			this->RFID->TabIndex = 10;
			this->RFID->TextChanged += gcnew System::EventHandler(this, &MainForm::RFID_TextChanged);
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(6, 52);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(151, 21);
			this->comboBox1->TabIndex = 11;
			this->comboBox1->Text = L"COM Ports";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(6, 19);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(151, 23);
			this->progressBar1->TabIndex = 12;
			// 
			// btnOpen
			// 
			this->btnOpen->Location = System::Drawing::Point(5, 112);
			this->btnOpen->Name = L"btnOpen";
			this->btnOpen->Size = System::Drawing::Size(77, 23);
			this->btnOpen->TabIndex = 13;
			this->btnOpen->Text = L"Open Port";
			this->btnOpen->UseVisualStyleBackColor = true;
			this->btnOpen->Click += gcnew System::EventHandler(this, &MainForm::btnOpen_Click);
			// 
			// grpPorts
			// 
			this->grpPorts->BackColor = System::Drawing::Color::Transparent;
			this->grpPorts->Controls->Add(this->comboBox2);
			this->grpPorts->Controls->Add(this->lblPortMsg);
			this->grpPorts->Controls->Add(this->btnClosePort);
			this->grpPorts->Controls->Add(this->progressBar1);
			this->grpPorts->Controls->Add(this->comboBox1);
			this->grpPorts->Controls->Add(this->btnOpen);
			this->grpPorts->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->grpPorts->Location = System::Drawing::Point(7, 102);
			this->grpPorts->Name = L"grpPorts";
			this->grpPorts->Size = System::Drawing::Size(169, 183);
			this->grpPorts->TabIndex = 14;
			this->grpPorts->TabStop = false;
			this->grpPorts->Text = L"COM Port Status";
			// 
			// comboBox2
			// 
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"9600", L"38400", L"115200"});
			this->comboBox2->Location = System::Drawing::Point(6, 85);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(151, 21);
			this->comboBox2->TabIndex = 16;
			this->comboBox2->Text = L"Baud rate";
			// 
			// lblPortMsg
			// 
			this->lblPortMsg->AutoSize = true;
			this->lblPortMsg->Location = System::Drawing::Point(11, 150);
			this->lblPortMsg->Name = L"lblPortMsg";
			this->lblPortMsg->Size = System::Drawing::Size(0, 13);
			this->lblPortMsg->TabIndex = 15;
			// 
			// btnClosePort
			// 
			this->btnClosePort->Location = System::Drawing::Point(89, 112);
			this->btnClosePort->Name = L"btnClosePort";
			this->btnClosePort->Size = System::Drawing::Size(68, 23);
			this->btnClosePort->TabIndex = 14;
			this->btnClosePort->Text = L"Close Port";
			this->btnClosePort->UseVisualStyleBackColor = true;
			this->btnClosePort->Click += gcnew System::EventHandler(this, &MainForm::btnClosePort_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->lblDBConnection);
			this->groupBox1->Location = System::Drawing::Point(6, 55);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(376, 39);
			this->groupBox1->TabIndex = 16;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Database Connection Status";
			// 
			// lblDBConnection
			// 
			this->lblDBConnection->AutoSize = true;
			this->lblDBConnection->ForeColor = System::Drawing::Color::ForestGreen;
			this->lblDBConnection->Location = System::Drawing::Point(15, 17);
			this->lblDBConnection->Name = L"lblDBConnection";
			this->lblDBConnection->Size = System::Drawing::Size(0, 13);
			this->lblDBConnection->TabIndex = 0;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->serverAddress_);
			this->groupBox2->Controls->Add(label1);
			this->groupBox2->Controls->Add(label2);
			this->groupBox2->Controls->Add(this->userName_);
			this->groupBox2->Controls->Add(label3);
			this->groupBox2->Controls->Add(this->password_);
			this->groupBox2->Location = System::Drawing::Point(5, 5);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(619, 44);
			this->groupBox2->TabIndex = 18;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Database";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->resultsList_);
			this->groupBox3->Location = System::Drawing::Point(388, 55);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(200, 143);
			this->groupBox3->TabIndex = 19;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Result";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->RFID);
			this->groupBox4->Location = System::Drawing::Point(186, 102);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(196, 57);
			this->groupBox4->TabIndex = 20;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"RFID";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->closeButton_;
			this->ClientSize = System::Drawing::Size(781, 309);
			this->ControlBox = false;
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->grpPorts);
			this->Controls->Add(this->closeButton_);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MainForm";
			this->ShowIcon = false;
			this->Text = L"RFID";
			this->grpPorts->ResumeLayout(false);
			this->grpPorts->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		 private: void findPorts(void)
             {
            // get port names
            array<Object^>^ objectArray = SerialPort::GetPortNames();
            // add string array to combobox
            this->comboBox1->Items->AddRange( objectArray );
 
             }

private: System::Void btnOpen_Click(System::Object^  sender, System::EventArgs^  e) {
	// set COM port
	
	StringComparer^ stringComparer = StringComparer::OrdinalIgnoreCase;
	if(stringComparer->Equals("COM Ports",this->comboBox1->Text)){
		this->lblPortMsg->Text="Please Select COM Port";
	}
	else if(stringComparer->Equals("Baud rate",this->comboBox2->Text)){
		this->lblPortMsg->Text="Please Select Baud Rate";
	}
	else{
	
		if(!this->_serialPort->IsOpen){
			this->_serialPort->PortName=this->comboBox1->Text;
			//this->textBox1->Text=this->comboBox1->Text;
			this->_serialPort->BaudRate=Int32::Parse(this->comboBox2->Text);
			//this->textBox1->Text=this->comboBox2->Text;
			//  this->lblPortMsg->Text="Enter Message Here";

			// ADDING EVENT HANDLER - @TODO-JET ; ADD EVENT HANDLER (READ THE DATA RECEIVED)
			this->_serialPort->ReadTimeout = 500;
			
			this->_serialPort->DataReceived += gcnew SerialDataReceivedEventHandler(this,&wforms::MainForm::DataReceivedHandler);
			//SerialPort ^sp = gcnew SerialPort("COM3");
			//sp->DataReceived::add(gcnew SerialDataReceivedEventHandler(this,&wforms::MainForm::DataReceivedHandler));

			//open serial port 
			this->_serialPort->Open();
			

		}
		// is open status
		if(this->_serialPort->IsOpen) this->progressBar1->Value=100; 
	}
}

private: System::Void btnClosePort_Click(System::Object^  sender, System::EventArgs^  e) {
		//close serialPort
                 this->_serialPort->Close();
                 if(!this->_serialPort->IsOpen){
                        this->progressBar1->Value=0;
                    }

		 }
private: System::Void btnTest_Click(System::Object^  sender, System::EventArgs^  e) {
			 lblDBConnection->Text = ""; 
			
		 }
};
}
