// $Id: RDBCform.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//     This file is part of the RDBC
//     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>
/////////////////////////////////////////////////////////////////////
//
// An example of Simple Form used for browsing of tables
//
//////////////////////////////////////////////////////////////////////
//
// Usage:
//  
// root[0] gSystem->Load("libRDBC");      // load library
// root[1] .L RDBCform.C                  // load macro
//    or
// root[1] .L RDBCform.C++                // compile and load macro
//
// root[2]  new DbForm();                 // create form  
//
//////////////////////////////////////////////////////////////////////
//
// To compile standalone executable program use:
//
// 1. Compile shared lib module RDBCform_C.so by:
//
//       root[0] gSystem->Load("libRDBC");
//       root[1] .L RDBCform.C++
//
// 2. Comiple executable by:
//
//    $ g++ -O -o RDBCform RDBCform.C `root-config --cflags --glibs` -lRDBC -DSTANDALONE 
//
//  Check the main() code at the bottom of this script
//
// 3. Run it (RDBCform_C.so should be in current dir):
// 
//    $ ./RDBCform&
//
//////////////////////////////////////////////////////////////////////
//
// Warnings:
//
//   - this macro uses TGxxx classes which don't work with win32
//   - some tables ( for example Oracle system tables) can
//     be browsed only with forward_only cursor. 
//     Use DbForm::SetForwardOnly() method to force this
//     setting.
//
//Begin_Html
/*
<img src="oform.gif">
*/
//End_Html

#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLStatement.h>
#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLDatabaseMetaData.h>

#include <TTimer.h>
#include <TGCanvas.h>
#include <TGToolBar.h>
#include <TGStatusBar.h>
#include <TGMsgBox.h>
#include <TVirtualX.h>
#include <TROOT.h>
#include <TInterpreter.h>
#include <TGMenu.h>
#include <TG3DLine.h>
#include <RQ_OBJECT.h>
#include <TSystem.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>


class DbForm* gForm = 0; // db form available from interpreter 

////////////////////////////////////////////////////////////////
//
//  Password Dialog widget used for entering 
//  Data Source Name, User Name and Password 
//
///////////////////////////////////////////////////////////////   
class PasswordDialog 
{
RQ_OBJECT()

private:
   TGMainFrame*   fMain;
   TGTextEntry*   fDSNEntry;  // data source name text entry
   TGTextEntry*   fUserEntry; // username text entry
   TGTextEntry*   fPswdEntry; // password text entry 
   TGTextButton*  fOKButton;        // OK button
   TGTextButton*  fCancelButton;    // Cancel button
   TGLabel*       fDSNLabel; 
   TGLabel*       fPswLabel;   
   TGLabel*       fUsrLabel;
   TGLabel*       fInfoLabel;
   TString        fDSN;  // last entered DSN
   TString        fUsername;  // last entered username
   TString        fPassword;  // last enterd password
        
public:
   PasswordDialog();
   ~PasswordDialog(); 
   
   void Hide();
   void Show();
   void Clear();
    
   TGMainFrame* GetMainFrame() const { return fMain; }
   TString GetDSN() const { return fDSN; }
   TString GetUsername() const { return fUsername; }
   TString GetPassword() const { return fPassword; }
   
   void Print(Option_t* opt="") const;      
   void Entered( const Text_t* dsn=0,
                 const Text_t* pwd=0,
                 const Text_t* usr=0 );     // *SIGNAL*

   // slots
   void  DSNEntered();
   void  PasswordEntered();
   void  UsernameEntered();
};

