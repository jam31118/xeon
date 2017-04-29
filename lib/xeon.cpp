#include <iostream>
#include "xeon.hpp"

using namespace std;

int isOver(XeonStruct *Xeon) {
	int lastInstrIsInWB = (Xeon->pc_max == Xeon->WB.BUS.PC);
	if (lastInstrIsInWB) return 1;
	/*
	if (lastInstrIsInWB) { 
		int emptyMEM = (Xeon->MEM.BUS.PC == 0);
		int emptyEX = (Xeon->EX.bus.PC == 0);
		int emptyID = (Xeon->ID.Bus.ID_IF_out.PC == 0);
		if (emptyMEM && emptyEX && emptyID) { return 0; }
	}*/
	return 0;
}

int initalizeXeon(struct XeonStruct *Xeon, unsigned int *reg, unsigned char *mem, unsigned int PC) {
	Xeon->ID.Register.reg_file = reg;
	Xeon->mem = mem;
	Xeon->IF.PC = PC;	

	// Initialize IF structure
	Xeon->IF.Func.fetch = &fetch;
	Xeon->IF.Func.setPC = &setPC;

	// Initialize ID structure
	Xeon->ID.Func.parseIDstage = &parseIDstage;
	Xeon->ID.Func.move2entrance = &move2entrance;
	Xeon->ID.Func.read_register = &read_register;
	Xeon->ID.Func.sign_extension_ID = &sign_extension_ID;
	Xeon->ID.Func.move2dest = &move2dest;
	Xeon->ID.Func.multiply_x4 = &multiply_x4;
    Xeon->ID.Func.generateControlSignal = &generateControlSignal;

	// Initialize EX structure 
	Xeon->EX.Func.shift_left2_fp = &shift_left2;
	Xeon->EX.Func.det_ALU_ctrl_input = &det_ALU_ctrl_input;
	Xeon->EX.Func.ALU_execute = &ALU_execute;
    
	// Initialize MEM structure
	Xeon->MEM.Func.move2src_MEM = &move2src_MEM;
	Xeon->MEM.Func.f_MEM = &f_MEM;

	// Initialize WB structure
	Xeon->WB.Func.conSig = &conSig;
	Xeon->WB.Func.move2src_WB = &move2src_WB;
	Xeon->WB.Func.f_WB = &f_WB;

	/* Other initialization of Xeon Structure */
	
	return 0;
}

// Return 1 if something got wrong
int move2bus(struct XeonStruct *Xeon) {	
	// Moves in IF stage starts
	Xeon->IF.BUS.PC = Xeon->IF.PC;

	// Moves in ID stage starts
	Xeon->ID.Bus.ID_IF_out.PC = Xeon->IF_ID.PC;
	Xeon->ID.Bus.ID_IF_out.instr = Xeon->IF_ID.instr;
	//cout << "[ LOG ] (in move2bus) IF_ID.instr == 0x" << hex << Xeon->IF_ID.instr << endl;
		
	// Moves in EX stage starts
	Xeon->EX.bus.PC = Xeon->ID_EX.PC;
	

	// Moves in MEM stage starts
	Xeon->MEM.BUS.PC = Xeon->EX_MEM.PC;
	Xeon->MEM.BUS.ALU_result = Xeon->EX_MEM.ALU_result;
	Xeon->MEM.BUS.PC_target = Xeon->EX_MEM.PC_target;

	// Moves in WB stage starts
	Xeon->WB.BUS.PC = Xeon->MEM_WB.PC;
	Xeon->WB.BUS.ALU_result = Xeon->EX_MEM.ALU_result;// need to check
	Xeon->WB.BUS.dest = Xeon->MEM_WB.dest;
	Xeon->WB.BUS.read_data = Xeon->MEM_WB.read_data;

    return 0; 
}

