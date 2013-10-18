#include <stdio.h>
#include <string.h>

#define DUMP_DATE_FRMT "Date: %d/%d/%d -- %d:%d:%d (uptime: %dd, %dh %dm %ds)"

int main(int argc, char *argv[]) {


	FILE *logfile = fopen("C-2013-stats.log", "r");

	char buff[128];

	char *logline = NULL;
	size_t buff_size=0;



	int month, day, year, hour, min, sec;
	int up_days, up_hours, up_mins, up_sec; 

	int new_dump;
	int nb_dump = 0;
	while(getline(&logline, &buff_size, logfile) > 0) {
	
		if(strstr(logline, "----") == logline) 
			continue; //we ignore lines starting by '----'

		if(strstr(logline, "Counter") == logline) 
			continue; //we ignore lines starting by 'Counter'

		new_dump = sscanf(logline, DUMP_DATE_FRMT, 
			&month, &day, &year, &hour, &min, &sec, 
			&up_days, &up_hours, &up_mins, &up_sec) == 10;	

		if(new_dump) {

			
			




			nb_dump++;			
		}

	}

	printf("Dumps : %d\n", nb_dump);


	return 0;
}