//____________________________________________________________________
PasswordDialog::PasswordDialog()
{
   // constructor

   fMain = new TGMainFrame(gClient->GetRoot(),500,200);
   fMain->SetWMSizeHints(500,200,500,200,0,0);
   fMain->SetWindowName("Enter DSN, Password, User Name");
   fMain->SetIconName("Enter DSN, Password ,User Name");
   
    // create username text entry
   fDSNEntry = new TGTextEntry(fMain,"");
   fDSNEntry->Resize(350,20);
   fDSNEntry->Move(100,50);
   fDSNLabel = new TGLabel(fMain,"DSN:");
   fDSNLabel->Move(30,52);
   
   fUserEntry = new TGTextEntry(fMain,"");
   fUserEntry->Resize(350,20);
   fUserEntry->Move(100,90);
   fUserEntry->SetMaxLength(20);  // maximum 20 symbols
   fUsrLabel = new TGLabel(fMain,"User Name:");
   fUsrLabel->Move(30,92);
      
   // create password text entry
   fPswdEntry = new TGTextEntry(fMain,"");
   fPswdEntry->Resize(350,20);
   fPswdEntry->Move(100,130);
   fPswdEntry->SetMaxLength(20);  // maximum 20 symbols
   
   // set password echo mode
   fPswdEntry->SetEchoMode(TGTextEntry::kPassword);    
  
   fPswLabel = new TGLabel(fMain,"Password:");
   fPswLabel->Move(30,132); 
   
   fInfoLabel =  new TGLabel(fMain,
                "Type DSN, Username and Password to log on");
   
   fInfoLabel->Move(30,20);
    
   fOKButton = new TGTextButton(fMain,"OK");
   fOKButton->Move(200,170);
   fOKButton->Resize(50,20);
         
   fCancelButton = new TGTextButton(fMain,"Cancel");
   fCancelButton->Move(300,170);
   fCancelButton->Resize(50,20);
    
   fMain->MapSubwindows();
   fMain->MapRaised();
   fMain->SetWMPosition(300,300);
   fMain->ChangeOptions(fMain->GetOptions() | kFixedSize);
          
   //*** Make connections between signals and  handlers

   fDSNEntry->Connect("ReturnPressed()","PasswordDialog",
                       this,"DSNEntered");
     
   // Connects TGTextEntry's signal "ReturnPressed" from 
   // fUserEntry to PasswordDialog::UsernameEntered() method 
   // of "this" object         
   
   fUserEntry->Connect("ReturnPressed()","PasswordDialog",
                        this,"UsernameEntered");
   
   // Connects TGTextEntry's signal "ReturnPressed" from
   // fPswdEntry to PasswordDialog::UsernameEntered() method 
   // of "this" object.
          
   fPswdEntry->Connect( "ReturnPressed()","PasswordDialog",
                        this,"PasswordEntered()");
  
   fCancelButton->Connect("Pressed()","PasswordDialog",
                           this,"Hide()");

   fCancelButton->Connect("Pressed()","PasswordDialog",
                           this,"Clear()");   
  
   fOKButton->Connect("Released()","PasswordDialog",
                           this,"PasswordEntered()");
   
   fDSNEntry->Connect("CursorOutDown()","TGTextEntry",fUserEntry,"SetFocus()");
   fUserEntry->Connect("CursorOutDown()","TGTextEntry",fPswdEntry,"SetFocus()");
   fUserEntry->Connect("CursorOutUp()","TGTextEntry",fDSNEntry,"SetFocus()");
   fPswdEntry->Connect("CursorOutUp()","TGTextEntry",fUserEntry,"SetFocus()");
}

//____________________________________________________________________
void PasswordDialog::Show()
{ 
   // 
   
   fMain->MapRaised();
}

//____________________________________________________________________
void PasswordDialog::Hide()
{ 
   //
   
   fMain->UnmapWindow();
}

//____________________________________________________________________
void PasswordDialog::Clear()
{ 
   // 
   
   fDSNEntry->Clear();
   fPswdEntry->Clear();
   fUserEntry->Clear();
}

//____________________________________________________________________
PasswordDialog::~PasswordDialog()
{ 
   // destructor 
 
   delete fDSNEntry;
   delete fDSNLabel;
   delete fUserEntry;
   delete fPswdEntry;
   delete fPswLabel;
   delete fUsrLabel;
   delete fInfoLabel;
   delete fOKButton;
   delete fCancelButton;
   delete fMain;     
}

//____________________________________________________________________
void PasswordDialog::Entered(const Text_t* dsn,const Text_t* usr,
                             const Text_t* pwd)
{
   // This signal is emitted when username and password entered.
 
   TString DSN = dsn;   
   TString USR = usr;        
   TString PWD = pwd;
   
      // User can add some validation procedure below
   if( DSN==fDSN && USR==fUsername && PWD==fPassword ) return;
   if( !DSN.IsNull() && !USR.IsNull()  ) {
     
      long args[3];
      
      args[0] = (long)DSN.Data();
      args[1] = (long)USR.Data();
      args[2] = (long)PWD.Data();
      
      fDSN = DSN;    // copy dsn
      fUsername = USR;    // copy username 
      fPassword = PWD;    // copy password 
         
      Emit("Entered(Text_t*,Text_t*,Text_t*)",args);
   }
}
   
//____________________________________________________________________
void PasswordDialog::DSNEntered()
{
   // Handles ReturnPressed signal from dsn text entry.
   // ... just move keyboard focus to username text entry.
   
   // move keyboard focus to password text entry
   fUserEntry->SetFocus();   
   
   // select text in the password text entry   
   fUserEntry->SelectAll();  
}
   
