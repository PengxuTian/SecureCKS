// Scheme.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#pragma once
#include "sdk.h"
#include "Client.h"
#include "Server.h"

using namespace std;




#define timeparamsSet() chrono::high_resolution_clock::time_point t_1, t_2;
#define start() t_1 = chrono::high_resolution_clock::now();
#define end() t_2 = chrono::high_resolution_clock::now();
#define cur_runtime static_cast<double>(chrono::duration_cast<chrono::microseconds>(t_2 - t_1).count() / 1e6)  //微秒
#define prints(msg) cout << msg << " time = " << cur_runtime << " seconds\t";
#define print(msg) cout << msg << " time = " << cur_runtime << " seconds" << endl;


#define reset_cin() cin.clear(); cin.sync();

  
#define SUBCNT 500 //pairs



void create(const string& fpath, const string& tfpath, Client& c, Server& s) {
	fstream f(fpath, ios::in);
	fstream tf(fpath, ios::in);
	if (f && tf) {
		fstream info("info.txt", ios::app);
		info << "\n" << fpath << ":" << endl;

		string faw_info("");
		unsigned sub_cnt(SUBCNT), cnt(0);
		string temp_line, temp_line2;
		unsigned num(0); 

		double inv_time(0.0), fwa_time(0.0);



		while (getline(f, temp_line) && getline(tf, temp_line2)) {
			num++;
			stringstream line(temp_line);
			stringstream line2(temp_line2);
			int tf_temp;
			string word;
			timeparamsSet();
			try {
				int* TF_cur = new int[MATRIX] {};
				int tf_i = 0;
				vector<int> tf_flag;
				while (line2 >> tf_temp && tf_i < MATRIX) {
					start();
					if (tf_temp) {
						TF_cur[tf_i] = tf_temp;
						tf_flag.emplace_back(tf_temp);
					}

					tf_i++;
					end();
					inv_time += cur_runtime;   
				}

				start();
				M_x(TF_cur, tf_flag); 
				end();
				inv_time += cur_runtime;


				while (line >> word) {
					start();
					invert_dict temp_inv = c.gen_inv(word, num, TF_cur);
					end();
					inv_time += cur_runtime;


					cnt++;
					if (cnt >= sub_cnt) {
						cout << cnt << " pairs creat time(inv) = " << inv_time << " s" << endl;
						info << cnt << " pairs creat time(inv) = " << inv_time << " s" << endl;
						sub_cnt += SUBCNT;
					}

					if (!s.invert_add(temp_inv.kw, temp_inv.value)) //if failed， output the num
						cerr << "error line num in " + fpath + " : " << num << endl;
				}

				acc++;
			}
			catch (exception& e) {
				cerr << e.what();
				cerr << "Insufficient memory! Program abort!\n";
				abort();
			}
		}
			
		cout << "EDB created by " << num << " files, and total number of pairs is: " << s.get_inv_size() << "\n";
		cout << "the total time cost of invert index creation:\t" << inv_time << " s\n";

		info << "EDB created by " << num << " files, and total number of pairs is: " << s.get_inv_size() << "\n";
		info << "the total time cost of invert index creation:\t" << inv_time << " s\n";
	}
	else
		cerr << "file " + fpath + " or " + tfpath + " open failed!\n";
}


bool com_list() {
	cout << "- - - - - - - - - - - - - - - - -\n";
	cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n";
	cout << "- - - - - - - - - - - - - - - - -\n";
	cout << "command list:\n";
	cout << "1. search keywords\n";
	cout << "2. random 2-dimension search from document\n";
	cout << "3. random 3-dimension search from document\n";
	cout << "4. search 2-dim from document pointed\n";
	cout << "5. search 3-dim from document pointed\n";
	cout << "6. single search test from document pointed\n";
	cout << "0. clear screen / save and exit\n";
	cout << "Please input integer number 0-6:" << endl;
	return true;
}


