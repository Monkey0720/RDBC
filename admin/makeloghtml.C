{
   G__loadfile("./include/RDBC/config.h");
   if(!gSystem->AccessPathName("./odbc/.libs/libRDBC.so")) gSystem->Load("./odbc/.libs/libRDBC.so");
   else gSystem->Load("libRDBC");

   TString  comment = RDBC_PACKAGE;
            comment += " Version ";
            comment += RDBC_VERSION;
            comment += " Release Notes";

   cout << endl;
   cout << "Generating hyperized version of ChangeLog in directory htmldoc/examples..." << endl;
   cout << endl;
   
   gEnv->SetValue("Unix.*.Root.Html.SourceDir","./:odbc/:include/RDBC/",kEnvChange);
   gEnv->SetValue("Root.Html.OutputDir","htmldoc",kEnvChange);
   gEnv->SetValue("Root.Html.Root","http://root.cern.ch/root/html",kEnvChange);

   THtml html; 
   html.Convert("ChangeLog",comment.Data());
}
