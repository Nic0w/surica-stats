SUBDIRS = src

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ all

all: subdirs

clean: 
	rm -Rf *.o surica-stats
