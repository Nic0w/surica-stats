SOURCE_FILES = main parser init_db
OBJECT_FILES = main.o parser.o init_db.o

.PHONY: objects $(SOURCE_FILES)

objects: $(SOURCE_FILES)

$(SOURCE_FILES): 
	gcc -g -lsqlite3 $@.c -c -o $@.o


all: objects
	gcc -lsqlite3 $(OBJECT_FILES) -o ../surica-stats

clean:
	rm -f *.o

