#include <iostream>
#include <sstream>
#include <string>
#include "print_func.hpp"
#include "func.hpp"
#include "xeon.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	string filename;
	int d = 0; // print registor file and current PC per every cycle
	int m = 0; // after end of simulation do memory dump
	unsigned int addr_begin = 0;
	unsigned int addr_end = 0;
	int p = 0; // print every PCs in each pipeline stage per every cycle
	int f = 
	// argv�� �д� �κ��Դϴ�. argv�� �дµ� stream�� �����ؾ� �Ѵٴ� ���� ���� �ټ� �������������ϴ�.
    if (getopt(argc,argv,&d,&m,&p,&addr_begin,&addr_end, filename)) { return 1; } // �߸��� �ɼ��Ľ�

	// input�� ���� �������� ������ ������� ������ ����.
	string input[1000];
	filename.pop_back();
	string infilename = filename + "s";
	string outfilename = filename + "o";
	readfile(infilename, input);

	// �������� ���� �Ҵ�.
	unsigned int* reg = new unsigned int[32];
	// �ʱ�ȭ
	for (int i = 0; i < 32; ++i) {
		reg[i] = 0x00000000;
	}

	// �޸��� ���� �Ҵ�. ���� �Ҵ� ���� �� �޸���, array ���� ������ ���ϱ� ���� ������ ���̸� �Ҵ� �޾ҽ��ϴ�.
	unsigned char* mem = new unsigned char[0x3fffffff];
	// �ʱ�ȭ
	for (int i = 0; i < 0x3fffffff; ++i) {
		mem[i] = 0;
	}
	// �ʱ� PC�Դϴ�. text�� ó�� ������ ������ 0x00400000�� ����ŵ�ϴ�.
	unsigned int pc = 0x00400000;
	//unsigned int* PC = &pc;

	// label�� �󺧵鸸 ���� ������ string�Դϴ�. �ִ� 10���� ������ �ʰڰŴ�.. �����Ͽ����ϴ�.
	string label[10];
	findlabel(input, label);
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ������ �ʵ��� ����ó���϶��� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savedata(input, mem);
	}
	
	// ������ �Լ����� �ϳ��� �������ݴϴ�.
	int data_size = lab2loc(input, label);
	la2lui(input);
	lab2pos(input, label);
	//int mainloc = formain(input);
	cleaner(input);

	// ��ȯ�� binary �ڵ带 ���� string array�Դϴ�.
	string binary[1000];
	int text_size = convert(input, binary);

	savefile(outfilename, binary, text_size, data_size);
	if (n != 0) { // n�� 0�� ���� �޸𸮿� ������ �ʵ��� ����ó���϶��� �ϼż�, if���� �߰��Ͽ����ϴ�.
		savetext(binary, mem);
	}

	
	// /* test start */
	// cout << "Xeon.clock == " << Xeon.clock << endl; 
	// // ID-head stage
	// cout << "Result of parseIDstage: " << Xeon.ID.Func.parseIDstage(&Xeon) << endl;
	// cout << "Result of move2entrance: "<< Xeon.ID.Func.move2entrance(&Xeon) << endl;
	// cout << "Result of move2dest: " << Xeon.ID.Func.move2dest(&Xeon) << endl;
	// cout << "Result of generateControlSignal: " << Xeon.ID.Func.generateControlSignal(&Xeon) << endl;
	// // ID-tail stage
	// cout << "Resulf of read_register: " << Xeon.ID.Func.read_register(&Xeon) << endl;
	// cout << "Result of sign_extension_ID: " << Xeon.ID.Func.sign_extension_ID(&Xeon) << endl;
	// cout << "Result of multiply_x4: " << Xeon.ID.Func.multiply_x4(&Xeon) << endl;

    // /* test ends */ 
    // ���ɾ� ���� �ٽ� �κ�
    
	// Initialize Xeon Structure
	XeonStruct Xeon;
	initalizeXeon(&Xeon,reg,mem,pc);
	
	unsigned int pc_max = Xeon.IF.PC + text_size - 4;
    if (n >= 0) { pc_max = Xeon.IF.PC + (n-1)*4; }
    while (pc_max >= Xeon.IF.PC) {
		//cout << "in while\n";
        //instruction(reg, mem, PC, mainloc);
        /* Clock 0 ~ 0.5 */
		move2bus(&Xeon);

		IF_HEAD(&Xeon);
		ID_HEAD(&Xeon);
		EX_HEAD(&Xeon);
		MEM_HEAD(&Xeon);
		WB_HEAD(&Xeon);

		/* Clock 0.5 ~ 1 */ 
		IF_TAIL(&Xeon); 
		ID_TAIL(&Xeon); 
		EX_TAIL(&Xeon);
		MEM_TAIL(&Xeon);
		WB_TAIL(&Xeon);

		Xeon.cycle++;
		if (d) { print_reg(&Xeon.IF.PC, reg); }
        if (d && m) { print_mem(mem, addr_begin, addr_end); }
    }
    if (d && !n) { print_reg(&Xeon.IF.PC, reg); }
    if (!d && m) { print_mem(mem, addr_begin, addr_end); }
    if (!d && !m) { print_reg(&(Xeon.IF.PC), reg); } 

	delete[] reg;

	/* TESTING START */
//	XeonStruct Xeon;
//	cout << "Xeon.clock == " << Xeon.clock << endl;
	//IFstage(&Xeon);
	/* TESTING END */

	return 0;
}
