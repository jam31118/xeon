#include <iostream>
#include <sstream>
#include <string>
#include "print_func.h"
#include "func.h"
#include "xeon.h"

using namespace std;

int main(int argc, char* argv[]) {
	string filename;
	int d = 0; // -d옵션이 입력되면 d=1로 하였습니다.
	int m = 0; // -m옵션이 입력되면 m=1로 하였습니다.
	unsigned int addr_begin = 0;
	unsigned int addr_end = 0;

	int n = -1; // -n 옵션이 따로 입력되지 않으면 n을 -1로 하였습니다.

	int argvi = 1;

	// argv를 읽는 부분입니다. argv를 읽는데 stream을 사용해야 한다는 것을 몰라 다소 지저분해졌습니다.
	while (true) {
		istringstream ss(argv[argvi]);
		string j;
		ss >> j;
		if (j[0] == '-') {
			if (j[1] == 'm') {
				m = 1;
				istringstream ss(argv[argvi + 1]);
				ss >> j;
				int sep;
				sep = j.find(":");
				addr_begin = hex2int(j.substr(0, sep));
				addr_end = hex2int(j.substr(sep + 1, j.length() - sep - 1));
				argvi += 2;
			}
			else if (j[1] == 'd') {
				d = 1;
				argvi += 1;
			}
			else if (j[1] == 'n') {
				istringstream ss(argv[argvi + 1]);
				ss >> j;
				n = atoi(j.c_str());
				argvi += 2;
			}
			else {
				cout << "Something wrong in argv" << endl;
			}
		}
		else {
			istringstream ss(argv[argvi]);
			ss >> filename;
			break;
		}
	}

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

	if (d == 1) { // d옵션이 있을 경우 instruction을 실행할 때마다 print해야하므로, 일단 d를 기준으로 나누었습니다.
		if (n == -1) {
			int mark = 0;
			while (mark == 0) { // instruction함수에서 끝날때 1을 반환합니다. 이 반환값은 mark에 담깁니다.
				mark = instruction(reg, mem, PC, mainloc);
				if (mark == 0) {
					print_reg(PC, reg);
				}
				if (m == 1) {
					print_mem(mem, addr_begin, addr_end);
				}
			}
		}
		else {
			for (int i = 0; i < n; ++i) {
				instruction(reg, mem, PC, mainloc);
				print_reg(PC, reg);
				if (m == 1) {
					print_mem(mem, addr_begin, addr_end);
				}
			}
		}
	}
	else {
		if (n == -1) {
			int mark = 0;
			while (mark == 0) {
				mark = instruction(reg, mem, PC, mainloc);
			}
		}
		else {
			for (int i = 0; i < n; ++i) {
				instruction(reg, mem, PC, mainloc);
			}
		}
		print_reg(PC, reg);
		if (m == 1) {
			print_mem(mem, addr_begin, addr_end);
		}
	}

	// 동적 할당받은 메모리들을 삭제하여줍니다.
	delete[] mem;
	delete[] reg;

	
	/* TESTING START */
	XeonStruct Xeon;
	cout << "Xeon.clock == " << Xeon.clock << endl;
	IFstage(&Xeon);
	/* TESTING END */

	return 0;
}
