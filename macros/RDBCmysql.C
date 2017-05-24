// $Id: RDBCmysql.C,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
//
//   !!! not working yet !!!
//  This is a simple test to be run against a mysql database
//  Assure you have create table permissions before you run.
//

#ifndef __CINT__   
// g++ -c -Wall RDBCmysql.C -I$ROOTSYS/include -I../include

#include <RDBC/TSQLDriverManager.h>
#include <RDBC/TSQLConnection.h>
#include <RDBC/TSQLResultSet.h>
#include <RDBC/TSQLResultSetMetaData.h>
#include <RDBC/TSQLPreparedStatement.h>

#endif // __CINT__ 


const char* testchars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

const int odbctestRows=5000;

const int longtestRows=30;
const int longtestSize=123456;

int assertionsFailed=0;

//___________________________________________________________________
Bool_t feq(const double& a, const double& b)
{
   //
   return (fabs(a-b)<=a*0.0000001);
}

//////////////////////////////////////////////////////////////////////
class ValueGen1 
{
private:
   Int_t fCnt;
   Int_t fMax;

public:
   ValueGen1(Int_t max) : fCnt(0), fMax(max) {}
   ~ValueGen1() {}
  
   Bool_t Next() { return ((++fCnt) <= fMax); }
   Int_t  cnt() const {return fCnt; }

   signed char i1() const { return (signed char)(fCnt%256 - 128); }
   short       i2() const { return (short)(fCnt%65536 - 32768); }
   int         i3() const { return (int)(fCnt%16777215-8388608); }
   int         i4() const { return (int)fCnt; }
   Long_t      i5() const { return ((Long_t)fCnt)*1000000L; }
   float       f1() const { return ((float)fCnt); }
   float       f2() const { return ((float)fCnt/10); }
   double      f3() const { return ((double)fCnt/20);}
   Long_t      d1() const { return (((Long_t)fCnt)-fMax/2)*4567;}
   TString     s1() const { return TString(&testchars[fCnt%strlen(testchars)],1); }
   TString     s3() const { return this->s1(); }
   TString     s2() const { char buf[41];
                            snprintf(buf,41,"This is row number %d",fCnt);
                            return TString_C(buf); }
   TString     s4() const { return this->s2(); }

  Date dt() {
    return Date((time_t)fCnt*10000);
  }

  Time t() {
    return Time((time_t)fCnt);
  }

  Timestamp ts() {
    return Timestamp((time_t)fCnt*10000);
  }
//this generates the values for odbctest
};


//////////////////////////////////////////////////////////////////////
class ValueGen2 {
private:
  Int_t fCnt;
  Int_t rows_;

  char currentC[longtestSize+1];
  char currentB[longtestSize];

public:
  ValueGen2(Int_t rows)
    :fCnt(0), rows_(rows) {}
  ~ValueGen2() {}

  Bool_t Next() {
    return ((++fCnt) <= rows_);
  }

  int id() { 
    return fCnt; 
  }

  TBuffer* c() {
    int len=strlen(testchars);
    for(int i=0; i<longtestSize; i++) {
      currentC[i]=testchars[(i+fCnt)%len];
    }
    currentC[longtestSize]=0;
#if !defined(ODBCXX_QT)
    isstream* s=new isstream(currentC,longtestSize);
#else
    QBuffer* s=new QBuffer();
    s->open(IO_WriteOnly);
    s->writeBlock(currentC,longtestSize);
    s->close();
    s->open(IO_ReadOnly);
#endif
    return s;
  }

  TBuffer* b() {
    for(int i=0; i<longtestSize; i++) {
      currentB[i]=((i+fCnt)%256);
    }
#if !defined(ODBCXX_QT)
    isstream* s=new isstream(currentB,longtestSize);
#else
    QBuffer* s=new QBuffer();
    s->open(IO_WriteOnly);
    s->writeBlock(currentB,longtestSize);
    s->close();
    s->open(IO_ReadOnly);
#endif
    return s;
  }

//this is for the BLOB/CLOB test
};

//___________________________________________________________________
Bool_t compareStreams(TBuffer* s1, TBuffer* s2)
{
  Int_t cnt=0;
#if !defined(ODBCXX_QT)
  char c1, c2;
  while(s1->get(c1) && s2->get(c2)) {
    cnt++;
    if(c1!=c2)
      return false;
  }
#else
  char buf1[1024];
  char buf2[1024];
  int r1, r2;
  while((r1=s1->readBlock(buf1,1024))!=-1 && 
	(r2=s2->readBlock(buf2,1024))!=-1) {
    if(r1!=r2) return false;

    for(int i=0; i<r1; i++) {
      cnt++;
      if(buf1[i]!=buf2[i]) return false;
    }
  }
#endif
  return (cnt==longtestSize);
}

