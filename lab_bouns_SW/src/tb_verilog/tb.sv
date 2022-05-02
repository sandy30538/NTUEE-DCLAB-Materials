`timescale 1ns/100ps

module tb;
	localparam CLK = 10;
	localparam HCLK = CLK/2;

	logic clk, rst;
	initial clk = 0;
	always #HCLK clk = ~clk;
	integer fp_test, fp_golden;
	integer a, b, c, d;
	logic [255:0] test_data_ref, test_data_read;
	
	integer data_num;
	logic signed [`DP_SW_SCORE_BITWIDTH-1:0]	golden_alignment_score;
	logic [$clog2(`REF_MAX_LENGTH)-1:0]    		golden_column;
    logic [$clog2(`READ_MAX_LENGTH)-1:0]   		golden_row;

	// For SW
	logic 										o_ready;
    logic 										i_valid;
    logic [2*`REF_MAX_LENGTH-1:0]      			i_sequence_ref;       // reference seq
    logic [2*`READ_MAX_LENGTH-1:0]      		i_sequence_read;       // query seq
    logic [$clog2(`REF_MAX_LENGTH):0]  			i_seq_ref_length;     // (1-based)
    logic [$clog2(`READ_MAX_LENGTH):0]  		i_seq_read_length;     // (1-based)
    
    logic                           			i_ready;
    logic 										o_valid;
    logic signed [`DP_SW_SCORE_BITWIDTH-1:0] 	o_alignment_score;
    logic [$clog2(`REF_MAX_LENGTH)-1:0]    		o_column;
    logic [$clog2(`READ_MAX_LENGTH)-1:0]   		o_row;
	
	SW_core u_SW_core(
        .clk             		(clk),
        .rst              		(rst),
		
        .o_ready              	(o_ready),
        .i_valid              	(i_valid),
        .i_sequence_ref        	(i_sequence_ref),		// reference seq
        .i_sequence_read    	(i_sequence_read),  	// read seq
        .i_seq_ref_length   	(i_seq_ref_length),		// (1-based)
        .i_seq_read_length   	(i_seq_read_length),	// (1-based)
        
        .i_ready              	(i_ready),
        .o_valid              	(o_valid),
        .o_alignment_score    	(o_alignment_score),	// SW score
		.o_column             	(o_column),
        .o_row                	(o_row)
    );

	integer err_cnt;
	initial begin
		$fsdbDumpfile("lab2.fsdb");
		$fsdbDumpvars;

		fp_test 	= $fopen("./../cpp/test_data/random_pattern.bin", "rb");
		fp_golden	= $fopen("./../cpp/test_data/pattern_ans.txt", "r");
		d 			= $fscanf(fp_golden, "%d", data_num);

		rst 	= 1;
		i_valid = 0;
		i_ready = 0;
		i_sequence_ref = 0;
		i_sequence_read = 0;
		i_seq_ref_length = 0;
		i_seq_read_length = 0;

		#(5*CLK)
		rst 	= 0;

		err_cnt = 0;
		for (int i = 0; i < data_num; i++) begin
			for (int j = 0; j < 10; j++) begin
				@(posedge clk);
			end

			// Read in one set of ref-read
			$fread(test_data_ref, fp_test);
			$fread(test_data_read, fp_test);

			// Read in golden answer
			a = $fscanf(fp_golden,"%d", golden_alignment_score);
    		b = $fscanf(fp_golden,"%d", golden_row);
    		c = $fscanf(fp_golden,"%d", golden_column);

			$display("=========");
			$display("ref  %3d = %64h", i, test_data_ref);
			$display("read %3d = %64h \n", i, test_data_read);

			// Reset Signal
			@(posedge clk)
			i_valid 			<= 0;
            i_sequence_ref 		<= 0;	// reference seq
            i_sequence_read 	<= 0;  	// read seq
            i_seq_ref_length 	<= 0;   // (1-based)
            i_seq_read_length 	<= 0;   // (1-based)
            i_ready 			<= 0;
			
			// Wait for SW module ready
			while(!o_ready) @(posedge clk);
			repeat(4) @(posedge clk);
			@(posedge clk)
			i_valid 			<= 1;
			i_sequence_ref 		<= {test_data_ref, {(`REF_MAX_LENGTH*2-`REF_LENGTH*2){1'b0}}};		// reference seq
			i_sequence_read 	<= {test_data_read, {(`READ_MAX_LENGTH*2-`READ_LENGTH*2){1'b0}}};  	// read seq
			i_seq_ref_length 	<= `REF_LENGTH;   				// (1-based)
			i_seq_read_length 	<= `READ_LENGTH;   				// (1-based)
			i_ready 			<= 0;

			@(posedge clk)
			i_valid 			<= 0;
			i_sequence_ref 		<= 0;	// reference seq
			i_sequence_read 	<= 0;	// read seq
			i_seq_ref_length 	<= 0; 	// (1-based)
			i_seq_read_length 	<= 0; 	// (1-based)
			i_ready 			<= 1;

			// Wait for SW output result
			@(posedge o_valid)
			$display("score %2d = %0d", i, o_alignment_score);
			$display("row   %2d = %0d", i, o_row);
			$display("col   %2d = %0d\n", i, o_column);
			$display("*Golden");
			$display("score %2d = %0d", i, golden_alignment_score);
			$display("row   %2d = %0d", i, golden_row);
			$display("col   %2d = %0d\n", i, golden_column);
			if ((o_alignment_score != golden_alignment_score) || (o_row != golden_row) || (o_column != golden_column)) begin
				$display("pattern %0d error", i);
				err_cnt = err_cnt + 1;
			end
			else begin
				$display("pattern %0d \033[38;5;%2dmcorrect!\033[00m", i, 10);
			end
			$display("=========");
		end
		
		if (err_cnt == 0)		$display("You have passed all patterns! (*>▽<*)");
		else if (err_cnt == 1)	$display("There is 1 error ...");
		else 					$display("There are %0d errors ...", err_cnt);
		$display("=========");
		$finish;
	end

	initial begin
		#(500000*CLK)
		$display("Too slow, abort.");
		$finish;
	end

endmodule
