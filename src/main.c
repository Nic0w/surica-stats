#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#include "parser.h"
#include "init_db.h"

#define MAX_FILENAME_SIZE 256


int main(int argc, char *argv[]) {

	sqlite3 *database = NULL;

	FILE *logfile = NULL;

	char opt;

	char *db_name = malloc(MAX_FILENAME_SIZE);

	int create = 0, delete=0, record = 0, stats = 0;

	while((opt = getopt(argc, argv, "hcdrs")) != -1) {
	
		switch(opt) {

			case 'h' : 
				printf("Options :\n");
				printf("-h : print this message\n");
				printf("-c : create the database identified by [db]\n");
				printf("-d : delete the database identified by [db]\n");
				printf("-r : record in [db] the content of the logs\n");
				printf("-s : produce stats for database [db]\n");

				return 0;

			case 'c' : create = 1; break;

				if(sqlite3_open("runs.db", &database) !=  SQLITE_OK) {
	
					printf("Error opening db !\n");
					return;
				}
				
				init_database(database);
				printf("Database created !\n");
				
				return;
				

			case 'd' : delete = 1; break;
			case 'r' : record = 1; break;
				
				if(sqlite3_open("runs.db", &database) !=  SQLITE_OK) {
	
					printf("Error opening db !\n");
					return;
				}

			case 's' : stats = 1; break;
				
				

			default : break; 
				//printf("Error, unknown option !\n");
				//return 0;			
		}
		
	}	

	strncpy(db_name, argv[optind++], MAX_FILENAME_SIZE);

	if(delete) {

		unlink(db_name);
		return 0;
	}


	if(sqlite3_open(db_name, &database) !=  SQLITE_OK) {
	
		printf("Error opening db : %s\n", sqlite3_errmsg(database));
		return 0;
	}


	if(create) {

		init_database(database);

		printf("Database was created !\n");
		
		return 0;
	}

	if(record) {
		
		while(optind < argc) {

			logfile = fopen(argv[optind++], "r");

			store(logfile, database);

			fclose(logfile);
		}

		sqlite3_close_v2(database);

		return 0;

	}

	if(stats) {



	}

	return 0;
}