int parseIDstage(XeonStruct *Xeon) {	
	struct MaskInstr Mask;
	//cout << Mask.OPCODE << endl;
	unsigned int instr = Xeon->ID.Bus.ID_IF_out.instr;	
	//cout << "[ LOG ] (in parseIDstage()) instr == 0x" << hex << instr << endl;
	unsigned int op, rs, rt, rd, imm, j26;
	/* Parsing */
	//cout << "[ LOG ] (in parseIDstage()) Mask.OPCODE == 0x" << hex << Mask.OPCODE << endl;
	op = (Mask.OPCODE & instr) >> 26;
	rs = (Mask.RS & instr) >> 21;
	rt = (Mask.RT & instr) >> 16;
	rd = (Mask.RD & instr) >> 11;
	//shift = (Mask.SHIFT & instr) >> 6;
	//funct = Mask.FUNCT & instr;
	imm = (Mask.IMM & instr);
	j26 = (Mask.J26 & instr);
	
	/* Assigning to each buses in ID stage */
	Xeon->ID.Bus.jump_x4_in = j26;
	Xeon->ID.Bus.control_in = op; 
	Xeon->ID.Bus.read_addr_1 = rs;
	Xeon->ID.Bus.read_addr_2 = rt;
	Xeon->ID.Bus.sign_extension_in = imm;
	Xeon->ID.Bus.dest_1 = rt;
	Xeon->ID.Bus.dest_2 = rd;

	// cout << "op == " << op << endl;
	// cout << "rs == " << rs << endl;
	// cout << "rt == " << rt << endl;
	// cout << "rd == " << rd << endl;
	// cout << "imm == " << imm << endl;
	// cout << "j26 == " << j26 << endl;

	return 0;
}

