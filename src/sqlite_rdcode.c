#include "road_name.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h> 

/*
*
DROP TABLE IF EXISTS TB_ADDR_MAP_TEMP_REF_JIBUN;

CREATE TABLE IF NOT EXISTS TB_ADDR_MAP_TEMP_RD_CD (	
	SIGG_CD	    VARCHAR2(5),
	RD_CD	    VARCHAR2(7),
	RD_NM	    VARCHAR2(80),
	RD_ENG	    VARCHAR2(80),
	EMD_SER	    VARCHAR2(2),
	SIDO_NM	    VARCHAR2(40),
	SIGG_NM	    VARCHAR2(40),
	EMD_GUBUN	VARCHAR2(1),
	EMD_CD	    VARCHAR2(3),
	EMD_NM	    VARCHAR2(40),
	UPPER_RD_CD	VARCHAR2(7),
	UPPER_RD_NM	VARCHAR2(80),
	USE_YN	    VARCHAR2(1),
	CHG_REASON	VARCHAR2(1),
	CHG_INFO	VARCHAR2(14),
	SIDO_ENG	VARCHAR2(40),
	SIGG_ENG	VARCHAR2(40),
	EMD_ENG	    VARCHAR2(40),
	ORIG_DATE	VARCHAR2(8),
	EXP_DATE	VARCHAR2(8)
);

*/
int create_rdcode_table(table_scheme_t *table_scheme)
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

