	module Shifter(in,out);
	input[31:0]  in;
	output[31:0] out;
	reg[31:0]    out;
	
	always
	  begin
		out=in<<2;
	  end
	endmodule
	