//____________________________________________________________________
void PasswordDialog::UsernameEntered()
{
   // Handles ReturnPressed signal from username  text entry.
   // ... just move keyboard focus to password text entry.
   
   // move keyboard focus to password text entry
   fPswdEntry->SetFocus();   
   
   // select text in the password text entry   
   fPswdEntry->SelectAll();  
}

//____________________________________________________________________
void PasswordDialog::PasswordEntered()
{
   // Handles ReturnPressed signal from password text entry 

   TString dsn = fDSNEntry->GetText();   
   TString usr = fUserEntry->GetText();        
   TString pwd = fPswdEntry->GetText();
   
   Clear();
   fDSNEntry->SetFocus();  // move back keyboard focus to username entry  
   Entered(dsn.Data(),usr.Data(),pwd.Data());   // emit  signal
   fDSN="";
   fUsername="";
   fPassword="";      
}

//____________________________________________________________________
void PasswordDialog::Print(Option_t*) const
{
   // Prints entered dsn, username and password
   
   printf("\nDSN: %s",fDSN.Data());
   printf("\nUsername: %s\n",fUsername.Data());
   printf("Password: %s\n",fPassword.Data());   
}
    
///////////////////////////////////////////////////////////////////
class DbFormEntry
{
RQ_OBJECT()

private:
   TGLabel* fLabel;  // column name
   TGTextEntry* fEntry; // column value as text
   TGCompositeFrame* fMain;  // 
   TGLayoutHints* fLayoutHints1; // hints for text
   TGLayoutHints* fLayoutHints2;  // hints for label
   Int_t fColumnIndex; // column index
   TString fColumnName; // column name
   TString fColumnValue;  // column value 

public:
   DbFormEntry(TGWindow* p,Int_t index,
               const TString& value, const TString& name);
   ~DbFormEntry();
   
   void Updated(Int_t index,const Text_t* value); //*SIGNAL*
   void SetColumnValue(const TString& value) 
      { fColumnValue = value; fEntry->SetText(value.Data()); }
   
   TGCompositeFrame* GetMain() const { return fMain; }
};

//____________________________________________________________________
DbFormEntry::DbFormEntry(TGWindow* p,Int_t index,
                        const TString& value,const TString& name)
{  
   // ctor
   
   fMain = new TGCompositeFrame(p,32, 32,kHorizontalFrame);
   fLayoutHints1 = new TGLayoutHints( kLHintsRight | kLHintsExpandX,
                                   5, 100, 5, 5);
   fLayoutHints2 = new TGLayoutHints( kLHintsNormal,
                                     100, 5, 5, 5);
   
   fColumnIndex = index;
   fColumnValue = value;
   fColumnName = name;
   
   fLabel = new TGLabel(fMain,fColumnName.Data());
   fEntry = new TGTextEntry(fMain,fColumnValue.Data());
   fEntry->SetEnabled(kFALSE); // disable
   fMain->AddFrame(fLabel,fLayoutHints2); 
   fMain->AddFrame(fEntry,fLayoutHints1);
   
   fEntry->Connect("ReturnPressed()","DbFormEntry",this,
                   "Updated(Int_t,Text_t*)");

}      

//____________________________________________________________________
DbFormEntry::~DbFormEntry()
{  
   // dtor with sanity check
   
   if(fLabel) fLabel->UnmapWindow();
   if(fEntry) fEntry->UnmapWindow();   
   if(fLabel) delete fLabel;
   if(fEntry) delete fEntry;
   if(fLayoutHints1) delete fLayoutHints1;
   if(fLayoutHints2) delete fLayoutHints2;
   if(fMain) delete fMain;
}

//____________________________________________________________________
void DbFormEntry::Updated(Int_t index,const Text_t* value)
{  
   // emit signal when text changed in the text entry 
   // (not used so far)
   
   long args[2];
   
   TString newValue = fEntry->GetText();
          
   if(fColumnValue!=newValue) {
      args[0] = (long)fColumnIndex;
      args[1] = (long)newValue.Data();
      Emit("Updated(Int_t,Text_t*)",args); 
   }
}

typedef DbFormEntry* DbFormEntryPtr; 
         
///////////////////////////////////////////////////////////////////   
class DbForm
{
RQ_OBJECT()

private:
   enum EFile { kConnect,kDisconnect,kExit };
   enum EGo { kPrev,kNext,kFirst,kLast,kAbsolute };
   enum EHelp { kAbout };
   
