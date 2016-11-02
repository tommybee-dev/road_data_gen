#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "road_name.h"
#include "dblib.h"


int test_select (void)
{
	int i;
	int ret;
	int ncols;
	char stmt [MAX_SQL_LEN];
	char data [MAX_DATA_LEN];
	
	strcpy (stmt, "SELECT * FROM TB_LOG_USR_LOGIN WHERE USERID = 'b_admin' AND USERIP='192.168.200.114';");
	
	//ret = ora_connect ("scott", "tiger", NULL);
	ret = ora_connect_tns("Asks_test/Asks_test@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=168.154.182.72)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=ECS)))");
	
	if(ret == SUCCESS)
	{
		printf("success!!!\n");
	}
	
	ret = ora_setstmt (stmt);
	
	ret = ora_select_open (&ncols);
	printf ("\n-------------------ncols %d------------------------------------\n", ncols);
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_fetch_name (data, i);
		printf ("%-10s ", data);
	}
	printf ("\n-------------------------------------------------------\n");
	
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_set_column (i, VARCHAR2, MAX_DATA_LEN);
	}
	
	
	while (ora_select_fetch_row () == SUCCESS)
	{
	
		if(ora_sqlcode () == FETCH_DONE) break;
		
		for (i = 1; i <= ncols; i ++)
		{
			ret = ora_select_fetch_data (data, i);
			
			printf ("%-10s", data);
		}
		printf ("\n");
	}
	
	ret = ora_disconnect (ROLLBACK);
	
	exit (EXIT_SUCCESS);
}

int test_arg_select (void)
{
	int i;
	int ret;
	int ncols;
	char stmt [MAX_SQL_LEN];
	char data [MAX_DATA_LEN];
	
	//strcpy (stmt, "SELECT * FROM TB_LOG_USR_LOGIN WHERE USERID = 'b_admin' AND USERIP='192.168.200.114';");
	strcpy (stmt, "SELECT * FROM TB_LOG_USR_LOGIN WHERE USERID = :userid AND USERIP=:userip;");
	
	//ret = ora_connect ("scott", "tiger", NULL);
	ret = ora_connect_tns("Asks_test/Asks_test@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=168.154.182.72)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=ECS)))");
	
	if(ret == SUCCESS)
	{
		printf("success!!!\n");
	}
	
	ret = ora_setstmt (stmt);
	
	ret = ora_select_open (&ncols);
	
	printf ("\n-------------------ncols %d------------------------------------\n", ncols);
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_fetch_name (data, i);
		printf ("%-10s ", data);
	}
	printf ("\n-------------------------------------------------------\n");
	
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_set_column (i, VARCHAR2, MAX_DATA_LEN);
	}
	
	
	while (ora_select_fetch_row () == SUCCESS)
	{
	
		if(ora_sqlcode () == FETCH_DONE) break;
		
		for (i = 1; i <= ncols; i ++)
		{
			ret = ora_select_fetch_data (data, i);
			
			printf ("%-10s", data);
		}
		printf ("\n");
	}
	
	ret = ora_disconnect (ROLLBACK);
	
	exit (EXIT_SUCCESS);
}

