#include "stats.h"

/*
Adds a new element at the top of the list.
*/
struct logline *add_log_line(struct logline *last_line, 
	int run, int uptime, const unsigned char *counter, const unsigned char *thread, long long int packet_count) {

	struct logline *new = malloc(sizeof(struct logline));

	new->run = run;
	new->uptime = uptime;

	new->counter = strndup(counter, 128);
	new->thread = strndup(thread, 128);

	new->packet_count = packet_count;


	new->next = last_line;

	return new;

}

/*
Free the logs
*/
void free_log_line(struct logline *last_line) {


	struct logline *temp = NULL;

	while(last_line != NULL) {

		temp = last_line->next;

		free(last_line->counter);
		free(last_line->thread);

		free(last_line);

		last_line = temp;
	}

}

/*
Store the content of the database into a chained list
*/
struct logline *build_logline_list(sqlite3 *database) {

	char ** runs = NULL;
	int nb_col = 0;
	int nb_run = 0;

	char *err_msg = NULL;

	char *select_all_log_lines = "SELECT * FROM log_line WHERE counter='capture.kernel_packets' OR counter='capture.kernel_drops';";
	sqlite3_stmt *request = NULL;

	//Prepare the request
	sqlite3_prepare_v2(database, select_all_log_lines, strlen(select_all_log_lines), &request, NULL);
	
	struct logline *list = NULL;

	int lines = 0;
	while(sqlite3_step(request) != SQLITE_DONE) { //For each line

		list = add_log_line(list, 
				sqlite3_column_int(request, 0),
				sqlite3_column_int(request, 1),
				sqlite3_column_text(request, 2),
				sqlite3_column_text(request, 3),
				sqlite3_column_int64(request, 4)
			);


		//printf("%d %s %s %lld\n", list->run, list->counter, list->thread, list->packet_count);

		lines++;

	}

	printf("Lines in list : %d\n", lines);

	return list;
}

/*
Print the stats given a database and a list of log lines.
*/
void print_stats(sqlite3 *database, struct logline * lines) {

	char **runs = NULL;
	char *err_msg;
	int nb_run = 0;
	int nb_col = 0;

	//Retrieve a list of runs
	if(sqlite3_get_table(database, "SELECT * FROM run;", &runs, &nb_run, &nb_col, &err_msg) != SQLITE_OK) {

		printf("Error while retrieving runs : %s\n", err_msg);
		free(err_msg);
		
		return;
	}
	
	long long int **run_stats = malloc(sizeof(long long int *) * nb_run);

	int i;
	int run_uptime = 0;
	int counter = 0;

	struct logline *threads = NULL;

	struct logline *temp = lines, *temp_thread;

	for(i = 0; i < nb_run; i++) { //For each run

		run_uptime = atoi(runs[5 + 3*i]); //we get the runtime of this run (max uptime)

		//2D array to store packet count sums of each counters (packets and drops)
		run_stats[i] = malloc(2 * sizeof(long long int));

		run_stats[i][0] = 0;
		run_stats[i][1] = 0;

		//printf("%d\n", run_uptime);
		counter = 1;

		lines = temp;
		threads = NULL;

		while(lines != NULL) {//for each line

			if(lines->uptime != run_uptime) { //we only want the lines of the last dump
				lines = lines->next;
				continue;
			}
	
			//Add thread packet count to a list.
			threads = add_log_line(threads, 0, 0, "", lines->thread, lines->packet_count);

			//add packet count to the counter and store it.
			run_stats[i][counter % 2] += lines->packet_count;

			counter++;

			lines = lines->next;
		}

		double ratio = ((double)run_stats[i][1])/((double)run_stats[i][0]);
		printf("Run %d; capture.kernel_packets=%lld\n", i, run_stats[i][0]);
		printf("Run %d; capture.kernel_drops=%lld\n", i, run_stats[i][1]);
		printf("Drop ratio for run %d : %lf\n", ratio);


		//Print stats per threads
		temp_thread = threads;
		while(threads != NULL) {

			double througput = ((double)threads->packet_count)/((double) run_uptime);

			printf("Thread %s : %.2lf packets/s\n", threads->thread, througput);

			threads = threads->next;
		}
		
		free_log_line(temp_thread);

	}

}

