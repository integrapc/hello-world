// �ɱ��ͬ�����д���ӿ�8255
// �����0��ʽ��1��ʽ

module pic8255(CS, RD, WR, A1, A0, RESET,
				D_in, D_out,
				PA_in, PB_in, PC_in,
				PA_out, PB_out, PC_out,
				d_en, pa_en, pb_en,
				pc0_en, pc1_en, pc2_en, pc3_en, pc4_en, pc5_en, pc6_en, pc7_en);
				
/***********************************��������˿��źŶ���*****************************************************/
	input CS;										// ���룬Ƭѡ�ź�CS
	input RD, WR;									// ��д�����ź�
	input A1, A0;									// Ƭ�ڵ�ַѡ���ź�
	input RESET;									// �����ź�	
	input[7:0] D_in;								// D�˿�����, CPU����������źŻ�����ź�
	input[7:0] PA_in, PB_in, PC_in;					// PA,PB,PC�˿ڵ�����
	output[7:0] D_out; 								// D�˿����
	output[7:0] PA_out, PB_out, PC_out;				// PA,PB,PC�˿ڵ����
	output pa_en, pb_en, d_en;						// D�˿�IO����ʹ���ź�
	output pc0_en, pc1_en, pc2_en, pc3_en, pc4_en, pc5_en, pc6_en, pc7_en; // PA,PB,PC�˿�IO����ʹ���ź�

	reg[7:0] PA_R_IN, PB_R_IN, PC_R_IN, D_R_IN;	// �Ĵ���������	
	reg[7:0] PA_R_OUT, PB_R_OUT, PC_R_OUT, D_R_OUT;	// �Ĵ��������
	wire[7:0] CTRL_R;
	reg[1:0] PA_MODE;	// PA�ڵĹ�����ʽ�Ĵ���
	reg PB_MODE;		// PB�ڵĹ�����ʽ�Ĵ���.
	reg PA_IO;			// PA������/�������ѡ��Ĵ���
	reg PB_IO;			// PB������/�������ѡ��Ĵ���
	reg PCH_IO;			// PC�ڸ�λ����/�������ѡ��Ĵ���
	reg PCL_IO; 		// PC�ڵ�λ����/�������ѡ��Ĵ���
	wire PA_BUS;		// PA�����뵽BUS�ź�
	wire PB_BUS;		// PB�����뵽BUS�ź�
	wire PC_BUS;		// PC�����뵽BUS�ź�
	wire BUS_PA;		// BUS�����PA���ź�
	wire BUS_PB;		// BUS�����PB���ź�
	wire BUS_PC;		// BUS�����PC���ź�
	wire BUS_CTRL;		// BUS�����CTRL���ź�
	wire BUS_Z; 		// ��������ź�
	wire PC_CTRL;		// д�������ź�
	wire W;			// д�ź�
	wire R;				// ���ź�

	reg testR;
/**********************************************��д�����߼�����*****************************************/
	assign PA_BUS = (!CS) & (!A1) & (!A0) & (!RD) & (WR);				// PA�����뵽BUS�ź�
	assign PB_BUS = (!CS) & (!A1) & (A0) & (!RD) & (WR);				// PB�����뵽BUS�ź�
	assign PC_BUS = (!CS) & (A1) & (!A0) & (!RD) & (WR);				// PC�����뵽BUS�ź�

	assign BUS_PA = (!CS) & (!A1) & (!A0) & (RD) & (!WR);				// BUS�����PA���ź�
	assign BUS_PB = (!CS) & (!A1) & (A0) & (RD) & (!WR);				// BUS�����PB���ź�
	assign BUS_PC = (!CS) & (A1) & (!A0) & (RD) & (!WR);				// BUS�����PC���ź�
	assign BUS_CTRL = (!CS) & (A1) & (A0) & (RD) & (!WR);				// BUS�����CTRL���ź�

//ע�����¼���assign ��ֵ���
 	assign PC_CTRL = (!CS) & (A1) & (RD) & (!WR);				// ��c�ںͿ��ƿڵ�д�������ź�, CS=0, A1=1, A0=X, RD=1, WR=0;
	assign BUS_Z = ((!CS) & (RD) & (WR)) | CS;						// д�������ź�, CS=0, RD=1, WR=1, OR CS=1
	assign R = (!CS) & (!RD) & (WR);   							// ���ź�, CS=0, RD=0, WR=1;
	assign W = (!CS) & (RD) & (!WR);							// д�ź�, CS=0, RD=1, WR=0;
	
	