int test_arg_auto_select (void)
{
	int i;
	int ret;
	int ncols;
	char stmt [MAX_SQL_LEN];
	char data [MAX_DATA_LEN];
	
	//strcpy (stmt, "SELECT * FROM TB_LOG_USR_LOGIN WHERE USERID = 'b_admin' AND USERIP='192.168.200.114';");
	//strcpy (stmt, "SELECT * FROM TB_LOG_USR_LOGIN WHERE USERID = :userid AND USERIP=:userip;");
	strcpy (stmt, "SELECT * FROM TB_ADDR_MAP_TEMP_REF_JIBUN WHERE BJD_CD = :bjd_cd;");
	
	//ret = ora_connect ("scott", "tiger", NULL);
	ret = ora_connect_tns("Asks_test/Asks_test@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=168.154.182.72)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=ECS)))");
	
	if(ret == SUCCESS)
	{
		printf("success!!!\n");
	}
	
	ret = ora_setstmt (stmt);
	
	ret = ora_bind_params (&ncols, (char*)"%s", (char*)"2911012200");
	
	printf ("\n-------------------ncols %d------------------------------------\n", ncols);
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_fetch_name (data, i);
		printf ("%-10s ", data);
	}
	printf ("\n-------------------------------------------------------\n");
	
	for (i = 1; i <= ncols; i ++)
	{
		ret = ora_select_set_column (i, VARCHAR2, MAX_DATA_LEN);
	}
	
	
	while (ora_select_fetch_row () == SUCCESS)
	{
	
		if(ora_sqlcode () == FETCH_DONE) break;
		
		for (i = 1; i <= ncols; i ++)
		{
			ret = ora_select_fetch_data (data, i);
			
			printf ("%-10s", data);
		}
		printf ("\n");
	}
	
	ret = ora_disconnect (ROLLBACK);
	
	exit (EXIT_SUCCESS);
}