// It moves data in input buses to input for each component
// such as Register, Control etc.
// It returns 1 if something gets wrong.
int move2entrance(XeonStruct *Xeon) {
	/* Move data in input buses of Register into Register input */
	Xeon->ID.Register.read_addr_1 = Xeon->ID.Bus.read_addr_1;
	Xeon->ID.Register.read_addr_2 = Xeon->ID.Bus.read_addr_2;

	/* Move data in input bus of Control into Control input */
	Xeon->ID.Control.input = Xeon->ID.Bus.control_in;
	return 0;	
}
int move2dest(XeonStruct *Xeon) {
	/* Check the validity of input buses */	
	if (!is_5bit(Xeon->ID.Bus.dest_1) || !is_5bit(Xeon->ID.Bus.dest_2)) {
		cerr << "[ERROR] The input of Bus.dest_1 or Bus.dest_2 is not valid." << endl;
		cerr << "[ERROR] The value of Bus.dest_1 == " << Xeon->ID.Bus.dest_1 << endl;
		cerr << "[ERROR] The value of Bus.dest_2 == " << Xeon->ID.Bus.dest_2 << endl;
		return 1;
	}
	/* Move date into ID_EX register */
	Xeon->ID_EX.PC = Xeon->ID.Bus.ID_IF_out.PC;
	Xeon->ID_EX.Data.dest_1 = Xeon->ID.Bus.dest_1;
	Xeon->ID_EX.Data.dest_2 = Xeon->ID.Bus.dest_2;
	/* Returns zero if there's no problem */
	return 0;
}
int generateControlSignal(XeonStruct *Xeon) {
    /* Check validity of input signal which should be 6-bit opcode */ 
    if (!is_n_bit(Xeon->ID.Bus.control_in, 6)) {
        cerr << "[ERROR] The input of Control unit is not valid" << endl;
        cerr << "[ERROR] The input of Contorl == " << Xeon->ID.Bus.control_in << endl;
        return 1;
    }
    unsigned int opcode = Xeon->ID.Bus.control_in;
	struct ConSig consigDB;
    if (opcode == 0) {
		if (opcode == 0x08) {
			/* In case of JR */
			/* Put a bubble instr. into  IF_ID register */
			Xeon->IF_ID.instr = 0;
			/* Put controal signal to stall (bubble) */
			putControlSignal(Xeon, consigDB.Jump);
			/* Send data required for calculating jumping PC value to IF stage */
			Xeon->IF.BUS.ConSig.jr = 1;
			Xeon->IF.BUS.ConSig.jump = 1;
		} else {
        	/* The instruction is R-type */
			putControlSignal(Xeon, consigDB.R);
			Xeon->IF.BUS.ConSig.jump = 0;
		}
    } else if (opcode >> 2 == 0) {
        /* The instruction is J-type */
		switch (opcode) {
			case 0x2: // J
				/* Put a bubble instr. into  IF_ID register */
				Xeon->IF_ID.instr = 0;
				/* Put controal signal to stall (bubble) */
				putControlSignal(Xeon, consigDB.Jump);
				Xeon->IF.BUS.ConSig.jump = 1;
				break;
			case 0x3: // JAL
				/* Put PC+4 into $ra  */
				Xeon->ID.Register.reg_file[31] = Xeon->ID.Bus.ID_IF_out.PC;
				/* Put a bubble instr. into  IF_ID register */
				Xeon->IF_ID.instr = 0;
				/* Put controal signal to stall (bubble) */
				putControlSignal(Xeon, consigDB.Jump);
				Xeon->IF.BUS.ConSig.jump = 1;
				break;
			default:
				cerr << "[ERROR] (generateControlSignal) No opcode match in J-type." << endl;
				return 1;
		}
    } else {
        /* The instruction is I-type */
		/* (Should imiplement more I-type!! at 170429) */
		switch (opcode) {
			case 0x04: // BEQ
				putControlSignal(Xeon, consigDB.Brch);
				break;
			case 0x05: // BNE
				putControlSignal(Xeon, consigDB.Brch);
				break;
			case 0x23: // LW
				putControlSignal(Xeon, consigDB.LW);
				break;
			case 0x2b: // SW
				putControlSignal(Xeon, consigDB.SW);
				break;
			case 0x09: // ADDIU
				putControlSignal(Xeon, consigDB.I_typical);
				break;
			case 0x0d: // ORI
				putControlSignal(Xeon, consigDB.I_typical);
				break;
			case 0x0b: // SLTIU
				putControlSignal(Xeon, consigDB.I_typical);
				break;
			case 0x0c: // ADDI
				putControlSignal(Xeon, consigDB.I_typical);
				break;
			case 0x0f: // LUI
				putControlSignal(Xeon, consigDB.I_typical);
				break;
			default:
				cerr << "[ERROR] (generateControlSignal) No opcode match in I-type." << endl;
				return 1;
		}
		Xeon->IF.BUS.ConSig.jump = 0;
    }
    return 0;
}
int read_register(XeonStruct *Xeon) {
	/* Check validity of input data */
	if (!is_register_index(Xeon->ID.Register.read_addr_1)) {return 1;}
	if (!is_register_index(Xeon->ID.Register.read_addr_2)) {return 1;}
	
	/* Read register */
	Xeon->ID.Register.read_data_1 = Xeon->ID.Register.reg_file[Xeon->ID.Register.read_addr_1];
	Xeon->ID.Register.read_data_2 = Xeon->ID.Register.reg_file[Xeon->ID.Register.read_addr_2];

	/* Move data to ID_EX register */
	Xeon->ID_EX.Data.reg_read_data_1 = Xeon->ID.Register.read_data_1;
	Xeon->ID_EX.Data.reg_read_data_2 = Xeon->ID.Register.read_addr_2;

	/* If Register.do_jr is true, we should send $rs to IF stage */
	if (Xeon->ID.Register.do_jr) {
		Xeon->IF.Tmp.jr = Xeon->ID.Register.reg_file[Xeon->ID.Register.read_addr_1];
	}
	/* Returns zero if there's no problem */
	return 0;
}
int sign_extension_ID(XeonStruct *Xeon) {
	/* Check validity of input of sign extensor
	 * i.e., check whether it is IMM value or not. */
	if (!is_imm(Xeon->ID.Bus.sign_extension_in)) { 
		cerr << "[ERROR] The input of sign_extensor is not valid" << endl;
		cerr << "[ERROR] The value of Bus.sign_extension_in == " << Xeon->ID.Bus.sign_extension_in << endl;
		return 1; 
	}
	/* Do sign extension */
	Xeon->ID.Bus.sign_extension_out = sign_extensor(Xeon->ID.Bus.sign_extension_in); 
	/* Move extended data into ID_EX register */
	Xeon->ID_EX.Data.imm = Xeon->ID.Bus.sign_extension_out;
	return 0;
}
int multiply_x4(XeonStruct *Xeon) {
	/* Check the validitiy of input of multiplier */
	if (!is_n_bit(Xeon->ID.Bus.jump_x4_in, 26)) {
		cerr << "[ERROR] (multiply_x4) The input of muliplier_x4 in ID stage is not valid" << endl;
		cerr << "[ERROR] (multiply_x4) The input value of multiplier_x4 == " << Xeon->ID.Bus.jump_x4_in << endl;
		return 1;
	}
	/* Multiply */
	Xeon->ID.Bus.jump_x4_out = multiplier_x4(Xeon->ID.Bus.jump_x4_in);
	/* Move data into IF.Tmp.jump in IF stage */
	Xeon->IF.Tmp.jump = Xeon->ID.Bus.jump_x4_out;
	/* Returns zero if there's no error */
	return 0;
}
/* Lemma function for ID stage */
int is_register_index(unsigned int idx) {
	return (idx <= 31);
}
int is_imm(unsigned int imm_tested) {
	unsigned int mask_upper_16 = 0xFFFF0000;
	if (imm_tested & mask_upper_16) {
		return 0; // The upper 16 bits is not zero so it is not IMM value.
	} else {
		return 1; // It is valid IMM value.
	}
}
unsigned int sign_extensor(unsigned int input) {
	unsigned int inited = (0x0000FFFF & input);
	unsigned int signMask = 0x00008000;
	if (signMask & input) {
		inited += 0xFFFF0000;
	}
	return inited;
}
int is_5bit(unsigned int input) {
	unsigned int mask_lower5 = 0xFFFFFFE0;
	if ((input & mask_lower5)) { 
		return 0; // It isn't 5 bit
	} else { 
		return 1; // It is 5 bit 
	}
}
int is_n_bit(unsigned int input, int n) {
	unsigned int mask = 0x00000000;
	int i = 0;
	for (i=0; i<n; i++) {
		mask += 0x00000001 << n;
	}
	mask = !mask;
	if (input & mask) {
		return 0;
	} else {
		return 1;
	}
}
unsigned int multiplier_x4(unsigned int input) {
	return input << 2;
}
int putControlSignal(XeonStruct *Xeon, unsigned int sig9bits[]) {
	Xeon->ID_EX.ConSig.EX.RegDst = sig9bits[0];
    Xeon->ID_EX.ConSig.EX.ALU_Op1 = sig9bits[1];
    Xeon->ID_EX.ConSig.EX.ALU_Op2 = sig9bits[2];
	Xeon->ID_EX.ConSig.EX.ALU_Src = sig9bits[3];
    Xeon->ID_EX.ConSig.MEM.Brch = sig9bits[4];
   	Xeon->ID_EX.ConSig.MEM.MemRead = sig9bits[5];
    Xeon->ID_EX.ConSig.MEM.MemWrite = sig9bits[6];
    Xeon->ID_EX.ConSig.WB.RegWrite = sig9bits[7];
   	Xeon->ID_EX.ConSig.WB.MemtoReg = sig9bits[8];
	return 0;
}
unsigned int toBigEndian(unsigned int little) {
	unsigned int big = 0;
	big |= ((little & 0xff000000) >> 24);
	big |= ((little & 0x00ff0000) >> 8);
	big |= ((little & 0x0000ff00) << 8);
	big |= ((little & 0x000000ff) << 24);
	return big;
}
unsigned int toLittleEndian(unsigned int big) {
	unsigned int little = 0;
	little |= ((big & 0xff000000) >> 24);
	little |= ((big & 0x00ff0000) >> 8);
	little |= ((big & 0x0000ff00) << 8);
	little |= ((big & 0x000000ff) << 24);
	return little;
}