   // GUI part
   TGMainFrame*  fMain;   // main frame
   TGPopupMenu*  fFileMenu; // file popup menu
   TGPopupMenu*  fGotoMenu; // go popup menu
   TGPopupMenu*  fHelpMenu; // help menu
   TGTextEntry*  fQueryTextEntry;  // SQL query text entry
   TGTextButton* fExecuteQueryButton; // excute query button
   TGTextButton* fCancelQueryButton; // cancel query button
   TGTextButton* fNextButton;  // next button
   TGTextButton* fPrevButton;  // prev button
   TGTextButton* fLastButton;  // last button
   TGTextButton* fFirstButton; // first button
   TTimer* fNextButtonTimer; // auotorepeat timer for next button
   TTimer* fPrevButtonTimer; // auotorepeat timer for prev button
   TGCompositeFrame* fButtonGroup; // button group for next,prev,first,last bttns
   TGStatusBar*   fStatusBar; // status bar 
   TGMenuBar*     fMenuBar;  // menu bar 
   TGLayoutHints* fMenuBarLayout;   //  layout hints 
   TGLayoutHints* fMenuBarItemLayout; // layout hints 
   TGLayoutHints* fMenuBarHelpLayout; // layout hints 
   TGHorizontal3DLine* fToolBarSep; // separator
   TGToolBar*     fToolBar;   // composite rame for execute query text entry and bttns
   TGLayoutHints* fBarLayout; // layout hints 
   TGLayoutHints* fQueryTextLayout; // layout for query text entry
   TGCanvas* fCanvasWindow; // window with sliders
   TGCompositeFrame* fContainer; //
   DbFormEntry** fEntries;  // form entries
   PasswordDialog* fPD; // password dialog used to enter dsn,uid,pwd
            
  // db part
   Bool_t fConnected; // kTRUE if connected to db
   TString fDSN;    // dsn name 
   Int_t fRstype; // result set type
   Int_t fRsconc; // result set concurrency   
   TSQLConnection* fConnection; // connection to db
   TSQLStatement* fStatement; // latest statement
   TSQLResultSet* fResultSet; // latest result set
   TSQLResultSetMetaData* fResultSetMetaData; // result set meta data 
 
   void DisplayResults();
   void UpdateColumn(Int_t index,const Text_t* value);
   void ClearCanvas();

public:
   DbForm();
   ~DbForm();
   
   TSQLConnection* GetConnection() const { return fConnection; }
   void SetForwardOnly();
   void SetConnection(TSQLConnection* con) { fConnection=con; HandleConnect(); }
   void SetStatement(TSQLStatement* stmt) 
         { SetConnection(stmt->GetConnection()); HandleExecuteQuery(); }

   void Hide() { fMain->UnmapWindow(); }
   void Show() { fMain->MapRaised(); }
   TGMainFrame* GetMainFrame() const { return fMain; }  

   // slots 
   void HandleFileMenu(Int_t id);
   void HandleGotoMenu(Int_t id);
   void HandleDisconnect();
   void HandleConnect();
   void HandleExecuteQuery();
   void First();
   void Last();
   void Next();
   void Prev();
   void AcceleratePrevButton();
   void AccelerateNextButton();
   void Connect(const Text_t* dsn, const Text_t* uid, const Text_t* pwd);    

};
      
