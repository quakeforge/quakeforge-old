SUBDIRS = qw_client qw_server standalone	#standalone isn't done merging yet

all %:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir $@; \
	done