/****************************************************���ܶ���**********************************************/
	// 0��ʽ���ȴ������ж��߼�
	wire stbacka, intea;  		// stbacka:A�˿�1��ʽ����������STBaѡͨ�ź�,�������ACKaӦ���ź�
								// intea:A�˿��ж������źţ�����ʱΪPC_R_IN[4]�����ʱΪPC_R_IN[6]
	reg ibfa;					// ���뻺�������ź�
	reg intra;					// �ж������ź�

	// staacka:��8255����1��ʽ��A�ڴ�������״̬ʱ������PC4�ڣ��������״̬ʱ������pc6�ڡ�
	assign stbacka = (PA_MODE == 2'B01 && PA_IO == 1) ? PC_in[4] : PC_in [6]; 

	wire[31:0] stb2, ack2;										// 2��ʽ�£�stb,ack;
	assign	stb2 = (PA_MODE == 2'B10) ? PC_in[4] : 1;			// 2��ʽ, A��STBa
	assign  ack2 = (PA_MODE == 2'B10) ? PC_in[6] : 1;			// 2��ʽ��A��ACKa

	wire stbackb, inteb;			// B�鹤��˳��˳����A���Ӧ��ͬ��		
	reg ibfb;
	reg intrb;
	assign stbackb = PC_in[2];		// ���������źź�Ӧ���źŶ�����PC2�ڣ�

	wire BUS_CTRL_new;							// ������ʽ�����źţ�����PC����λ��
	assign BUS_CTRL_new = BUS_CTRL & D_in[7];	//  D_in[7] == 1, ��ʾ��ʽѡ���������

	wire PA_BUS_new;				// ibfa�����źţ������޸ķ�ʽ�����źŶ�ibfaӰ��
	assign PA_BUS_new = PA_BUS | BUS_CTRL_new;

	// A�����룬8255�����������뻺�������źš�
	wire stbacka_new_1;							// ����2��ʽ��stb2�źŶ�ibfa�Ŀ���
	assign stbacka_new_1 = stbacka & stb2;

	always @(negedge stbacka_new_1 or negedge PA_BUS_new)
	begin
		if(stbacka_new_1 == 0)						//������8255������ѡͨ�ź���Ч
		begin
			if((PA_MODE == 2'B01 && PA_IO == 1) || (PA_MODE == 2'B10))		//8255��A��1��ʽ��
				ibfa <= 1;													//���������ź���Ч
		end
		else																//���źŽ�����Ч
		begin
			if((PA_MODE == 2'B01 && PA_IO == 1) || (PA_MODE == 2'B10))		//��ʽ�ж� 
				ibfa <= 0;													//���������
		end
	end

	wire PA_0;										// intra�����źţ������޸ķ�ʽ�����źŶ�ibfaӰ��
	assign PA_0 = PA_BUS | BUS_PA | BUS_CTRL_new;	// ����A��������������������A���������	

	// A�����룬8255��CPU�ж��źš�
	// wire stbacka_new_3;
	// assign stbacka_new_3 = stbacka & stb2;			// ����2��ʽ��stb2�źŶ�ibfa�Ŀ�

	always @(posedge stbacka or posedge PA_0)
	begin
		if(PA_0)									// ��д�����ź���Ч
		begin
			if(PA_BUS)								// A�ڴ���1��ʽ����״̬��PA_BUS��?������ж��źš�
			begin
				if(PA_MODE == 2'B01 && PA_IO == 1)
					intra <= 0;
			end
			else if(BUS_PA)							//A�ڴ���1��ʽ���״̬��BUS_PA��?������ж��źš�
			begin
				if((PA_MODE == 2'B01 && PA_IO == 0) || (PA_MODE == 2'B10))
					intra <= 0;
			end	
			else if(BUS_CTRL_new)					//�޸ķ�ʽ�����ź���Ч
			begin
				if(PA_MODE == 2'B01 && PA_IO == 1)	//A�ڴ���1��ʽ����״̬����intra�ź�����
					intra <= 0;
				else if((PA_MODE == 2'B01 && PA_IO == 0) || (PA_MODE == 2'B10))
					intra <= 1;						//A�ڴ���1��ʽ���״̬����intra�ź���λ
			end
		end
		else
		begin
			if((PA_MODE == 2'B01 && (PC_R_OUT[4] | PC_R_OUT[6])) || (PA_MODE == 2'B10))	//�����������߼��ļ򻯣�
				intra <= 1;
		end
	end

	//B��1��ʽ�£�IBFB,INTRB�źŵĿ��ƣ������A����ȫ��ͬ��
	wire PB_BUS_new;
	assign PB_BUS_new = PB_BUS | BUS_CTRL_new;

	//B��1��ʽ�£�IBFB�źŵĿ���
	always @(negedge stbackb or negedge PB_BUS_new)
	begin
		if(stbackb == 0)
		begin	
			if(PB_MODE == 2'B01 && PB_IO == 1)
				ibfb <= 1;
		end
		else
		begin
			if(PB_MODE == 2'B01 && PB_IO == 1) 
				ibfb <= 0;
		end
	end

	wire PB_0;
	assign PB_0 = PB_BUS | BUS_PB | BUS_CTRL_new;

	//B��1��ʽ�£iINTRB�źŵĿ���
	always @(posedge stbackb or posedge PB_0)
	begin
		if(PB_0)
		begin
			if(PB_BUS)	
			begin
				if(PB_MODE == 2'B01 && PB_IO == 1)
					intrb <= 0;
			end
			else if(BUS_PB)  
			begin
				if(PB_MODE == 2'B01 && PB_IO == 0)
					intrb <= 0;
			end
			else if(BUS_CTRL_new)
			begin
				if(PB_MODE == 2'B01 && PB_IO == 1)
					intrb <= 0;
				else if(PB_MODE == 2'B01 && PB_IO == 0)
					intrb <= 1;
			end
		end
		else
		begin
			if(PB_MODE == 2'B01 && PC_R_OUT[2])
				intrb <= 1;
		end
	end

	reg obfa, obfb;			// 1��ʽ�£�8255������������������ź�
	wire stbacka_new_2;		// A�飬����Ӧ���źţ�����״̬�����źŵ�Ӱ?
	assign stbacka_new_2 = stbacka & (!BUS_CTRL_new) & ack2; 	// �����ź�ack2��2��ʽ�¶ԼĴ���obfa��Ӱ��

	always @(negedge BUS_PA or negedge stbacka_new_2)
	begin
		if(stbacka_new_2 == 0)							// ����Ӧ���ź���Ч������״̬�����ź���?�����
		begin
			if((PA_MODE == 2'B01 || PA_MODE == 2'B10) && PA_IO == 0 && !intra)	// ��obfa��λ
				obfa <= 1;
		end
		else											// �����ź���Ч�������
		begin
			if((PA_MODE == 2'B01 || PA_MODE == 2'B10) && PA_IO == 0 && !intra)
				obfa <= 0;								// ���obfaλ						
		end
	end

	// B��obfb�źſ��ƣ���ʽ��a����ͬ
	wire stbackb_new;
	assign stbackb_new = stbackb & (!BUS_CTRL_new);

	always @(negedge BUS_PB or negedge stbackb_new)
	begin
		if(stbackb_new == 0)
		begin
			if(PB_MODE == 2'B01 && PB_IO == 0 && !intrb)
				obfb <= 1;
		end
		else
		begin
			if(PB_MODE == 2'B01 && PB_IO == 0 && !intrb)
				obfb <= 0;
		end
	end

	// I/O��̬�ſ����ź�
	// RESET�źŶ��������̬�Ŀ��ƣ�reset_enΪ1��ʾ���ȫ��Ϊ����̬��
	// RESET�źŸߵ�ƽ��Ч��
	reg reset_en;
	always @(posedge RESET or posedge BUS_CTRL_new)
	begin
		if(RESET)
			reset_en <= 1;
		else 
			reset_en <= 0;
	end

	// A��
	assign pa_en = !reset_en & 
				   ((PA_MODE == 0 && PB_MODE == 0 && PA_IO == 0 && BUS_PA) |// 0��ʽ�£�A�鴦�����״̬��BUS_PA��Ч��������������
				   (PA_MODE == 1 && PB_MODE == 1 && !PA_IO && !intra) | 	// 1��ʽ�£�A�鴦�����״̬��intra��Ч��������������
				   (PA_MODE == 2 && !ack2));								// 2��ʽ��, A���������
	// B�� 
	assign pb_en = !reset_en & 
				   ((PA_MODE == 0 && PB_MODE == 0 && PB_IO == 0 && BUS_PB) |// 0��ʽ�£�B�鴦�����״̬��BUS_PB��?������������?
				   (PA_MODE == 1 && PB_MODE == 1 && !PB_IO && !intrb));		// 2��ʽ�£�B�鴦�����״̬��intrb��Ч������������;
	// D�ڣ�0��ʽ�£�(PA_BUS|PB_BUS|PC_BUS)��Ч��������������1��ʽ�£�(PA_BUS|PB_BUS)��?������������;
	assign d_en = !reset_en & 
				  ((PA_MODE == 0 && PB_MODE == 0 && (PA_BUS | PB_BUS | PC_BUS)) | 
				  (PA_MODE == 1 && PB_MODE == 1 &&(PA_BUS | PB_BUS)) | 
				  (PA_MODE == 2 & WR));

	// C�飬0��ʽ��c�������������BUS_PC��Ч������£�pc�ڴ������״̬�£������
	// 1��ʽ�£�pc0,pc1,pc3,pc5,pc7�������pc2,pc4,pc6�����룻
	// 2��ʽ��3��5��7�����2��4����
	assign pc0_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) | 
					((PA_MODE == 1) & (PB_MODE ==1)));
	assign pc1_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) | 
					((PA_MODE == 1) & (PB_MODE == 1)));
	assign pc2_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))));
	assign pc3_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) | 
					((PA_MODE == 1) & (PB_MODE == 1)) | (PA_MODE == 2));
	assign pc4_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))));
	assign pc5_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) | 
					((PA_MODE == 1) & (PB_MODE == 1)) | (PA_MODE == 2));
	assign pc6_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))));
	assign pc7_en = !reset_en & 
					(((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) | 
					((PA_MODE == 1) & (PB_MODE == 1)) | (PA_MODE == 2));


	// 0��ʽ�¶�C��Ĳ����������Կ��Ʒ�ʽ�Ĵ����Ĳ������Լ�RESET����
	always @(posedge PC_CTRL or posedge RESET)
	begin	
		if(RESET)						// RESET�ź���Ч��PC�ڼĴ������㣬PA,PB����0��ʽ��PA,PB,PC�ھ�Ϊ���뷽ʽ
		begin	
			PA_MODE <= 0;
			PA_IO <= 0;
			PCH_IO <= 0;
			PB_MODE <= 0;
			PB_IO <= 0;
			PCL_IO <= 0;
			PC_R_OUT <= 8'b0;
		end
		else 							// PC_CTRL�ź���Ч
		begin
			if(A0)								// BUS_PC��Ч, �������ߵ����ƼĴ���
			begin
				if(D_in[7])						// �������λΪ1��д��״̬�Ĵ���
				begin
					PA_MODE <= D_in[6:5];		
					PA_IO <= D_in[4];
					PCH_IO <= D_in[3];
					PB_MODE <= D_in[2];
					PB_IO <= D_in[1];
					PCL_IO <= D_in[0];
				end
				else if((D_in[7] == 0) && D_in[0])	// �������Ϊ0�����λΪ1����C����λ
					case(D_in[3:1])
						0 : PC_R_OUT[0] <= 1;
						1 : PC_R_OUT[1] <= 1;
						2 : PC_R_OUT[2] <= 1;
						3 : PC_R_OUT[3] <= 1;
						4 : PC_R_OUT[4] <= 1;
						5 : PC_R_OUT[5] <= 1;
						6 : PC_R_OUT[6] <= 1;
						7 : PC_R_OUT[7] <= 1;
					endcase
				else if((D_in[7] == 0) && (D_in[0] == 0))	// �������Ϊ0�����λΪ0����C������
					case(D_in[3:1])
						0 : PC_R_OUT[0] <= 0;
						1 : PC_R_OUT[1] <= 0;
						2 : PC_R_OUT[2] <= 0;
						3 : PC_R_OUT[3] <= 0;
						4 : PC_R_OUT[4] <= 0;
						5 : PC_R_OUT[5] <= 0;
						6 : PC_R_OUT[6] <= 0;
						7 : PC_R_OUT[7] <= 0;
					endcase
			end 
			else									// BUS_PC��Ч��ֱ��������C��.CPU->��������->PC��
			begin
				if((!PCH_IO) && PCL_IO)				// C�ڸ�λ�������λ����
				begin
					PC_R_OUT[7:4] <= D_in[7:4];
					PC_R_OUT[3:0] <= 4'b0;
				end
				else if((PCH_IO) && (!PCL_IO))		// C�ڵ�λ�������λ����
				begin
					PC_R_OUT[7:4] <= 4'b0;
					PC_R_OUT[3:0] <= D_in[3:0];
				end
				else if((!PCH_IO) && (!PCL_IO))		// C�ڸ�λ��λȫ�����
				begin
					PC_R_OUT[7:4] <= D_in[7:4];
					PC_R_OUT[3:0] <= D_in[3:0];
				end
			end
		end	
	end

	wire PC_out_new;
	assign PC_out_new = (PB_IO == 1) ? ibfb : obfb;

	assign PC_out[0] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[0] : (intrb ? 1 : 0);
	assign PC_out[1] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[1] : PC_out_new;
	assign PC_out[2] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[2] : 0;
	assign PC_out[3] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[3] : (intra ? 1 : 0);
	assign PC_out[4] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[4] : 0;
	assign PC_out[5] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[5] : (ibfa ? 1 : 0);
	assign PC_out[6] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[6] : 0;
	assign PC_out[7] = ((PA_MODE == 0) & (PB_MODE == 0) & (BUS_CTRL & (!D_in[7])) | (BUS_PC & (!PCH_IO | !PCL_IO))) ? PC_R_OUT[7] : (obfa ? 1 : 0);

	//0��ʽ�º�1��ʽ��������A,B��	
	always @(posedge BUS_PA)			//A��
	begin
		if(PA_MODE == 0 || PA_MODE ==1 )
		begin
			if(!PA_IO)					//�����
				PA_R_OUT <= D_in;
		end
		else if(PA_MODE == 2)
			PA_R_OUT <= D_in;
	end

	always @(posedge BUS_PB)			//B��
	begin
		if(PB_MODE == 0 || PB_MODE == 1)
			if(!PB_IO)					//�����
				PB_R_OUT <= D_in;
	end

	//0��ʽ�£�������A,B������
	always @(PA_in)						//��A������������			
	begin
		if(PA_MODE == 0)
			if(PA_IO)					//������
				PA_R_IN <= PA_in;		//д������Ĵ���
	end

	always @(PB_in)						//��B������������
	begin
		if(PB_MODE == 0)
			if(PB_IO)					//������
				PB_R_IN <= PB_in;		//д������Ĵ���
	end

	//0��ʽ�£�C�ڶ���
	always @(PC_in)						//��C������������
	begin
		if((PA_MODE == 0) && (PB_MODE == 0))		
			if(PCH_IO && (!PCL_IO))		//C�ڸ�λ����
			begin
				PC_R_IN[7:4] <= PC_in[7:4];
				PC_R_IN[3:0] <= 4'b0;
			end
			else if((!PCH_IO) && PCL_IO)	//C�ڵ�λ����
			begin
				PC_R_IN[7:4] <= 4'b0;
				PC_R_IN[3:0] <= PC_in[3:0];
			end
			else if(PCH_IO && PCL_IO)		//C��ȫ�������룬ȫ������
				PC_R_IN <= PC_in;	
	end

	wire stb;							//D����������ź�
										//��0��ʽ�£����ź���Ч�������1��ʽ�£�A��Ϊ����ʱSTBA�ź���Чʱ�����B��Ϊ����ʱSTBB�ź���Чʱ�����
	assign stb = (PA_MODE == 1 && PA_IO == 1 && (stbacka != 1)) || 
				 (PB_MODE == 1 && PB_IO == 1 && (stbackb != 1)) || 
				 (PA_MODE == 2 && !stb2); //|| R;
	
	always @(posedge stb or posedge R)
	begin								//�����ź���Ч
		if(R)								
		begin
			
				if({A1, A0} == 2'B00)		//��A��
					D_R_IN <= PA_R_IN;
				else if({A1, A0} == 2'B01)	//��B��
					D_R_IN <= PB_R_IN;
				else if({A1, A0} == 2'B10)	//��C��
					D_R_IN <= PC_R_IN;
		end
		
	end

	assign D_out = ({A1, A0}==2'b01)?PB_in:PC_in;					// D�����
	assign PB_out = PB_R_OUT;				// PB�����
	assign PA_out = PA_R_OUT;				// PA�����
endmodule	