void branch_predict(struct XeonStruct *Xeon, int t)
{
	Xeon->IF.BUS.ConSig.branch = 0;
	if(t == 0){
		//printf("ANTP");
		if (Xeon->ID.Bus.control_in == 4) // beq
		{
		//	printf("ID.BUS.CONTROL_IN %d\n",Xeon->ID.Bus.control_in);
			if (Xeon->ID.Register.read_data_1 == Xeon->ID.Register.read_data_2)
			{
				Xeon->IF.BUS.ConSig.branch = 1;
				Xeon->IF.Tmp.branch = (Xeon->ID.Bus.sign_extension_in * 4) + Xeon->ID.Bus.ID_IF_out.PC;

				Xeon->IF_ID.instr = 0;
				Xeon->IF_ID.PC = 0;
				
			}
		}
		if (Xeon->ID.Bus.control_in == 5)
			{
				//printf("ID.BUS.CONTROL_IN %d\n",Xeon->ID.Bus.control_in);
				if (Xeon->ID.Register.read_data_1 != Xeon->ID.Register.read_data_2)
				{
					Xeon->IF.BUS.ConSig.branch = 1;
					Xeon->IF.Tmp.branch = (Xeon->ID.Bus.sign_extension_in * 4) + Xeon->ID.Bus.ID_IF_out.PC;

					Xeon->IF_ID.instr = 0;
					Xeon->IF_ID.PC = 0;
				}
			}
		}
	if(t == 1){
		//printf("ATP");
		if (Xeon->ID.Bus.control_in == 4) {
			//printf("ID.BUS.CONTROL_IN %d\n",Xeon->ID.Bus.control_in);
			Xeon->IF_ID.instr = 0;
			Xeon->IF_ID.PC = 0;
			if (Xeon->ID.Register.read_data_1 == Xeon->ID.Register.read_data_2)
			{
				Xeon->IF.BUS.ConSig.branch = 1;
				Xeon->IF.Tmp.branch = (Xeon->ID.Bus.sign_extension_in * 4) + Xeon->ID.Bus.ID_IF_out.PC;
			}
			else
				Xeon->IF.PC = Xeon->IF.PC - 4;
		}
		if (Xeon->ID.Bus.control_in == 5) {
			//printf("ID.BUS.CONTROL_IN %d\n",Xeon->ID.Bus.control_in);
			Xeon->IF_ID.instr = 0;
			Xeon->IF_ID.PC = 0;
			if (Xeon->ID.Register.read_data_1 != Xeon->ID.Register.read_data_2)
			{
				Xeon->IF.BUS.ConSig.branch = 1;
				Xeon->IF.Tmp.branch = (Xeon->ID.Bus.sign_extension_in * 4) + Xeon->ID.Bus.ID_IF_out.PC;
			}
			else
				Xeon->IF.PC = Xeon->IF.PC - 4;
		}

	}
}











