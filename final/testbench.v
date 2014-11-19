/****************************************
    testbench.v

    EE312 2014 KRP-VIII Processor
    Final Project
    
    Team00 : 
        20080000    Gil Dong Hong
        20080001    Cheol Soo
****************************************/

module testbench;

    reg             CLK, RST;

    /* CLOCK Generator */
    parameter   PER = 5.0;
    parameter   HPER = PER/2.0;

    initial CLK <= 1'b0;
    always #(HPER) CLK <= ~CLK;


    wire              IREQ;
    wire    [29:0]    IADDR;
    wire    [31:0]    INSTR;
    wire              DREQ;
    wire              nDRW;
    wire    [29:0]    DADDR;
    wire    [31:0]    DWDATA;
    wire    [31:0]    DRDATA;

	KRP8	KRP8	(
		.CLK		(CLK),
		.RST		(RST),
		.IREQ		(IREQ),
		.IADDR		(IADDR),
		.INSTR		(INSTR),
		.DREQ		(DREQ),
		.nDRW		(nDRW),
		.DADDR		(DADDR),
		.DWDATA		(DWDATA),
		.DRDATA		(DRDATA)
	);

	SRAM	INST_MEM	(
		.CLK		(CLK),
		.CSN		(~IREQ),
		.A			(IADDR[11:2]),
		.WEN		(1'b1),
		.DI			(),
		.DOUT		(INSTR)
	);

	SRAM	DATA_MEM	(
		.CLK		(CLK),
		.CSN		(~DREQ),
		.A			(DADDR[11:2]),
		.WEN		(~nDRW),
		.DI			(DWDATA),
		.DOUT		(DRDATA)
	);

	

	// --------------------------------------------
	// Load test vector to inst and data memory
	// --------------------------------------------
	// Caution : Assumption : input file has hex data like below. 
	//			 input file : M[0x03]M[0x02]M[0x01]M[0x00]
	//                        M[0x07]M[0x06]M[0x05]M[0x04]
	//									... 
	//           If the first 4 bytes in input file is 1234_5678
	//           then, the loaded value is mem[0x0000] = 0x1234_5678 (LSB)
	defparam testbench.INST_MEM.INIT_FILE = "inst.hex";

	initial begin
		RST <= 1'b1;
		#(10*PER)
		RST <= 1'b0;

		#(10000*PER);
		$finish();
	end

	/* Waveform Dump */
	initial begin
		$display("Dump variables..");
		$dumpfile("./DUMP_FILE");
		$dumpvars;
	end

endmodule

