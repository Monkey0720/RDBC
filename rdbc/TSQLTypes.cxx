// $Id: TSQLTypes.cxx,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//*-- Author : Valeriy Onuchin 14/02/2000
//

////////////////////////////////////////////////////////////////////
//
//
// ESQLTypes::kBIGINT =    SQL BIGINT        
// ESQLTypes::kBINARY =    SQL BINARY (fixed length) 
// ESQLTypes::kBIT    =    SQL BIT    
// ESQLTypes::kCHAR   =    SQL CHAR (fixed length)     
// ESQLTypes::kDATE   =    SQL DATE  
// ESQLTypes::kDECIMAL=    SQL DECIMAL (precision,scale) 
// ESQLTypes::kDOUBLE =    SQL DOUBLE 
// ESQLTypes::kFLOAT  =    SQL FLOAT 
// ESQLTypes::kINTEGER =   SQL INTEGER 
// ESQLTypes::kLONGVARBINARY= SQL LONGVARBINARY (variable length, huge) 
// ESQLTypes::kLONGVARCHAR =  SQL LONGVARCHAR (variable length, huge) 
// ESQLTypes::kNUMERIC =   SQL NUMERIC (precision,scale) 
// ESQLTypes::kREAL =      SQL REAL   
// ESQLTypes::kSMALLINTSQL =  SMALLINT 
// ESQLTypes::kTIME =      SQL TIME 
// ESQLTypes::kTIMESTAMP = SQL TIMESTAMP 
// ESQLTypes::kTINYINT =   SQL TINYINT 
// ESQLTypes::kVARBINARY = SQL VARBINARY (variable length less than 256)
// ESQLTypes::kVARCHAR =   SQL VARCHAR (variable length less than 256) 
//
// See also:
//             TSQL
//
// TSQLDate TSQLTime TSQLTimestamp
//
////////////////////////////////////////////////////////////////////

#include <RDBC/TSQLTypes.h>

ClassImp(TSQLDate)
ClassImp(TSQLTime)
ClassImp(TSQLTimestamp)

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLDate::TSQLDate( Int_t year, Int_t month, Int_t day ) 
{
   // ctor
   
   Set(year,month,day,0,0,0);
}

//___________________________________________________________________
void TSQLDate::SetYear( Int_t year ) 
{
   // Sets the year of this date
   
   Set( year*10000+100*GetMonth()+GetDay(),GetTime() );
}

//___________________________________________________________________
void TSQLDate::SetMonth( Int_t month ) 
{
   // Sets the month of this date
    
   Set( 10000*GetYear()+100*month+GetDay(),GetTime() );
}

//___________________________________________________________________
void TSQLDate::SetDay( Int_t day ) 
{
   //  Sets the day of this date 

   Set( 10000*GetYear()+100*GetMonth()+day,GetTime() );
}

//___________________________________________________________________
TString TSQLDate::ToString() const
{
   // Gets the date as a string in the YYYY-MM-DD format
   // not implemented ...

   TString str;
   str = AsSQLString();
   return str;
}

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TSQLTime::TSQLTime( Int_t hour, Int_t minute, Int_t second ) 
{
   // ctor
   
   Set(0,0,0,hour,minute,second);
}

//___________________________________________________________________
void TSQLTime::SetHour( Int_t hour ) 
{
   // sets hour of this time
   
   Set( GetDate(),10000*hour+100*GetMinute()+GetSecond() );
}

//___________________________________________________________________
void TSQLTime::SetMinute( Int_t minute ) 
{
   // Sets the minute of this time
    
   Set( GetDate(),10000*GetHour()+100*minute+GetSecond() );
}

//___________________________________________________________________
void TSQLTime::SetSecond( Int_t second ) 
{
   //  Sets the second of this time 

   Set( GetDate(),10000*GetHour()+100*GetMinute()+second );
}

//___________________________________________________________________
TString TSQLTime::ToString() const
{
   // Gets the date as a string in the  HH:MM:SS format
   // not implemented

   TString str;
   str = AsSQLString();
   return str;
}

/////////////////////////////////////////////////////////////////////
//___________________________________________________________________
TString TSQLTimestamp::ToString() const
{
   // Gets the date as a string in the 
   // YYYY-MM-DD HH:MM:SS[.NNN...] format
   // not implemented
   
   TString str;
   str = AsString();
   return str;
}

//___________________________________________________________________
void TSQLTimestamp::SetYear( Int_t year ) 
{
   // Sets the year of this date
   
   Set( year*10000+100*GetMonth()+GetDay(),GetTime() );
}

//___________________________________________________________________
void TSQLTimestamp::SetMonth( Int_t month ) 
{
   // Sets the month of this date
    
   Set( 10000*GetYear()+100*month+GetDay(),GetTime() );
}

//___________________________________________________________________
void TSQLTimestamp::SetDay( Int_t day ) 
{
   //  Sets the day of this date 

   Set( 10000*GetYear()+100*GetMonth()+day,GetTime() );
}

//___________________________________________________________________
void TSQLTimestamp::SetHour( Int_t hour ) 
{
   // Sets hour of this time
   
   Set( GetDate(),10000*hour+100*GetMinute()+GetSecond() );
}

//___________________________________________________________________
void TSQLTimestamp::SetMinute( Int_t minute ) 
{
   // Sets the minute of this time
    
   Set( GetDate(),10000*GetHour()+100*minute+GetSecond() );
}

//___________________________________________________________________
void TSQLTimestamp::SetSecond( Int_t second ) 
{
   //  Sets the second of this time 

   Set( GetDate(),10000*GetHour()+100*GetMinute()+second );
}
