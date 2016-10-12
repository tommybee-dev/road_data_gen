//static char rcsid[] = "$Id: shell_log.c,v 1.3 1994/03/21 05:15:53 dhb Exp $";

/*
** $Log: shell_log.c,v $
** Revision 1.3  1994/03/21  05:15:53  dhb
** Added include "header.h" for getopt() externs.
**
** Revision 1.2  1994/03/21  02:44:17  dhb
** 2.0 command argument changes.
** 	do_logon() accepts the same args, but will close an open log and
** 	open a new one if both the -off and file name appear together.
**
** Revision 1.1  1992/12/11  19:04:37  dhb
** Initial revision
**http://web.cs.ucla.edu/classes/cs190/genesis/src/shell/shell_log.c
*/

/* mds3 changes */
/* Changed various 'printf/fprintf' statements to 'vprintf/vfprintf' */
#include <stdio.h>
//#include <varargs.h>
#include <stdarg.h>
#include <time.h>
//#include "header.h"

//typedef struct {
//	int	arg[50];
//} VARG;

char	*ctime();

static 	FILE	*logfp = NULL;
static 	long	log_clock = 0;
static	int	monitor_time = 0;
static	int	monitor_interval = 1;

/*
int do_logon(int argc, char	**argv) 
{ 
    int	status;

    initopt(argc, argv, "[file-name] -off");
    while ((status = getopt(argc, argv)) == 1) {
	if (strcmp(optopt, "-off") == 0)
	    do_logoff();
    }

    if (status != 0 || optargc > 2) {
	printoptusage(argc, argv);
	return 0;
    }

    if (optargc == 2) {
	do_logoff();
	OpenLog(optargv[1]);
	LogHeader(optargv[1]);
    }

    return 1;
}
*/

void LogTrailer()
{
	//char *timestr;
	
	if(logfp){
		//timestr = Time();
		time_t rawtime;
		struct tm * timeinfo;
		
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		
		printf("\nlogging done at %s\n",asctime (timeinfo));
		fprintf(logfp,"\nlogging done at %s\n",asctime (timeinfo));
	}
}

void CloseLog()
{
    fclose(logfp);
    logfp = NULL;
}

void do_logoff() 
{ 
	if(logfp){
		LogTrailer();
		CloseLog();
	}
}

void OpenLog(char *file)
{
	if((logfp = fopen(file,"a")) == NULL){
		//Error();
		printf("unable to write to logfile '%s'\n",file);
	}
}

void LogTime()
{
	long	clock;

    /*
    ** check the monitor time
    */
    if(logfp && monitor_time > 0){
		time(&clock);
		if(clock - log_clock >= monitor_interval){
		    log_clock = clock;
		    /*
		    ** print the time
		    */
		    fprintf(logfp,"===> %s\n",ctime(&log_clock));
		}
    }
}

char *Time()
{
	long	clock;
	char	*timestr;
	
    time(&clock);
    timestr = ctime(&clock);
    
    return(timestr);
}

void LogHeader(char *file)
{
	char *timestr;

    if(logfp){
		timestr = Time();
		printf("\nlogging to '%s' at %s\n",file,timestr);
		fprintf(logfp,"\nlogging started at %s\n",timestr);
    }
}



void lprintf(char *com, ...)
{
	va_list ap;

#ifdef LATER
	char	*arg[100];
	char	*com;
	char	*ptr;
	int	nargs;

    va_start(ap, com);
    /*
    ** determine the number of arguments based on the format string
    */
    nargs = 0;
    for(ptr=com;ptr;ptr++){
	if(*ptr == '%') nargs++;
    }
    for(i=0;i<nargs;i++){
	arg[i] = va_arg(ap, char *);
    }
    va_end(ap);
#endif
    /*
    ** write to stdout
    */
/* mds3 changes */
/*  printf(com,va_alist); */
    va_start(ap, com);
    vprintf(com,ap);
    /*
    ** is logging selected
    */
    if(logfp){
		LogTime();
		/*
		** write to the log
		*/
	/* mds3 changes */
	/*  	fprintf(logfp,com,va_alist); */
		va_start(ap, com);
		vfprintf(logfp,com,ap);
    }
}

void lprint_only(char *com, ...) 
{
/* mds3 changes */
/* Added single line below */
  va_list ap;
  if(logfp){
    LogTime();
    /*
     ** write to the log
     */
/* mds3 changes */
/*    fprintf(logfp,com,va_alist); */
    va_start(ap, com);
    vfprintf(logfp,com,ap);
  }
}

