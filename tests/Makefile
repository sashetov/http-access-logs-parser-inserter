all: clean subtests

subtests:
	$(MAKE) -C uap-cpp/              all
	$(MAKE) -C combined-c-c++-test/  all
	$(MAKE) -C worker-pool/          all

clean:
	$(MAKE) -C uap-cpp/              clean
	$(MAKE) -C combined-c-c++-test/  clean
	$(MAKE) -C worker-pool/          clean
