SUBDIRS = qw_client qw_server uquake

all %:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir $@; \
	done