void setPC(struct XeonStruct *Xeon){
	unsigned int tmp_1 = Xeon->IF.PC+4;
	if(Xeon->IF.BUS.ConSig.branch==1)
		tmp_1 = Xeon->IF.Tmp.branch;
	if(Xeon->IF.BUS.ConSig.jump==1)
		tmp_1 = Xeon->IF.Tmp.jump;
	if(Xeon->IF.BUS.ConSig.jr ==1)
		tmp_1 = Xeon->IF.Tmp.jr;
	Xeon->IF.PC = tmp_1;

	/* If PC exceeds pc_max */
	if (Xeon->IF.PC > Xeon->pc_max) {
		Xeon->IF_ID.PC = 0;
	}
}
// fetch instruction
void fetch(struct XeonStruct *Xeon) {
	unsigned int instrBigEndian = *(unsigned int*)(Xeon->mem + Xeon->IF.BUS.PC);
	Xeon->IF_ID.instr = toLittleEndian(instrBigEndian);
	// cout << "[ LOG ] (in fetch()) Xeon->IF.BUS.PC == 0x" << hex << Xeon->IF.BUS.PC << endl;
	// cout << "[ LOG ] (in fetch()) Xeon->mem[Xeon->IF.BUS.PC] == 0x" << hex << Xeon->mem[Xeon->IF.BUS.PC] << endl;
	// cout << "[ LOG ] (in fetch()) *(unsigned int*)(Xeon->mem + Xeon->IF.BUS.PC) == 0x" << hex << *(unsigned int*)(Xeon->mem + Xeon->IF.BUS.PC) << endl;
	 Xeon->IF_ID.PC = Xeon->IF.BUS.PC;
	//cout << "[ LOG ] Xeon->IF.BUS.PC == 0x" << hex << Xeon->IF.BUS.PC << endl;
	//printf("TESTING fetch\n");
}

//MEM BUS to MEM
void move2src_MEM(struct XeonStruct *Xeon) {
	Xeon->EX_MEM.PC = Xeon->EX.bus.PC;
	Xeon->MEM.addr_src = Xeon->MEM.BUS.ALU_result;
	Xeon->MEM.write_data = Xeon->ID_EX.Data.reg_read_data_2;
	//printf("TESTING move2src_MEM\n");
}

