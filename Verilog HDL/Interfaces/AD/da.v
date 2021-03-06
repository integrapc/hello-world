/*module da(IOR, IOW, AEN, A, D0, READ, WRITE, DATAIN, DATAOUT, OUT);
	input IOR,IOW,AEN,D0;
	input[9:0] A;														//地址总线
	input[7:0] DATAIN;													//DATAIN、DATAOUT面向CPU数据总线
	
	output READ,WRITE;													
	output[7:0] OUT,DATAOUT;											//OUT面向DAC0808
	
	wire READ, WRITE;
	reg[7:0] OUT;

	assign READ = (AEN && A==10'b0111111000) ? ~IOR : 0;
	assign WRITE = (AEN && A==10'b0111111000) ? ~IOW : 0;
	assign DATAOUT = READ ? (OUT & {7'b1111111, D0}) : 8'b00000000;
	
	always @(posedge WRITE)
	begin	
		OUT <= DATAIN;
	end
endmodule
*/
module da(IOR, IOW, AEN, A, D0, READ, WRITE, DATAIN, DATAOUT, OUT);
	input IOR,IOW,AEN,D0;
	input[9:0] A;														//地址总线
	input[7:0] DATAIN;													//DATAIN、DATAOUT面向CPU数据总线
	
	output READ,WRITE;													
	output[7:0] OUT,DATAOUT;											//OUT面向DAC0808
	
	wire READ, WRITE;
	reg[7:0] OUT;

	assign READ = (AEN && A==10'b0111111000) ? !IOR : 0;
	assign WRITE = (AEN && A==10'b0111111000) ? !IOW : 0;
	assign DATAOUT = READ ? (OUT & {7'b1111111, D0}) : 8'b00000000;
	
	always @(posedge WRITE)
	begin	
		OUT <= DATAIN;
	end
endmodule
