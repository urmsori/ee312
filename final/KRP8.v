/****************************************
    KRP8.v

    EE312 2014 KRP-VIII Processor
    Final Project
    
    Team00 : 
        20110343    Gil Dong Hong
        20080001    Cheol Soo
****************************************/



/*
    IF YOU WANT MODULARIZED DESIGN,
    DEFINE MODULES HERE,
    AND INSTANTIATE THESE MODULES IN KRP8.
*/



/////////////////////////////
//  TOP MODULE OF KRP8
/////////////////////////////
module KRP8 (
    input     wire              CLK,
    input     wire              RST,
    output    wire              IREQ,
    output    wire    [29:0]    IADDR,
    input     wire    [31:0]    INSTR,
    output    wire              DREQ,
    output    wire              nDRW,
    output    wire    [29:0]    DADDR,
    output    wire    [31:0]    DWDATA,
    input     wire    [31:0]    DRDATA
);


    // YOUR CODE HERE!


    // REGISTER FILE FOR GENRAL PURPOSE REGISTERS
    REGFILE    #(.AW(5), .ENTRY(32))    RegFile (
                    .CLK    (CLK),
                    .RST    (RST),
                    .WEN    (),
                    .WA     (),
                    .DI     (),
                    .RA0    (),
                    .RA1    (),
                    .DOUT0  (),
                    .DOUT1  ()
    );


    // YOUR CODE HERE!


endmodule


/*
    IF YOU WANT TO TEST YOUR DESIGN,
    CREATE A TESTBENCH FILE (e.g. testbench.v),
    INSTANTIATE A KRP8 AND MEMORIES IN THAT FILE,
    AND WIRE THEM APPROPRIATELY.

    THEN, CREATE VERILOG LIST FILE (e.g. test.f),
    INCLUDE model.v, KRP8.v, and testbench.v.
*/