// MEM stage acting fucntion
void f_MEM(struct XeonStruct *Xeon) {
	if (Xeon->EX_MEM.ConSig.MEM.Brch== 0) {
		if (Xeon->EX_MEM.ConSig.MEM.MemRead == 0) {
			if (Xeon->EX_MEM.ConSig.MEM.MemWrite == 0) {
				// r type instructin
			}
			else {
				// sw instruction
				Xeon->mem[Xeon->MEM.addr_src] = Xeon->MEM.write_data;
			}
		}
		else {
			// lw instrution
			Xeon->MEM.read_data = Xeon->mem[Xeon->MEM.addr_src];
		}
	}
	else {
		if (Xeon->EX_MEM.ConSig.is_zero == 1) {
			//beq instruction
			Xeon->IF.BUS.ConSig.branch= 1;
			Xeon->IF.Tmp.branch= Xeon->MEM.BUS.PC_target;
			
			//Flush
			//Xeon->IF.BUS.ConSig.flush = 1;
			Xeon->IF_ID.instr = 0;
			Xeon->IF_ID.PC = 0;
			
			Xeon->ID_EX.ConSig.EX.RegDst = 0;
			Xeon->ID_EX.ConSig.EX.ALU_Op1 = 0;
			Xeon->ID_EX.ConSig.EX.ALU_Op2 = 0;
			Xeon->ID_EX.ConSig.EX.ALU_Src = 0;

			Xeon->ID_EX.ConSig.MEM.Brch = 0;
			Xeon->ID_EX.ConSig.MEM.MemRead = 0;
			Xeon->ID_EX.ConSig.MEM.MemWrite = 0;

			Xeon->ID_EX.ConSig.WB.RegWrite = 0;
			Xeon->ID_EX.ConSig.WB.MemtoReg = 0;

			Xeon->EX_MEM.ConSig.MEM.Brch = 0;
			Xeon->EX_MEM.ConSig.MEM.MemRead = 0;
			Xeon->EX_MEM.ConSig.MEM.MemWrite = 0;

			Xeon->EX_MEM.ConSig.WB.RegWrite = 0;
			Xeon->EX_MEM.ConSig.WB.MemtoReg = 0;
		}
		else {
		//	Xeon->IF.BUS.ConSig.flush = 0;
			Xeon->IF.BUS.ConSig.branch = 0;
		}
	}
	//printf("TESTING f_MEM\n");
}
//WB BUS to WB
void move2src_WB(struct XeonStruct *Xeon) {
	Xeon->MEM_WB.PC= Xeon->MEM.BUS.PC;
	Xeon->WB.ALU_result = Xeon->WB.BUS.ALU_result;
	Xeon->WB.dest = Xeon->WB.BUS.dest;
	Xeon->WB.read_data = Xeon->WB.BUS.read_data;
	//printf("TESTING move2scr_WB\n");
}
void conSig(struct XeonStruct *Xeon){
	Xeon->MEM_WB.ConSig.WB.RegWrite = Xeon->EX_MEM.ConSig.WB.RegWrite;
	Xeon->MEM_WB.ConSig.WB.MemtoReg = Xeon->EX_MEM.ConSig.WB.MemtoReg;
}

//EX stage initialize
//Prepare 

