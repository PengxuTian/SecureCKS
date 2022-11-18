#pragma once
#include "sdk.h"
#include <cmath>



class Client
{
private:
	string F1(const string& plain) {
		string key = "ASDFGHJKL1239870";
		return H_mac(plain, key);
	}

	unordered_map <string, unsigned> pos;
	unordered_map <string, key_info> kw_dict;

public:

	
	bool kwdict_add(const string& kw, const unsigned& cver, const unsigned& lnum) {
		if (this->kw_dict.emplace(kw, key_info{ cver, lnum }).second)
			return true;
		cerr << "error in kwdict_add:  " + kw + " added failed!" << endl;
		return false;
	}


	bool pos_add(const string& kw, const unsigned& pos) {
		if (this->pos.emplace(kw, pos).second)
			return true;
		cerr << "error in pos_add():  " + kw + " added failed!" << endl;
		return false;
	}

	void kwdict_initialize(const string& path) {
		fstream f(path, ios::in);
		string line;
		if (f) {
			unsigned counter(0);
			while (getline(f, line)) {
				counter++;
				stringstream ss(line);
				string kw;
				unsigned cver, lnum;
				ss >> kw >> cver >> lnum;
				if (!this->kwdict_add(kw, cver, lnum))
					cerr << "error line number in " + path + " : " << counter << endl;
			}
		}
		else
			cerr << "Error in kwdict_initialize(): " + path + " open failed  !" << endl;
	}


	void pos_initialize(const string& path) {
		fstream f(path, ios::in);
		string line;
		if (f) {
			unsigned counter(0);
			while (getline(f, line)) {
				counter++;
				stringstream ss(line);
				string kw;
				unsigned pos;
				ss >> kw >> pos;
				if (!this->pos_add(kw, pos))
					cerr << "error line number in " + path + " : " << counter << endl;
			}
		}
		else
			cerr << "Error in pos_initialize(): " + path + " open failed  !" << endl;
	}


	void kwdict_save(const string& path) {
		fstream f(path, ios::out);
		if (f) {
			for (auto const& temp : this->kw_dict) {
				f << temp.first + " " << temp.second.cnt << " " << temp.second.ver << endl;
			}
			cout << "kw_dict is saved in file " + path + " sucessfully!" << endl;
		}
		else
			cerr << "Error in kwdict_save(): " + path + " open failed  !" << endl;

	}


	key_info kwdict_get(const string& kw) {


		auto temp = this->kw_dict.find(kw);
		if (temp == this->kw_dict.end()) { 
			this->kw_dict.emplace(kw, key_info{ 1, 1 });
			return { 1, 1 };
		}
		temp->second.cnt++;
		return temp->second;
	}


	key_info kwdict_retrival(const string& kw) {
		auto temp = this->kw_dict.find(kw);
		if (temp == this->kw_dict.end()) {
			cerr << "error: can not find keyword(" + kw + ") in kw_dict!" << endl;
			return { 0, 0 }; 
		}
		return { temp->second.cnt, temp->second.ver };
	}

	/////////////////////////////////////////////
	////////      generate index        /////////
	/////////////////////////////////////////////

	invert_dict gen_inv(const string& kw, const unsigned& ind, int *TF) {
		invert_dict inv;
		key_info k_info = this->kwdict_get(kw); 
		stringstream ss;


		ss << kw << k_info.cnt << k_info.ver;
		string sk = this->F1(ss.str());
		inv.kw = sha_256(sk);



		ss.clear(); 
		ss.str(""); 
		auto t = k_info.cnt - 1;
		if (t) {
			ss << kw << t << k_info.ver;
			sk = this->F1(ss.str());
			inv.value.prev = sk;
		}




		inv.value.id = ind;


		inv.value.I = TF;


		sha_256(sk); 
		Xor(XORLEN); 

		return inv;
	}


	/////////////////////////////////////////////
	////////      generate token        /////////
	/////////////////////////////////////////////


