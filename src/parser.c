#include "parser.h"

time_t get_timestamp(int month, int day, int year, int hour, int min, int sec) {

	struct tm date;

	date.tm_sec	= sec;
	date.tm_min	= min;
	date.tm_hour	= hour;
	date.tm_mday	= day;
	date.tm_mon	= month-1;
	date.tm_year	= year - 1900;

  	return mktime(&date);
}

int store(FILE *logfile, sqlite3 *database) {

	char buff[128];

	char *logline = NULL;
	size_t buff_size=0;

	int month=0, day=0, year=0, hour=0, min=0, sec=0;
	int up_days=0, up_hours=0, up_mins=0, up_sec=0; 

	char counter_name[32], thread_name[32];
	long long int packet_count;
	int line=0;

	int new_dump;


	int nb_dump = 0;
	int nb_run = 0;
	int nb_line = 0;

	int dump_count = 0;
	int line_count = 0;

	char *insert = malloc(256);
	char *values = malloc(128);
	char *plop = NULL;
	
	int malloc_size = 128;
	
	int diff = 0;
	

	int first = 0;
	int run_timestamp=0, uptime=0, last_uptime = 0, last_timestamp = 0;

	char *err_msg;

	sqlite3_exec(database, "PRAGMA synchronous = OFF", NULL, NULL, &err_msg);

	while(getline(&logline, &buff_size, logfile) > 0) {
	
		if(strstr(logline, "----") == logline) 
			continue; //we ignore lines starting by '----'

		if(strstr(logline, "Counter") == logline) 
			continue; //we ignore lines starting by 'Counter'

		if(strstr(logline, "Date") == logline) {

			if(nb_line != 0) { //we just detected a new dump

				dump_count++;

				if(sqlite3_exec(database, "COMMIT TRANSACTION", NULL, NULL, &err_msg) != SQLITE_OK) {

					printf("Error while commiting data : %s\n", err_msg);
					free(insert);
				}

				last_timestamp = run_timestamp;
				last_uptime = uptime;
				nb_line = 0;
			}
			

			new_dump = sscanf(logline, DUMP_DATE_FRMT, 
				&month, &day, &year, &hour, &min, &sec, 
				&up_days, &up_hours, &up_mins, &up_sec) == 10;	
			
			run_timestamp = get_timestamp(month, day, year, hour, min, sec);

			uptime = (up_days DAY) + (up_hours HOUR) + (up_mins MINUTE) + (up_sec SEC);
			
		 	if(uptime < last_uptime) { //new run

				//printf("Dumps treated : %d\n", dump_count);

				//printf("New run detected ! (%d)\n", run_timestamp);
				
				//dump_count=0;

				nb_run++;
			}


			first = 1;

//			printf("New dump detected : t=%d, u=%d\n", run_timestamp, uptime);

			if(sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, &err_msg) != SQLITE_OK) {

				printf("Error begin : %s\n", err_msg);
				free(insert);
			}

			continue;
		}
	
		nb_line++;
		line_count++;
		

		line = sscanf(logline, DUMP_COUNT_FRMT, &counter_name, &thread_name, &packet_count);

		//printf("nb_run");

		sprintf(insert, "INSERT INTO log_line VALUES (%d, %d, '%s', '%s', %lli);", 
			nb_run, 
			uptime, 
			counter_name, 
			thread_name, 
			packet_count
		);

		if(sqlite3_exec(database, insert, NULL, NULL, &err_msg) != SQLITE_OK) {

			printf("Error while inserting data : %s\n", err_msg);
			//free(insert);
		}
	}

	printf("Treated %d run(s) in %d dumps (%d lines).\n", nb_run+1, dump_count+1, line_count);
	
	return line_count;
}