//EX stage : 0~0.5 clock
void EX_HEAD(struct XeonStruct *Xeon)
{
	//이용할 signal값 저장
	Xeon->EX.ConSig.ALUOp_sig = (Xeon->ID_EX.ConSig.EX.ALU_Op1)*2 + Xeon->ID_EX.ConSig.EX.ALU_Op2;
	Xeon->EX.ConSig.ALUSrc_sig = Xeon->ID_EX.ConSig.EX.ALU_Src;
	Xeon->EX.ConSig.RegDst_sig = Xeon->ID_EX.ConSig.EX.RegDst;
	//이용 안 할 signal 값
	Xeon->EX.ConSig.Brch = Xeon->ID_EX.ConSig.MEM.Brch;
	Xeon->EX.ConSig.MemRead = Xeon->ID_EX.ConSig.MEM.MemRead;
	Xeon->EX.ConSig.MemWrite = Xeon->ID_EX.ConSig.MEM.MemWrite;
	Xeon->EX.ConSig.RegWrite = Xeon->ID_EX.ConSig.WB.RegWrite;
	Xeon->EX.ConSig.MemtoReg = Xeon->ID_EX.ConSig.WB.MemtoReg;
	
	Xeon->EX.bus.op_code = Xeon->ID_EX.Data.opcode;
	Xeon->EX.PC = Xeon->ID_EX.PC;
	Xeon->EX.bus.sign_extended = Xeon->ID_EX.Data.imm;
	Xeon->EX.bus.Register_Addr2 = Xeon->ID_EX.Data.dest_1;		//rt
	Xeon->EX.bus.Register_write = Xeon->ID_EX.Data.dest_2;		//rd
	
	//ALU source 준비
	Xeon->EX.bus.RegisterRs_data = Xeon->ID_EX.Data.reg_read_data_1;			//먼저 이렇게 넣어놓고 나중에 전방전달로 바꾸든가 한다.
	Xeon->EX.bus.RegisterRt_data = Xeon->ID_EX.Data.reg_read_data_2;
/*	
	if (Xeon->EX_MEM.ConSig.WB.RegWrite && Xeon->EX_MEM.RegisterRd != 0 && (Xeon->EX_MEM.RegisterRd == Xeon->ID_EX.RegisterRs))
		Xeon->EX.bus.RegisterRs_data = Xeon->MEM.BUS.ALU_result;
	
	if (Xeon->EX_MEM.ConSig.WB.RegWrite && Xeon->EX_MEM.RegisterRd != 0 && (Xeon->EX_MEM.RegisterRd == Xeon->ID_EX.RegisterRt))	
		Xeon->EX.bus.RegisterRt_data = Xeon->MEM.BUS.ALU_result;
	
	if (Xeon->MEM_WB.RegWrite && Xeon->EX_MEM.RegisterRd != 0 && (Xeon->EX_MEM.RegisterRd == Xeon->ID_EX.RegisterRs) && (Xeon->EX_MEM.RegisterRd != Xeon->ID_EX.RegisterRs))
		Xeon->EX.bus.RegisterRs_data = Xeon->WB.bus.fwd_WB;
	if (Xeon->EX_MEM.RegWrite && Xeon->EX_MEM.RegisterRd != 0 && (Xeon->EX_MEM.RegisterRd == Xeon->ID_EX.RegisterRt) && (Xeon->EX_MEM.RegisterRd != Xeon->ID_EX.RegisterRt))
		Xeon->EX.bus.RegisterRt_data = Xeon->WB.bus.fwd_WB;
*/
	Xeon->EX.ALUSrc1 = Xeon->EX.bus.RegisterRs_data;		//ALU에다 집어넣을 src1을 저장(얘는 확정)
	Xeon->EX.ALUSrc2 = Xeon->EX.bus.RegisterRt_data;		//ALU에다 집어넣을 src2을 '일단은' 저장
	Xeon->EX.SW_data = Xeon->EX.ALUSrc2;		//SW instr.의 경우 이 값이 data memory에 쓸 데이터에 해당함.

	Xeon->EX.ALUSrc_mux = Xeon->EX.ConSig.ALUSrc_sig;		//이게 1이 뜨면 sign extended된 값을 ALU source2로 이용하게 된다. ALUSrc2가 전방전달된 값을 받았을 경우 ALUSrc_sig는 1이 될 리가 없다.
	if (Xeon->EX.ALUSrc_mux == 1)
		Xeon->EX.ALUSrc2 = Xeon->EX.bus.sign_extended;

	Xeon->EX.shifted_value = Xeon->EX.Func.shift_left2_fp(Xeon->EX.bus.sign_extended);
}
//EX stage : 0.5~1 clock
void EX_TAIL(struct XeonStruct *Xeon)
{
	unsigned int six_bit_field = 0;
	Xeon->EX.RegDst_mux = Xeon->EX.ConSig.RegDst_sig;
	if (Xeon->EX.RegDst_mux == 1)
		Xeon->EX.RegDst = Xeon->EX.bus.Register_write;
	else
		Xeon->EX.RegDst = Xeon->EX.bus.Register_Addr2;
		
	if (Xeon->EX.ConSig.ALUOp_sig == 2)
		six_bit_field = (Xeon->EX.bus.sign_extended) & 63;		//funct field 6비트 저장
	if (Xeon->EX.ConSig.ALUOp_sig == 3)
		six_bit_field = Xeon->EX.bus.op_code;		//op field 6비트 저장

	Xeon->EX.ALU_control_input = Xeon->EX.Func.det_ALU_ctrl_input(Xeon->EX.ConSig.ALUOp_sig, six_bit_field);
	Xeon->EX.ALU_result = Xeon->EX.Func.ALU_execute(Xeon->EX.ALUSrc1, Xeon->EX.ALUSrc2, Xeon->EX.ALU_control_input);


	//결과값들을 EX_MEM에 모두 저장
	Xeon->EX_MEM.ALU_result = Xeon->EX.ALU_result;
	Xeon->EX_MEM.PC_target = Xeon->EX.shifted_value + Xeon->EX.PC;
	Xeon->EX_MEM.fin_reg_dst = Xeon->EX.RegDst;
	Xeon->EX_MEM.SW_data = Xeon->EX.SW_data;

	//이용하지 않은 signal을 모두 저장
	Xeon->EX_MEM.ConSig.MEM.Brch = Xeon->EX.ConSig.Brch;
	Xeon->EX_MEM.ConSig.MEM.MemRead = Xeon->EX.ConSig.MemRead;
	Xeon->EX_MEM.ConSig.MEM.MemWrite = Xeon->EX.ConSig.MemWrite;
	Xeon->EX_MEM.ConSig.WB.RegWrite = Xeon->EX.ConSig.RegWrite;
	Xeon->EX_MEM.ConSig.WB.MemtoReg = Xeon->EX.ConSig.MemtoReg;

}

// EX stage functions
unsigned int shift_left2(unsigned int val)
{
	val = val << 2;		//왼쪽으로 2칸 shift
	return val;
}