int insrt_rdcode_table(table_scheme_t *table_scheme)
{
	LOG_TRACE(LOG_ERROR, "Insert %s table...\n", table_scheme->table_name);
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;
	
    //char *err_msg = 0;
    FILE *fp;
    char linebuf[1024];
    char err[1024];
    char sql[1024 * 10];
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
		char sigg_cd	[5 + 1];
		char rd_cd	    [7 + 1];
		char rd_nm	    [80 + 1];
		char rd_eng	    [80 + 1];
		char emd_ser	[2 + 1];
		char sido_nm	[40 + 1];
		char sigg_nm	[40 + 1];
		char emd_gubun	[1 + 1];
		char emd_cd	    [3 + 1];
		char emd_nm	    [40 + 1];
		char upper_rd_cd[7 + 1];
		char upper_rd_nm[80 + 1];
		char use_yn	    [1 + 1];
		char chg_reason	[1 + 1];
		char chg_info	[14 + 1];
		char sido_eng	[40 + 1];
		char sigg_eng	[40 + 1];
		char emd_eng	[40 + 1];
		char orig_date	[8 + 1];
		char exp_date	[8 + 1];
		
		struct ROWVAL *next_row;
	} rowval_t;
	
	//struct ROWVAL *head_row;
	//struct ROWVAL *cur_row;
	//cur_row = head_row = NULL;
	
	rc = sqlite3_open_v2(table_scheme->database, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    
    if (rc != SQLITE_OK) {
        
        log_print_error( "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
	errmsg = err;
	
	sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, &errmsg);
	sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, &errmsg);

	
	rc = sqlite3_prepare_v2(db, table_scheme->sql, -1, &stmt, 0);
    
    log_print_error( "\nThe statement has %d wildcards\n", sqlite3_bind_parameter_count(stmt));
	
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
				
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.sigg_cd, sizeof(cur_row.sigg_cd), token); } //else { strcpy(cur_row.sigg_cd, ""); }
			//printf("sigg_cd >> %s token >> %s\n", cur_row.sigg_cd, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.rd_cd, sizeof(cur_row.rd_cd), token); } //else { strcpy(cur_row.rd_cd, ""); }
			//printf("rd_cd >> %s token >> %s\n", cur_row.rd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			//printf("token >> %s \n", token);		
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.rd_nm, sizeof(cur_row.rd_nm), output_buf_ptr); 
				//printf("output_buf_ptr >> %s \n", output_buf_ptr);
			} 
			//else { strcpy(cur_row.rd_nm, ""); }
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.rd_eng, sizeof(cur_row.rd_eng), token); } //else { strcpy(cur_row.rd_eng, ""); }
			//printf("rd_eng >> %s token >> %s\n", cur_row.rd_eng, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_ser, sizeof(cur_row.emd_ser), token); } //else { strcpy(cur_row.emd_ser, ""); }
			//printf("emd_ser >> %s token >> %s\n", cur_row.emd_ser, token);
			
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
			//else { strcpy(cur_row.sido_nm, ""); }
			
			token = strsep_cust(&line, "|"); 
			//printf("token >> %s \n", token);		
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.sigg_nm, sizeof(cur_row.sigg_nm), output_buf_ptr); 
				//printf("output_buf_ptr >> %s \n", output_buf_ptr);
			} 
			//else { strcpy(cur_row.sigg_nm, ""); }
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_gubun, sizeof(cur_row.emd_gubun), token); } //else { strcpy(cur_row.emd_gubun, ""); }
			//printf("emd_gubun >> %s token >> %s\n", cur_row.emd_gubun, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_cd, sizeof(cur_row.emd_cd), token); } //else { strcpy(cur_row.emd_cd, ""); }
			//printf("emd_cd >> %s token >> %s\n", cur_row.emd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			//printf("token >> %s \n", token);		
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.emd_nm, sizeof(cur_row.emd_nm), output_buf_ptr); 
				//printf("output_buf_ptr >> %s \n", output_buf_ptr);
			} 
			//else { strcpy(cur_row.sigg_nm, ""); }
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.upper_rd_cd, sizeof(cur_row.upper_rd_cd), token); } //else { strcpy(cur_row.upper_rd_cd, ""); }
			//printf("upper_rd_cd >> %s token >> %s\n", cur_row.upper_rd_cd, token);
			
			token = strsep_cust(&line, "|"); 
			//printf("token >> %s \n", token);		
			if(token && strcmp(token, "") != 0) { 
				trim(token);
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr(token, &output_buf_ptr);
				
				strcpy_s(cur_row.upper_rd_nm, sizeof(cur_row.upper_rd_nm), output_buf_ptr); 
				//printf("output_buf_ptr >> %s \n", output_buf_ptr);
			} 
			//else { strcpy(cur_row.upper_rd_nm, ""); }
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.use_yn, sizeof(cur_row.use_yn), token); } //else { strcpy(cur_row.use_yn, ""); }
			//printf("use_yn >> %s token >> %s\n", cur_row.use_yn, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.chg_reason, sizeof(cur_row.chg_reason), token); } //else { strcpy(cur_row.chg_reason, ""); }
			//printf("chg_reason >> %s token >> %s\n", cur_row.chg_reason, token);
			
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.chg_info, sizeof(cur_row.chg_info), token); } //else { strcpy(cur_row.chg_info, ""); }
			//printf("chg_info >> %s token >> %s\n", cur_row.chg_info, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.sido_eng, sizeof(cur_row.sido_eng), token); } //else { strcpy(cur_row.sido_eng, ""); }
			//printf("sido_eng >> %s token >> %s\n", cur_row.sido_eng, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.sigg_eng, sizeof(cur_row.sigg_eng), token); } //else { strcpy(cur_row.sigg_eng, ""); }
			//printf("sigg_eng >> %s token >> %s\n", cur_row.sigg_eng, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_eng, sizeof(cur_row.emd_eng), token); } //else { strcpy(cur_row.emd_eng, ""); }
			//printf("emd_eng >> %s token >> %s\n", cur_row.emd_eng, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.orig_date, sizeof(cur_row.orig_date), token); } //else { strcpy(cur_row.orig_date, ""); }
			//printf("orig_date >> %s token >> %s\n", cur_row.orig_date, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.exp_date, sizeof(cur_row.exp_date), token); } //else { strcpy(cur_row.exp_date, ""); }
			//printf("exp_date >> %s token >> %s\n", cur_row.exp_date, token);
			
			
			if(bind_text(db, stmt, 1, cur_row.sigg_cd, strlen(cur_row.sigg_cd)) == false){ log_print_error("Fail to insert column 1: %s\n", cur_row.sigg_cd); }
			if(bind_text(db, stmt, 2, cur_row.rd_cd, strlen(cur_row.rd_cd)) == false){ log_print_error("Fail to insert column 2: %s\n", cur_row.rd_cd); }
			if(bind_text(db, stmt, 3, cur_row.rd_nm, strlen(cur_row.rd_nm)) == false){ log_print_error("Fail to insert column 3: %s\n", cur_row.rd_nm); }
			if(bind_text(db, stmt, 4, cur_row.rd_eng, strlen(cur_row.rd_eng)) == false){ log_print_error("Fail to insert column 4: %s\n", cur_row.rd_eng); }
			if(bind_text(db, stmt, 5, cur_row.emd_ser, strlen(cur_row.emd_ser)) == false){ log_print_error("Fail to insert column 5: %s\n", cur_row.emd_ser); }
			if(bind_text(db, stmt, 6, cur_row.sido_nm, strlen(cur_row.sido_nm)) == false){ log_print_error("Fail to insert column 6: %s\n", cur_row.sido_nm); }
			if(bind_text(db, stmt, 7, cur_row.sigg_nm, strlen(cur_row.sigg_nm)) == false){ log_print_error("Fail to insert column 7: %s\n", cur_row.sigg_nm); }
			if(bind_text(db, stmt, 8, cur_row.emd_gubun, strlen(cur_row.emd_gubun)) == false){ log_print_error("Fail to insert column 8: %s\n", cur_row.emd_gubun); }
			if(bind_text(db, stmt, 9, cur_row.emd_cd, strlen(cur_row.emd_cd)) == false){ log_print_error("Fail to insert column 9: %s\n", cur_row.emd_cd); }		
			if(bind_text(db, stmt, 10, cur_row.emd_nm, strlen(cur_row.emd_nm)) == false){ log_print_error("Fail to insert column 10: %s\n", cur_row.emd_nm); }
			if(bind_text(db, stmt, 11, cur_row.upper_rd_cd, strlen(cur_row.upper_rd_cd)) == false){ log_print_error("Fail to insert column 11: %s\n", cur_row.upper_rd_cd); }
			if(bind_text(db, stmt, 12, cur_row.upper_rd_nm, strlen(cur_row.upper_rd_nm)) == false){ log_print_error("Fail to insert column 12: %s\n", cur_row.upper_rd_nm); }
			if(bind_text(db, stmt, 13, cur_row.use_yn, strlen(cur_row.use_yn)) == false){ log_print_error("Fail to insert column 13: %s\n", cur_row.use_yn); }
			if(bind_text(db, stmt, 14, cur_row.chg_reason, strlen(cur_row.chg_reason)) == false){ log_print_error("Fail to insert column 14: %s\n", cur_row.chg_reason); }
			if(bind_text(db, stmt, 15, cur_row.chg_info, strlen(cur_row.chg_info)) == false){ log_print_error("Fail to insert column 15: %s\n", cur_row.chg_info); }
			if(bind_text(db, stmt, 16, cur_row.sido_eng, strlen(cur_row.sido_eng)) == false){ log_print_error("Fail to insert column 16: %s\n", cur_row.sido_eng); }
			if(bind_text(db, stmt, 17, cur_row.sigg_eng, strlen(cur_row.sigg_eng)) == false){ log_print_error("Fail to insert column 17: %s\n", cur_row.sigg_eng); }
			if(bind_text(db, stmt, 18, cur_row.emd_eng, strlen(cur_row.emd_eng)) == false){ log_print_error("Fail to insert column 18: %s\n", cur_row.emd_eng); }
			if(bind_text(db, stmt, 19, cur_row.orig_date, strlen(cur_row.orig_date)) == false){ log_print_error("Fail to insert column 19: %s\n", cur_row.orig_date); }
			if(bind_text(db, stmt, 20, cur_row.exp_date, strlen(cur_row.exp_date)) == false){ log_print_error("Fail to insert column 20: %s\n", cur_row.exp_date); }
				
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				log_print_error("Could not step (execute) rows[%d] stmt: %s\n", total_cnt, sqlite3_errmsg(db));
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
        log_print_error( "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    if(fp) { fflush(fp); fclose(fp); fp = NULL;}
    	
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &errmsg);
	sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errmsg);
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
