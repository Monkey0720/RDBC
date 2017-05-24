
               The RDBC:   ROOT Data Base Connectivity

=============================================================================


   The current RDBC's home page is at 

        http://www.phenix.bnl.gov/WWW/publish/onuchin/RDBC

Please be sure to visit this site for information, documentation, news, etc.



                           Introduction to ROOT
                           ====================


ROOT is a collection of applications, utilities and C++ libraries that
is designed for analyzing data.  ROOT is particularly well suited to
analyzing large amounts of data, as it has been specially optimized for
this purpose (ROOT can easily handle terabyte data samples).

Included with ROOT are histograms in 1, 2 and 3 dimensions, and
facilities for curve fitting, function evaluation and minimization.
Graphics and visualization classes allow the easy setup of an analysis
system that can query and process the data interactively or in batch
mode.

Thanks to the builtin C++ interpreter (called `cint') the command
language, scripting (or macro) language and programming language are all
one in the same, namely C++.

`cint' allows for fast prototyping of macros since it removes the time
consuming compile/link cycle.  It also provides a good environment to
learn C++.  If more performance is needed then the interactively
developed C++ macros can be compiled using a C++ compiler and then
dynamically linked in at runtime.

ROOT is an open system that can be dynamically extended by linking
with external libraries.  This makes ROOT a premier platform on which
to build data acquisition, simulation and data analysis systems.

The ROOT home page is http://root.cern.ch/.



                           Introduction to RDBC
                           ====================
                           
                           
This library is ROOT wrappers around libodbc++ <http://orcane.net/freeodbc++>
Author: Manush Dodunekov <manush@stendahls.net>

This library aims for JDBC-2.0 'compliance' 
<http://java.sun.com/j2se/1.3/docs/api/java/sql/package-summary.htm>

It provides the single application programming interface (API) for accessing  
different types of relational databases.
 
There's an almost complete class reference and documentation residing in

http://www.phenix.bnl.gov/WWW/publish/onuchin/RDBC



                           Directory Contents
                           ==================


INSTALL        - generic installation instructions
INSTALL.local  - instructions for "local installation" of main components 
                 ( no system privileges required ): root,unixODBC,MySQL-client,MyODBC,RDBC 
README         - this file
include        - RDBC header files
libodbc++*     - libodbc++ source directory
odbc           - source code for the ROOT wrappers of libodbc++ (ROOT-ODBC bridge)
macros         - test suite macros



--
Valeriy Onuchin <onuchin@sirius.ihep.su>
IHEP, Protvino, Moscow region, Russia

$Id: README,v 1.1.1.1 2004/02/18 20:58:02 dave Exp $
