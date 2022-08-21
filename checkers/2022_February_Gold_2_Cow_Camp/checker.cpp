#include <bits/stdc++.h>

using namespace std;

template<typename T>
bool in_range(T x, T l, T r) { return (x >= l) and (x <= r); }

ofstream result_file;
int max_points;

void save_result(string verdict, int points = -42) {
	if (points == -42) points = max_points;
	result_file << points << endl;
	result_file << verdict << endl;
}

void save_error(string verdict) {
	save_result(verdict, 0);
}

typedef vector<map<int, int>> Graph;

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n;
	long double user_ans, corr_ans;
	if (not(correct_out >> corr_ans)) { save_error("0_0"); return; }
	if (not(user_out >> user_ans)) { save_error("Failed to read user output"); return; }
	if (abs(user_ans - corr_ans) > 0.00001) { save_error("Wrong answer"); return; }
	save_result("OK");
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		cout << "USAGE: " << argv[0] << " input correct_output user_output max_points" << endl;
		return 1;
	}
	ifstream task_inp(argv[1]);
	ifstream correct_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, correct_out, user_out);
	return 0;
}
