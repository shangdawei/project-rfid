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
#include <newEntry1.h>

namespace wforms {
	using namespace FormNewEntry;
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

	private:
	System::Void DataReceivedHandler(System::Object^ sender,SerialDataReceivedEventArgs^ e){
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
		public:
		Void GetCurrentSheet(){
			this->dataGridView1->Rows->Clear();

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

			string str1(tmpRFID);
				trim(str1);

				//CONNECT TO THE DATABASE
			if (!con.connect("test", acServerAddress, acUserName, acPassword)) {
				lblDBConnection->Text = gcnew String(con.error());
				return;
			}

			if(con.connected()){
				//SHOW CURRENT SHEET
				mysqlpp::Query query = con.query("SELECT * FROM loginsheet WHERE `in` REGEXP CURDATE()");
				mysqlpp::StoreQueryResult res = query.store();
				size_t i = 0;
				for (size_t ctr = 0; ctr < res.num_rows(); ++ctr) {
					mysqlpp::Query query2 = con.query("SELECT * FROM login WHERE `cardnum` = %0q");
					query2.parse();
					mysqlpp::StoreQueryResult res2 = query2.store(res[ctr]["cardnum"]);
					this->dataGridView1->Rows->Add(ToUCS2(res[ctr]["cardnum"]),ToUCS2(res[ctr]["in"]),ToUCS2(res[ctr]["out"]),ToUCS2(res2[i]["firstname"]),ToUCS2(res2[i]["lastname"]),ToUCS2(res2[i]["department"]),ToUCS2(res2[i]["course"]),ToUCS2(res2[i]["yearlevel"]));
					//this->dataGridView1->Rows->Add(ToUCS2(res[ctr]["cardnum"]),ToUCS2(res[ctr]["in"]),ToUCS2(res[ctr]["out"]),res2.num_rows());
				}
			}
		}


	// Insert a text string into the output list control
		Void AddMessage(String^ msg)
		{
			resultsList_->Items->Add(msg);
		}

		Void ProcessAccess(String^ access){
			if(this->_serialPort->IsOpen){
				if(access == "Access Granted"){
					this->_serialPort->WriteLine("1");
				}
				else {
					
					this->_serialPort->WriteLine("0");
				}
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
			userLogs->Items->Clear();
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

				mysqlpp::Query query = con.query("SELECT * FROM login WHERE cardnum = %0q");				
				query.parse();
			
				if (mysqlpp::StoreQueryResult res = query.store(str1)) {
					size_t i = 0;
					if(res.num_rows() == i){
						AddMessage("No Entry Found");
					}
					else {
						//SHOW USER LOGS
						mysqlpp::Query querylogs = con.query("SELECT * FROM loginrecords WHERE cardnum = %0q ORDER BY `date` DESC");
						querylogs.parse();
						mysqlpp::StoreQueryResult getlogs = querylogs.store(str1);
						for (size_t ctr = 0; ctr < getlogs.num_rows(); ++ctr) {
							userLogs->Items->Add(ToUCS2(getlogs[ctr]["date"]));
						}

						


						//for (size_t i = 0; i < res.num_rows(); ++i) {

						//this->dataGridView1->Rows->Add(ToUCS2(res[i]["cardnum"]),ToUCS2(res[i]["firstname"]),ToUCS2(res[i]["lastname"]),ToUCS2(res[i]["department"]),ToUCS2(res[i]["course"]),ToUCS2(res[i]["yearlevel"]));
						
						AddMessage("Firstname: " + ToUCS2(res[i]["firstname"]));
						AddMessage("Lastname: " + ToUCS2(res[i]["lastname"]));
						AddMessage("Department: " + ToUCS2(res[i]["department"]));
						AddMessage("Course: " + ToUCS2(res[i]["course"]));
						AddMessage("Year Level: " + ToUCS2(res[i]["yearlevel"]));
						AddMessage("Last Access: " + ToUCS2(res[i]["lastlogin"]));
						String^ isBanned = ToUCS2(res[i]["banned"]);
						if(isBanned == "1"){
							AddMessage("Banned : Yes");
							ProcessAccess("Access Denied");
						}
						else {
							//CHECK USER'S CURRENT DATE LOGS
							mysqlpp::Query querytmp = con.query("SELECT * FROM loginsheet WHERE `in` REGEXP CURDATE() AND cardnum = %0q AND `out` IS NULL");
							querytmp.parse();
							mysqlpp::StoreQueryResult res2 = querytmp.store(str1);

							//IF NOT LOGGED IN
							if(res2.num_rows() == i){
								querytmp.reset();
								querytmp << "INSERT INTO loginsheet (cardnum,`in`,`out`) VALUES (%0q,NOW(),NULL)";
								querytmp.parse();
								res2 = querytmp.store(str1);
							}
							else {
							//IF LOGGED IN, UPDATE LOGOUT COLUMN
								querytmp.reset();
								querytmp << "UPDATE loginsheet SET `out` = NOW() WHERE id = %0q";
								querytmp.parse();
								res2 = querytmp.store(res2[i]["id"]);
							}

							AddMessage("Banned : No");
							ProcessAccess("Access Granted");

							querytmp.reset();
							querytmp << "UPDATE login SET lastlogin = NOW() WHERE cardnum = %0q";
							querytmp.parse();
							res2 = querytmp.store(str1);

							//UPDATING RECORDS
							querytmp.reset();
							querytmp << "INSERT INTO loginrecords (`cardnum`,`date`) VALUES (%0q,NOW())";
							querytmp.parse();
							res2 = querytmp.store(str1);
						}
						GetCurrentSheet();
						RFID->Text = "";
					}
					//}
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

private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabResult;
private: System::Windows::Forms::TabPage^  tabLogs;
private: System::Windows::Forms::ListBox^  userLogs;
private: System::Windows::Forms::ListBox^  resultsList_;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::MenuStrip^  menuStrip1;
private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  newEntryToolStripMenuItem;
private: System::Windows::Forms::DataGridView^  dataGridView1;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  CardNum;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  LOGIN;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  LOGOUT;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  FirstName;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  LastName;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Department;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Course;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Year;
private: System::Windows::Forms::Button^  button1;


























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
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabResult = (gcnew System::Windows::Forms::TabPage());
			this->resultsList_ = (gcnew System::Windows::Forms::ListBox());
			this->tabLogs = (gcnew System::Windows::Forms::TabPage());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->userLogs = (gcnew System::Windows::Forms::ListBox());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newEntryToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->CardNum = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->LOGIN = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->LOGOUT = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->FirstName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->LastName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Department = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Course = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Year = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->button1 = (gcnew System::Windows::Forms::Button());
			label1 = (gcnew System::Windows::Forms::Label());
			label2 = (gcnew System::Windows::Forms::Label());
			label3 = (gcnew System::Windows::Forms::Label());
			this->grpPorts->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabResult->SuspendLayout();
			this->tabLogs->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
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
			this->grpPorts->Location = System::Drawing::Point(4, 125);
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
			this->groupBox1->Location = System::Drawing::Point(3, 78);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(782, 39);
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
			this->groupBox2->Location = System::Drawing::Point(2, 28);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(619, 44);
			this->groupBox2->TabIndex = 18;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Database";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->RFID);
			this->groupBox4->Location = System::Drawing::Point(183, 125);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(196, 57);
			this->groupBox4->TabIndex = 20;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"RFID";
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabResult);
			this->tabControl1->Controls->Add(this->tabLogs);
			this->tabControl1->Location = System::Drawing::Point(385, 125);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(247, 153);
			this->tabControl1->TabIndex = 21;
			// 
			// tabResult
			// 
			this->tabResult->Controls->Add(this->resultsList_);
			this->tabResult->Location = System::Drawing::Point(4, 22);
			this->tabResult->Name = L"tabResult";
			this->tabResult->Padding = System::Windows::Forms::Padding(3);
			this->tabResult->Size = System::Drawing::Size(239, 127);
			this->tabResult->TabIndex = 0;
			this->tabResult->Text = L"Result";
			this->tabResult->UseVisualStyleBackColor = true;
			// 
			// resultsList_
			// 
			this->resultsList_->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->resultsList_->FormattingEnabled = true;
			this->resultsList_->Location = System::Drawing::Point(6, 6);
			this->resultsList_->Name = L"resultsList_";
			this->resultsList_->Size = System::Drawing::Size(226, 117);
			this->resultsList_->TabIndex = 22;
			// 
			// tabLogs
			// 
			this->tabLogs->Controls->Add(this->label4);
			this->tabLogs->Controls->Add(this->userLogs);
			this->tabLogs->Location = System::Drawing::Point(4, 22);
			this->tabLogs->Name = L"tabLogs";
			this->tabLogs->Padding = System::Windows::Forms::Padding(3);
			this->tabLogs->Size = System::Drawing::Size(239, 127);
			this->tabLogs->TabIndex = 1;
			this->tabLogs->Text = L"Logs";
			this->tabLogs->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Arial", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(3, 5);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(96, 14);
			this->label4->TabIndex = 1;
			this->label4->Text = L"Dates Logged In";
			// 
			// userLogs
			// 
			this->userLogs->FormattingEnabled = true;
			this->userLogs->Location = System::Drawing::Point(5, 25);
			this->userLogs->Name = L"userLogs";
			this->userLogs->Size = System::Drawing::Size(226, 95);
			this->userLogs->TabIndex = 0;
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->fileToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(826, 24);
			this->menuStrip1->TabIndex = 22;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->newEntryToolStripMenuItem, 
				this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// newEntryToolStripMenuItem
			// 
			this->newEntryToolStripMenuItem->Name = L"newEntryToolStripMenuItem";
			this->newEntryToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->newEntryToolStripMenuItem->Text = L"New Entry";
			this->newEntryToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::newEntryToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::exitToolStripMenuItem_Click);
			// 
			// dataGridView1
			// 
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(8) {this->CardNum, 
				this->LOGIN, this->LOGOUT, this->FirstName, this->LastName, this->Department, this->Course, this->Year});
			this->dataGridView1->Location = System::Drawing::Point(3, 351);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->Size = System::Drawing::Size(818, 169);
			this->dataGridView1->TabIndex = 23;
			// 
			// CardNum
			// 
			this->CardNum->HeaderText = L"CardNum";
			this->CardNum->Name = L"CardNum";
			this->CardNum->ReadOnly = true;
			// 
			// LOGIN
			// 
			this->LOGIN->HeaderText = L"LOGIN";
			this->LOGIN->Name = L"LOGIN";
			this->LOGIN->ReadOnly = true;
			// 
			// LOGOUT
			// 
			this->LOGOUT->HeaderText = L"LOGOUT";
			this->LOGOUT->Name = L"LOGOUT";
			this->LOGOUT->ReadOnly = true;
			// 
			// FirstName
			// 
			this->FirstName->HeaderText = L"FirstName";
			this->FirstName->Name = L"FirstName";
			this->FirstName->ReadOnly = true;
			// 
			// LastName
			// 
			this->LastName->HeaderText = L"LastName";
			this->LastName->Name = L"LastName";
			this->LastName->ReadOnly = true;
			// 
			// Department
			// 
			this->Department->HeaderText = L"Department";
			this->Department->Name = L"Department";
			this->Department->ReadOnly = true;
			// 
			// Course
			// 
			this->Course->HeaderText = L"Course";
			this->Course->Name = L"Course";
			this->Course->ReadOnly = true;
			// 
			// Year
			// 
			this->Year->HeaderText = L"Year";
			this->Year->Name = L"Year";
			this->Year->ReadOnly = true;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(5, 323);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 24;
			this->button1->Text = L"Refresh List";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(826, 532);
			this->ControlBox = false;
			this->Controls->Add(this->button1);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->grpPorts);
			this->Controls->Add(this->menuStrip1);
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
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabResult->ResumeLayout(false);
			this->tabLogs->ResumeLayout(false);
			this->tabLogs->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

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
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 Application::Exit();
		 }
