



module open_ed(
	
	
	//cpu bus
	input [7:0]cpu_data,
	input cpu_addr21, time_n, oe_n, we_lo_n, ce_lo_n, rst_n,
	
	//level shifters
	output dbus_dir,
	
	//memory
	output ram_ce_n,
	output rom_ce_n,
	output rom_bank,
	
	//spi io
	output spi_mosi, spi_sck, spi_ss_n1, spi_ss_n2,
	input  spi_miso,
	
	//var
	output led,	
	output reg_oe_n,
	
	
	output [7:0]reg_do
);

	
	
//************************************************* 74hc02 (1)
	wire reg_oe			= nor1_do[0];
	assign nor1_a[0] 	= oe_n;
	assign nor1_b[0] 	= time_n;

	wire reg_we			= nor1_do[1];
	assign nor1_a[1] 	= we_lo_n;
	assign nor1_b[1] 	= time_n;
	
	assign bus_oe_n	= nor1_do[2];
	assign nor1_a[2] 	= reg_oe;
	assign nor1_b[2] 	= cart_oe;
	
	wire cart_oe		= nor1_do[3];
	assign nor1_a[3] 	= ce_lo_n;
	assign nor1_b[3] 	= oe_n; 
	
//************************************************* 74hc02 (2)		
	wire spi_sck_i		= nor2_do[0];
	assign nor2_a[0] 	= reg_oe;
	assign nor2_b[0] 	= spi_ck;
	
	assign spi_ss_n2	= nor2_do[1];
	assign nor2_a[1] 	= spi_ss2;
	assign nor2_b[1] 	= 0;
	
	wire cpu_d2_rst	= nor2_do[2];
	assign nor2_a[2] 	= cpu_data[2];
	assign nor2_b[2] 	= rst_i;
	
	wire reg_set_n		= nor2_do[3];
	assign nor2_a[3] 	= reg_we;
	assign nor2_b[3] 	= rst_i;
//************************************************* 74hc04
	assign spi_sck		= inv_do[1];
	assign inv_di[1]	= spi_sck_i;
	
	wire reg_set		= inv_do[3];
	assign inv_di[3]	= reg_set_n;
	
	wire rst_i			= inv_do[2];
	assign inv_di[2]	= rst_n;
	
	assign reg_oe_n  	= inv_do[0];
	assign inv_di[0]	= reg_oe;
		
	assign dbus_dir	= inv_do[4];
	assign inv_di[4]	= bus_oe_n;
	
	assign spi_ss_n1	= inv_do[5];
	assign inv_di[5]	= spi_ss1;
//************************************************* 74hc139	
	
	assign ram_ce_n	= dec1_s[0];
	assign rom_ce_n	= dec1_s[1];
	assign dec1_a[0]	= ram_on_n;
	assign dec1_a[1] 	= ce_lo_n;
	
	
	wire ram_on_n		= dec2_s[3];
	assign dec2_a[0]	= cpu_addr21;
	assign dec2_a[1]	= ram_on;
	
//************************************************* 74hc574 W register (cpu writes here)	
	wire 	 ram_on		= reg_di[0];
	assign rom_bank 	= reg_di[2];
	assign led 			= reg_di[3];
	wire 	 spi_ss2		= reg_di[4];
	wire   spi_ss1		= reg_di[5];
	wire 	 spi_ck		= reg_di[6];	
	assign spi_mosi	= reg_di[7];
//*************************************************


	wire [7:0]reg_di;

	hc574 reg_di_inst(
		.di({cpu_data[7:3], cpu_d2_rst, cpu_data[1:0]}),
		.do(reg_di),
		.le(reg_set),
		.oe(0)
	);
	

	
	//74hc574 R register (cpu reads from here)	
	hc574 reg_do_inst(
		.di({8'd0, spi_miso}),
		.do(reg_do),
		.le(reg_oe),
		.oe(reg_oe_n)
	);
	
	
	
	wire [3:0]nor1_a, nor1_b, nor1_do;
	
	hc02 nor1_inst(
	
		.di_a(nor1_a),
		.di_b(nor1_b),
		.do(nor1_do)
	);
	
	
	
	wire [3:0]nor2_a, nor2_b, nor2_do;
	
	hc02 nor2_inst(
	
		.di_a(nor2_a),
		.di_b(nor2_b),
		.do(nor2_do)
	);
	
	
	
	wire [5:0]inv_di, inv_do;
	
	hc04 inv_inst(
	
		.di(inv_di),
		.do(inv_do)
	);
	
	
	
	wire [1:0]dec1_a, dec2_a;
	wire [3:0]dec1_s, dec2_s;
	
	hc139 dec_inst(
		
		.e_a(0),
		.addr_a(dec1_a),
		.sel_a(dec1_s),
		
		.e_b(0),
		.addr_b(dec2_a),
		.sel_b(dec2_s)
	);

endmodule


//************************************************* 74 series logic

//nor gate
module hc02(
	input  [3:0]di_a,
	input  [3:0]di_b,
	output [3:0]do
);

	assign  do[0] = !(di_a[0] | di_b[0]);
	assign  do[1] = !(di_a[1] | di_b[1]);
	assign  do[2] = !(di_a[2] | di_b[2]);
	assign  do[3] = !(di_a[3] | di_b[3]);

endmodule


//flip-flop
module hc574(
	input  [7:0]di,
	output [7:0]do,
	input  le, 
	input  oe
);
	
	assign do[7:0] = !oe ? data[7:0] : 8'hff;
	
	reg [7:0]data;
	
	always @(posedge le)
	begin
		
		data[7:0] <= di[7:0];
		
	end

endmodule



//decoder
module hc139(

	input  e_a,
	input  [1:0]addr_a,
	output reg [3:0]sel_a,

	input  e_b,
	input  [1:0]addr_b,
	output reg [3:0]sel_b
);
	
	
	
	always @(*)
	begin
	
		if(e_a)sel_a = 4'b1111;
			else
		case(addr_a[1:0])
			0:sel_a = 4'b1110;
			1:sel_a = 4'b1101;
			2:sel_a = 4'b1011;
			3:sel_a = 4'b0111;
		endcase
		
		if(e_b)sel_b = 4'b1111;
			else
		case(addr_b[1:0])
			0:sel_b = 4'b1110;
			1:sel_b = 4'b1101;
			2:sel_b = 4'b1011;
			3:sel_b = 4'b0111;
		endcase
		
	end
	
	
endmodule



//hex inverter
module hc04(
	input  [5:0]di,
	output [5:0]do
);

	assign do[5:0] = di[5:0] ^ 6'b111111;

endmodule
