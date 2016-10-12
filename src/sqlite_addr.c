#include "road_name.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h> 

/*
*
DROP TABLE IF EXISTS TB_ADDR_MAP_TEMP_REF_JIBUN;

CREATE TABLE TB_ADDR_MAP_TEMP_REF_JIBUN (	
	BJD_CD	VARCHAR2(10),
	SIDO_NM	VARCHAR2(40),
	SIGG_NM	VARCHAR2(40),
	EMD_NM	VARCHAR2(40),
	RI_NM	VARCHAR2(40),
	SAN_CK	VARCHAR2(1),
	JIBUN_MAIN	VARCHAR2(4),
	JIBUN_SUB	VARCHAR2(4),
	RD_CD	VARCHAR2(12),
	GRND_CK	VARCHAR2(1),
	BLDNO_MAIN	VARCHAR2(5),
	BLDNO_SUB	VARCHAR2(5),
	EMD_SERIAL	VARCHAR2(10),
	MOVE_CODE	VARCHAR2(2)
       
);

*/
int create_addr_table(table_scheme_t *table_scheme)
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

int insrt_addr_table(table_scheme_t *table_scheme)
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
		char bjd_cd		[10 + 1];
		char sido_nm	[40 + 1];
		char sigg_nm	[40 + 1];
		char emd_nm		[40 + 1];
		char ri_nm		[40 + 1];
		char san_ck		[1 + 1];
		char jibun_main	[4 + 1];
		char jibun_sub	[4 + 1];
		char rd_cd		[12 + 1];
		char grnd_ck	[1 + 1];
		char bldno_main	[5 + 1];
		char bldno_sub	[5 + 1];
		char emd_serial	[10 + 1];
		char move_code	[2 + 1];
		
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
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.grnd_ck, sizeof(cur_row.grnd_ck), token); } //else { strcpy(cur_row.grnd_ck, ""); }
			//printf("grnd_ck >> %s token >> %s\n", cur_row.grnd_ck, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bldno_main, sizeof(cur_row.bldno_main), token); } //else { strcpy(cur_row.bldno_main, ""); }
			//printf("bldno_main >> %s token >> %s\n", cur_row.bldno_main, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.bldno_sub, sizeof(cur_row.bldno_sub), token); } //else { strcpy(cur_row.bldno_sub, ""); }
			//printf("bldno_sub >> %s token >> %s\n", cur_row.bldno_sub, token);
						
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.emd_serial, sizeof(cur_row.emd_serial), token); } //else { strcpy(cur_row.emd_serial, ""); }
			//printf("emd_serial >> %s token >> %s\n", cur_row.emd_serial, token);
			
			token = strsep_cust(&line, "|"); 
			if(token && strcmp(token, "") != 0) { trim(token), strcpy_s(cur_row.move_code, sizeof(cur_row.move_code), token); } //else { strcpy(cur_row.hjd_cd, ""); }
			//printf("move_code >> %s token >> %s\n", cur_row.move_code, token);
			
			
			if(bind_text(db, stmt, 1, cur_row.bjd_cd, strlen(cur_row.bjd_cd)) == false){ log_print_error("Fail to insert column 1: %s\n", cur_row.bjd_cd); }
			if(bind_text(db, stmt, 2, cur_row.sido_nm, strlen(cur_row.sido_nm)) == false){ log_print_error("Fail to insert column 2: %s\n", cur_row.sido_nm); }
			if(bind_text(db, stmt, 3, cur_row.sigg_nm, strlen(cur_row.sigg_nm)) == false){ log_print_error("Fail to insert column 3: %s\n", cur_row.sigg_nm); }
			if(bind_text(db, stmt, 4, cur_row.emd_nm, strlen(cur_row.emd_nm)) == false){ log_print_error("Fail to insert column 4: %s\n", cur_row.emd_nm); }
			if(bind_text(db, stmt, 5, cur_row.ri_nm, strlen(cur_row.ri_nm)) == false){ log_print_error("Fail to insert column 5: %s\n", cur_row.ri_nm); }
			if(bind_text(db, stmt, 6, cur_row.san_ck, strlen(cur_row.san_ck)) == false){ log_print_error("Fail to insert column 6: %s\n", cur_row.san_ck); }
			if(bind_text(db, stmt, 7, cur_row.jibun_main, strlen(cur_row.jibun_main)) == false){ log_print_error("Fail to insert column 7: %s\n", cur_row.jibun_main); }
			if(bind_text(db, stmt, 8, cur_row.jibun_sub, strlen(cur_row.jibun_sub)) == false){ log_print_error("Fail to insert column 8: %s\n", cur_row.jibun_sub); }
			if(bind_text(db, stmt, 9, cur_row.rd_cd, strlen(cur_row.rd_cd)) == false){ log_print_error("Fail to insert column 9: %s\n", cur_row.rd_cd); }
			if(bind_text(db, stmt, 10, cur_row.grnd_ck, strlen(cur_row.grnd_ck)) == false){ log_print_error("Fail to insert column 11: %s\n", cur_row.grnd_ck); }
			if(bind_text(db, stmt, 11, cur_row.bldno_main, strlen(cur_row.bldno_main)) == false){ log_print_error("Fail to insert column 12: %s\n", cur_row.bldno_main); }
			if(bind_text(db, stmt, 12, cur_row.bldno_sub, strlen(cur_row.bldno_sub)) == false){ log_print_error("Fail to insert column 13: %s\n", cur_row.bldno_sub); }
			if(bind_text(db, stmt, 13, cur_row.emd_serial, strlen(cur_row.emd_serial)) == false){ log_print_error("Fail to insert column 14: %s\n", cur_row.emd_serial); }	
			if(bind_text(db, stmt, 14, cur_row.move_code, strlen(cur_row.move_code)) == false){ log_print_error("Fail to insert column 14: %s\n", cur_row.move_code); }	
			
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
        LOG_TRACE(LOG_ERROR, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
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
