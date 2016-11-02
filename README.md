With this program, you can generates korean street based address data from the official site to the database
You might check the bin directory, there are several batch files of windows.

Some conditions must be proceeded prior to execute the programs in the bin directory.

First, you must have oracle instant client library from oracle site.
In my opinion, you have to download two versions that is precompiled and basic each archive version.

Extrat each files and copy all files together in one directory.

Second, you make your build environment for this program.
Msys2 + MinGW + gcc will be the best as same env. as my case.

Third, here are my system variables

SET ORACLE_HOME=C:\DEV\COMP\OracleClient32

SET PATH=.;%PATH%;%ORACLE_HOME%;%ORACLE_HOME%\sdk:.

SET NLS_LANG=AMERICAN_AMERICA.KO16KSC5601

The ORACLE_HOME variable must be right directory in which oracle instant client is installed.

If you want to save data only to sqlite database, you don't need the whole steps above.
You can do this just adjust some part of make file.

Enjoy.

Any problems are very welcome to me.

tommybee@naver.com

