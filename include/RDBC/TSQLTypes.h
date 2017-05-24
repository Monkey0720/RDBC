// $Id: TSQLTypes.h,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $

#ifndef RDBC_TSQLTypes_h
#define RDBC_TSQLTypes_h

//
// TSQLTypes, TSQLDate, TSQLTime, TSQLTimestamp classes
//

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif
#ifndef ROOT_TDatime
#include <TDatime.h>
#endif

enum ESQLTypes { 
          kBIGINT = -5,
          kBINARY = -2,
          kBIT = -7,
          kCHAR = 1,
#ifdef ODBC_VER_LESS_30 
          kDATE = 9,
          kTIME = 10,
          kTIMESTAMP = 11,
#endif     
          kDATE = 91,
          kTIME = 92,
          kTIMESTAMP = 93,
          kSMALLINT = 5,
          kDECIMAL = 3,
          kDOUBLE = 8,
          kFLOAT = 6,
          kINTEGER = 4,
          kLONGVARBINARY = -4,
          kLONGVARCHAR = -1,
          kNUMERIC = 2,
          kREAL = 7,
          kTINYINT = -6,
          kVARBINARY = -3,
          kVARCHAR  = 12 
   };


/////////////////////////////////////////////////////////////////////
class TSQLDate: public TDatime
{
public:

   TSQLDate(Int_t year, Int_t month, Int_t day);
   TSQLDate():TDatime() {}
   TSQLDate(UInt_t t) { fDatime=t; } 
//    TSQLDate(const TString& str);
   TSQLDate(const TSQLDate& d):TDatime(d) {}
   virtual ~TSQLDate() {}
    
    TSQLDate& operator=(const TSQLDate& d); 

//    UInt_t GetTime() const { return Convert(); }
   Int_t GetYear() const { return  (fDatime>>26)+1995; }
   Int_t GetMonth() const { return (fDatime<<6)>>28; }
   Int_t GetDay() const { return (fDatime<<10)>>27; }
   void  SetYear(Int_t year);
   void  SetMonth(Int_t month);
   void  SetDay(Int_t day);
   TString ToString() const;

ClassDef(TSQLDate,0) // allows to identify this as a SQL_DATE
};

/////////////////////////////////////////////////////////////////////
class TSQLTime: public TDatime
{
public:

   TSQLTime(Int_t hour, Int_t minute, Int_t second);
   TSQLTime():TDatime() {}
   TSQLTime(UInt_t t) { fDatime=t; }
//   TSQLTime(const TString& str);
   TSQLTime(const TSQLTime& t):TDatime(t) {}
   virtual ~TSQLTime() {}

   TSQLTime& operator=(const TSQLTime& d);

//   UInt_t getTime() const { return Convert(); }    
   Int_t GetHour() const { return (fDatime<<15)>>27; }
   Int_t GetMinute() const { return (fDatime<<20)>>26; }
   Int_t GetSecond() const { return (fDatime<<26)>>26; }
   void  SetHour(Int_t h);
   void  SetMinute(Int_t m);
   void  SetSecond(Int_t s);
   TString ToString() const;

ClassDef(TSQLTime,0) // allows to identify this as a SQL_TIME
};

/////////////////////////////////////////////////////////////////////
class TSQLTimestamp: public TDatime
{
protected:
    Int_t fNanos; // nanoseconds

public:
 
   TSQLTimestamp(Int_t year, Int_t month, Int_t day,
                 Int_t hour, Int_t minute, Int_t second, Int_t nanos =0) 
   { Set(year,month,day,hour,minute,second); SetNanos(nanos); }
   
   TSQLTimestamp():TDatime(),fNanos(0) {}
   TSQLTimestamp(UInt_t t):fNanos(0) { fDatime=t; }
//   TSQLTimestamp(const TString& s);
   TSQLTimestamp(const TSQLTimestamp& t):TDatime(t),fNanos(0) {}
   virtual ~TSQLTimestamp() {}
   TSQLTimestamp& operator=(const TSQLTimestamp& d); 

//   UInt_t GetTime() const { return Convert(); } 
   Int_t GetYear() const { return  (fDatime>>26)+1995; }
   Int_t GetMonth() const { return (fDatime<<6)>>28; }
   Int_t GetDay() const { return (fDatime<<10)>>27; }
   Int_t GetHour() const { return (fDatime<<15)>>27; }
   Int_t GetMinute() const { return (fDatime<<20)>>26; }
   Int_t GetSecond() const { return (fDatime<<26)>>26; }
   Int_t GetNanos() const { return fNanos; }  
   void  SetYear(Int_t year);
   void  SetMonth(Int_t month);
   void  SetDay(Int_t day);
   void  SetHour(Int_t h);
   void  SetMinute(Int_t m);
   void  SetSecond(Int_t s);
   void  SetNanos(Int_t nanos) { fNanos = nanos>0 ? nanos: 0; }
//   Bool_t After(const TSQLTimestamp& t) const;
//   Bool_t Before(const TSQLTimestamp& t) const;
//   Bool_t Equals(const TSQLTimestamp& t) const;   
   TString ToString() const;

ClassDef(TSQLTimestamp,0) // allows to identify this as a SQL_TIMESTAMP 
};

//___________________________________________________________________
inline TSQLDate& TSQLDate::operator=(const TSQLDate& d) 
{
   // Assignment operator
   
   fDatime = d.fDatime;
   return *this;
}

//___________________________________________________________________
inline TSQLTime&  TSQLTime::operator=(const TSQLTime& d) 
{
   // Assignment operator
   
   fDatime = d.fDatime;
   return *this;
}

//___________________________________________________________________
inline TSQLTimestamp&  TSQLTimestamp::operator=(const TSQLTimestamp& d) 
{
   // Assignment operator
   
   fDatime = d.fDatime;
   fNanos = d.fNanos; 
   return *this;
}

#endif //  RDBC_TSQLTypes_h
