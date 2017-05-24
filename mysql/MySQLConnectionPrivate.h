// $Id: MySQLConnectionPrivate.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_MySQLConnectionPrivate_h
#define RDBC_MySQLConnectionPrivate_h

//
// Very private MySQL part of TSQLConnection
//

#include <mysql.h>

/////////////////////////////////////////////////////////////////////
class MySQLConnectionPrivate 
{
friend class TSQLConnection;

private:
   static const Int_t fgCheckIfAlive;  //Seconds between queries for ping (MyODBC-2.50.36)

   static Int_t fgMaxAllowedPacket = 65536;
   static Int_t fgNetBufferLength = 16384;
   static Int_t fgMaxReconnects = 3;
   static Int_t fgInitialTimeout = 2.0;

//static const TString gPING_COMMAND = "SELECT 1";


   MYSQL*   fMYSQL; //
   Bool_t   fIsConnected; //
   Bool_t   fLocked;   //

   Bool_t   fReadOnly;  //
   TSQLUrl  fURL;       //
   time_t   fLastQueryTime; //
   Int_t    fLoginTimeout; //

   MySQLConnectionPrivate() { mysql_init(fMYSQL); }

public:
   Bool_t CheckIfServerIsAlive();
   
};


const Int_t MySQLConnectionPrivate::fgCheckIfAlive = 3600;

//___________________________________________________________________
Bool_t MySQLConnectionPrivate::CheckIfServerIsAlive()
{
   //
   time_t seconds = (time_t) time((time_t*) 0);
   Bool_t result  = kFALSE;

   if ((ULong_t) (seconds - fLastQueryTime) >= fgCheckIfAlive) {
      result = (mysql_ping(&dbc->mysql) && 
               mysql_errno(&dbc->mysql) == CR_SERVER_GONE_ERROR);
   }
  
   fLastQueryTime = seconds;
   return result;
}


#endif // RDBC_MySQLConnectionPrivate_h
