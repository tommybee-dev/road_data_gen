#include "road_name.h"


#include <iconv.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include <ctype.h>

static dictionary * ini;

void create_dir(char *dir)
{
#ifdef __MINGW32__
	if (mkdir(dir) < 0) {
#else
	if (mkdir(dir, 0755) < 0) {
#endif    
        if (errno != EEXIST) {
            perror(dir);
            exit(1);
        }
    }
}

int has_hangul(char *input_text)
{
	int is_hangul = 0;
	char *tmp_text;
	
	tmp_text = input_text;
	
	while(*tmp_text){
		char letter = *tmp_text;
        if((letter & 0x80))
        {
        	is_hangul = 1;           
        	break;         
        }
		tmp_text++;
	}
	return is_hangul;
}

void load_ini(const char *szFileName)
{
	ini = iniparser_load((char*)szFileName);
	if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", szFileName);
        return;
    }
    /*iniparser_dump(ini, stderr);*/

}

void unload_ini()
{
	if(ini) iniparser_freedict(ini);
	
	ini = NULL;
}

//http://stackoverflow.com/questions/8512958/is-there-a-windows-variant-of-strsep
char* strsep_cust(char** stringp, const char* delim)
{
  char* start = *stringp;
  char* p;

  p = (start != NULL) ? strpbrk(start, delim) : NULL;

  if (p == NULL)
  {
    *stringp = NULL;
  }
  else
  {
    *p = '\0';
    *stringp = p + 1;
  }

  return start;
}


static iconv_t conv_st;
static char convbuf[1024];

void open_iconv(const char *src, const char *dest)
{
	conv_st = iconv_open( src, dest  );
}


int conv_str( char *outBuf, size_t *outLength, char *inBuf, size_t inLength, const char *sEnc, const char *tEnc )
{
	//iconv_t cd = iconv_open( "EUC-KR", "UTF-8" );
	//iconv_t cd = iconv_open( sEnc, tEnc  );
	 
	//int ires = (int)iconv( conv_st, &inBuf, &inLength, &outBuf, outLength );
	 
	//iconv_close(cd);
	 
	return (int)iconv( conv_st, &inBuf, &inLength, &outBuf, outLength );
}

int iconv_utf8_kr(char *inStr, char **output_buf_ptr)
{
	int ret;
	size_t in_size, out_size;
	memset(convbuf, 0x00, sizeof(convbuf));
	
	in_size = strlen(inStr) +1;
	
	out_size = sizeof(convbuf);
	
	ret = conv_str( convbuf, &out_size, inStr, in_size, "UTF-8", "EUC-KR" );
	
	//printf("in[%s] out[%s] \n",inStr, convbuf);
	memcpy(*output_buf_ptr, convbuf, strlen(convbuf) + 1);
		
	return ret;
}


void close_iconv()
{
	iconv_close(conv_st);
}

//http://www.codeproject.com/Tips/672470/Simple-Character-Encoding-Detection
//0  = UTF-8
//1  = UTF-16BE
//2  = UTF-16LE
//3  = UTF-32BE
//4  = UTF-32LE 
int String_GetEncoding(char *string)
  {
    unsigned c, i = 0, flags = 0;
    while (string[i] | string[i + 1] | string[i + 2] | string[i + 3])
      flags = (c = string[i++]) ? flags | ((!(flags % 4) && 
      c > 0x7F) << 3) : flags | 1 | (!(i & 1) << 1) 
      | ((string[i] == 0) << 2);
    return (flags & 1) + ((flags & 2) != 0) + 
    ((flags & 4) != 0) + ((flags & 8) != 0);
  }   

int conv_utf8_kr(char *inStr, char **output_buf_ptr)
{
	int ret;
	char tmpbuf[1024];
	size_t in_size, out_size;
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	trim(inStr);
	in_size = strlen(inStr) +1;
	//printf("input size[%d]\n", in_size);

	out_size = sizeof(tmpbuf);
	//out_size = sizeof(utf_buf);
	
	ret = convStr( tmpbuf, &out_size, inStr, in_size, "UTF-8", "EUC-KR" );
	memcpy(*output_buf_ptr, tmpbuf, strlen(tmpbuf) + 1);
	//if(szResult) free(szResult), szResult=NULL;
		
	return ret;
}

