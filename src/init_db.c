#include "init_db.h"

int init_database(sqlite3 *database) {
	
	char *err_msg = NULL;

	printf("Creating table 'log_line'...\n");
	
	if(sqlite3_exec(database,  LOG_LINE_TABLE, NULL, NULL, &err_msg) != SQLITE_OK) {

		printf("Failed : %s\n", err_msg);
		free(err_msg);

		return -1;
	}

	printf("Creating table 'run'...\n");
	
	if(sqlite3_exec(database,  RUN_TABLE, NULL, NULL, &err_msg) != SQLITE_OK) {

		printf("Failed : %s\n", err_msg);
		free(err_msg);

		return -1;
	}

	return 0;
}
