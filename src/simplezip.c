/* Copyright (C) 2011 rocenting#gmail.com */
/*https://gist.githubusercontent.com/mobius/1759816/raw/0ef83867917977110cccbe924eaf1d2dfdc1405e/libzip_simple.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>

#ifndef __MINGW32__
#include <sys/resource.h>
#endif

#include <sys/types.h>
#include <zip.h>

#include "road_name.h"

const char *prg;
 
static void safe_create_dir(const char *dir)
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

//http://stackoverflow.com/questions/744766/how-to-compare-ends-of-strings-in-c
int ends_with(const char *str, const char *suffix)
{
    char *dot = strrchr(str, '.');

    if (NULL == dot) return 0;
    return strcmp(dot, suffix) == 0;
}

int is_binary(const char *str)
{
	if(ends_with(str, ".pdf")) return 1;
	else if(ends_with(str, ".exe")) return 1;
	else if(ends_with(str, ".bin")) return 1;
	else if(ends_with(str, ".xls")) return 1;
				
    return 0;
}


int smpl_unzip(char *zipname)
{
    const char *archive;
    struct zip *za;
    struct zip_file *zf;
    struct zip_stat sb;
    char buf[100];
    int err;
    int i, len;
    int fd;
    long long sum;
    /*
    prg = argv[0];
    if (argc != 2) {
        fprintf(stderr, "usage: %s archive/n", prg);
        return 1;
    }
 	
    archive = argv[1];
    */
    
    archive = zipname;

    if ((za = zip_open(archive, 0, &err)) == NULL) {
        zip_error_to_str(buf, sizeof(buf), err, errno);
        fprintf(stderr, "%s: can't open zip archive `%s': %s/n", prg,
            archive, buf);
        return 1;
    }
 	
 	char tmpbuf[1024];
 	char *output_buf_ptr;
 	//const char *tmp;
 	output_buf_ptr = tmpbuf;
 	
    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
        if (zip_stat_index(za, i, 0, &sb) == 0) {
            printf("\n==================\n");
            
            len = strlen(sb.name);
            
           
            //printf("Size: [%llu], ", sb.size);
            printf("Size: [%lu], ", (unsigned long)sb.size);
            printf("mtime: [%u]\n", (unsigned int)sb.mtime);
            
            if(has_hangul((char*)sb.name))
			{
				sb.name = zip_get_name(za, i, ZIP_FL_ENC_RAW);
				conv_utf8_kr((char*)sb.name, &output_buf_ptr);
				
				printf("Name: [%s], ", output_buf_ptr);
			}
			else
				printf("Name: [%s], ", sb.name);
			
            if (sb.name[len - 1] == '/') {
                safe_create_dir(sb.name);
            } else {
                zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    fprintf(stderr, "boese, boese\n");
                    exit(100);
                }
 				
 				if(is_binary(sb.name))
					fd = open(sb.name, O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0644);
				else
                	fd = open(sb.name, O_RDWR | O_TRUNC | O_CREAT, 0644);
                	
                if (fd < 0) {
                    fprintf(stderr, "boese, boese[%s]\n", sb.name);
                    exit(101);
                }
 
                sum = 0;
                while (sum != (long long)sb.size) {
                    len = zip_fread(zf, buf, 100);
                    if (len < 0) {
                        fprintf(stderr, "boese, boese\n");
                        exit(102);
                    }
                    write(fd, buf, len);
                    sum += len;
                }
                
                printf("SUM ... [%llu] \n", sum);
                
                close(fd);
                zip_fclose(zf);
            }
        } else {
            printf("File[%s] Line[%d]\n", __FILE__, __LINE__);
        }
    }   
 
    if (zip_close(za) == -1) {
        fprintf(stderr, "%s: can't close zip archive `%s'\n", prg, archive);
        return 1;
    }
 
    return 0;
}