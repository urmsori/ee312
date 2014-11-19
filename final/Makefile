all: krp
krp: KRP8.v model.v testbench.v
	iverilog KRP8.v model.v testbench.v -o krp
clean:
	rm krp *~ DUMP_FILE *#
