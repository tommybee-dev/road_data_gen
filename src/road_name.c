#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> 

#include "road_name.h"

//static int ncount;    // 쓰레드간 공유되는 자원 
//static pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER; // 쓰레드 초기화 
static char ini_location[FILENAME_MAX];
static void download(char *path, char *downurl, char *downpath, char *outfilename);

typedef enum {
    SOURCE_TYPE_NONE,
    SOURCE_TYPE_DOWNLOAD,
    SOURCE_TYPE_UNZIP,
    SOURCE_TYPE_SQLITE,
    SOURCE_TYPE_ORACLE,
    SOURCE_TYPE_TEST
} source_type_t;

typedef struct dispatch_table_s {
    const char *cmdline_name;
    int argument_count;
    const char *arg_names;
    const char *description;
    int (*function)(int argc, char *argv[]);
} dispatch_table_t;

//http://mwultong.blogspot.com/2006/10/c-string-replace-all.html
static int move_data_ora(int argc, char *argv[])
{
	char *table_name;
	char *user;
	char *passwd;
	char *tns_id; //[] = "tns";
	char *sel_id; //[] = "select";
	char *ins_id; //[] = "insert";
	
	char sfile[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char tnsbuf[1024] = {0x00, };
	char sbuf[1024] = {0x00, };
	char ibuf[1024] = {0x00, };
	
	char ubuf[512] = {0x00, };
	char pbuf[512] = {0x00, };
	
	char *schmefile = sfile;
	char *section4 = (char*)SECTION4;
	char *database = dbase;
	char *tns = tnsbuf;
	char *sel_buf = sbuf;
	char *ins_buf = ibuf;
	
	user = ubuf;
	passwd = pbuf;
	
	printf("%s \n " , ini_location);
	table_move_t table_scheme;
	
	load_ini(ini_location);

	read_ini(section4, "SCHEME", NULL, &schmefile);
	read_ini(section4, "DATA", NULL, &database);
	read_ini(section4, "USER", NULL, &user);
	read_ini(section4, "PASSWD", NULL, &passwd);

	log_print_error("schmefile[%s] \n", schmefile);
	//printf("---> areas[%s] \n", areas);
	unload_ini();
	
	table_name = argv[0];
	tns_id = argv[1];
	sel_id = argv[2];
	ins_id = argv[3];
	
	log_print_error("%s \n", table_name);
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	load_depth1(schmefile, tns_id, &tns);
	//log_print_error("%s \n", tns);
	
	load_depth2(schmefile, table_name, sel_id, &sel_buf);
	//log_print_error("%s \n", sel_buf);
	
	load_depth2(schmefile, table_name, ins_id, &ins_buf);
	//log_print_error("%s \n", ins_buf);
	
	memset(table_scheme.tns, 0x00, sizeof(table_scheme.tns));
	
	sprintf(table_scheme.tns, "%s/%s@%s", user, passwd, tns);
	
	//strcpy(table_scheme.tns, tns);
	strcpy(table_scheme.sqlite_dbase, database);
	strcpy(table_scheme.select_buf, sel_buf);
	strcpy(table_scheme.insert_buf, ins_buf);
	strcpy(table_scheme.user, user);
	strcpy(table_scheme.passwd, passwd);
	
	
	add_addr(&table_scheme);
	
	return 0;
}

static int create_rdcode(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	create_rdcode_table(&table_scheme);
	
	return 0;
	
}

static int set_rdcode(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char filepath[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char dndir[FILENAME_MAX] = {0x00, };
	char pfix[FILENAME_MAX] = {0x00, };
	char sfix[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section1 = (char*)SECTION1;
	char *section2 = (char*)SECTION2;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	int total_rows = 0;
	
	char *downdir = dndir;
	char *rdtxt = pfix;
	char *suffix = sfix;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	
	read_ini(section1, "DOWN_PATH", NULL, &downdir);
	
	read_ini(section2, "ROAD_CODE", NULL, &rdtxt);
	read_ini(section2, "GLOBAL_SUFFIX", NULL, &suffix);
	
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	
	log_print_error(" database[%s] \n", database);
	
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	//[./road_name_data/build_daejeontxt]
	//http://www.isfull.com/bbs/board.php?bo_table=tb01&wr_id=1427
	//<< strtok 사용 >>
	//char* token = strtok_s(str, ",");
	//while(token)
	//{
	//   token=strtok(0,",");
	//}

	//<< strtok_s 사용 >>
	// token 파싱된앞문자열, context 남은문자열
	sprintf(filepath, "%s/%s.%s", downdir, rdtxt, suffix);
	log_print_error("filepath [%s]  \n", filepath);
	
	strcpy(table_scheme.txt_path, filepath);
	total_rows = insrt_rdcode_table(&table_scheme);
	log_print_error("Insert total [%d] rows  \n", total_rows);
	
	
	return 0;
}


static int create_addr(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	create_addr_table(&table_scheme);
	
	return 0;
	
}

static int set_addr(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char filepath[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char area[FILENAME_MAX] = {0x00, };
	char dndir[FILENAME_MAX] = {0x00, };
	char pfix[FILENAME_MAX] = {0x00, };
	char sfix[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section1 = (char*)SECTION1;
	char *section2 = (char*)SECTION2;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	char *areas = area;
	int total_rows = 0;
	
	char *downdir = dndir;
	char *prefix = pfix;
	char *suffix = sfix;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	
	read_ini(section1, "DOWN_PATH", NULL, &downdir);
	
	
	read_ini(section2, "JIBUN_PREFIX", NULL, &prefix);
	read_ini(section2, "GLOBAL_SUFFIX", NULL, &suffix);
	
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	read_ini(section3, "AREA_NAMES", NULL, &areas);
	
	log_print_error("database[%s] \n", database);
	//printf("---> areas[%s] \n", areas);
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	//[./road_name_data/build_daejeontxt]
	//http://www.isfull.com/bbs/board.php?bo_table=tb01&wr_id=1427
	//<< strtok 사용 >>
	//char* token = strtok_s(str, ",");
	//while(token)
	//{
	//   token=strtok(0,",");
	//}


	//<< strtok_s 사용 >>
	// token 파싱된앞문자열, context 남은문자열
	char* context = NULL;
	char* token = strtok_s(areas, ",", &context);
	
	sprintf(filepath, "%s/%s%s.%s", downdir, prefix, token, suffix);
	log_print_error("filepath [%s]  \n", filepath);
	
	strcpy(table_scheme.txt_path, filepath);
	total_rows = insrt_addr_table(&table_scheme);
	log_print_error("Insert total [%d] rows  \n", total_rows);
	
	while(token)
	{
		token = strtok_s(NULL, ",", &context);
		
		if(token && (strcmp(token, "") != 0))
		{
			memset(table_scheme.txt_path, 0x00, sizeof(table_scheme.txt_path));
			total_rows = 0;
			
			sprintf(filepath, "%s/%s%s.%s", downdir, prefix, token, suffix);
			//printf("---> area [%s]  \n", token);
			log_print_error("filepath [%s]  \n", filepath);
			
			strcpy(table_scheme.txt_path, filepath);
			total_rows = insrt_addr_table(&table_scheme);
			log_print_error("Insert total [%d] rows  \n", total_rows);
		}
	}
	
	return 0;
}


static int create_bldg(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	create_bldg_table(&table_scheme);
	
	return 0;
	
}

static int set_bldg(int argc, char *argv[])
{
	char *table_name;
	char *operation;
	//const char *filename;
	char sfile[FILENAME_MAX] = {0x00, };
	char filepath[FILENAME_MAX] = {0x00, };
	char dbase[FILENAME_MAX] = {0x00, };
	char area[FILENAME_MAX] = {0x00, };
	char dndir[FILENAME_MAX] = {0x00, };
	char pfix[FILENAME_MAX] = {0x00, };
	char sfix[FILENAME_MAX] = {0x00, };
	char sqlbuf[10240] = {0x00, };
	
	char *schmefile = sfile;
	char *section1 = (char*)SECTION1;
	char *section2 = (char*)SECTION2;
	char *section3 = (char*)SECTION3;
	char *database = dbase;
	char *sql = sqlbuf;
	char *areas = area;
	int total_rows = 0;
	
	char *downdir = dndir;
	char *prefix = pfix;
	char *suffix = sfix;
	
	table_scheme_t table_scheme;
	
	load_ini(ini_location);
	
	read_ini(section1, "DOWN_PATH", NULL, &downdir);
	
	
	read_ini(section2, "BUILDING_PREFIX", NULL, &prefix);
	read_ini(section2, "GLOBAL_SUFFIX", NULL, &suffix);
	
	read_ini(section3, "SCHEME", NULL, &schmefile);
	read_ini(section3, "DATA", NULL, &database);
	read_ini(section3, "AREA_NAMES", NULL, &areas);
	
	log_print_error("database[%s] \n", database);
	//printf("---> areas[%s] \n", areas);
	unload_ini();
	
	table_name = argv[1];
	operation = argv[2];
	
	memset(&table_scheme, 0x00, sizeof(table_scheme));
	//printf("---> size table_scheme[%d] \n", sizeof(table_scheme));	
	
	pre_load_scheme(schmefile, table_name, operation, &sql);
	
	log_print_error("%s \n", sql);
	
	strcpy(table_scheme.table_name, table_name);
	strcpy(table_scheme.sql, sql);
	strcpy(table_scheme.database, database);
	
	//[./road_name_data/build_daejeontxt]
	//http://www.isfull.com/bbs/board.php?bo_table=tb01&wr_id=1427
	//<< strtok 사용 >>
	//char* token = strtok_s(str, ",");
	//while(token)
	//{
	//   token=strtok(0,",");
	//}


	//<< strtok_s 사용 >>
	// token 파싱된앞문자열, context 남은문자열
	char* context = NULL;
	char* token = strtok_s(areas, ",", &context);
	
	sprintf(filepath, "%s/%s%s.%s", downdir, prefix, token, suffix);
	log_print_error("filepath [%s]  \n", filepath);
	
	strcpy(table_scheme.txt_path, filepath);
	total_rows = insrt_addr_table(&table_scheme);
	log_print_error("Insert total [%d] rows  \n", total_rows);
	
	while(token)
	{
		token = strtok_s(NULL, ",", &context);
		
		if(token && strcmp(token, "") != 0)
		{
			memset(table_scheme.txt_path, 0x00, sizeof(table_scheme.txt_path));
			total_rows = 0;
			
			sprintf(filepath, "%s/%s%s.%s", downdir, prefix, token, suffix);
			//printf("---> area [%s]  \n", token);
			log_print_error("filepath [%s]  \n", filepath);
			
			strcpy(table_scheme.txt_path, filepath);
			total_rows = insrt_addr_table(&table_scheme);
			log_print_error("Insert total [%d] rows  \n", total_rows);
		}
	}
	
	return 0;
}

dispatch_table_t dispatch_table[] = {
    { "bldg_c", 3, "building data table scheme", "create building data to the table of sqlite database", create_bldg },
    { "bldg_s", 3, "set table data to sqlite database", "create building table to sqlite database", set_bldg },
    { "addr_c", 3, "address data table scheme", "create address data to the table of sqlite database", create_addr },
    { "addr_s", 3, "set table data to sqlite database", "create address table to sqlite database", set_addr },
    { "rdcode_c", 3, "address data table scheme", "create address data to the table of sqlite database", create_rdcode },
    { "rdcode_s", 3, "set table data to sqlite database", "set address table to sqlite database", set_rdcode },
    { "move_to_ora", 4, "move sqlite table data to oracle database", "move to oracle database", move_data_ora },
};

static int
dispatch(int argc, char *argv[])
{
    unsigned int i;
    
    
    for (i=0; i<sizeof(dispatch_table)/sizeof(dispatch_table_t); i++) {
    	//fprintf(stderr, "command '%s' %s\n", dispatch_table[i].cmdline_name, argv[0]);
    	
		if (strcmp(dispatch_table[i].cmdline_name, argv[0]) == 0) {
		    argc--;
		    argv++;
		    /* 1 for the command, argument_count for the arguments */
		    if (argc < dispatch_table[i].argument_count) {
				fprintf(stderr, "not enough arguments for command '%s': %d available, %d needed\n", dispatch_table[i].cmdline_name, argc, dispatch_table[i].argument_count);
				return -1;
		    }
		    if (dispatch_table[i].function(argc, argv) == 0)
			return 1 + dispatch_table[i].argument_count;
		    return -1;
		}
    }

    fprintf(stderr, "unknown command '%s'\n", argv[0]);
    return -1;
}


void getIniValue(char *section, char *key, char **output_buf_ptr)
{
	char ret_buf[1024] = {0x00, }; 
	char *value;
	
#ifdef WITH_CONV
	size_t in_size, out_size;
#endif	
	
	value = ret_buf;
	
	read_ini(section, key, NULL, &value);
	printf("in [%s][%s]=--> [%s] \n", section, key, value);
	
	
#ifdef WITH_CONV
	char tmpbuf[1024];	
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	
	in_size = strlen(value) +1;
	out_size = sizeof(tmpbuf);
	
	convStr( tmpbuf, &out_size, ret_buf, in_size, "EUC-KR", "UTF-8");
	memcpy(*output_buf_ptr, tmpbuf, strlen(tmpbuf) + 1);
	
#else
	memcpy(*output_buf_ptr, value, strlen(value)+1);
#endif

	printf("---> out[%s] \n", *output_buf_ptr);	
}

void unzipRoadData(char *downpath, char *path, char* outfilename)
{
	char buff[1024];
	char tmpbuf[1024];	
	char ret_buf[1024];	
	
	size_t in_size, out_size;
	
	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	memset(ret_buf, 0x00, sizeof(ret_buf));
	
	create_dir(downpath);
	
	rename(outfilename, path);

	if (chdir(downpath) == 0) {
	    printf("Directory changed successfully.[%s] --> %s\n",outfilename, path);
	}else{
	    printf("Could not change directory.[%s] --> %s\n",outfilename, path);
	}
	
	in_size = strlen(outfilename) +1;
	out_size = sizeof(tmpbuf);
	
	convStr( tmpbuf, &out_size, outfilename, in_size, "UTF-8", "EUC-KR");
	
	getcwd( buff, 1024);                   // 작업 디렉토리 구하기
	printf( "=: Current Working dir[%s]\n", buff);
	
	printf( "=: Current file[%s][%s]\n", tmpbuf, outfilename);
	smpl_unzip(tmpbuf);
}

void parseScheme(char *filename)
{
	//char *retText = new char[FILENAME_MAX];
	
	//retText = (char*)malloc(sizeof(char)*FILENAME_MAX);
	
	//char **xmlText = &retText;
	//const char *table_name = "tb_addr_map_tmp2";
	//const char *oper = "create";
	
	//pre_load_scheme(filename, table_name, oper, &retText);
	
	//printf("---> out[%s] \n", retText);	
	
	//if(retText) free(retText);
	//delete retText;
	//retText = NULL;
}

static void
usage(const char *progname, const char *reason)
{
    unsigned int i;
    FILE *out;
    
    if (reason == NULL) out = stdout;
    else out = stderr;
	
    fprintf(out, "usage: %s [-duhso] ini \n", progname);
    
    if (reason != NULL) {
		fprintf(out, "%s\n", reason);
		exit(1);
    }

    fprintf(out, "\nSupported options are:\n"
	    "\t-d\tdownload road name ascii text, which is zip file format from url specified in INI file\n"
	    "\t-u\tunzip zip file (must use with -d option)\n"
	    "\t-h\tdisplay this usage\n"
	    "\t-s\tcopy extracted road name data to sqlite database with scheme provided with xml file\n"
	    "\t-o\tfrom sqlite database to oracle database after verificantion of data\n"
	    "\t-t\ttest zip file, extract the data from that, make sqlite database and tranfer database to oracle database finally\n");
	    
    for (i=0; i<sizeof(dispatch_table)/sizeof(dispatch_table_t); i++) {
		fprintf(out, "\t%s %s\n\t    %s\n\n", dispatch_table[i].cmdline_name, dispatch_table[i].arg_names, dispatch_table[i].description);
    }
    
    fprintf(out, "\nThe index is zero-based.\n");
    exit(0);
}

int main(int argc, char *argv[])
{
	const char *archive;
	//unsigned int i;
	int c, arg, err, flags;
	const char *prg;
	source_type_t source_type = SOURCE_TYPE_NONE;
	
	flags = 0;
	prg = argv[0];
	
	if (argc < 2) usage(prg, "too few arguments");
	
	while ((c=getopt(argc, argv, "duhsot")) != -1) {
		switch (c) {
			case 'd':
			    flags |= ROAD_DOWNLOAD;
			    source_type = SOURCE_TYPE_DOWNLOAD;
			    break;
			case 'u':
			    flags |= ROAD_UNZIP;
			    source_type = SOURCE_TYPE_UNZIP;
			    break;
			case 'h':
			    usage(prg, NULL);
			    break;
			case 's':
			    flags |= ROAD_SQLITE;
			    source_type = SOURCE_TYPE_SQLITE;
			    break;
			case 'o':
			    flags |= ROAD_ORACLE;
			    source_type = SOURCE_TYPE_ORACLE;
			    break;
			case 't':
			    flags |= ROAD_TEST;
			    source_type = SOURCE_TYPE_TEST;
			    break;
			
			default:
			{
			    char reason[128];
			    snprintf(reason, sizeof(reason), "invalid option -%c", optopt);
			    usage(prg, reason);
			}
		}
	}
	
	arg = optind;
	
	archive = argv[arg++];
	
	if (flags == 0) flags = ROAD_DOWNLOAD;
	
	char durl[FILENAME_MAX] = {0x00, }; 
	char filename[FILENAME_MAX] = {0x00, }; 
	char dpath[FILENAME_MAX] = {0x00, }; 
	char path[FILENAME_MAX] = {0x00, }; 
	char sfile[FILENAME_MAX] = {0x00, };
	
	char *downpath;
	char *downurl;
	
	char *outfilename = filename;
	char *schmefile = sfile;
	char *section1 = (char*)SECTION1;
	char *section3 = (char*)SECTION3;
	
	if(!archive)
	{
		load_ini(INI_FILE);
		strcpy(ini_location, INI_FILE);
	}
	else
	{
		load_ini(archive);
		strcpy(ini_location, archive);
	}
	
	//pthread_mutex_lock(&mutex); // 잠금을 생성한다.
	
	
	downpath = dpath;
	downurl = durl;
	
	//outfilename = getIniValue(SECTION1, "OUT_FILE", 1);
	//getIniValue(SECTION1, "OUT_FILE", &outfilename);
	read_ini(section1, "OUT_FILE", NULL, &outfilename);
	//printf("file =--> [%s][%d][%d] \n", outfilename, sizeof(outfilename), strlen(outfilename));
	
	//getIniValue(SECTION1, "DOWN_URL", &downurl); 
	read_ini(section1, "DOWN_URL", NULL, &downurl);
	//printf("url =--> [%s][%d][%d] \n", downurl, sizeof(downurl), strlen(downurl));
	
	read_ini(SECTION1, "DOWN_PATH", NULL, &downpath);
	sprintf(path, "%s/%s", downpath, outfilename);
	
	read_ini(section3, "SCHEME", NULL, &schmefile);
	//printf("SCHEME/%s \n", schmefile);
	
	
	switch (source_type) {
		case SOURCE_TYPE_NONE:
			download(path, downurl, downpath, outfilename);
			break;
		
		case SOURCE_TYPE_DOWNLOAD:
			download(path, downurl, downpath, outfilename);
			break;
		
		case SOURCE_TYPE_UNZIP: {
			unzipRoadData(downpath, path, outfilename);
			break;
		}
		case SOURCE_TYPE_SQLITE:
			parseScheme(schmefile);
			break;
		
		case SOURCE_TYPE_ORACLE:
			break;
		
		case SOURCE_TYPE_TEST: {
			/*char *url = "https://www.juso.go.kr/addrlink/addressBuildDev.do";*/
			//strcpy( downurl, 
			//	"https://www.juso.go.kr/dn.do?reqType=ALLRDNM&fileName=201608전체주소(도로명코드)_전체분.zip&realFileName=201608ALLRDNM00.zip&regYmd=2016&ctprvnCd=00&gubun=RDNM&stdde=201608");
			//strcpy(filename, "201608전체주소(도로명코드)_전체분.zip");
			//
			//printf("file =--> [%s][%d][%d] \n", outfilename, sizeof(outfilename), strlen(outfilename));
			//printf("url =--> [%s][%d][%d] \n", downurl, sizeof(downurl), strlen(downurl));
			ora_test_road ();
			break;
		}
	}
	

	//printf("parsing ... done ....%d, %d \n", arg, argc); 
	
	err = 0;
    while (arg < argc) {
	int ret;
		ret = dispatch(argc-arg, argv+arg);
		if (ret > 0) {
		    arg += ret;
		} else {
		    err = 1;
		    break;
		}
    }
	
	
	unload_ini();
	
	return err;
}

void download(char *path, char *downurl, char *downpath, char *outfilename)
{
	if(!file_exists(path))
	{
		download_http(downurl, outfilename);
		
	}
	else
		printf("%s exists!!\n", path);
}

