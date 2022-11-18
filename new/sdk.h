#pragma once
#include <iostream>  
#include <stdio.h>
#include <sstream>   
#include <fstream>

#include <string>  
#include <string.h>


#include <osrng.h>
#include <cryptlib.h>
#include <hmac.h>
#include <sha.h>
#include <sha3.h>
#include <secblock.h>
#include <modes.h>
#include <aes.h>  
#include <Hex.h>      
#include <modes.h>  
#include <osrng.h>
#include <modes.h>
#include <base64.h>
#include <filters.h>

#include <random> 
#include <chrono>
#include <time.h>
#include <thread>
#include <math.h>


#include <unordered_map>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <algorithm>


#define OUTLEN 12  
#define PRFLEN 16  
#define MATRIX 2000 
#define XORLEN 2005 

#define NUM_S1 1000 
#define NUM_S2 1000 


using namespace std;
using namespace CryptoPP;

#pragma comment(lib, "cryptlib.lib" )


extern int acc;


extern int M1[MATRIX][MATRIX];

extern int M2[MATRIX][MATRIX];


void init_M(int  M[MATRIX][MATRIX]);


int dot_product(int TF[], int IDF[]);


void x_M(int IDF[], const vector<int>& v);


void M_x(int TF[], const vector<int>& v);



struct key_info {
	unsigned int cnt = 0;
	unsigned int ver = 0;
};

struct data1{
	string prev = "";
	unsigned short op = 0; 
	unsigned int id = 0;  
	int* I = nullptr; 

	data1() = default;
	data1(data1&& d) noexcept :prev(d.prev), op(d.op), id(d.id), I(d.I) {
		d.I = nullptr;
	}
};


struct invert_dict {
	string kw = ""; 
	struct data1 value; 

	invert_dict() = default;
	invert_dict(invert_dict && inv)noexcept: kw(inv.kw), value(std::move(inv.value)){}
};



struct stoken {
	string T1 = ""; 
	int  *T2 = nullptr; 
	int T3 = 0; 

	stoken() = default;
	stoken(stoken&& st) noexcept : T1(st.T1), T2(st.T2), T3(st.T3) {
		st.T2 = nullptr;
	}
	~stoken() {
		delete[] T2;
	}
};



unsigned get_ran32();

void Xor(const int& len);


string sha_256(const string& plain);

string H_mac(const string& plain, const string& key);
