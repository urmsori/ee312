//////////////////////////////////////////
//                                      //
// EE312 Intro to Computer Architecture //
// Assignment 1                         //
//                                      //
//          Test Bench Module           //
//                                      //
//////////////////////////////////////////

// Student ID : 
// Name       : 

module TB ();

reg				CLK;
reg				RST;
reg		[1:0]	COIN;

wire			MONEY;
wire			COKE;
wire			CHANGE;


// Clock generation
parameter PER = 4.0;
parameter HPER = PER/2.0;

initial CLK <= 1'b0;
always #(HPER) CLK <= ~CLK;


// Testing module
Vending_Machine vm (
	.CLK		(CLK   ),
	.RST		(RST   ),
	.COIN		(COIN  ),
	.MONEY		(MONEY ),
	.COKE		(COKE  ),
	.CHANGE		(CHANGE)
);


// Simulations
initial begin
	// System reset
	RST <= 1'b1;
	COIN <= 2'b00;
	
	// End of reset
	#(2.0*PER)
	RST <= 1'b0;

	// Write your test-bench here!
	#PER
	COIN <= 2'b01;
	#(PER*4.5)
	COIN <= 2'b10;
	#(PER*2.5)
	COIN <= 2'b01;
	#PER
	COIN <= 2'b10;
	#PER
	COIN <= 2'b00;
	
	#PER
	COIN <= 2'b01;
	#PER
	RST <= 1'b1;
	#(PER*4.5)
	RST <= 1'b0;
	COIN <= 2'b10;
	#(PER*2.5)
	RST <= 1'b1;
	#PER
	RST <= 1'b0;
	COIN <= 2'b00;
	
	// Simulation finish
	#(100*PER)
	$finish();	
end

// ncverilog
//initial begin
//	$shm_open("wave.shm");
//	$shm_probe("AC");
//end

// icarus
initial begin
	$dumpfile("wave.dmp");
	$dumpvars;
end

endmodule
