EXE=road_name_gen
#CFLAGS = -std=gnu99 -fPIC -Wall -I../include
#CFLAGS = -std=gnu99 -fPIC -Wall -I../include -I/mingw32/include
#CFLAGS = -std=gnu99 -Wall -g -I../include -I/mingw32/include 
CFLAGS = -Wall -g -I../include -I/mingw32/include 
LIBS = -L/mingw32/lib -lzip -liconv -L../lib -lcurl -liniparser -lsqliteX -ltinyxml2
#LIBS = -L/usr/lib -lpthread -ldl -L../lib -lcurl -liniparser -lsqliteX -ltinyxml2

TARGET  = ../BIN/$(EXE)
CC=g++
STRIP=strip

LIB_SRCS = log.c log2.c scheme.c sqlite_util.c sqlite_rdcode.c sqlite_bldg.c sqlite_addr.c http_downloader.c util.c road_name.c simplezip.c 
LIB_OBJS = $(LIB_SRCS:%.c=%.o)

.SUFFIXES : .c .o 

all : $(TARGET) 

$(TARGET): $(LIB_OBJS)       
	$(CC) $(CFLAGS) -o $@ $(LIB_OBJS) $(LIBS)
	$(STRIP) $(TARGET).exe 

clean :      
	$(RM) $(LIB_OBJS) $(TARGET)