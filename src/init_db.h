#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#define LOG_LINE_TABLE "CREATE TABLE \"log_line\" ( \
				\"run_id\" INTEGER, \
				\"uptime\" INTEGER, \
				\"counter\" TEXT, \
				\"thread\" TEXT, \
				\"packet_count\" INTEGER \
			);"

#define RUN_TABLE "CREATE TABLE \"run\" ( \
		\"run_id\" INTEGER, \
    		\"start_time\" INTEGER, \
    		\"duration\" INTEGER \
	);"


int init_database(sqlite3 *database);