/////////////////////////// DbForm implementation ////////////////////
//____________________________________________________________________
DbForm::DbForm()
{
   // constructor
      
   TGLayoutHints *lo = 0;

   fConnection = 0; // connection to db
   fStatement = 0; // latest statement
   fResultSet = 0; // latest result set
   fResultSetMetaData = 0; // result set meta data 
   fConnected = kFALSE;   

   if(gForm) gInterpreter->DeleteGlobal(gForm); //delete gForm;
       
   fMain = new TGMainFrame(gClient->GetRoot(),500,500);
   fMain->SetWMSizeHints(500,500,500,500,0,0);
   fMain->SetWindowName("Simple Database Form");
   fMain->SetIconName("Simple of Database Form");   
   fMain->SetClassHints("DbForm", "DbForm");
   fMain->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
   
   fFileMenu = new TGPopupMenu(gClient->GetRoot());
   fFileMenu->AddEntry("&Connect",kConnect);
   fFileMenu->AddEntry("&Disconnect",kDisconnect);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Exit",kExit);

   fGotoMenu = new TGPopupMenu(gClient->GetRoot());
   fGotoMenu->AddEntry("&First",kFirst);   
   fGotoMenu->AddEntry("&Prev",kPrev);   
   fGotoMenu->AddEntry("&Next",kNext);
   fGotoMenu->AddEntry("&Last",kLast);
   
   fHelpMenu = new TGPopupMenu(gClient->GetRoot());
   fHelpMenu->AddEntry("&About",kAbout);
   
   fMenuBarLayout = new TGLayoutHints( kLHintsTop | 
                                       kLHintsLeft | 
                                       kLHintsExpandX,5,5,3,1);
   
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | 
                                          kLHintsLeft,0,20,3,1);
   
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | 
                                          kLHintsRight,0,5,3,1);
      
   fMenuBar = new TGMenuBar(fMain,1,1,kHorizontalFrame);
   fMenuBar->AddPopup("&File",fFileMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Go",fGotoMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help",fHelpMenu, fMenuBarHelpLayout);
   fMain->AddFrame(fMenuBar, fMenuBarLayout);
  
   fToolBarSep = new TGHorizontal3DLine(fMain);
   fBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMain->AddFrame(fToolBarSep,fBarLayout);

   fToolBar = new TGToolBar(fMain,60,20, kHorizontalFrame);
   fMain->AddFrame(fToolBar,fBarLayout);
    
   fQueryTextEntry = new TGTextEntry(fToolBar,"");
   fToolBar->AddFrame(fQueryTextEntry,fMenuBarLayout);
   
   fExecuteQueryButton = new TGTextButton(fToolBar,"Query");
   fCancelQueryButton = new TGTextButton(fToolBar,"Cancel");

   fToolBar->AddFrame(fCancelQueryButton,fMenuBarHelpLayout);
   fToolBar->AddFrame(fExecuteQueryButton,fMenuBarHelpLayout);

   fStatusBar = new TGStatusBar(fMain, 60, 10);
   int parts[] = { 25, 75 };
   fStatusBar->SetParts(parts, 2);
   lo = new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 3, 0);
   fMain->AddFrame(fStatusBar,lo);
   fButtonGroup = new TGCompositeFrame(fMain,60,10,kHorizontalFrame);
   fMain->AddFrame(fButtonGroup,lo);
   
   fFirstButton = new TGTextButton(fButtonGroup,"First",kFirst);
   fPrevButton = new TGTextButton(fButtonGroup,"Prev",kPrev);
   fNextButton = new TGTextButton(fButtonGroup,"Next",kNext);
   fLastButton = new TGTextButton(fButtonGroup,"Last",kLast);

   fButtonGroup->AddFrame(fFirstButton,fMenuBarLayout);
   fButtonGroup->AddFrame(fPrevButton,fMenuBarLayout);
   fButtonGroup->AddFrame(fNextButton,fMenuBarLayout);
   fButtonGroup->AddFrame(fLastButton,fMenuBarLayout);

   fCanvasWindow = new TGCanvas(fMain, 200, 400);
   lo = new TGLayoutHints( kLHintsBottom | 
                           kLHintsExpandX | 
                           kLHintsExpandY, 3, 3, 3, 3);
   fMain->AddFrame(fCanvasWindow,lo);

   fContainer = new TGCompositeFrame(fCanvasWindow->GetViewPort(), 10, 10,
                                     kHorizontalFrame);
   
   fContainer->SetLayoutManager(new TGVerticalLayout(fContainer));
   fCanvasWindow->SetContainer(fContainer);

   fMain->MapSubwindows();
   fMain->SetWMPosition(300,300);
   fMain->Layout();
   fMain->MapRaised();

   TSQL::SetHandler("Catch(TSQLException*)");
   HandleDisconnect();

   // connect signals  
   TQObject::Connect(fCancelQueryButton,"Pressed()","TGTextEntry",
                     fQueryTextEntry,"Clear()");

   fFileMenu->Connect("Activated(Int_t)","DbForm",this,
                      "HandleFileMenu(Int_t)");

   fGotoMenu->Connect("Activated(Int_t)","DbForm",this,
                      "HandleGotoMenu(Int_t)");

   fExecuteQueryButton->Connect("Released()","DbForm",this,
                      "HandleExecuteQuery()");
   
   fQueryTextEntry->Connect("ReturnPressed()","DbForm",this,
                      "HandleExecuteQuery()");   
   
   fNextButtonTimer = new TTimer(250); 
   fPrevButtonTimer = new TTimer(250); 

   fFirstButton->Connect("Released()","DbForm",this,"First()");
   fLastButton->Connect("Released()","DbForm",this,"Last()");
 
   // create accelerating autorepeat effect for Next/Prev buttons 
   fNextButton->Connect("Pressed()","TTimer",fNextButtonTimer,"TurnOn()");
   fNextButton->Connect("Released()","TTimer",fNextButtonTimer,"TurnOff()");   
   fNextButton->Connect("Released()","TTimer",fNextButtonTimer,"SetTime(=250)");   
   fNextButtonTimer->Connect("Timeout()","DbForm",this,"Next()");  //
   fNextButtonTimer->Connect("Timeout()","DbForm",this,"AccelerateNextButton()");  //

   fPrevButton->Connect("Pressed()","TTimer",fPrevButtonTimer,"TurnOn()");
   fPrevButton->Connect("Released()","TTimer",fPrevButtonTimer,"TurnOff()");
   fPrevButton->Connect("Released()","TTimer",fPrevButtonTimer,"SetTime(=250)");    
   fPrevButtonTimer->Connect("Timeout()","DbForm",this,"Prev()");
   fPrevButtonTimer->Connect("Timeout()","DbForm",this,"AcceleratePrevButton()");  //

   gForm = this;
   fPD = 0;
}

