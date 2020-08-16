CFLAGS += $(SYNBENCH_DATASIZE) $(BENCHMARK_CONFIG)

install: $(TARGET).img
	cp -p $< /home/caspar/git/Raspberry-Pi-Networkboot/tftp/ac558ec1/

install3: $(TARGET).img
	cp -p $< /home/caspar/git/Raspberry-Pi-Networkboot/tftp/bcab9c80/
