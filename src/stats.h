#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

struct logline {

	int run;
	int uptime;
	
	char *counter;
	char *thread;

	long long int packet_count;

	struct logline *next;
};

void print_stats(sqlite3 *database, struct logline * lines);

struct logline *build_logline_list(sqlite3 *database);

void free_log_line(struct logline *last_line);


