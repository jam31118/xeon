#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>

using namespace std;

bool isinst(string input);
void wmem(unsigned char* mem, unsigned int n, unsigned int a);
unsigned int rmem(unsigned char* mem, unsigned int n);

unsigned int hex2int(string input);
string int2bi(unsigned int num, int len);
string int2sbi(int num, int len);
unsigned int bi2int(string input);
int sbi2int(string input);

void readfile(string filename, string* input);
void findlabel(string* input, string* label);
void savedata(string* input, unsigned char* mem);
int lab2loc(string* input, string* label);
void la2lui(string* input);
void lab2pos(string* input, string* label);
int formain(string* input);
void cleaner(string* input);
int convert(string* input, string* binary);
void savefile(string filename, string* input, int text, int data);
void savetext(string* input, unsigned char* mem);
int instruction(unsigned int* reg, unsigned char* mem, unsigned int* PC, int mainloc);