	stoken gen_stok(string& all) {

		cout << "Please input keywords to search(Enter + Ctrl + Z to end): " << endl;
		string word;
		set<string> in_temp; 

		while (cin >> word || !in_temp.size()) { 
			if (in_temp.size() && cin.eof())
				break;
			in_temp.emplace(word);
		}


		vector<pair<string, key_info>> kw_list;
		for (auto& i : in_temp) {
			key_info infotemp = this->kwdict_retrival(i);
			if (infotemp.cnt == 0) {
				cerr << "Error: keyword " + i + " not found in local kw_dict!\n Search founcation return!\n";

				return stoken(); 
			}
			kw_list.emplace_back(i, infotemp);
		}

		all = "";
		stoken stok;
		cout << "kw_list.size: " << kw_list.size() << endl;
		if (kw_list.size() > 1) {

			auto t_min = kw_list.begin();

			for (auto t1 = kw_list.begin() + 1; t1 != kw_list.end(); t1++) {
				if (t1->second.cnt < t_min->second.cnt) {
					t_min = t1;
				}
			}


			all += t_min->first + " ";
			

			stringstream ss;
			ss << t_min->first << t_min->second.cnt << t_min->second.ver;
			stok.T1 = this->F1(ss.str());


			stok.T2 = new int[MATRIX];
			vector<int> v;
			int mid = MATRIX >> 1;
			for (auto i = kw_list.begin(); i != kw_list.end(); i++) {
				if (i != t_min) { 
					all += i->first + " "; 
					int idf = static_cast<int>(log10(acc/i->second.cnt));
					stok.T2[pos[i->first]] = idf + NUM_S1;
					v.emplace_back(pos[i->first]);
					stok.T2[pos[i->first] + mid] = NUM_S2;
					v.emplace_back(pos[i->first] + mid);
				}
			}
			x_M(stok.T2, v);

			all.pop_back();


			int len = in_temp.size()-1;
			stok.T3 = len * (NUM_S1 + NUM_S2);
		}

		else if (kw_list.size() == 1) {

			stringstream ss;
			ss << kw_list[0].first << kw_list[0].second.cnt << kw_list[0].second.ver;
			stok.T1 = this->F1(ss.str());
			all = kw_list[0].first;
		}



		cout << "search keywords:\t" << all;
		cout << endl;

		return stok;
	}



	stoken gen_stok_ff(const string& in, string& all) {

		vector<pair<string, key_info>> kw_list;
		stringstream in_ss(in);
		set<string> in_temp;
		string input;
		while (in_ss >> input) {
			in_temp.emplace(input);
		}


		for (auto& i : in_temp) {
			key_info infotemp = this->kwdict_retrival(i);
			if (infotemp.cnt == 0) {
				cerr << "Error: keyword " + i + " not found in local kw_dict!\n Search founcation return!\n";

				return stoken(); 
			}
			kw_list.emplace_back(i, infotemp);
		}

		all = "";
		stoken stok;


		if (kw_list.size() > 1) {

			auto t_min = kw_list.begin();

			for (auto t1 = kw_list.begin() + 1; t1 != kw_list.end(); t1++) {
				if (t1->second.cnt < t_min->second.cnt) {
					t_min = t1;
				}
			}

			all += t_min->first + " ";


			stringstream ss;
			ss << t_min->first << t_min->second.cnt << t_min->second.ver;
			stok.T1 = this->F1(ss.str());


			stok.T2 = new int[MATRIX];
			vector<int> v;
			int mid = MATRIX >> 1;
			for (auto i = kw_list.begin(); i != kw_list.end(); i++) {
				if (i != t_min) {
					all += i->first + " ";
					int idf = static_cast<int>(log10(acc / i->second.cnt));
					stok.T2[pos[i->first]] = idf + NUM_S1;
					v.emplace_back(pos[i->first]);
					stok.T2[pos[i->first] + mid] = NUM_S2;
					v.emplace_back(pos[i->first] + mid);
				}
			}
			x_M(stok.T2, v);

			all.pop_back();


			int len = in_temp.size() - 1;
			stok.T3 = len * (NUM_S1 + NUM_S2);
		}

		if (kw_list.size() == 1) {

			stringstream ss;
			ss << kw_list[0].first << kw_list[0].second.cnt << kw_list[0].second.ver;
			stok.T1 = this->F1(ss.str());
			all = kw_list[0].first;
		}



		return stok;
	}

};
