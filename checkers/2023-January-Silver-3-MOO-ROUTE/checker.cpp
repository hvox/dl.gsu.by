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
	if (not(task_inp >> n)) { save_error("CHECKER ERROR: failed to read task input!"); return; }
	vector<int> arr(n + 1, 0);
	for (int i = 0; i < n; i++)
		if (not(task_inp >> arr[i])) { save_error("CHECKER ERROR: failed to read task input!"); return; }
	string user_ans, jury_ans;
	if (not(correct_out >> jury_ans)) { save_error("CHECKER ERRORσ Failed to read jury answer"); return; }
	int jury_score = 0;
	char last_dir = 'R';
	for (char ch: jury_ans) {
		if (ch != last_dir) jury_score++;
		last_dir = ch;
	}

	if (not(user_out >> user_ans)) { save_error("Failed to read user answer"); return; }
	int x = 0;
	last_dir = 'R';
	int score = 0;
	for (char ch: user_ans) {
		if (ch == 'R') {
			arr[x]--;
			x++;
			if (x > n) { save_error("out of boundaries: too much R"); return; }
		} else {
			x--;
			if (x < 0) { save_error("out of boundaries: too much L"); return; }
			arr[x]--;
		}
		if (ch != last_dir) score++;
		last_dir = ch;
	}
	if (score > jury_score) {save_error("Jury has found better solution"); return;}
	for (int i = 0; i < n; i++)
		if (arr[i] != 0)
	 {save_error("Wrong number of visits in some points"); return;}
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
