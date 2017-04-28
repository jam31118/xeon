#include <iostream>
#include "xeon.hpp"

using namespace std;

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
		
	// Moves in EX stage starts
	/* ... */
	

	// Moves in MEM stage starts
	Xeon->MEM.BUS.ALU_result = Xeon->EX_MEM.ALU_result;
	Xeon->MEM.BUS.PC_target = Xeon->EX_MEM.PC_target;

	// Moves in WB stage starts
	Xeon->WB.BUS.ALU_result = Xeon->EX_MEM.ALU_result;// need to check
	Xeon->WB.BUS.dest = Xeon->MEM_WB.dest;
	Xeon->WB.BUS.read_data = Xeon->MEM_WB.read_data;

    return 0; 
}

int parseIDstage(XeonStruct *Xeon) {	
	struct MaskInstr Mask;
	//cout << Mask.OPCODE << endl;
	unsigned int instr = Xeon->ID.Bus.ID_IF_out.instr;	
	unsigned int op, rs, rt, rd, shift, imm, j26;
	/* Parsing */
	op = (Mask.OPCODE & instr) >> 26;
	rs = (Mask.RS & instr) >> 21;
	rt = (Mask.RT & instr) >> 16;
	rd = (Mask.RD & instr) >> 11;
	shift = (Mask.SHIFT & instr) >> 6;
	//funct = Mask.FUNCT & instr;
	imm = (Mask.IMM & instr) >> 16;
	j26 = (Mask.J26 & instr) >> 26;
	
	/* Assigning to each buses in ID stage */
	Xeon->ID.Bus.jump_x4_in = j26;
	Xeon->ID.Bus.control_in = op;
	Xeon->ID.Bus.read_addr_1 = rs;
	Xeon->ID.Bus.read_addr_2 = rt;
	Xeon->ID.Bus.sign_extension_in = imm;
	Xeon->ID.Bus.dest_1 = rt;
	Xeon->ID.Bus.dest_2 = rd;
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
        /* The instruction is R-type */
		putControlSignal(Xeon, consigDB.R);
    } else if (opcode >> 2 == 0) {
        /* The instruction is J-type */
        
    } else {
        /* The instruction is I-type */

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
	Xeon->ID.Bus.ConSig.jump = multiplier_x4(Xeon->ID.Bus.jump_x4_in);
	/* Move data into Bus.ConSig.jump in IF stage */
	Xeon->IF.BUS.ConSig.jump = Xeon->ID.Bus.ConSig.jump;
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
/* This puts control signals according to mod such as R(-type), lw etc.*/
/*
int putControlSignal(XeonStruct *Xeon, char mod) {
	struct ConSig consigDB;	
	switch (mod) {
		case 'R':
	        Xeon->ID_EX.ConSig.EX.RegDst = consigDB.R.EX[0];
    	    Xeon->ID_EX.ConSig.EX.ALU_Op1 = consigDB.R.EX[1];
        	Xeon->ID_EX.ConSig.EX.ALU_Op2 = consigDB.R.EX[2];
	        Xeon->ID_EX.ConSig.EX.ALU_Src = consigDB.R.EX[3];
    	    Xeon->ID_EX.ConSig.MEM.Brch = consigDB.R.MEM[0];
        	Xeon->ID_EX.ConSig.MEM.MemRead = consigDB.R.MEM[1];
	        Xeon->ID_EX.ConSig.MEM.MemWrite = consigDB.R.MEM[2];
    	    Xeon->ID_EX.ConSig.WB.RegWrite = consigDB.R.WB[0];
        	Xeon->ID_EX.ConSig.WB.MemtoReg = 0;
			break;
		default:
			cerr << "[ERROR] There is corresponding mode" << endl;
			cerr << "[ERROR] The input mod == " << mod << endl;
			return 1;
	} 
	return 0;
}
*/

void setPC(struct XeonStruct *Xeon){
	if(Xeon->IF.BUS.ConSig.PC_src==1)
	{
		if(Xeon->IF.BUS.ConSig.jump==1)
			Xeon->IF.PC=Xeon->IF.Tmp.jump;
		else
			Xeon->IF.PC=Xeon->IF.Tmp.branch;
	}
	else
		Xeon->IF.PC= Xeon->IF.PC + 4; //BUS ??
	//printf("THIS IS setPC FUNCTION\n");
}
// fetch instruction
void fetch(struct XeonStruct *Xeon) {
	Xeon->IF_ID.instr = Xeon->mem[Xeon->IF.BUS.PC];
	Xeon->IF_ID.PC = Xeon->IF.PC;
	//printf("TESTING fetch\n");
}

//MEM BUS to MEM
void move2src_MEM(struct XeonStruct *Xeon) {
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
			Xeon->IF.BUS.ConSig.PC_src= 1;
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
			Xeon->IF.BUS.ConSig.PC_src = 0;
		}
	}
	//printf("TESTING f_MEM\n");
}
//WB BUS to WB
void move2src_WB(struct XeonStruct *Xeon) {
	Xeon->WB.ALU_result = Xeon->WB.BUS.ALU_result;
	Xeon->WB.dest = Xeon->WB.BUS.dest;
	Xeon->WB.read_data = Xeon->WB.BUS.read_data;
	//printf("TESTING move2scr_WB\n");
}
void conSig(struct XeonStruct *Xeon){
	Xeon->MEM_WB.ConSig.WB.RegWrite = Xeon->EX_MEM.ConSig.WB.RegWrite;
	Xeon->MEM_WB.ConSig.WB.MemtoReg = Xeon->EX_MEM.ConSig.WB.MemtoReg;
}
// WB function
void f_WB(struct XeonStruct *Xeon) {
	// store at REG
	if (Xeon->MEM_WB.ConSig.WB.RegWrite == 1)
	{
		if (Xeon->MEM_WB.ConSig.WB.MemtoReg == 1) {
			Xeon->ID.Register.reg_file[Xeon->WB.dest] = Xeon->WB.read_data;// lw instruction
		}
		else {
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
	//printf("THIS IS ID_HEAD STAGE\n");
}
void EX_HEAD(struct XeonStruct *Xeon) {
	Xeon->MEM.Func.move2src_MEM(Xeon);
	//printf("THIS IS EX_HEAD STAGE\n");
}
void MEM_HEAD(struct XeonStruct *Xeon) {
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
	//printf("THIS IS ID_TAIL STAGE\n");
}
void EX_TAIL(struct XeonStruct * Xeon) {
	//printf("THIS IS EX_TAIL STAGE\n");
}
void MEM_TAIL(struct XeonStruct *Xeon) {
	Xeon->MEM.Func.f_MEM(Xeon);
	//printf("THIS IS MEM_TAIL STAGE\n");
}
void WB_TAIL(struct XeonStruct *Xeon) {
	
	//printf("THIS IS WB_TAIL STAGE\n");
}
