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


int init_database(sqlite3 *database);