int conv_kr_utf8(char *inStr, char **output_buf_ptr)
{
	int ret;
	char tmpbuf[1024];
	size_t in_size, out_size;
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	
	in_size = strlen(inStr) +1;
	printf("input size[%d]\n", in_size);

	out_size = sizeof(tmpbuf);
	//out_size = sizeof(utf_buf);
	
	ret = convStr( tmpbuf, &out_size, inStr, in_size, "UTF-8", "EUC-KR" );
	
	memcpy(*output_buf_ptr, tmpbuf, strlen(tmpbuf) + 1);
	//if(szResult) free(szResult), szResult=NULL;
		
	return ret;
}


/*
EUC-KR -> UTF8 : iconv_open( "EUC-KR", "UTF-8" )
*/
int convStr( char *outBuf, size_t *outLength, char *inBuf, size_t inLength, const char *sEnc, const char *tEnc )
{
	//iconv_t cd = iconv_open( "EUC-KR", "UTF-8" );
	iconv_t cd = iconv_open( sEnc, tEnc  );
	 
	int ires = (int)iconv( cd, &inBuf, &inLength, &outBuf, outLength );
	 //printf("33in[%s] ---> out[%s][%d] \n", inBuf, outBuf, *outLength);
	iconv_close(cd);
	 
	return ires;
}


int file_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")) != NULL)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

/*
http://makefile.tistory.com/entry/Iconv-%EC%82%AC%EC%9A%A9%EC%98%88%EC%A0%9C
*/
void read_ini(const char *szSection, const char* szKey, const char *szDefaultValue, char **output_buf_ptr)
{
	char sectionKey[256];
	char* szResult;
	int cnt;
	int offset;
#ifdef WITH_CONV	
	size_t in_size, out_size;
#endif	

	memset(sectionKey, 0x00, 256);
	
	//printf("szSection [%s]\n", szSection);
	//printf("szKey [%s]\n", szKey);
	
	cnt = strlen(szSection);
	memcpy(sectionKey, szSection, cnt);
	
	offset = cnt;
	memcpy(sectionKey+offset, ":", 1);
	offset +=1;
	
	cnt = strlen(szKey);
	memcpy(sectionKey+offset, szKey, cnt);
	

	//method 1.
	//ret = GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, 255, szFileName); 


	//method 2.
	//sprintf(sectionKey, "%s:%s", szSection, szKey);
	
	szResult = iniparser_getstring(ini, sectionKey, NULL);
	
	
#ifdef WITH_CONV	
	char tmpbuf[1024];
	
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	
	in_size = strlen(szResult) +1;
	//printf("input size[%d]\n", in_size);

	out_size = sizeof(tmpbuf);
	//out_size = sizeof(utf_buf);
	//convStr( output_buf_ptr, out_size, szResult, in_size, "EUC-KR", "UTF-8" );
	convStr( tmpbuf, &out_size, szResult, in_size, "UTF-8", "EUC-KR" );
	//convStr( output_buf_ptr, &out_size, szResult, in_size, "UTF-8", "UTF-8" );
	
	//printf("in[%s] ---> out[%s] \n", szResult, output_buf_ptr);
	memcpy(*output_buf_ptr, tmpbuf, strlen(tmpbuf) + 1);
	if(szResult) free(szResult), szResult=NULL;
#else
	memcpy(*output_buf_ptr, szResult, strlen(szResult) + 1);
#endif	
	
}

/* LOG_TRACE(log level, format, args ) */  
void LOG_TRACE(LOG_LEVEL lvl, const char *fmt, ... )   
{   
  va_list  list;   
  char *s, c;   
  int i;   
  
  if( (lvl==LOG_INFO) || (lvl==LOG_ERROR))   
  {   
     va_start( list, fmt );   
  
     while(*fmt)   
     {   
        if ( *fmt != '%' )   
           putc( *fmt, stdout );   
        else  
        {   
           switch ( *++fmt )   
           {   
              case 's':   
                 /* set r as the next char in list (string) */  
                 s = va_arg( list, char * );   
                 printf("%s", s);   
                 break;   
  
              case 'd':   
                 i = va_arg( list, int );   
                 printf("%d", i);   
                 break;   
  
              case 'c':   
                 c = va_arg( list, int);   
                 printf("%c",c);   
                 break;   
  
              default:   
                 putc( *fmt, stdout );   
                 break;   
           }   
        }   
        ++fmt;   
     }   
     va_end( list );   
  }   
  fflush( stdout );   
}

char *trim2(char *str)
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }


    return str;
}