//____________________________________________________________________
void DbForm::AccelerateNextButton()
{
   // accelerator

   Long_t nt = fNextButtonTimer->GetTime();
   fNextButtonTimer->SetTime(nt-10);
}

//____________________________________________________________________
void DbForm::AcceleratePrevButton()
{
   // accelerator

   Long_t pt = fPrevButtonTimer->GetTime();
   fPrevButtonTimer->SetTime(pt-10);
}

//____________________________________________________________________
DbForm::~DbForm()
{
   // destructor

   delete fNextButtonTimer;
   delete fPrevButtonTimer;
   delete fContainer;
   delete fCanvasWindow;
   delete fQueryTextLayout;
   delete fBarLayout;
   delete fToolBar;
   delete fToolBarSep;
   delete fMenuBarHelpLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarLayout;
   delete fMenuBar;
   delete fStatusBar;
   delete fButtonGroup;
   delete fFirstButton;
   delete fLastButton;
   delete fPrevButton;
   delete fNextButton;
   delete fCancelQueryButton;
   delete fExecuteQueryButton;
   delete fQueryTextEntry;
   delete fHelpMenu;
   delete fGotoMenu;
   delete fFileMenu;  
   HandleDisconnect();
   delete fPD;
   delete fMain;
}

//____________________________________________________________________
void DbForm::UpdateColumn(Int_t index,const Text_t* value)
{
   // handler method used when column was updated 
   // (not used yet)
  
   TString str=value;

   if( fResultSet &&
       !fResultSet->IsBeforeFirst() && 
       !fResultSet->IsAfterLast() ) fResultSet->UpdateString(index,str);
}
   
//____________________________________________________________________
void DbForm::First()
{  
   // scroll to first row
   
   fResultSet->First();
   DisplayResults();
}

//____________________________________________________________________
void DbForm::Last()
{  
   // scroll to last row
      
   fResultSet->Last();
   DisplayResults();
}

//____________________________________________________________________
void DbForm::Next()
{  
   // scroll to next row
   
   fResultSet->Next();
   DisplayResults();
}

//____________________________________________________________________
void DbForm::Prev()
{
   // scroll to previous row
    
   fResultSet->Previous();
   DisplayResults();  
}

//____________________________________________________________________
void DbForm::DisplayResults()
{
   // display new row
   
   char rowstr[15];
   Int_t row = fResultSet->GetRow();
   sprintf(rowstr,"Row: %d",row);
   fStatusBar->SetText(rowstr,0);
   
   if(fResultSet->IsAfterLast()) {
     fGotoMenu->DisableEntry(kNext);
     fNextButton->SetState(kButtonDisabled);
   } else {
     fGotoMenu->EnableEntry(kNext);
     if(fNextButton->GetState()==kButtonDisabled) fNextButton->SetState(kButtonUp);
   }
     
   if(fResultSet->IsBeforeFirst()) {
     fGotoMenu->DisableEntry(kPrev);
     fPrevButton->SetState(kButtonDisabled);
   } else {
     fGotoMenu->EnableEntry(kPrev);
     if(fPrevButton->GetState()==kButtonDisabled) fPrevButton->SetState(kButtonUp);
   }
   
   if(fRstype == kTYPE_FORWARD_ONLY)  SetForwardOnly();  
        
   Int_t ncollumns = fResultSetMetaData->GetColumnCount();

   TString value; 
          
   for( int i=1; i <= ncollumns; ++i ) {
      if(!fResultSet->IsBeforeFirst() && !fResultSet->IsAfterLast()) {
         value = fResultSet->GetString(i);         
      } else {
         value = "";
      }
      fEntries[i-1]->SetColumnValue(value); 
   }   
}