private: System::Void newEntryToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			//newEntry1 myForm;
			/* const int kInputBufSize = 100;
			char acServerAddress[kInputBufSize];
			char acUserName[kInputBufSize];
			char acPassword[kInputBufSize];
			char tmpRFID[kInputBufSize];
			ToUTF8(acServerAddress, kInputBufSize, serverAddress_->Text);
			ToUTF8(acUserName, kInputBufSize, userName_->Text);
			ToUTF8(acPassword, kInputBufSize, password_->Text);
			ToUTF8(tmpRFID, kInputBufSize, RFID->Text);
			*/

			 
            if(this->_serialPort->IsOpen){
                this->_serialPort->Close();
				this->progressBar1->Value=0;
            }

			FormNewEntry::newEntry^ form = gcnew FormNewEntry::newEntry(serverAddress_->Text,userName_->Text,password_->Text,"test",RFID->Text,this->comboBox1->Text,this->comboBox2->Text);
			//form->LoadDefaults(serverAddress_->Text,userName_->Text,password_->Text,"test",RFID->Text,this->comboBox1->Text,this->comboBox2->Text);
			form->Show();

			//FormNewEntry::newEntry
			/*if(myForm.ShowDialog() == System::Windows::Forms::DialogResult::OK){
				//Do stuff
			}*/
		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {			
			GetCurrentSheet();
		 }
};
}
