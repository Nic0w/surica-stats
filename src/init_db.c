#include "init_db.h"

int init_database(sqlite3 *database) {
	
	char *err_msg = NULL;


	return sqlite3_exec(database,  LOG_LINE_TABLE, NULL, NULL, &err_msg) == SQLITE_OK ? 0 : -1;

}