int test_arg_auto_insert (char * tns)
{
	//int i;
	int ret;
	int ncols;
	int idx;
	char stmt [MAX_SQL_LEN];
	//char data [MAX_DATA_LEN];
	
	char sql_qry [MAX_DATA_LEN];
	
	idx = sprintf(sql_qry, "%s", "INSERT INTO TB_ADDR_MAP_TEMP_REF_JIBUN ");
	idx += sprintf(sql_qry+idx, "%s", "(                                       ");
	idx += sprintf(sql_qry+idx, "%s", "	BJD_CD,                              ");
	idx += sprintf(sql_qry+idx, "%s", "	SIDO_NM,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	SIGG_NM,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	EMD_NM,                              ");
	idx += sprintf(sql_qry+idx, "%s", "	RI_NM,                               ");
	idx += sprintf(sql_qry+idx, "%s", "	SAN_CK,                              ");
	idx += sprintf(sql_qry+idx, "%s", "	JIBUN_MAIN,                          ");
	idx += sprintf(sql_qry+idx, "%s", "	JIBUN_SUB,                           ");
	idx += sprintf(sql_qry+idx, "%s", "	RD_CD,                               ");
	idx += sprintf(sql_qry+idx, "%s", "	GRND_CK,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	BLDNO_MAIN,                          ");
	idx += sprintf(sql_qry+idx, "%s", "	BLDNO_SUB,                           ");
	idx += sprintf(sql_qry+idx, "%s", "	EMD_SERIAL,                          ");
	idx += sprintf(sql_qry+idx, "%s", "	MOVE_CODE                            ");
	idx += sprintf(sql_qry+idx, "%s", ") VALUES (                              ");
	idx += sprintf(sql_qry+idx, "%s", "	:BJD_CD,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	:SIDO_NM,                            ");
	idx += sprintf(sql_qry+idx, "%s", "	:SIGG_NM,                            ");
	idx += sprintf(sql_qry+idx, "%s", "	:EMD_NM,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	:RI_NM,                              ");
	idx += sprintf(sql_qry+idx, "%s", "	:SAN_CK,                             ");
	idx += sprintf(sql_qry+idx, "%s", "	:JIBUN_MAIN,                         ");
	idx += sprintf(sql_qry+idx, "%s", "	:JIBUN_SUB,                          ");
	idx += sprintf(sql_qry+idx, "%s", "	:RD_CD,                              ");
	idx += sprintf(sql_qry+idx, "%s", "	:GRND_CK,                            ");
	idx += sprintf(sql_qry+idx, "%s", "	:BLDNO_MAIN,                         ");
	idx += sprintf(sql_qry+idx, "%s", "	:BLDNO_SUB,                          ");
	idx += sprintf(sql_qry+idx, "%s", "	:EMD_SERIAL,                         ");
	idx += sprintf(sql_qry+idx, "%s", "	:MOVE_CODE                           ");
	idx += sprintf(sql_qry+idx, "%s", "); ");
   
	
	strcpy (stmt, sql_qry);
	//printf("%s\n", sql_qry);

	ret = ora_connect_tns(tns);
	
	if(ret == SUCCESS)
	{
		printf("[connect] success!!!\n");
	}
	
	ret = ora_setstmt (stmt);
	
	if(ret == SUCCESS)
	{
		printf("[setstmt] success!!!\n");
	}
	
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
	} rowval_t;
	
	char args[] = "%s %s %s %s %s %s %s %s %s %s %s %s %s %s";
	rowval_t row;
	
	/*
	char tmpstr[FILENAME_MAX] = {0x00, };
	char *output_buf_ptr;
	
	open_iconv("UTF-8", "EUC-KR");
	memcpy(row.sido_nm, "±¤ÁÖ±¤¿ª½Ã", sizeof(row.sido_nm));
	memset(tmpstr, 0x00, sizeof(tmpstr));
	output_buf_ptr = tmpstr;
	iconv_utf8_kr(row.sido_nm, &output_buf_ptr);
	
	
	memset(&row.sido_nm, 0x00, sizeof(row.sido_nm));
	memcpy(row.sido_nm, output_buf_ptr, sizeof(row.sido_nm));
	printf("---> %s\n", row.sido_nm );
	close_iconv();
	
	open_iconv("EUC-KR", "UTF-8");
	memset(tmpstr, 0x00, sizeof(tmpstr));
	output_buf_ptr = tmpstr;
	iconv_utf8_kr(row.sido_nm, &output_buf_ptr);
	printf("%s ---> %s", row.sido_nm, output_buf_ptr );
	close_iconv();
	*/
	
	memset(&row, 0x00, sizeof(rowval_t));
	memcpy(row.bjd_cd, "2911012200", sizeof(row.bjd_cd));
	memcpy(row.sido_nm, "±¤ÁÖ±¤¿ª½Ã", sizeof(row.sido_nm));
	memcpy(row.sigg_nm, "µ¿±¸", sizeof(row.sigg_nm));
	memcpy(row.emd_nm, "ÇÐµ¿", sizeof(row.emd_nm));
	memcpy(row.ri_nm, "00", sizeof(row.ri_nm));
	memcpy(row.san_ck, "0", sizeof(row.san_ck));
	memcpy(row.jibun_main, "81", sizeof(row.jibun_main));
	memcpy(row.jibun_sub, "0", sizeof(row.jibun_sub));
	memcpy(row.rd_cd, "291102009001", sizeof(row.rd_cd));
	memcpy(row.grnd_ck, "0", sizeof(row.grnd_ck));
	memcpy(row.bldno_main, "15", sizeof(row.bldno_main));
	memcpy(row.bldno_sub, "0", sizeof(row.bldno_sub));
	memcpy(row.emd_serial, "2967", sizeof(row.emd_serial));
	memcpy(row.move_code, "", sizeof(row.move_code));
	
	
	/*
	ret = ora_bind_params (&ncols,args, 
		row.bjd_cd, row.sido_nm, row.sigg_nm, row.emd_nm, 
		row.ri_nm, row.san_ck, row.jibun_main, row.jibun_sub, 
		row.rd_cd, row.grnd_ck, row.bldno_main, row.bldno_sub, 
		row.emd_serial, row.move_code);
	*/	
	ret = ora_bind_u_params(
		args, 
		row.bjd_cd, row.sido_nm, row.sigg_nm, row.emd_nm, 
		row.ri_nm, row.san_ck, row.jibun_main, row.jibun_sub, 
		row.rd_cd, row.grnd_ck, row.bldno_main, row.bldno_sub, 
		row.emd_serial, row.move_code);
		//"%s %s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","±¤ÁÖ±¤¿ª½Ã","µ¿±¸","ÇÐµ¿","","0","81","0","291102009001","0","15","0","2967", ""); 
		//"%s %s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","dddd","sss","aaa","00","0","81","0","291102009001","0","15","0","2967","aa"); 
		//"%s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","dddd","sss","aaa","4","0","81","0","291102009001","0","15","0","2967"); 
	
	ret = ora_execute (&ncols);
	
	if(ret == SUCCESS)
	{
		printf("insert success!!![%d]\n", ncols);
	}
	
	ret = ora_disconnect (ROLLBACK);
	
	exit (EXIT_SUCCESS);
}