string get_curtime() {
	char cur_t[100];
	time_t tt = time(nullptr);
	tm lt;
	localtime_s(&lt, &tt);
	if (strftime(cur_t, sizeof(cur_t), "%Y-%m-%d %H:%M:%S", &lt) == 0)
		return  "error in get_time()!";
	else
		return cur_t;
}

int main()
{
	init_M(M1);
	init_M(M2);

	Client c;
	c.pos_initialize("pos.txt"); 

	Server s;

	cout << "start creat EDB..." << endl;

	timeparamsSet(); 

	create("test.txt", "tf.txt", c, s); 

	cout << "EDB creat finished! " << endl;



	    
	while (true) {
		unsigned com;

		while (com_list() && (cin >> com).good()) {
			if (com == 0) {
				cin.get();
				cout << "Sure to exit()? y/n" << endl;
				string buf;
				getline(cin, buf);
				if (buf[0] == 'y' || buf[0] == 'Y') {
					exit(0);
				}
				system("cls");
				reset_cin();
				continue;
			}
			else if (com > 10) {
				cin.get();
				cout << "Please input integer number 0-10!" << endl;
				cin.get();
				reset_cin();
				continue;
			}


			cout << "input command is:\t" << com << endl;

			string all("");
			vector<int> result;
			int lnum; 

			if (com == 1) {
				fstream sea_inf("search_info.txt", ios::app);
				string t = get_curtime();
				cout << "current time: " << t << endl;
				sea_inf << "\n" << t;
				stoken sstok(c.gen_stok(all));

				sea_inf << "\ncurrent search content ordered is:\t " << all << endl;

				start();
				result = s.search(sstok);
				end(); print("search ");
				sea_inf << "times search process time = " << cur_runtime << " s\n";


				lnum = result.size();
				cout << "search ultimate found files: " << lnum << "\n";
				cout << "the final result files is: \n";
				sea_inf << "search ultimate found files: " << lnum << "\n";
				sea_inf << "the final result files is: \n";
				if (!lnum) {
					cout << "NULL" << endl;
					sea_inf << "NULL" << endl;
					continue;
				}


				for (auto & i : result) 
					cout << i << "\t";				  
				cout << endl;
			}

			//2维随机查询
			if (com == 2) {
				fstream sea_inf("search_info2.txt", ios::app);
				unsigned threshold(0);
				string t = get_curtime();
				cout << t << endl;
				cout << "random 2-dimension search from document:\n" << endl;
				sea_inf << "\n" << t;
				sea_inf << "random 2-dimension search test from document:\n\n";

				//要查询的文件
				string doc = "ran_test.txt";

				for (unsigned i = 0; i < 6; i++) {
					threshold += 1000;

					fstream f(doc, ios::in);
					string line;
					unsigned cnt(0);
					double all_time(0.0); 
					while (cnt < threshold && getline(f, line)) {
						cnt++;
						stoken sstok(c.gen_stok_ff(line, all));
						start();
						result = s.search(sstok);
						end();
						all_time += cur_runtime;
					}
					cout << threshold << " times search process time = " << all_time << " s\n";
					sea_inf << threshold << " times search process time = " << all_time << " s\n";
				}
				cout << endl;
				sea_inf << endl;
			}

			//3维随机查询
			if (com == 3) {
				fstream sea_inf("search_info3.txt", ios::app);
				unsigned threshold(0);
				string t = get_curtime();
				cout << t << endl;
				cout << "random 3-dimension search test from document:\n" << endl;
				sea_inf << "\n" << t;
				sea_inf << "random 3-dimension search test from document:\n\n";

				string doc = "ran_test3.txt";

				for (unsigned i = 0; i < 6; i++) {
					threshold += 1000;


					fstream f(doc, ios::in);
					string line;
					unsigned cnt(0);
					double all_time(0.0); 
					while (cnt < threshold && getline(f, line)) {
						cnt++;

						stoken sstok(c.gen_stok_ff(line, all)); 
						
						start();
						result = s.search(sstok);
						end();
						all_time += cur_runtime;

					}
					cout << threshold << " times search process time = " << all_time << " s\n";
					sea_inf << threshold << " times search process time = " << all_time << " s\n";
				}
				cout << endl;
				sea_inf << endl;
			}

			//2维查询(选择)
			if (com == 4) {
				fstream sea_inf("search_info2.txt", ios::app);


				string doc[2] = { "S_e_2.txt", "S_e_2.txt" };


				for (const auto& d : doc) {

					string t = get_curtime();
					cout << t << endl;
					sea_inf << t << endl;

					cout << "2-dimension test from document " + d << "\n\n";
					sea_inf << "2-dimension search test from document " << d << " :\n\n";


					fstream f(d, ios::in);
					if (!f) {
						cerr << "file open failed!\n";
						continue;
					}
					 
					string line;
					unsigned cnt(0);

					while (getline(f, line)) {
						cnt++;
						stoken sstok(c.gen_stok_ff(line, all)); 

		
						cout << "input: " + all << endl;
						sea_inf << "input: " + all << endl;


						start();
						result = s.search(sstok);
						end();

						cout << "search process time = " << cur_runtime << " s\n";
						sea_inf << "search process time = " << cur_runtime << " s\n";
						cout << endl;
						sea_inf << endl;
					}
					cout << endl;
					sea_inf << endl;
				}

			}

			//3维查询(选择)
			if (com == 5) {
				fstream sea_inf("search_info3.txt", ios::app);


				string doc[2] = { "S_e_3.txt", "S_e_3.txt" };


				for (const auto& d : doc) {


					string t = get_curtime();
					cout << t << endl;
					sea_inf << t << endl;

					cout << "3-dimension test from document " + d << "\n\n";
					sea_inf << "3-dimension search test from document " << d << " :\n\n";

					fstream f(d, ios::in);
					if (!f) {
						cerr << "file open failed!\n";
						continue;
					}

					string line;
					unsigned cnt(0);

					while (getline(f, line)) {
						cnt++;


						stoken sstok(c.gen_stok_ff(line, all)); 



						cout << "input: " + all << endl;
						sea_inf << "input: " + all << endl;


						start();
						result = s.search(sstok);
						end();

						cout << "search process time = " << cur_runtime << " s\n";
						sea_inf << "search process time = " << cur_runtime << " s\n";
						cout << endl;
						sea_inf << endl;
					}
					cout << endl;
					sea_inf << endl;
				}

			}


			if (com == 6) {
				fstream sea_inf("search_info1.txt", ios::app);


				string doc = "S_e_1.txt";;


				string t = get_curtime();
				cout << t << endl;
				sea_inf << t << endl;

				cout << "single search test from document " + doc << "\n\n";
				sea_inf << "single search search test from document " << doc << " :\n\n";

	
				fstream f(doc, ios::in);
				if (!f) {
					cerr << "file open failed!\n";
					continue;
				}

				string line;
				unsigned cnt(0);

				while (getline(f, line)) {
					cnt++;


					stoken sstok(c.gen_stok_ff(line, all)); 

					cout << "input: " + all << endl;
					sea_inf << "input: " + all << endl;


					start();
					result = s.search(sstok);
					end();

					lnum = result.size();
					cout << "search result: " << lnum << endl;    
					sea_inf << "search result: " << lnum << endl;
					cout << "search process time = " << cur_runtime << " s\n";
					sea_inf << "search process time = " << cur_runtime << " s\n";
					cout << endl;
					sea_inf << endl;
				}
				cout << endl;
				sea_inf << endl;
			}


			//////////////

			reset_cin();
			cin.get();
			cout << "Please press any key to continue..." << endl;
			cin.get();


			reset_cin();
		}

		cerr << "Input error! Please input number!" << endl;
		reset_cin();
		cin.get();

		reset_cin();

	}


}



