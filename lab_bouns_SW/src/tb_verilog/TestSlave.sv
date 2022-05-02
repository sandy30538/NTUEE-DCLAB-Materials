module TestSlave (
	input               avm_clk,
	input               avm_rst_n,
	input        [4:0]  avm_address,
	input               avm_actually_read,
	output logic [31:0] avm_readdata,
	input               avm_actually_write,
	input        [31:0] avm_writedata,

	output             to232_rdy,
	input              to232_ack,
	output logic [7:0] to232_dat,
	input              from232_rdy,
	output             from232_ack,
	input  logic [7:0] from232_dat
);

	logic [7:0] txdat;
	logic [7:0] rxdat;
	logic tx_src_rdy;
	logic tx_src_ack;
	logic rx_dst_rdy;
	logic rx_dst_ack;
	PPForward u_txctrl(
		.clk(avm_clk),
		.rst_n(avm_rst_n),
		.src_rdy(tx_src_rdy),
		.src_ack(tx_src_ack),
		.dst_rdy(to232_rdy),
		.dst_ack(to232_ack)
	);
	PPForward u_rxctrl(
		.clk(avm_clk),
		.rst_n(avm_rst_n),
		.src_rdy(from232_rdy),
		.src_ack(from232_ack),
		.dst_rdy(rx_dst_rdy),
		.dst_ack(rx_dst_ack)
	);
	assign rx_dst_ack = avm_actually_read && avm_address == 0;
	assign tx_src_rdy = avm_actually_write && avm_address == 4;
	assign to232_dat = txdat;

	always @(posedge avm_clk or negedge avm_rst_n) begin
		if (!avm_rst_n) begin
			txdat <= 0;
			rxdat <= 0;
		end else begin
			if (tx_src_ack) begin
				txdat <= avm_writedata;
			end
			if (from232_ack) begin
				rxdat <= from232_dat;
			end
		end
	end

	always @* begin
		avm_readdata = 32'bx;
		if (avm_actually_read) begin
			case (avm_address)
				0: avm_readdata = rxdat;
				8: avm_readdata = {24'b0,rx_dst_rdy,!to232_rdy,6'b0};
			endcase
		end
	end
endmodule

