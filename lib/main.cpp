#include <iostream>
#include <sstream>
#include <string>
#include "print_func.hpp"
#include "func.hpp"
#include "xeon.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	string filename;
	int d = 0; // -d옵션이 입력되면 d=1로 하였습니다.
	int m = 0; // -m옵션이 입력되면 m=1로 하였습니다.
	unsigned int addr_begin = 0;
	unsigned int addr_end = 0;
	int n = -1; // -n 옵션이 따로 입력되지 않으면 n을 -1로 하였습니다.

	// argv를 읽는 부분입니다. argv를 읽는데 stream을 사용해야 한다는 것을 몰라 다소 지저분해졌습니다.
    if (getopt(argc,argv,&d,&m,&n,&addr_begin,&addr_end, filename)) { return 1; } // 잘못된 옵션파싱

	// input은 읽을 어셈블리 파일을 덩어리별로 저장할 공간.
	string input[1000];
	filename.pop_back();
	string infilename = filename + "s";
	string outfilename = filename + "o";
	readfile(infilename, input);

	// 레지스터 동적 할당.
	unsigned int* reg = new unsigned int[32];
	// 초기화
	for (int i = 0; i < 32; ++i) {
		reg[i] = 0x00000000;
	}

	// 메모리 동적 할당. 동적 할당 받을 때 메모리, array 길이 오류를 피하기 위해 적당한 길이를 할당 받았습니다.
	unsigned char* mem = new unsigned char[0x3fffffff];
	// 초기화
	for (int i = 0; i < 0x3fffffff; ++i) {
		mem[i] = 0;
	}
	// 초기 PC입니다. text가 처음 저장될 공간인 0x00400000을 가리킵니다.
	unsigned int pc = 0x00400000;
	unsigned int* PC = &pc;

	// label은 라벨들만 따로 저장할 string입니다. 최대 10개가 넘지는 않겠거니.. 생각하였습니다.
	string label[10];
	findlabel(input, label);
	if (n != 0) { // n이 0일 때는 메모리에 담기지 않도록 예외처리하라고 하셔서, if문을 추가하였습니다.
		savedata(input, mem);
	}
	
	// 제작한 함수들을 하나씩 실행해줍니다.
	int data_size = lab2loc(input, label);
	la2lui(input);
	lab2pos(input, label);
	int mainloc = formain(input);
	cleaner(input);

	// 변환된 binary 코드를 담을 string array입니다.
	string binary[1000];
	int text_size = convert(input, binary);

	savefile(outfilename, binary, text_size, data_size);
	if (n != 0) { // n이 0일 때는 메모리에 담기지 않도록 예외처리하라고 하셔서, if문을 추가하였습니다.
		savetext(binary, mem);
	}

	// Initialize Xeon Structure
	XeonStruct Xeon;
	initalizeXeon(&Xeon,reg,mem,pc);
	//fetch(&Xeon);
	//move2src_MEM(&Xeon);
	//f_MEM(&Xeon);
	//move2src_WB(&Xeon);
	//f_WB(&Xeon);
	/* test start */
	cout << "Xeon.clock == " << Xeon.clock << endl; 
	// ID-head stage
	cout << "Result of parseIDstage: " << Xeon.ID.Func.parseIDstage(&Xeon) << endl;
	cout << "Result of move2entrance: "<< Xeon.ID.Func.move2entrance(&Xeon) << endl;
	// ID-tail stage
	cout << "Resulf of read_register: " << Xeon.ID.Func.read_register(&Xeon) << endl;
	cout << "Result of sign_extension_ID: " << Xeon.ID.Func.sign_extension_ID(&Xeon) << endl;

    /* test ends */ 
    // 명령어 실행 핵심 부분
    unsigned int pc_max = pc + text_size - 4;
    if (n >= 0) { pc_max = pc + (n-1)*4; }
    while (pc_max >= pc) {
		//cout << "in while\n";
        instruction(reg, mem, PC, mainloc);
        if (d) { print_reg(PC, reg); }
        if (d && m) { print_mem(mem, addr_begin, addr_end); }
    }
    if (d && !n) { print_reg(PC, reg); }
    if (!d && m) { print_mem(mem, addr_begin, addr_end); }
    if (!d && !m) { print_reg(PC, reg); } 

	delete[] reg;

	/* TESTING START */
//	XeonStruct Xeon;
//	cout << "Xeon.clock == " << Xeon.clock << endl;
	//IFstage(&Xeon);
	/* TESTING END */

	return 0;
}
