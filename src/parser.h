#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#define SEC  * 1
#define MINUTE * 60 SEC
#define HOUR * 60 MINUTE
#define DAY * 24 HOUR

#define DUMP_DATE_FRMT "Date: %d/%d/%d -- %d:%d:%d (uptime: %dd, %dh %dm %ds)"

#define DUMP_COUNT_FRMT "%[a-z0-9._] | %[A-Za-z0-9] | %lli"

int store(FILE *logfile, sqlite3 *database);
