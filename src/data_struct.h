

struct logline {

	char *counter;
	char *thread;

	long long int value;
};


struct dump {

	struct logline *line;

	struct dump *next;
};

