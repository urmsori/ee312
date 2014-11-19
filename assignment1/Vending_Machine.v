//////////////////////////////////////////
//                                      //
// EE312 Intro to Computer Architecture //
// Assignment 1                         //
//                                      //
//      Vending Machine Module          //
//                                      //
//////////////////////////////////////////

// Student ID : 20110343
// Name       : JinYeob Kim

module Vending_Machine (
	input	wire			CLK,
	input	wire			RST,
	input	wire	[1:0]	COIN,

	output	wire			MONEY,
	output	wire			COKE,
	output	wire			CHANGE,
	
	output reg r_money, r_coke, r_change,
	output reg	[0:2]	STATE
);
assign MONEY = r_money;
assign COKE = r_coke;
assign CHANGE = r_change;

localparam ST_INIT = 3'd0;
localparam ST_50   = 3'd1;
localparam ST_100  = 3'd2;
localparam ST_150  = 3'd3;
localparam ST_200  = 3'd4;

// Design the FSM here!
initial begin
	STATE = ST_INIT;
end

always @ (posedge CLK) begin
	if(RST == 1'b1)	begin
		STATE = ST_INIT;
		r_money = 1'b0;
		r_coke = 1'b0;
		r_change = 1'b0;
	end
	else begin
		r_money = (~STATE[0]&~STATE[1]&~STATE[2]&COIN[1]&~COIN[0])+(~STATE[0]&~STATE[1]&STATE[2]&~COIN[1]&~COIN[0])+(~STATE[0]&~STATE[1]&~COIN[1]&COIN[0])+(~STATE[0]&STATE[1]&~STATE[2]&~COIN[1]&~COIN[0]);
		r_coke = (~STATE[0]&~STATE[1]&STATE[2]&COIN[1]&~COIN[0])+(~STATE[0]&STATE[1]&~STATE[2]&~COIN[1]&COIN[0])+(~STATE[0]&STATE[1]&~STATE[2]&COIN[1]&~COIN[0]);
		r_change = (~STATE[0]&STATE[1]&~STATE[2]&COIN[1]&~COIN[0]);

		//Change state
		if(STATE == ST_INIT) begin
			if(COIN == 2'b00) begin
				STATE = ST_INIT;
			end
			else if(COIN == 2'b01) begin
				STATE = ST_50;
			end
			else if(COIN == 2'b10) begin
				STATE = ST_100;
			end
		end
		else if(STATE == ST_50) begin
			if(COIN == 2'b00) begin
				STATE = ST_50;
			end
			if(COIN == 2'b01) begin
				STATE = ST_100;
			end
			if(COIN == 2'b10) begin
				STATE = ST_INIT;
			end
		end
		else if(STATE == ST_100) begin
			if(COIN == 2'b00) begin
				STATE = ST_100;
			end
			if(COIN == 2'b01) begin
				STATE = ST_INIT;
			end
			if(COIN == 2'b10) begin
				STATE = ST_INIT;
			end
		end
		else if(STATE == ST_150) begin
			STATE = ST_INIT;
		end
		else if(STATE == ST_200) begin
			STATE = ST_INIT;
		end
		else begin
			$display("error occur");
		end
	end
end

endmodule
