#include "sdk.h"





 int acc = 0;


int M1[MATRIX][MATRIX] = {};

int M2[MATRIX][MATRIX] = {};


void init_M(int  M[MATRIX][MATRIX]) {
    for (int i(0); i < MATRIX; i++) {
        M[i][i] = 1;
    }
}



int dot_product(int TF[], int IDF[]) {
    int res = 0;
    for (int i = 0; i < MATRIX; i++) {
        res += TF[i] * IDF[i];
    }
    return res;
}


void x_M(int IDF[], const vector<int>& v) {
    int t[MATRIX]{};
    for (auto i : v) {
        t[i] = IDF[i];
    }

    for (int i = 0; i < MATRIX; i++) {
        int res = 0;
        for (auto j : v) {
            res += t[j] * M1[j][i];
        }
        IDF[i] = res;
    }
}


void M_x(int TF[], const vector<int>& v) {
    int t[MATRIX]{};
    for (auto i : v) {
        t[i] = TF[i];
    }

    for (int i = 0; i < MATRIX; i++) {
        int res = 0;
        for (auto j : v) {
            res += M2[i][j] * t[j];
        }
        TF[i] = res;
    }
}


void Xor(const int& len) {

    string s1 = "0123456789";
    string s2 = "ABCDEFGHIJ";

    for (int i = 0; i < len; i++) {
        s1[i%10] ^= s2[i%10];
    }
}

unsigned get_ran32() {

    unsigned seed = static_cast<unsigned>(chrono::steady_clock::now().time_since_epoch().count());
    mt19937 mt(seed);
    uniform_int_distribution<unsigned> dist(0, 0xFFFFFFFF);  
    return dist(mt);
}


string sha_256(const string& plain) {

    string hvalue;

    try
    {
        SHA256 h;
        hvalue.clear();
        StringSource ss(plain, true ,
            new HashFilter(h,
                new Base64Encoder(
                    new StringSink(hvalue)
                ),             
                false, OUTLEN) 
        );                    
    }
    catch (const CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
        exit(1);
    }

    hvalue.erase(hvalue.size() - 1);

    return hvalue;
}



string H_mac(const string& plain, const string& key) {

    string encoded;

    try
    {
        HMAC< SHA256 > hmac((byte*)key.c_str(), key.size());

        encoded.clear();
        StringSource ss(plain, true,
            new HashFilter(hmac,
                new Base64Encoder(
                    new StringSink(encoded)
                ),             
                false, PRFLEN) 
        );                     
    }
    catch (const CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
        exit(1);
    }
    encoded.erase(encoded.size() - 1);
    return encoded;

}


