{
   TString cfg = "./include/RDBC/config.h";
   TString odbclib = "./odbc/.libs/libRDBCodbc.so"; 
   TString rdbclib = "./rdbc/.libs/libRDBC.so"; 

   if(gSystem->AccessPathName(odbclib.Data())) gSystem->Exec("gmake");

   gSystem->Load(rdbclib.Data());
   gSystem->Load(odbclib.Data());

   G__loadfile(cfg.Data());

   TString comment = "Generating HTML class docs for ";
   comment += RDBC_PACKAGE;
   comment += "-";
   comment += RDBC_VERSION;
   comment += " in directory htmldoc/...";

   cout << endl;
   cout << comment << endl;
   cout << endl;

   gEnv->SetValue("Root.Html.Author","//*-- Author :",kEnvChange);
   gEnv->SetValue("Root.Html.LastUpdate","//*-- Modified :",kEnvChange); 
   gEnv->SetValue("Root.Html.HomePage","http://www.phenix.bnl.gov/WWW/publish/onuchin/RDBC",kEnvChange);     
   gEnv->SetValue("Unix.*.Root.Html.SourceDir","rdbc/:odbc/:include/RDBC/",kEnvChange);
   gEnv->SetValue("Root.Html.OutputDir","htmldoc",kEnvChange);
   gEnv->SetValue("Root.Html.Root","http://root.cern.ch/root/html",kEnvChange);

   THtml html;
   html.MakeAll();
}