//____________________________________________________________________
void DbForm::HandleFileMenu(Int_t id)
{
   // handler for File menu popup actions

   switch(id) {
   case kConnect:
      fConnected = kFALSE;
            
      if(!fPD) {
         fPD = new PasswordDialog();
         fPD->Connect("Entered(Text_t*,Text_t*,Text_t*)",
                               "DbForm",this,
                               "Connect(Text_t*,Text_t*,Text_t*)");
      } else {
         fPD->Show();
      }   
      break;   
   case kDisconnect:
      HandleDisconnect();
      break;
   case kExit:
      HandleDisconnect();
      gSystem->Exit(0);
      break;
   default:
      break;       
   }
}      

//____________________________________________________________________
void DbForm::HandleGotoMenu(Int_t id)
{
   // handler for Go menu popup actions

   switch(id) {
      case kFirst:
         First();
         break;   
      case kLast:
         Last();
         break;
      case kPrev:
         Prev();
         break;
      case kNext:
         Next();
         break;   
      default:
         break;       
   }
}      

//____________________________________________________________________
void DbForm::ClearCanvas()
{
   // clear canvas

   Int_t ncollumns;
   
   if(fResultSetMetaData) { // clear canvas window 
      TGCompositeFrame* cont = (TGCompositeFrame*)fCanvasWindow->GetContainer();
      TList* list = cont->GetList();
      ncollumns = fResultSetMetaData->GetColumnCount();

      for( int i=1; i <= ncollumns; ++i ) {
         delete fEntries[i-1];
      }

      list->Clear();
      cont->MapSubwindows();
      cont->Layout();
   }   
}
   
//____________________________________________________________________
void DbForm::HandleExecuteQuery()
{
   // handler for use to execute SQL query
   
   TString sql = fQueryTextEntry->GetText();
   Int_t ncollumns;
   char rowstr[15];
        
   if(sql.IsNull()) return;    // validate sql
     
   ClearCanvas();
    
   if(fStatement) delete fStatement;
   fStatement = fConnection->CreateStatement(fRstype,fRsconc);   
   
   if(!fStatement) {
      return;
   }
      
   fResultSet = fStatement->ExecuteQuery(sql);
   
   if(fResultSet) { 
      fStatusBar->SetText(sql.Data(),1);
      fResultSetMetaData = fResultSet->GetMetaData(); // new metadata
      if(fRstype != kTYPE_FORWARD_ONLY) fGotoMenu->EnableEntry(kFirst);
      if(fRstype != kTYPE_FORWARD_ONLY) fGotoMenu->EnableEntry(kPrev);
      fGotoMenu->EnableEntry(kNext);
      fGotoMenu->EnableEntry(kLast);
      if(fRstype != kTYPE_FORWARD_ONLY) fFirstButton->SetState(kButtonUp);
      if(fRstype != kTYPE_FORWARD_ONLY) fPrevButton->SetState(kButtonUp);
      fNextButton->SetState(kButtonUp);
      fLastButton->SetState(kButtonUp);
       
      Int_t row = fResultSet->GetRow();
      sprintf(rowstr,"Row: %d",row);
      fStatusBar->SetText(rowstr,0);
   
      ncollumns = fResultSetMetaData->GetColumnCount();
      Int_t type;
      
      fEntries = new DbFormEntryPtr[ncollumns];
      
      for( int i=1; i <= ncollumns; ++i ) {
         type = fResultSetMetaData->GetColumnType(i);
         fEntries[i-1] = new DbFormEntry(fCanvasWindow->GetContainer(),i,
                              TString(""),fResultSetMetaData->GetColumnName(i));

         fCanvasWindow->AddFrame(fEntries[i-1]->GetMain(), fMenuBarLayout);

//         fEntries[i-1]->Connect("Updated(Int_t,Text_t*)",
//                        "DbForm",this,"UpdateColumn(Int_t,Text_t*)");
      }
      
      if(fRstype == kTYPE_FORWARD_ONLY)  SetForwardOnly();  
      fMain->MapSubwindows();
      fMain->Layout();
   } else {
      sql = "Failed to execute " + sql;
      fStatusBar->SetText(sql.Data(),1);
      fGotoMenu->DisableEntry(kFirst);
      fGotoMenu->DisableEntry(kPrev);
      fGotoMenu->DisableEntry(kNext);
      fGotoMenu->DisableEntry(kLast);
      fFirstButton->SetState(kButtonDisabled);
      fPrevButton->SetState(kButtonDisabled);
      fNextButton->SetState(kButtonDisabled);
      fLastButton->SetState(kButtonDisabled);
      fQueryTextEntry->SetFocus();
      fStatusBar->SetText("",0); 
   }
}
      
