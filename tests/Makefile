all: clean subtests

subtests:
	$(MAKE) -C dirents_stat_mv_tests/ all
	$(MAKE) -C mysql_client_tests/    all
	$(MAKE) -C pthread_tests/         all
	$(MAKE) -C hashtables_tests/      all
	$(MAKE) -C geoip_tests/           all

clean:
	$(MAKE) -C dirents_stat_mv_tests clean
	$(MAKE) -C mysql_client_tests    clean
	$(MAKE) -C pthread_tests         clean
	$(MAKE) -C hashtables_tests      clean
	$(MAKE) -C geoip_tests/          clean
