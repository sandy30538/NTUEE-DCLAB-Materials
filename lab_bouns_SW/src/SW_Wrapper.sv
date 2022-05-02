
`define REF_MAX_LENGTH              128
`define READ_MAX_LENGTH             128

`define REF_LENGTH                  128
`define READ_LENGTH                 128

//* Score parameters
`define DP_SW_SCORE_BITWIDTH        10

`define CONST_MATCH_SCORE           1
`define CONST_MISMATCH_SCORE        -4
`define CONST_GAP_OPEN              -6
`define CONST_GAP_EXTEND            -1

module SW_Wrapper (
    input         avm_rst,
    input         avm_clk,
    output  [4:0] avm_address,
    output        avm_read,
    input  [31:0] avm_readdata,
    output        avm_write,
    output [31:0] avm_writedata,
    input         avm_waitrequest
);

localparam RX_BASE     = 0*4;
localparam TX_BASE     = 1*4;
localparam STATUS_BASE = 2*4;
localparam TX_OK_BIT   = 6;
localparam RX_OK_BIT   = 7;

// Feel free to design your own FSM!

// Remember to complete the port connection
SW_core sw_core(
    .clk				(avm_clk),
    .rst				(avm_rst),

	.o_ready			(),
    .i_valid			(),
    .i_sequence_ref		(),
    .i_sequence_read	(),
    .i_seq_ref_length	(),
    .i_seq_read_length	(),
    
    .i_ready			(),
    .o_valid			(),
    .o_alignment_score	(),
    .o_column			(),
    .o_row				()
);

task StartRead;
    input [4:0] addr;
    begin
        avm_read_w = 1;
        avm_write_w = 0;
        avm_address_w = addr;
    end
endtask
task StartWrite;
    input [4:0] addr;
    begin
        avm_read_w = 0;
        avm_write_w = 1;
        avm_address_w = addr;
    end
endtask

// TODO
always_comb begin
    
end

// TODO
always_ff @(posedge avm_clk or posedge avm_rst) begin
    if (avm_rst) begin
    	

    end
	else begin
    	

    end
end

endmodule