int ora_test_road (void)
{
	
	int ret;
	char *tns = (char*)"Asks_test/Asks_test@(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=168.154.182.72)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=ECS)))";
	//ret = test_select();
	
	//ret = test_arg_select ();
	
	//ret = test_arg_auto_select();
	ret = test_arg_auto_insert(tns);
	
	return ret;
}

int add_addr (table_move_t *table_scheme)
{
	int rc;
	int ret;
	int ret_ora;
	int cols;
	int ncols;
	char err[1024];
   
    char *errmsg;
    
	sqlite3 *db;
    //char *err_msg = 0;
    //char sql[1024];
    char stmt [MAX_SQL_LEN];
    
	sqlite3_stmt *statement; /* SQL Statement Object*/
	//ret = test_select();
	//ret = test_arg_select ();
	//ret = test_arg_auto_select();
	//printf("[tns] %s\n", table_scheme->tns);
	//ret = test_arg_auto_insert(table_scheme->tns);
	
	//LOG_TRACE(LOG_ERROR, "Select %s table...\n", ptsmsopt->tablename);
    
    strcpy (stmt, table_scheme->insert_buf);
	//printf("%s\n", sql_qry);

	
	
    rc = sqlite3_open_v2(table_scheme->sqlite_dbase, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    
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
	
	ret = sqlite3_prepare_v2(db, table_scheme->select_buf, -1, &statement, NULL);
	if (ret != SQLITE_OK) {
	
		LOG_TRACE(LOG_ERROR, "Cannot sqlite3_prepare_v2 database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
	}
	
	cols = sqlite3_column_count(statement);
	
	for (int col = 0; col < cols; col++) {
		printf("%s ", sqlite3_column_name(statement, col));
	};
	printf("\n");
	
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
	} rowval_t;
	
	char args[] = "%s %s %s %s %s %s %s %s %s %s %s %s %s %s";
	rowval_t row;
	
	//sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
	
	const unsigned char *token;
	char tmpstr[FILENAME_MAX] = {0x00, };
	char *output_buf_ptr;
	
	
	ret_ora = ora_connect_tns(table_scheme->tns);
	//printf("TNS::: %s\n", table_scheme->tns);
	
	if(ret_ora == SUCCESS) printf("[connect] success!!!\n");
	
	ret_ora = ora_setstmt (stmt);
	
	if(ret_ora == SUCCESS) printf("[setstmt] success!!!\n");
	
	init_descriptor ();
	
	open_iconv("EUC-KR", "UTF-8");
	
	while (true) {
		ret = sqlite3_step(statement);
		if (ret == SQLITE_ROW) {
			open_curs();
			memset(&row, 0x00, sizeof(rowval_t));
			memcpy(row.bjd_cd,     sqlite3_column_text(statement, 0), sizeof(row.bjd_cd));
			
			token = sqlite3_column_text(statement, 1);
			if(token && strcmp((char*)token, "") != 0) { 
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr((char*)token, &output_buf_ptr);
				
				memcpy(row.sido_nm,    output_buf_ptr, sizeof(row.sido_nm));
			} 
			
			token = sqlite3_column_text(statement, 2);
			if(token && strcmp((char*)token, "") != 0) { 
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr((char*)token, &output_buf_ptr);
				
				memcpy(row.sigg_nm,  output_buf_ptr, sizeof(row.sigg_nm));
			}
			
			token = sqlite3_column_text(statement, 3);
			if(token && strcmp((char*)token, "") != 0) { 
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr((char*)token, &output_buf_ptr);
				
				memcpy(row.emd_nm, output_buf_ptr, sizeof(row.emd_nm));
			}
			
			token = sqlite3_column_text(statement, 4);
			if(token && strcmp((char*)token, "") != 0) { 
				memset(tmpstr, 0x00, sizeof(tmpstr));
						
				output_buf_ptr = tmpstr;
				iconv_utf8_kr((char*)token, &output_buf_ptr);
				
				memcpy(row.ri_nm,      sqlite3_column_text(statement, 4), sizeof(row.ri_nm));
			}  
			
			memcpy(row.san_ck,     sqlite3_column_text(statement, 5), sizeof(row.san_ck));
			memcpy(row.jibun_main, sqlite3_column_text(statement, 6), sizeof(row.jibun_main));
			memcpy(row.jibun_sub,  sqlite3_column_text(statement, 7), sizeof(row.jibun_sub));
			memcpy(row.rd_cd,      sqlite3_column_text(statement, 8), sizeof(row.rd_cd));
			memcpy(row.grnd_ck,    sqlite3_column_text(statement, 9), sizeof(row.grnd_ck));
			memcpy(row.bldno_main, sqlite3_column_text(statement, 10), sizeof(row.bldno_main));
			memcpy(row.bldno_sub,  sqlite3_column_text(statement, 11), sizeof(row.bldno_sub));
			memcpy(row.emd_serial, sqlite3_column_text(statement, 12), sizeof(row.emd_serial));
			memcpy(row.move_code,  sqlite3_column_text(statement, 13), sizeof(row.move_code));
			
			
			
			ret_ora = ora_global_bind_params(
				args, 
				row.bjd_cd, row.sido_nm, row.sigg_nm, row.emd_nm, 
				row.ri_nm, row.san_ck, row.jibun_main, row.jibun_sub, 
				row.rd_cd, row.grnd_ck, row.bldno_main, row.bldno_sub, 
				row.emd_serial, row.move_code);
			//"%s %s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","±¤ÁÖ±¤¿ª½Ã","µ¿±¸","ÇÐµ¿","","0","81","0","291102009001","0","15","0","2967", ""); 
			//"%s %s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","dddd","sss","aaa","00","0","81","0","291102009001","0","15","0","2967","aa"); 
			//"%s %s %s %s %s %s %s %s %s %s %s %s %s", "2911012200","dddd","sss","aaa","4","0","81","0","291102009001","0","15","0","2967"); 
			
			//if(ret_ora != SUCCESS)	printf("insert fail!!![%d]\n", ncols);
			
		/*
			for (int col = 0; col < cols; col++) {
				switch (sqlite3_column_type(statement, col)) {
					case SQLITE_INTEGER:
						printf("%d", sqlite3_column_int(statement, col));
					break;
					case SQLITE_FLOAT:
						printf("%f", sqlite3_column_double(statement, col)); 
					break;
					case SQLITE_TEXT:
						printf("%s", sqlite3_column_text(statement, col)); 
					break;
					case SQLITE_NULL:
						printf("NULL"); 
					break;
				}
			};
			printf("\n");
		*/
			ret_ora = ora_execute (&ncols);
			
			close_curs();
		
		} else if (ret == SQLITE_DONE) 
		{
			printf("done =\n");
			break;
		} else {
			printf("ret : %d\n", ret);
			break;
		}
	}
	
	close_descriptor ();
	ret_ora = ora_disconnect (COMMIT);
	
	close_iconv();
	
	//sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &errmsg);
	//sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errmsg);
	
	sqlite3_finalize(statement);
  	sqlite3_close(db);
	
	
	
	//exit (EXIT_SUCCESS);
	
	return ret_ora;
}