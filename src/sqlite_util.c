#include "road_name.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef USE_ICONV
void conv_str( char* msg_buf, char *msg_out, const char *dest_cod, const char *src_cod ){
	char * out_string;
	// Conversion descriptor.     
	iconv_t conv_desc;
	conv_desc = initialize (src_cod, dest_cod);
	out_string = euc2utf8 (conv_desc, msg_buf);
	finalize (conv_desc);
	
	if (out_string) {
		/*printf ("Final iconv output: %s\n", out_string);   */
		strcpy(msg_out,out_string);
	}
}

bool is_hangul(const char *input_text)
{
    bool is_hangul = true; 
    
    while(*input_text)
    {
        
        char letter = *input_text;
        
        if(!(letter & 0x80))
        { 
            is_hangul = false;
            break;
        }
        
        input_text++;
    }
    
    return is_hangul; 
    
}

bool has_hangul(char *input_text)
{
	bool is_hangul = false;
	char *tmp_text;
	
	tmp_text = input_text;
	
	while(*tmp_text){
		char letter = *tmp_text;
        if((letter & 0x80))
        {
        	is_hangul = true;           
        	break;         
        }
		tmp_text++;
	}
	return is_hangul;
}

/*
bool is_hangul(char *input_text)
{
	bool is_hangul = true;
	char *tmp_text;
	
	tmp_text = input_text;
	
	while(*tmp_text)
	{
		char letter = *tmp_text;
		
		if(!(letter & 0x80))
		{
			is_hangul = false;            
			break;        
		}
		
		tmp_text++;    
	}
	
	return   is_hangul;
}
*/

#endif

void trim(char * const a)
{
    char *p = a, *q = a;
    while (isspace(*q))            ++q;
    while (*q)                     *p++ = *q++;
    *p = '\0';
    while (p > a && isspace(*--p)) *p = '\0';
}

void encrypt_db(char *key, char *charset, sqlite3 *db)
{
	char *err_msg;
    char sql[1024];
    int rc;
    
#ifdef ENCRIPT_DB
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "PRAGMA key = '%s';", key);   
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	
	if (rc != SQLITE_OK) {
        
        LOG_TRACE(LOG_ERROR, "Failed to encrypt data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return;
    } 
#endif /* ENCRIPT_DB */ 
	
	LOG_TRACE(LOG_ERROR, "Character set: %s\n", charset);
	
	if(strcmp(charset, DEFULT_DB_CHARSET) != 0)
	{
		memset(sql, 0x00, sizeof(sql));
	    sprintf(sql, "PRAGMA encoding = '%s';", charset); 
	    
	    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
		
		if (rc != SQLITE_OK) {
	        
	        LOG_TRACE(LOG_ERROR, "Failed to covert encoding data: %s\n", sqlite3_errmsg(db));
	        sqlite3_close(db);
	        
	        return;
	    }	
	}
	 

	return;
}

bool bind_text(sqlite3 *db, sqlite3_stmt *stmt, int index, char *pstr, int sz)
{
	if (sqlite3_bind_text (
			stmt, index,  // Index of wildcard
    		pstr, sz,  // length of text
    		SQLITE_STATIC
		)!= SQLITE_OK
	){
		LOG_TRACE(LOG_ERROR, "\nCould not bind text... %s.\n", sqlite3_errmsg(db));
		
		return false;
	}
	
	return true;
}


bool bind_int(sqlite3 *db, sqlite3_stmt *stmt, int index, int val)
{
	if (sqlite3_bind_int (
			stmt, 
			index,  // Index of wildcard
    		val
		)!= SQLITE_OK
	){
		LOG_TRACE(LOG_ERROR, "\nCould not bind int ... %s.\n", sqlite3_errmsg(db));
		
		return false;
	}
	
	return true;
}

bool bind_double(sqlite3 *db, sqlite3_stmt *stmt, int index, double val)
{
	if (sqlite3_bind_double (
			stmt, 
			index,  // Index of wildcard
    		val
		)!= SQLITE_OK
	){
		LOG_TRACE(LOG_ERROR, "\nCould not bind int ... %s.\n", sqlite3_errmsg(db));
		
		return false;
	}
	
	return true;
}

void substr(char *dest, char *src, int offset, int len)
{
	int i;
	for(i = 0; i < len && src[offset + i] != '\0'; i++)
		dest[i] = src[i + offset];
	dest[i] = '\0';
}

void to_ascii_comp(char* dest, char *text){
	int i = 0;
	int len = strlen(text);

	for(i=0; i<len; i=i+2){
		//unsigned char chunk[3];
		char chunk[3];
		substr(chunk,text, i,2);
		//unsigned char chuck_conv[2];
		char chuck_conv[2];
		//const char * punt = strtoul(chunk, NULL, 16);
		sprintf (chuck_conv, "%c", (int)strtoul(chunk, NULL, 16));
		dest = strcat ( dest, chuck_conv );
	}
}


void to_ascii( unsigned char* dest, unsigned char *text )
{
    unsigned int ch ;
    for( ; sscanf( (const char*)text, "%2x", &ch ) == 1 ; text += 2 )
          *dest++ = ch ;
    *dest = 0 ; // null-terminate
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	//char tmp[512];
    NotUsed = 0;
    
    LOG_TRACE(LOG_INFO, "[callback]in Callback.....size[%d]\n", argc);   
    
    for (int i = 0; i < argc; i++) {
        LOG_TRACE(LOG_INFO, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  
#ifdef HEX_ENCODE       
		if(argv[i] && (strcmp(azColName[i],HEX_COL_DISP_NM) == 0 || strcmp(azColName[i],HEX_COL_TITLE) == 0))	
		{
			to_ascii(tmp, argv[i]);
        	LOG_TRACE(LOG_INFO, "toASCII = %s\n", tmp);
		}
#endif
        /*	
        if(argv[i])	
        {
#ifdef HEX_ENCODE    
    char **hexStr;
    hexStr = (char**)malloc(sizeof(char*));
#endif

        	to_hex(argv[i], strlen(argv[i]), hexStr);
        	
        	LOG_TRACE(LOG_INFO, "toHEX = %s\n", *hexStr);
        	
        	memset(tmp, 0x00, sizeof(tmp));
        	to_ascii(tmp, *hexStr);
        	
        	LOG_TRACE(LOG_INFO, "toASCII = %s\n", tmp);
        	
        	if(*hexStr)
        	{
        		free(*hexStr);
        		*hexStr = NULL;
        	}
        }
free(hexStr),hexStr = NULL;
        */
    }
    
    LOG_TRACE(LOG_INFO, "\n");
    
    return 0;
}


void to_hex(unsigned char *bin, unsigned int binsz, char **result)
{
	char          hex_str[]= "0123456789abcdef";
	unsigned int  i;
	
	*result = (char *)malloc(binsz * 2 + 1);
	(*result)[binsz * 2] = 0;
	
	if (!binsz)
	return;
	
	for (i = 0; i < binsz; i++)
	{
		(*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
		(*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
	}  
	
	/*LOG_TRACE(LOG_INFO, "HEX111 = %s\n", (*result));*/
}
