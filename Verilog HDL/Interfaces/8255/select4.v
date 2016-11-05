module select4(reset,sel,in0,in1,in2,in3,mode,out);
input reset,sel;
input [7:0]in0,in1,in2,in3;
output [1:0]mode;
output [7:0]out;
reg [1:0]mode;
reg [7:0]out;

always@(negedge reset or negedge sel)
begin
	if(!reset)
	begin
		mode=0;
		
	end
	else
	begin//sel add
		mode=mode+1;
		

	end//end of esle
end//end of always

always
begin
	case(mode)
	2'b00: out=in0;
	2'b01: out=in1;
	2'b10: out=in2;
	2'b11: out=in3;		
	endcase
end


endmodule
