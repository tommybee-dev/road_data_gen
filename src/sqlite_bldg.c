#include "road_name.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h> 

/*
*
DROP TABLE IF EXISTS TB_ADDR_MAP_TMP2;

CREATE TABLE TB_ADDR_MAP_TMP2 (	
	BJD_CD		VARCHAR2(10 BYTE),       
	SIDO_NM		VARCHAR2(50 BYTE),       
	SIGG_NM		VARCHAR2(50 BYTE),       
	EMD_NM		VARCHAR2(50 BYTE),       
	RI_NM		VARCHAR2(50 BYTE),       
	SAN_CK		VARCHAR2(1 BYTE),       
	JIBUN_MAIN	VARCHAR2(20 BYTE),   
	JIBUN_SUB	VARCHAR2(20 BYTE),   
	RD_CD		VARCHAR2(255 BYTE,       
	RD_NM		VARCHAR2(255 BYTE,       
	GRND_CK		VARCHAR2(1 BYTE),       
	BLDNO_MAIN	VARCHAR2(20 BYTE),   
	BLDNO_SUB	VARCHAR2(20 BYTE),   
	BLD_NM		VARCHAR2(50 BYTE),       
	BLD_DETAIL	VARCHAR2(100 BYTE,   
	BLD_MNGNO	VARCHAR2(50 BYTE),   
	EMD_SERIAL	VARCHAR2(2 BYTE),   
	HJD_CD		VARCHAR2(10 BYTE),       
	HJD_NM		VARCHAR2(50 BYTE),       
	ZIP_CODE	VARCHAR2(6 BYTE),   
	COL21		VARCHAR2(3 BYTE),       
	COL22		VARCHAR2(40 BYTE),       
	COL23		VARCHAR2(2 BYTE),       
	COL24		VARCHAR2(8 BYTE),       
	COL25		VARCHAR2(25 BYTE),       
	COL26		VARCHAR2(200 BYTE,       
	GONGDONG_YN	VARCHAR2(1 BYTE),   
	COL28		VARCHAR2(5 BYTE),       
	COL29		VARCHAR2(1 BYTE),       
	COL30		VARCHAR2(15 BYTE),       
	COL31		VARCHAR2(15 BYTE),       
);

*/
int create_bldg_table(table_scheme_t *table_scheme)
{
	LOG_TRACE(LOG_ERROR, "Create %s table...\n", table_scheme->table_name);
	sqlite3 *db;
    char *err_msg = 0;
    char sql[1024 * 10];
    
    int rc = sqlite3_open_v2(table_scheme->database, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    
    if (rc != SQLITE_OK) {
        
        LOG_TRACE(LOG_ERROR, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
	memset(sql, 0x00, sizeof(sql));
	
	//encrypt_db(ptsmsopt, db);
	
    rc = sqlite3_exec(db, table_scheme->sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        LOG_TRACE(LOG_ERROR, "Failed to create data\n");
        LOG_TRACE(LOG_ERROR, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
   
    LOG_TRACE(LOG_ERROR, "Create %s table...Done\n", table_scheme->table_name);
    
    return 0;
}

int insrt_bldg_table(table_scheme_t *table_scheme)
{
	LOG_TRACE(LOG_ERROR, "Insert %s table...\n", table_scheme->table_name);
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;
	
    //char *err_msg = 0;
    FILE *fp;
    char linebuf[1024];
    char sql[1024 * 10];
   	char err[1024];
    //int sIdx = 0;
    char *errmsg;
    
    
    if((fp = (FILE*)fopen(table_scheme->txt_path, "r")) == NULL)
	{
		LOG_TRACE(LOG_ERROR, "Cannot open table file: %s\n",  table_scheme->txt_path);
		return 1;
    }
    
	memset(linebuf, 0x00, sizeof(linebuf));
	memset(sql, 0x00, sizeof(sql));
	
	typedef struct ROWVAL
	{
		char bjd_cd		[10 + 1];
		char sido_nm	[50 + 1];
		char sigg_nm	[50 + 1];
		char emd_nm		[50 + 1];
		char ri_nm		[50 + 1];
		char san_ck		[1 + 1];
		char jibun_main	[20 + 1];
		char jibun_sub	[20 + 1];
		char rd_cd		[255 + 1];
		char rd_nm		[255 + 1];
		char grnd_ck	[1 + 1];
		char bldno_main	[20 + 1];
		char bldno_sub	[20 + 1];
		char bld_nm		[50 + 1];
		char bld_detail	[100 + 1];
		char bld_mngno	[50 + 1];
		char emd_serial	[2 + 1];
		char hjd_cd		[10 + 1];
		char hjd_nm		[50 + 1];
		char zip_code	[6 + 1];
		char col21		[3 + 1];
		char col22		[40 + 1];
		char col23		[2 + 1];
		char col24		[8 + 1];
		char col25		[25 + 1];
		char col26		[200 + 1];
		char gongdong_yn[1 + 1];
		char col28		[5 + 1];
		char col29		[1 + 1];
		char col30		[15 + 1];
		char col31		[15 + 1];
		struct ROWVAL *next_row;
	} rowval_t;
	
	//struct ROWVAL *head_row;
	//struct ROWVAL *cur_row;
	//cur_row = head_row = NULL;
	
	rc = sqlite3_open_v2(table_scheme->database, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    
    if (rc != SQLITE_OK) {
        
        LOG_TRACE(LOG_ERROR, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
	errmsg = err;
	
	sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, &errmsg);

	
	rc = sqlite3_prepare_v2(db, table_scheme->sql, -1, &stmt, 0);
    
    LOG_TRACE(LOG_INFO, "\nThe statement has %d wildcards\n", sqlite3_bind_parameter_count(stmt));
	
	int total_cnt =0;
	
	if (rc == SQLITE_OK) {
		
		rowval_t cur_row;
		
		char tmpstr[FILENAME_MAX] = {0x00, };
		char *output_buf_ptr;
		char *line;
		char *token;
		
		open_iconv("UTF-8", "EUC-KR");
		
		sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
		
		while(fgets(linebuf, 10240, fp) != NULL)
		{
			memset(&cur_row, 0x00, sizeof(cur_row));
			line = linebuf;
			
			token = strsep_cust(&line, "|"); 
				
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bjd_cd, sizeof(cur_row.bjd_cd), token); } //else { strcpy(cur_row.bjd_cd, ""); }
			//printf("bjd_cd >> %s token >> %s\n", cur_row.bjd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			//printf("token >> %s \n", token);		
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.sido_nm, sizeof(cur_row.sido_nm), output_buf_ptr); 
				//printf("output_buf_ptr >> %s \n", output_buf_ptr);
			} 
			else { strcpy(cur_row.sido_nm, ""); }
			//printf("sido_nm >> %s \n", cur_row.sido_nm);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
				
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.sigg_nm, sizeof(cur_row.sigg_nm), output_buf_ptr); 
				
			} 
			//else { strcpy(cur_row.sigg_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.sigg_nm, sizeof(cur_row.sigg_nm), token); } else { strcpy(cur_row.sigg_nm, ""); }
			//printf("sigg_nm >> %s \n", cur_row.sigg_nm);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.emd_nm, sizeof(cur_row.emd_nm), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.emd_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.emd_nm, sizeof(cur_row.emd_nm), token); } else { strcpy(cur_row.emd_nm, ""); }
			//printf("emd_nm >> %s \n", cur_row.emd_nm);
				
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.ri_nm, sizeof(cur_row.ri_nm), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.ri_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.ri_nm, sizeof(cur_row.ri_nm), token); } else { strcpy(cur_row.ri_nm, ""); }
			//printf("ri_nm >> %s token >> %s\n", cur_row.ri_nm, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.san_ck, sizeof(cur_row.san_ck), token); } //else { strcpy(cur_row.san_ck, ""); }
			//printf("san_ck >> %s token >> %s\n", cur_row.san_ck, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.jibun_main, sizeof(cur_row.jibun_main), token); } //else { strcpy(cur_row.jibun_main, ""); }
			//printf("jibun_main >> %s token >> %s\n", cur_row.jibun_main, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.jibun_sub, sizeof(cur_row.jibun_sub), token); } //else { strcpy(cur_row.jibun_sub, ""); }
			//printf("jibun_sub >> %s token >> %s\n", cur_row.jibun_sub, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.rd_cd, sizeof(cur_row.rd_cd), token); } //else { strcpy(cur_row.rd_cd, ""); }
			//printf("rd_cd >> %s token >> %s\n", cur_row.rd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.rd_nm, sizeof(cur_row.rd_nm), output_buf_ptr); 
			}
			// else { strcpy(cur_row.rd_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.rd_nm, sizeof(cur_row.rd_nm), token); } else { strcpy(cur_row.rd_nm, ""); }
			//printf("rd_nm >> %s token >> %s\n", cur_row.rd_nm, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.grnd_ck, sizeof(cur_row.grnd_ck), token); } //else { strcpy(cur_row.grnd_ck, ""); }
			//printf("grnd_ck >> %s token >> %s\n", cur_row.grnd_ck, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bldno_main, sizeof(cur_row.bldno_main), token); } //else { strcpy(cur_row.bldno_main, ""); }
			//printf("bldno_main >> %s token >> %s\n", cur_row.bldno_main, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bldno_sub, sizeof(cur_row.bldno_sub), token); } //else { strcpy(cur_row.bldno_sub, ""); }
			//printf("bldno_sub >> %s token >> %s\n", cur_row.bldno_sub, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.bld_nm, sizeof(cur_row.bld_nm), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.bld_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.bld_nm, sizeof(cur_row.bld_nm), token); } else { strcpy(cur_row.bld_nm, ""); }
			//printf("bld_nm >> %s token >> %s\n", cur_row.bld_nm, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.bld_detail, sizeof(cur_row.bld_detail), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.bld_detail, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.bld_detail, sizeof(cur_row.bld_detail), token); } else { strcpy(cur_row.bld_detail, ""); }
			//printf("bld_detail >> %s token >> %s\n", cur_row.bld_detail, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bld_mngno, sizeof(cur_row.bld_mngno), token); } //else { strcpy(cur_row.bld_mngno, ""); }
			//printf("bld_mngno >> %s token >> %s\n", cur_row.bld_mngno, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_serial, sizeof(cur_row.emd_serial), token); } //else { strcpy(cur_row.emd_serial, ""); }
			//printf("emd_serial >> %s token >> %s\n", cur_row.emd_serial, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.hjd_cd, sizeof(cur_row.hjd_cd), token); } //else { strcpy(cur_row.hjd_cd, ""); }
			//printf("hjd_cd >> %s token >> %s\n", cur_row.hjd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.hjd_nm, sizeof(cur_row.hjd_nm), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.hjd_nm, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.hjd_nm, sizeof(cur_row.hjd_nm), token); } else { strcpy(cur_row.hjd_nm, ""); }
			//printf("hjd_nm >> %s token >> %s\n", cur_row.hjd_nm, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.zip_code, sizeof(cur_row.zip_code), token); } //else { strcpy(cur_row.zip_code, ""); }
			//printf("zip_code >> %s token >> %s\n", cur_row.zip_code, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col21, sizeof(cur_row.col21), token); } //else { strcpy(cur_row.col21, ""); }
			//printf("col21 >> %s token >> %s\n", cur_row.col21, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col22, sizeof(cur_row.col22), token); } //else { strcpy(cur_row.col22, ""); }
			//printf("col22 >> %s token >> %s\n", cur_row.col22, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col23, sizeof(cur_row.col23), token); } //else { strcpy(cur_row.col23, ""); }
			//printf("col23 >> %s token >> %s\n", cur_row.col23, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col24, sizeof(cur_row.col24), token); } //else { strcpy(cur_row.col24, ""); }
			//printf("col24 >> %s token >> %s\n", cur_row.col24, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col25, sizeof(cur_row.col25), token); } //else { strcpy(cur_row.col25, ""); }
			//printf("col25 >> %s token >> %s\n", cur_row.col25, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.col26, sizeof(cur_row.col26), output_buf_ptr); 
			} 
			//else { strcpy(cur_row.col26, ""); }
			//if(token) { trim(token), strcpy_s(cur_row.col26, sizeof(cur_row.col26), token); } else { strcpy(cur_row.col26, ""); }
			//printf("col26 >> %s token >> %s\n", cur_row.col26, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.gongdong_yn, sizeof(cur_row.gongdong_yn), token); } //else { strcpy(cur_row.gongdong_yn, ""); }
			//printf("gongdong_yn >> %s token >> %s\n", cur_row.gongdong_yn, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col28, sizeof(cur_row.col28), token); } //else { strcpy(cur_row.col28, ""); }
			//printf("col28 >> %s token >> %s\n", cur_row.col28, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col29, sizeof(cur_row.col29), token); } //else { strcpy(cur_row.col29, ""); }
			//printf("col29 >> %s token >> %s\n", cur_row.col29, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col30, sizeof(cur_row.col30), token); } //else { strcpy(cur_row.col30, ""); }
			//printf("col30 >> %s token >> %s\n", cur_row.col30, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.col31, sizeof(cur_row.col31), token); } //else { strcpy(cur_row.col31, ""); }
			//printf("col31 >> %s token >> %s\n\n", cur_row.col31, token);
			//token = strtok_s(&line, "|"); 
			
			if(bind_text(db, stmt, 1, cur_row.bjd_cd, strlen(cur_row.bjd_cd)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 1: %s\n", cur_row.bjd_cd); }
			if(bind_text(db, stmt, 2, cur_row.sido_nm, strlen(cur_row.sido_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 2: %s\n", cur_row.sido_nm); }
			if(bind_text(db, stmt, 3, cur_row.sigg_nm, strlen(cur_row.sigg_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 3: %s\n", cur_row.sigg_nm); }
			if(bind_text(db, stmt, 4, cur_row.emd_nm, strlen(cur_row.emd_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 4: %s\n", cur_row.emd_nm); }
			if(bind_text(db, stmt, 5, cur_row.ri_nm, strlen(cur_row.ri_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 5: %s\n", cur_row.ri_nm); }
			if(bind_text(db, stmt, 6, cur_row.san_ck, strlen(cur_row.san_ck)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 6: %s\n", cur_row.san_ck); }
			if(bind_text(db, stmt, 7, cur_row.jibun_main, strlen(cur_row.jibun_main)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 7: %s\n", cur_row.jibun_main); }
			if(bind_text(db, stmt, 8, cur_row.jibun_sub, strlen(cur_row.jibun_sub)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 8: %s\n", cur_row.jibun_sub); }
			if(bind_text(db, stmt, 9, cur_row.rd_cd, strlen(cur_row.rd_cd)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 9: %s\n", cur_row.rd_cd); }
			if(bind_text(db, stmt, 10, cur_row.rd_nm, strlen(cur_row.rd_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 10: %s\n", cur_row.rd_nm); }
			if(bind_text(db, stmt, 11, cur_row.grnd_ck, strlen(cur_row.grnd_ck)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 11: %s\n", cur_row.grnd_ck); }
			if(bind_text(db, stmt, 12, cur_row.bldno_main, strlen(cur_row.bldno_main)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 12: %s\n", cur_row.bldno_main); }
			if(bind_text(db, stmt, 13, cur_row.bldno_sub, strlen(cur_row.bldno_sub)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 13: %s\n", cur_row.bldno_sub); }
			if(bind_text(db, stmt, 14, cur_row.bld_nm, strlen(cur_row.bld_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 14: %s\n", cur_row.bld_nm); }	
			if(bind_text(db, stmt, 15, cur_row.bld_detail, strlen(cur_row.bld_detail)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 15: %s\n", cur_row.bld_detail); }
			if(bind_text(db, stmt, 16, cur_row.bld_mngno, strlen(cur_row.bld_mngno)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 16: %s\n", cur_row.bld_mngno); }
			if(bind_text(db, stmt, 17, cur_row.emd_serial, strlen(cur_row.emd_serial)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 17: %s\n", cur_row.emd_serial); }
			if(bind_text(db, stmt, 18, cur_row.hjd_cd, strlen(cur_row.hjd_cd)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 18: %s\n", cur_row.hjd_cd); }
			if(bind_text(db, stmt, 19, cur_row.hjd_nm, strlen(cur_row.hjd_nm)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 19: %s\n", cur_row.hjd_nm); }		
			if(bind_text(db, stmt, 20, cur_row.zip_code, strlen(cur_row.zip_code)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 20: %s\n", cur_row.zip_code); }
			if(bind_text(db, stmt, 21, cur_row.col21, strlen(cur_row.col21)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 21: %s\n", cur_row.col21); }
			if(bind_text(db, stmt, 22, cur_row.col22, strlen(cur_row.col22)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 22: %s\n", cur_row.col22); }
			if(bind_text(db, stmt, 23, cur_row.col23, strlen(cur_row.col23)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 23: %s\n", cur_row.col23); }
			if(bind_text(db, stmt, 24, cur_row.col24, strlen(cur_row.col24)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 24: %s\n", cur_row.col24); }
			if(bind_text(db, stmt, 25, cur_row.col25, strlen(cur_row.col25)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 25: %s\n", cur_row.col25); }
			if(bind_text(db, stmt, 26, cur_row.col26, strlen(cur_row.col26)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 26: %s\n", cur_row.col26); }
			if(bind_text(db, stmt, 27, cur_row.gongdong_yn, strlen(cur_row.gongdong_yn)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 27: %s\n", cur_row.gongdong_yn); }
			if(bind_text(db, stmt, 28, cur_row.col28, strlen(cur_row.col28)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 28: %s\n", cur_row.col28); }
			if(bind_text(db, stmt, 29, cur_row.col29, strlen(cur_row.col29)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 29: %s\n", cur_row.col29); }
			if(bind_text(db, stmt, 30, cur_row.col30, strlen(cur_row.col30)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 30: %s\n", cur_row.col30); }
			if(bind_text(db, stmt, 31, cur_row.col31, strlen(cur_row.col31)) == false){ log_print_error(NULL, __LINE__, "Fail to insert column 31: %s\n", cur_row.col31); }
			
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				log_print_error( "Could not step (execute) rows[%d] stmt: %s\n", total_cnt, sqlite3_errmsg(db));
				return 1;
			}
			
			sqlite3_clear_bindings(stmt);
			sqlite3_reset(stmt);
			//cur_row->next_row = head_row;
			//head_row = cur_row;	
			
			total_cnt++;
		}
		
		close_iconv();
		
	} else {
        LOG_TRACE(LOG_ERROR, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
	if(fp) { fflush(fp); fclose(fp); fp = NULL;}
		
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &errmsg);	
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    
    	
	LOG_TRACE(LOG_ERROR, "Insert %s table...Done\n", table_scheme->table_name);  
	return total_cnt;
}
/*
int sel_TSMS_LAYERS(PTSMS_OPT ptsmsopt)
{
	LOG_TRACE(LOG_ERROR, "Select %s table...\n", ptsmsopt->tablename);
	sqlite3 *db;
    char *err_msg = 0;
    char sql[1024];
    
    int rc = sqlite3_open_v2(ptsmsopt->database, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    
    if (rc != SQLITE_OK) {
        
        LOG_TRACE(LOG_ERROR, "Cannot open database: %s\n", 
                sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    memset(sql, 0x00, sizeof(sql));
	encrypt_db(ptsmsopt, db);
 
    sprintf(sql, "SELECT * FROM %s;", ptsmsopt->tablename);
    //sql = "SELECT name FROM sqlite_master WHERE type='table'";
        
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        LOG_TRACE(LOG_ERROR, "Failed to select data\n");
        LOG_TRACE(LOG_ERROR, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    LOG_TRACE(LOG_ERROR, "Select %s table...Done\n", ptsmsopt->tablename);
    
	return 0;
}
*/
