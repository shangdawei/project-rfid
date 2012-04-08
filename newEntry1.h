#pragma once
#include <myHeader.h>
//#include <MainForm.h>



namespace FormNewEntry {

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
	/// <summary>
	/// Summary for newEntry
	/// </summary>


	public ref class newEntry : public System::Windows::Forms::Form
	{
	private:
	String^ server2;
	String^ user2;
	String^ pass2;
	String^ db2;
	String^ cardnum;
	String^ PortName;
	String^ BaudRate;

	private: System::Windows::Forms::TextBox^  RFID;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  firstname;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TextBox^  lastname;

	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::TextBox^  course;

	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::TextBox^  department;

	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::TextBox^  yearlevel;
	private: System::Windows::Forms::Label^  status;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::TextBox^  email;
	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::ProgressBar^  progressBar1;

	private: System::IO::Ports::SerialPort^  _serialPort;


	private:
	System::Void DataReceivedHandler(System::Object^ sender,SerialDataReceivedEventArgs^ e){
		this->status->Text = "";
		if (RFID->InvokeRequired){
			RFID->Invoke(
				gcnew System::IO::Ports::SerialDataReceivedEventHandler(this,&FormNewEntry::newEntry::DataReceivedHandler),
				gcnew array<System::Object^> { sender, e }
			);
		
		}
		else {
			try {
				System::String^ indata = this->_serialPort->ReadLine();
				this->RFID->Text = indata;
			
			}
			catch (TimeoutException ^) {
				this->status->Text = "TIMEOUT";
			}

		} 
	}
			 

	public:
		Void ToUTF8(char* pcOut, int nOutLen, String^ sIn)
		{
			array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(sIn);
			nOutLen = Math::Min(nOutLen - 1, bytes->Length);
			System::Runtime::InteropServices::Marshal::Copy(bytes, 0, 
				IntPtr(pcOut), nOutLen);
			pcOut[nOutLen] = '\0';
		}

		Void InitializePortListening(){
			if(!this->_serialPort->IsOpen){
				this->_serialPort->PortName=this->PortName;
				this->_serialPort->BaudRate=Int32::Parse(this->BaudRate);
				this->_serialPort->ReadTimeout = 500;
				this->_serialPort->DataReceived += gcnew SerialDataReceivedEventHandler(this,&FormNewEntry::newEntry::DataReceivedHandler);				
				this->_serialPort->Open();		

			}
			if(this->_serialPort->IsOpen) this->progressBar1->Value=100;
		}

		
		Void LoadDefaults(String^ server,String^ user,String^ pass,String^ db,String^ cardnum,String^ portName,String^ baudRate)
		{
			this->server2 = server;
			this->user2 = user;
			this->pass2 = pass;
			this->db2 = db;
			this->cardnum = cardnum;
			this->PortName = portName;
			this->BaudRate = baudRate;

		}

		newEntry(String^ server,String^ user,String^ pass,String^ db,String^ cardnum,String^ portName,String^ baudRate)
		{

			this->server2 = server;
			this->user2 = user;
			this->pass2 = pass;
			this->db2 = db;
			this->cardnum = cardnum;
			this->PortName = portName;
			this->BaudRate = baudRate;

			InitializeComponent();
			InitializePortListening();
			//InitializePortListening();

			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~newEntry()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnAddEntry;
	private: System::ComponentModel::IContainer^  components;

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
			this->btnAddEntry = (gcnew System::Windows::Forms::Button());
			this->RFID = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->firstname = (gcnew System::Windows::Forms::TextBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->lastname = (gcnew System::Windows::Forms::TextBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->course = (gcnew System::Windows::Forms::TextBox());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->department = (gcnew System::Windows::Forms::TextBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->yearlevel = (gcnew System::Windows::Forms::TextBox());
			this->status = (gcnew System::Windows::Forms::Label());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->email = (gcnew System::Windows::Forms::TextBox());
			this->_serialPort = (gcnew System::IO::Ports::SerialPort(this->components));
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->SuspendLayout();
			// 
			// btnAddEntry
			// 
			this->btnAddEntry->Location = System::Drawing::Point(510, 11);
			this->btnAddEntry->Name = L"btnAddEntry";
			this->btnAddEntry->Size = System::Drawing::Size(75, 23);
			this->btnAddEntry->TabIndex = 0;
			this->btnAddEntry->Text = L"Add";
			this->btnAddEntry->UseVisualStyleBackColor = true;
			this->btnAddEntry->Click += gcnew System::EventHandler(this, &newEntry::btnAddEntry_Click);
			// 
			// RFID
			// 
			this->RFID->Location = System::Drawing::Point(6, 17);
			this->RFID->Name = L"RFID";
			this->RFID->Size = System::Drawing::Size(100, 20);
			this->RFID->TabIndex = 2;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->RFID);
			this->groupBox1->Location = System::Drawing::Point(31, 49);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(115, 42);
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"CardNum";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->firstname);
			this->groupBox2->Location = System::Drawing::Point(31, 97);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(115, 42);
			this->groupBox2->TabIndex = 15;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"FirstName";
			// 
			// firstname
			// 
			this->firstname->Location = System::Drawing::Point(6, 17);
			this->firstname->Name = L"firstname";
			this->firstname->Size = System::Drawing::Size(100, 20);
			this->firstname->TabIndex = 2;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->lastname);
			this->groupBox3->Location = System::Drawing::Point(152, 97);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(115, 42);
			this->groupBox3->TabIndex = 16;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"LastName";
			// 
			// lastname
			// 
			this->lastname->Location = System::Drawing::Point(6, 17);
			this->lastname->Name = L"lastname";
			this->lastname->Size = System::Drawing::Size(100, 20);
			this->lastname->TabIndex = 2;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->course);
			this->groupBox4->Location = System::Drawing::Point(152, 145);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(115, 42);
			this->groupBox4->TabIndex = 18;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Course";
			// 
			// course
			// 
			this->course->Location = System::Drawing::Point(6, 17);
			this->course->Name = L"course";
			this->course->Size = System::Drawing::Size(100, 20);
			this->course->TabIndex = 2;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->department);
			this->groupBox5->Location = System::Drawing::Point(31, 145);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(115, 42);
			this->groupBox5->TabIndex = 17;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Department";
			// 
			// department
			// 
			this->department->Location = System::Drawing::Point(6, 17);
			this->department->Name = L"department";
			this->department->Size = System::Drawing::Size(100, 20);
			this->department->TabIndex = 2;
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->yearlevel);
			this->groupBox7->Location = System::Drawing::Point(273, 145);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(115, 42);
			this->groupBox7->TabIndex = 19;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Year Level";
			// 
			// yearlevel
			// 
			this->yearlevel->Location = System::Drawing::Point(6, 17);
			this->yearlevel->Name = L"yearlevel";
			this->yearlevel->Size = System::Drawing::Size(100, 20);
			this->yearlevel->TabIndex = 2;
			// 
			// status
			// 
			this->status->AutoSize = true;
			this->status->Location = System::Drawing::Point(9, 19);
			this->status->Name = L"status";
			this->status->Size = System::Drawing::Size(0, 13);
			this->status->TabIndex = 20;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->status);
			this->groupBox6->Location = System::Drawing::Point(32, -1);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(235, 41);
			this->groupBox6->TabIndex = 21;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Status";
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->email);
			this->groupBox8->Location = System::Drawing::Point(273, 97);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(115, 42);
			this->groupBox8->TabIndex = 17;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"Email";
			// 
			// email
			// 
			this->email->Location = System::Drawing::Point(6, 17);
			this->email->Name = L"email";
			this->email->Size = System::Drawing::Size(100, 20);
			this->email->TabIndex = 2;
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->progressBar1);
			this->groupBox9->Location = System::Drawing::Point(279, 0);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(194, 40);
			this->groupBox9->TabIndex = 22;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"Port Status";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(8, 12);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(100, 23);
			this->progressBar1->TabIndex = 0;
			// 
			// newEntry
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(597, 265);
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->groupBox8);
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->groupBox7);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->btnAddEntry);
			this->Name = L"newEntry";
			this->Text = L"newEntry";
			this->Load += gcnew System::EventHandler(this, &newEntry::newEntry_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->groupBox9->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnAddEntry_Click(System::Object^  sender, System::EventArgs^  e) {
			mysqlpp::Connection con(false);
			const int kInputBufSize = 100;
			char acServerAddress[kInputBufSize];
			char acUserName[kInputBufSize];
			char acPassword[kInputBufSize];
			char tmpRFID[kInputBufSize];

			char firstname[kInputBufSize];
			char lastname[kInputBufSize];
			char email[kInputBufSize];
			char department[kInputBufSize];
			char course[kInputBufSize];
			char year[kInputBufSize];


			ToUTF8(acServerAddress, kInputBufSize, this->server2);
			ToUTF8(acUserName, kInputBufSize, this->user2);
			ToUTF8(acPassword, kInputBufSize, this->pass2);
			ToUTF8(tmpRFID, kInputBufSize, this->RFID->Text);


			ToUTF8(firstname, kInputBufSize, this->firstname->Text);
			ToUTF8(lastname, kInputBufSize, this->lastname->Text);
			ToUTF8(email, kInputBufSize, this->email->Text);
			ToUTF8(department, kInputBufSize, this->department->Text);
			ToUTF8(course, kInputBufSize, this->course->Text);
			ToUTF8(year, kInputBufSize, this->yearlevel->Text);


			if (!con.connect("test", acServerAddress, acUserName, acPassword)) {
				status->Text = gcnew String(con.error());
				return;
			}

			//TRIM RFID
			string str1(tmpRFID);
			string firstname_(firstname);
			string lastname_(lastname);
			string department_(department);
			string course_(course);
			string year_(year);

			trim(str1);

			if(str1.empty() || firstname_.empty() || lastname_.empty() || department_.empty() || course_.empty() || year_.empty()){
				status->Text = "ERROR:: One of the required fields is empty";
			}
			else {
				mysqlpp::Query tmpquery = con.query("SELECT loginid FROM login WHERE cardnum = %0q");
				tmpquery.parse();
				mysqlpp::StoreQueryResult tmpres = tmpquery.store(str1);

				//mysqlpp::Query query = con.query("SELECT loginid FROM login WHERE cardnum = %0q");
				//query.parse();
				//mysqlpp::SimpleResult res = query.execute(str1);
				size_t ii = 0;

				if(tmpres.num_rows()> ii){
					status->Text = "ERROR:: CARDNUM IS ALREADY USED!";
					
				}
				else {
					mysqlpp::Query query = con.query("INSERT INTO `login` (cardnum,firstname,lastname,email,department,course,yearlevel) VALUES (%0q,%1q,%2q,%3q,%4q,%5q,%6q)");
					query.parse();
					mysqlpp::SimpleResult res = query.execute(str1,firstname,lastname,email,department,course,year);

					if(res.rows()==0){
						status->Text = gcnew String(con.error());	
					} else {
						status->Text = "Save Success!";
					}
				}

			}

			//mysqlpp::Query query = con.query("SELECT * FROM loginsheet WHERE `in` REGEXP CURDATE() AND cardnum = %0q AND `out` IS NULL");
			//query.parse();
			//mysqlpp::StoreQueryResult res2 = query.store(str1);



			//AddMessage("WA");
			// tmpText->Text = wforms::MainForm::serverAddress_->Text;
					//wforms::MainForm::findPorts();
				 //mysqlpp::Connection con(false);
			 }
	private: System::Void label6_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void newEntry_Load(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 //RFID->Text = this->BaudRate;
			 InitializePortListening();
		 }
};
}
