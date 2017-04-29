#xeon
## xeon.h
 struct XeonStrut
	XeonStruct is main frame of our Pipelining model :
		It contains IF,IF_ID,ID,ID_EX,EX,EX_MEM,MEM,MEM_WB,WB structs which implies each stage and registor of pipeline.
		and also several variable PC(Program Counter),PC_bus,clock(CPU_clock),mem(Memory),reg_mem(Registor memory)

	Rule of BUS in each struct which implies stages :
		To make accurate model of pipeline and solving pipeline hazards.
		
	IF struct
		PC : program counter value 
		*fetch : instruction fetching function pointer
	IF_ID struct
		instr : instruction which has been fetched at IF stage
		PC : program counter value
	ID struct
	ID_EX struct
	EX struct
	EX_MEM struct
		ALU_result : integer value which has been calculated at EX stage
		PC_target : target PC value when executing brach instruction

		ConSig
			MEM : control signal that determines action of MEM stage
			WB : control signal that determines action of WB stage
	MEM struct
		addr_src : value which has been calculated at EX stage(from EX_MEM.ALU_result)
		wirte_data : value which will be written in memory that came from ID stage when sw instruction
		read_data : value which will be readed from memory when lw instruction
		
		BUS : bus variables between EX_MEM and MEM

		*move2bus_MEM : function which moves values from EX_MEM registor to MEM bus
		*move2src_MEM : function which moves values from MEM bus to MEM
		*f_MEM : function act differently depends on control signals as write in or read from memory or brach
	MEM_WB struct
		dest : registor address of readed data
		read_data : value that want to store in registor

		ConSig
			WB : control signal that determines action of WB stage
	WB struct
		
DGIST 2017 Computer Architecture Assignment 02 - Pipelined MIPS


[ Controal Signal ]
		RegDst	ALUOP1	ALUOP2	ALUSrc	Brch	MemRead	MemWrit	RegWrit	MemtoReg
ADDIU	0	0	0	1	0	0	0	1	0	(implemented)
ORI		(same as above)(implemented)
SLTIU	(same as above)(implemented)
ANDI	(same as above)(implemented)
LUI		(same as above)(implemented)
		(RegDst is in $rt / in ALU, it should sll 16bits in order to make sign-extended imm to be upper code)(implemented)
LW		0	0	0	1	0	1	0	1	1	(implemented)
SW		X	0	0	1	0	0	1	0	X	(implemented)
BEQ		X	0	1	0	1	0	0	0	X	(implemented)
BNE		(same as above)						(implemented)
J		0	0	0	0	0	0	0	0	0	(implemented)
		(+ jump should change the mux value which is right before the IF_ID register)(implemented)
JAL		(same as above, except wrting ID.Bus.PC into ID.Register.regfile[rs] directly)(implemented)
		(+ jump should change the mux value which is right before the IF_ID register)(implemented)
JR		0	0	0	0	0	0	0	0	0	(implemented)
		(+ jump should change the mux value which is right before the IF_ID register)(implemented)
		(+ may have to read rs value at ID stage directly or stall three statges instead - but we must use only one stall :: in Register func.)(implemented)
		(reading rs is in ID-TAIL stage && so generation of consig should be in ID-HEAD stage)(implemented)
SLL		(R-type)(implemented)
SRL		(R-type)(implemented)

zero extended 는 어디에서 되는가? sign extension 에서?? 아니면,, and 용으로 따로 만들어야 하나? 오히려 ALU 에서 할 듯?




