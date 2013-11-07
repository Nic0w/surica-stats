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
	
	int malloc_size = 128;
	
	int diff = 0;
	

	int first = 1;
	int run_timestamp=0, uptime=0, last_uptime = 0, last_timestamp = 0;

	char *err_msg;
	
	 int nb_rq = 0;

	char *run_insert = malloc(128);
	char *insert = malloc(128);
	char *values = malloc(128);
	sprintf(insert, "INSERT INTO log_line VALUES ");


	sqlite3_exec(database, "PRAGMA synchronous = OFF", NULL, NULL, &err_msg);

	while(getline(&logline, &buff_size, logfile) > 0) {

		if(strstr(logline, "----") == logline) 
			continue; //we ignore lines starting by '----'

		if(strstr(logline, "Counter") == logline) 
			continue; //we ignore lines starting by 'Counter'

		if(strstr(logline, "Date") == logline) {

			if(nb_line != 0) { //we just detected a new dump

				dump_count++;

				last_timestamp = run_timestamp;
				last_uptime = uptime;

			}
			

			new_dump = sscanf(logline, DUMP_DATE_FRMT, 
				&month, &day, &year, &hour, &min, &sec, 
				&up_days, &up_hours, &up_mins, &up_sec) == 10;	
			
			run_timestamp = get_timestamp(month, day, year, hour, min, sec);

			uptime = (up_days DAY) + (up_hours HOUR) + (up_mins MINUTE) + (up_sec SEC);
			
		 	if(uptime < last_uptime) { //We just detected a new run

				sprintf(run_insert, "INSERT INTO run VALUES (%d, %d, %d)", 
							nb_run, last_timestamp - last_uptime, last_uptime);

				if(sqlite3_exec(database, run_insert, NULL, NULL, &err_msg) != SQLITE_OK) {

					printf("Error while inserting new run : %s\n", err_msg);
					//printf("%s\n", insert);			
				}


				nb_run++;
			}

			continue;
		}

		if(nb_line == 500) {

			if(sqlite3_exec(database, insert, NULL, NULL, &err_msg) != SQLITE_OK) {

				printf("Error while inserting data : %s\n", err_msg);
				//printf("%s\n", insert);			
			}

			free(insert);
			insert = malloc(128);
			sprintf(insert, "INSERT INTO log_line VALUES ");

			nb_rq++;
			first = 1;
			nb_line = 0;
		}

		line = sscanf(logline, DUMP_COUNT_FRMT, &counter_name, &thread_name, &packet_count);

		sprintf(values, "%s(%d, %d, '%s', '%s', %lli)", 
			first ? "" : ", ",
			nb_run, 
			uptime, 
			counter_name, 
			thread_name, 
			packet_count
		);

		first = 0;
		
		insert = realloc(insert, 128 + nb_line*128);
		strcat(insert, values);

		line_count++;
		nb_line++;
	}

	if(nb_line != 0) {

		//printf("Treated %d before last request; last request is for %d lines.\n", nb_rq*500, nb_line );
		if(sqlite3_exec(database, insert, NULL, NULL, &err_msg) != SQLITE_OK) {

			printf("Error while inserting data : %s\n", err_msg);
			printf("%s\n", insert);			

			//
			}

			free(insert);
			nb_rq++;
	}

	sprintf(run_insert, "INSERT INTO run VALUES (%d, %d, %d)", 
					nb_run, last_timestamp - last_uptime, last_uptime);

	if(sqlite3_exec(database, run_insert, NULL, NULL, &err_msg) != SQLITE_OK) { //inserting last run

		printf("Error while inserting new run : %s\n", err_msg);
		//printf("%s\n", insert);			
	}


	free(values);
	free(run_insert);

	printf("Inserts in database : %d\n", nb_rq);

	printf("Treated %d run(s) in %d dumps (%d lines).\n", nb_run+1, dump_count+1, line_count);
	
	return line_count;
}