//____________________________________________________________________
void DbForm::HandleDisconnect()
{
   // handler for disconnect from db 
   
   ClearCanvas();   
   if(fConnection) fConnection->Close();
   fConnection = 0;
   fConnected = kFALSE;
   fGotoMenu->DisableEntry(kFirst);
   fGotoMenu->DisableEntry(kPrev);
   fGotoMenu->DisableEntry(kNext);
   fGotoMenu->DisableEntry(kLast);
   fFirstButton->SetState(kButtonDisabled);
   fPrevButton->SetState(kButtonDisabled);
   fNextButton->SetState(kButtonDisabled);
   fLastButton->SetState(kButtonDisabled);
   fExecuteQueryButton->SetState(kButtonDisabled);
   fCancelQueryButton->SetState(kButtonDisabled);
   fFileMenu->DisableEntry(kDisconnect);
   fQueryTextEntry->SetEnabled(kFALSE);
   fQueryTextEntry->Clear(); 
   fStatusBar->SetText("Disconnected",1);  
   fStatusBar->SetText("",0);
   fResultSetMetaData=0;  
   fResultSet=0;  
   fStatement=0;
}

//____________________________________________________________________
void DbForm::HandleConnect()
{
   // handler used to connect to db 
   
   fExecuteQueryButton->SetState(kButtonUp);
   fCancelQueryButton->SetState(kButtonUp);
   fFileMenu->EnableEntry(kDisconnect);
   
   TString info = "Connected to DSN - ";
   info += fDSN;  
   fStatusBar->SetText(info.Data(),1);
   
   TSQLDatabaseMetaData* md = fConnection->GetMetaData();

   if(md->SupportsResultSetType(kTYPE_SCROLL_INSENSITIVE)) {
      fRstype = kTYPE_SCROLL_INSENSITIVE;
   } else if(md->SupportsResultSetType(kTYPE_SCROLL_SENSITIVE)) {
      fRstype = kTYPE_SCROLL_SENSITIVE;
   } else {
      SetForwardOnly();
   }

   if(md->SupportsResultSetConcurrency(fRstype,kCONCUR_READ_ONLY)) {
      fRsconc = kCONCUR_READ_ONLY;
   } else {
      fRsconc = kCONCUR_UPDATABLE;
   }

   fQueryTextEntry->SetEnabled(kTRUE);
   fQueryTextEntry->SetFocus();      
}      

//____________________________________________________________________
void DbForm::Connect(const Text_t* dsn,const Text_t* uid,const Text_t* pwd)
{
   // password dialog 
   
   HandleDisconnect();
   
   fConnection = TSQLDriverManager::GetConnection(dsn,uid,pwd);
   fConnected = fConnection != 0;
   
   if(fConnected) {
      fDSN = dsn;
      fPD->Hide();
      HandleConnect();
   }
}

//____________________________________________________________________
void DbForm::SetForwardOnly()
{
   // 
       
   fRstype = kTYPE_FORWARD_ONLY;
   fPrevButton->SetState(kButtonDisabled);
   fFirstButton->SetState(kButtonDisabled);  
   fLastButton->SetState(kButtonDisabled);   
   fGotoMenu->DisableEntry(kFirst);
   fGotoMenu->DisableEntry(kPrev);
   fGotoMenu->DisableEntry(kLast);   
}
      
//____________________________________________________________________
void Catch(TSQLException* e)
{
   // exception handling function
   // can be switched off by TSQL::UnsetHandler()

   EMsgBoxIcon icontype = kMBIconExclamation; 
   Int_t buttons = 0; 
   Int_t retval;

   TString str  = e->GetMessage();
   TGMsgBox* errb;

   if(gForm) {   
      errb = new TGMsgBox(gClient->GetRoot(),gForm->GetMainFrame(),
                          "Error", str.Data(),icontype, buttons, &retval);
   } else  {
      printf("%s\n",str.Data());
   }      
}

///////////////////////////////////////////////////////////////////////////////
void RDBCform()
{
   new DbForm();
}

//////////////////////////// Main program ////////////////////////////////////
#ifdef STANDALONE

#include <TROOT.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TInterpreter.h>

//---- Main program ------------------------------------------------------------

TROOT root("RDBCform", "GUI form for table browsing");

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }
   gDebug =1;
   gSystem->Load("libRDBC");
   gSystem->Load("./RDBCform_C");
   gInterpreter->ProcessLine("gForm = new DbForm();");
   theApp.Run();
   return 0;
}
#endif