unsigned int det_ALU_ctrl_input(unsigned int aluop_sig, unsigned int six_bit_field)
{
	switch (aluop_sig)
	{
	case 0:		// lw, sw, ADDIU
		return 2;		// 0010
		break;
	case 1:		// beq, bne
		return 6;
		break;		// 0110
	case 2:		// R type
		switch (six_bit_field)
		{
		case 33:	//ADDU
			return 2;
			break;
		case 35:	//SUBU
			return 6;
			break;
		case 36:	//AND
			return 0;
			break;
		case 37:	//OR
			return 1;
			break;
		case 39:	//NOR
			return 14;
			break;
		case 42:	//SLTU
			return 7;
			break;
		case 0:		//SLL
			return 12;
			break;
		case 2:		//SRL
			return 3;
		}
		break;
	case 3:		//I type
		switch (six_bit_field)
		{
		case 15:	//LUI
			return 12;
			break;
		case 13:	//ORI
			return 1;
			break;
		case 12:	//ANDI
			return 0;
			break;
		case 11:	//SLTIU
			return 7;
		}
		break;
	}
}

unsigned int ALU_execute(unsigned int alu_src1, unsigned int alu_src2, unsigned int alu_control_input)
{
	switch (alu_control_input)
	{
	case 0:
		return alu_src1 & alu_src2;
	case 1:
		return alu_src1 | alu_src2;
	case 2:
		return alu_src1 + alu_src2;
	case 3:
		return alu_src2 >> alu_src1;		//shamt를 alu_src1으로 대체할 방법 구상 필요
	case 6:
		return alu_src1 - alu_src2;
	case 7:
		if (alu_src1 < alu_src2)
			return 1;
		else
			return 0;
	case 12:
		return alu_src2 << alu_src1;		//shamt를 alu_src1으로 대체할 방법 구상 필요
	case 14:
		return ~(alu_src1 | alu_src2);
	}
}

// WB function

void f_WB(struct XeonStruct *Xeon) {
	// store at REG
	if (Xeon->MEM_WB.ConSig.WB.RegWrite == 1)
	{
		if (Xeon->MEM_WB.ConSig.WB.MemtoReg == 1) {
			Xeon->WB.BUS.fwd_WB =Xeon->WB.read_data;
			Xeon->ID.Register.reg_file[Xeon->WB.dest] = Xeon->WB.read_data;// lw instruction
		}
		else {
			Xeon->WB.BUS.fwd_WB = Xeon->WB.ALU_result;
			Xeon->ID.Register.reg_file[Xeon->WB.dest] = Xeon->WB.ALU_result;// r type instruction
		}
	}
	//printf("TESTING f_WB\n");
}


void IF_HEAD(struct XeonStruct *Xeon){
	Xeon->IF.Func.setPC(Xeon);
	Xeon->IF.Func.fetch(Xeon);
	//printf("THIS IS IF_HEAD STAGE\n");
}
void ID_HEAD(struct XeonStruct *Xeon) {
	Xeon->ID.Func.parseIDstage(Xeon);
	Xeon->ID.Func.move2entrance(Xeon);
	Xeon->ID.Func.move2dest(Xeon);
	Xeon->ID.Func.generateControlSignal(Xeon);
	//printf("THIS IS ID_HEAD STAGE\n");
}
void MEM_HEAD(struct XeonStruct *Xeon) {
	Xeon->MEM.Func.move2src_MEM(Xeon);
	//printf("THIS IS MEM_HEAD STAGE\n");
}
void WB_HEAD(struct XeonStruct *Xeon) {
	Xeon->WB.Func.conSig(Xeon);
	Xeon->WB.Func.move2src_WB(Xeon);
	Xeon->WB.Func.f_WB(Xeon);
	//printf("THIS IS WB_HEAD STAGE\n");
}
void IF_TAIL(struct XeonStruct *Xeon) {
  /* Nothing */
  //printf("THIS IS IF_TAIL STAGE\n");
}
void ID_TAIL(struct XeonStruct *Xeon) {
	Xeon->ID.Func.read_register(Xeon);
	Xeon->ID.Func.sign_extension_ID(Xeon);
	Xeon->ID.Func.multiply_x4(Xeon);
	//printf("THIS IS ID_TAIL STAGE\n");
}
void MEM_TAIL(struct XeonStruct *Xeon) {
	Xeon->MEM.Func.f_MEM(Xeon);
	//printf("THIS IS MEM_TAIL STAGE\n");
}
void WB_TAIL(struct XeonStruct *Xeon) {
	
	//printf("THIS IS WB_TAIL STAGE\n");
}
