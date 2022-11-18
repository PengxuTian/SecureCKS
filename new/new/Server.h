#pragma once
#include "sdk.h"


class Server
{
private:


	unordered_map<string, data1> invert;

public:

	size_t get_inv_size() {
		return this->invert.size();
	}

	bool invert_add(const string& kw, data1 &dt) {
		if (!this->invert.count(kw)) {
			invert[kw].prev = dt.prev;
			invert[kw].op = dt.op;
			invert[kw].id = dt.id;
			invert[kw].I = dt.I;
			dt.I = nullptr;
			return true;
		}
		cerr << "error in invert_add():  " + kw + " add failed!" << endl;
		return false;
	}
	

	vector<int> search(stoken& sstok) { 

		vector<int> res;
		map<int, vector<int>> m_res;

		try {

			string inv = sha_256(sstok.T1); 
			auto iter = this->invert.find(inv);

			if(iter == this->invert.end())
				throw runtime_error("inverted index failed found At first time! Please check key value!\n");

			if (sstok.T3 == 0) {

				while (1) {


					sha_256(sstok.T1); 
					Xor(XORLEN);
					res.emplace_back(iter->second.id); 

						if (iter->second.prev != "") {
							inv = sha_256(iter->second.prev);
							iter = this->invert.find(inv);
							if (iter == this->invert.end())
								throw runtime_error("inverted index failed found Latter!\n");
						}
						else
							break;
				}
			}
			else {


				while (1) {

					sha_256(sstok.T1); 

					Xor(XORLEN);


					int t_pd = dot_product(iter->second.I, sstok.T2);



					if (t_pd > sstok.T3) {
						m_res[t_pd].emplace_back(iter->second.id);
					}

						if (iter->second.prev != "") {
							inv = sha_256(iter->second.prev);
							iter = this->invert.find(inv);
							if (iter == this->invert.end())
								throw runtime_error("inverted index failed found Latter!\n");
						}
						else
							break;
				}
			}
			
		}
		catch (runtime_error& e) {
			cerr << e.what();
		}
		catch (...) {
			cerr << "Unknown Error in function invert_search(...)!" << endl;
		}

		if (!m_res.empty()) {
			for (auto& i : m_res) {
				res.insert(res.end(), i.second.begin(), i.second.end());
			}
		}

		return res;
	}

};