//___________________________________________________________________
void createTables(TSQLConnection* con)
{
  cout << "Creating tables:" << flush;
  TSQLPreparedStatement* pstmt = con->PrepareStatement
    ("create table odbctest ("
     "i1 tinyint not null, "
     "i2 smallint not null, "
     "i3 mediumint not null, "
     "i4 int not null, "
     "i5 bigint not null, "
     "f1 float(4) not null, "
     "f2 float(8) not null, "
     "f3 double(10,3) not null, "
     "d1 decimal(20,5) not null, "
     "s1 char(1) not null, "
     "s2 char(40) not null, "
     "s3 varchar(1) not null, "
     "s4 varchar(40) not null, "
     "dt date not null, "
     "t time not null, "
     "ts datetime not null"
     ")");
  pstmt->executeUpdate();
  cout << " odbctest" << flush;
  delete pstmt;

  pstmt=con->PrepareStatement
    ("create table odbctest2 ("
     "id int not null, "
     "c mediumtext, "
     "b mediumblob)");
  pstmt->executeUpdate();
  cout << " odbctest2" << flush;

  delete pstmt;
  
  cout << endl;
}

//___________________________________________________________________
void populateTables(TSQLConnection* con)
{
  
  cout << "Populating:" << flush;

  PreparedStatement* pstmt=con->PrepareStatement
    ("insert into odbctest(i1,i2,i3,i4,i5,f1,f2,f3,d1,s1,s2,s3,s4,dt,t,ts) "
     "values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

  ValueGen1 vg(odbctestRows);
  while(vg.Next()) {
    pstmt->setByte(1,vg.i1());
    pstmt->setShort(2,vg.i2());
    pstmt->setInt(3,vg.i3());
    pstmt->setInt(4,vg.i4());
    pstmt->setLong(5,vg.i5());
    pstmt->setFloat(6,vg.f1());
    pstmt->setFloat(7,vg.f2());
    pstmt->setDouble(8,vg.f3());
    pstmt->setLong(9,vg.d1());
    pstmt->setString(10,vg.s1());
    pstmt->setString(11,vg.s2());
    pstmt->setString(12,vg.s3());
    pstmt->setString(13,vg.s4());
    pstmt->setDate(14,vg.dt());
    pstmt->setTime(15,vg.t());
    pstmt->setTimestamp(16,vg.ts());
    pstmt->executeUpdate();
  }
  
  delete pstmt;
  cout << " odbctest (" << odbctestRows << ")" << flush;
  
  pstmt=con->PrepareStatement
    ("insert into odbctest2(id,c,b) values(?,?,?)");

  ValueGen2 vg2(longtestRows);
  while(vg2.Next()) {
    pstmt->setInt(1,vg2.id());
    TBuffer* cs=vg2.c();
    pstmt->setAsciiStream(2,cs,longtestSize);
    TBuffer* bs=vg2.b();
    pstmt->setBinaryStream(3,bs,longtestSize);
   
    pstmt->executeUpdate();

    delete cs;
    delete bs;
  }

  // insert an extra row containing NULL for c and b
  pstmt->setInt(1,vg2.id()+1);
  pstmt->setNull(2,Types::LONGVARCHAR);
  pstmt->setNull(3,Types::LONGVARBINARY);
  pstmt->executeUpdate();

  delete pstmt;
  
  cout << " odbctest2 (" << longtestRows << ")" << flush;

  cout << endl;
}

//___________________________________________________________________
void checkTables(TSQLConnection* con)
{
  cout << "Checking:" << flush;

  Statement* stmt=con->createStatement();
  stmt->setFetchSize(37); // some odd number
  ResultSet* rs=stmt->executeQuery
    ("select i1,i2,i3,i4,i5,f1,f2,f3,d1,s1,s2,s3,s4,dt,t,ts from odbctest");

  cout << " odbctest" << flush;
  
  ValueGen1 vg(odbctestRows);
  Int_t cnt=0;
  while(rs->Next() && vg.Next()) {
    cnt++;

    Assert(vg.cnt()==rs->GetRow());

    Assert(vg.i1()==rs->GetByte(1) && 
	   vg.i1()==rs->GetByte("i1"));
    Assert(vg.i2()==rs->GetShort(2) &&
	   vg.i2()==rs->GetShort("i2"));
    Assert(vg.i3()==rs->GetInt(3) && 
	   vg.i3()==rs->GetInt("i3"));
    Assert(vg.i4()==rs->GetInt(4) &&
	   vg.i4()==rs->GetInt("i4"));
    Assert(vg.i5()==rs->GetLong(5) && 
	   vg.i5()==rs->GetLong("i5"));
    
    Assert(feq(vg.f1(),rs->GetFloat(6)) &&
	   feq(vg.f1(),rs->GetFloat("f1")));
    Assert(feq(vg.f2(),rs->GetFloat(7)) &&
	   feq(vg.f2(),rs->GetFloat("f2")));
    Assert(feq(vg.f3(),rs->GetDouble(8)) &&
	   feq(vg.f3(),rs->GetDouble("f3")));

    Assert(vg.d1()==rs->GetLong(9) &&
	   vg.d1()==rs->GetLong("d1"));

    Assert(vg.s1()==rs->GetString(10)
	   && vg.s1()==rs->GetString("s1"));
    Assert(vg.s2()==rs->GetString(11) && 
	   vg.s2()==rs->GetString("s2"));
    Assert(vg.s3()==rs->GetString(12) &&
	   vg.s3()==rs->GetString("s3"));
    Assert(vg.s4()==rs->GetString(13) &&
	   vg.s4()==rs->GetString("s4"));

    Assert(vg.dt().toString()==rs->GetString(14));
    Assert(vg.t().toString()==rs->GetString(15));
    Assert(vg.ts().toString()==rs->GetString(16));
  }

  delete rs;
  delete stmt;

  Assert(cnt==odbctestRows);
  cout << "(" << cnt << ")" << flush;

  stmt=con->createStatement();
  stmt->setFetchSize(10);
  rs=stmt->executeQuery("select id,c,b from odbctest2");
  // since we have LONGVARwhatevers in the result set
  // this should fall down to 1
  Assert(rs->GetFetchSize()==1);
  
  ValueGen2 vg2(longtestRows);
  cnt=0;
  cout << " odbctest2" << flush;
  while(rs->Next() && vg2.Next()) {
    cnt++;
    Assert(vg2.id()==rs->GetInt("id"));
    TBuffer* cs=vg2.c();
    TBuffer* bs=vg2.b();
    Assert(compareStreams(cs,rs->GetAsciiStream("c")));
    Assert(compareStreams(bs,rs->GetBinaryStream("b")));
    delete cs;
    delete bs;
  }

  // here, rs->next has been called an extra time above 
  // (we're at the row containing NULL values)
  TBuffer* tmp=rs->GetAsciiStream("c");
  Assert(rs->WasNull());
  tmp=rs->GetBinaryStream("b");
  Assert(rs->WasNull());
  
  Assert(cnt==longtestRows);
  cout << "(" << cnt << ")" << flush;

  delete rs;
  delete stmt;

  cout << endl;
}

//___________________________________________________________________
Bool_t dropTable(TSQLConnection* con, const TString& tableName)
{
  Bool_t r=true;
  TSQLPreparedStatement* pstmt=con->PrepareStatement
    ("drop table "+tableName);
  try {
    pstmt->executeUpdate();
  } catch(SQLException& e) { r=false; }

  delete pstmt;
  return r;
}

//___________________________________________________________________
void dropTables(TSQLConnection* con) 
{
  cout << "Dropping tables:" << flush;
  if(dropTable(con,"odbctest")) {
    cout << " odbctest" << flush;
  }

  if(dropTable(con," odbctest2")) {
    cout << " odbctest2" << flush;
  }
  cout << endl;
}


#include "../src/dtconv.h"

////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  if(argc!=2 && argc!=4) {
    cerr << "Usage: " << argv[0] << " connect-string" << endl
	       << "or     " << argv[0] << " dsn username password" << endl;
    return 0;
  }


  try {
    Connection* con;
    if(argc==2) {
      cout << "Connecting to " << argv[1] << "..." << flush;
      con=TSQLDriverManager::GetConnection(argv[1]);
    } else {
      cout << "Connecting to dsn=" << argv[1]
	   << ", uid=" << argv[2] 
	   << ", pwd=" << argv[3] << "..." << flush;
      con=TSQLDriverManager::GetConnection(argv[1],argv[2],argv[3]);
    }
    cout << " done." << endl;

    dropTables(con);
    createTables(con);

    populateTables(con);

    checkTables(con);

    dropTables(con);
    
    delete con;

    DriverManager::shutdown();

    if(assertionsFailed) {
      cerr << assertionsFailed << " assertions failed" << endl;
      return 1;
    } else {
      cout << "Apparently, this worked!" << endl;
    }

    
  } catch(SQLException& e) {
    cerr << endl << e.getMessage() << endl;
    return 2;
  }

  return 0;
}
