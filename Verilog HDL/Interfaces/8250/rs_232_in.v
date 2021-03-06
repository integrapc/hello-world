//8位异步串行通信接收器
//本模块按照固定的波特率（9600*16bit/s）来接收串行信号；
//采用起止式异步协议，起始位为一位的低电平，数据长度为8位，不设置奇偶校验位，停止位为2位高电平；
//输出完成后输出结束信号，以及输出数据；
module rs_232_in(clk, shiftin, in_data, data_finish);

	input clk;					//输入时钟，(9600*16)hz
	input shiftin;				//串行数据输入端

	output[7:0] in_data;		//接收完成信号8位并行输出
	output data_finish;			//数据接受结束信号
	
	reg[7:0] in_data;			//接收数据寄存器
	reg data_finish;			//数据接受结束信号
	reg flag;					//数据接收状态寄存器，0：等待状态。1：接收状态。
	reg[7:0] count;				//程序计数器

	always @(posedge clk)
	begin
		//当接收器处在等待状态时，接收到起始位有?信号，置状态寄存器为1，进入接受状态。
		if(flag == 0 && shiftin == 0)
			flag <= 1;
		//当接收器处在接收状态时,计数器开始计数。
		else if(flag == 1)
			count <= count + 8'b1;
		//结束信号位置0。
		data_finish <= 0;
		//数据接收逻辑部分；
		if((count[7:4] <= 8) && (count[3:0] == 0) && (flag == 1))  	//当计数器计数16次时，开始接受数据，一共接受8位；
		begin
			in_data <= in_data >> 1;								//寄存器做1位的循环右移	
			in_data[7] <= shiftin;								//将接收到的数据写入寄存器高位
		end
		else if((count[7:4] == 11) && (count[3:0] == 0))			//计数结束
		begin
			flag <= 0;											//接收器由接收状态进入等待状态
			data_finish <= 1;									//输出接受结束信号
			count <= 0;										//计数器置0
		end
	end
endmodule



