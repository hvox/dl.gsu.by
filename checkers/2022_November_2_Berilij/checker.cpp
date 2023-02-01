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
	string user_ans, correct_ans;
	if (not(correct_out >> correct_ans)) { save_error("FAILED TO READ JURY ANSWER!"); return; }
	if (not(user_out >> user_ans)) { save_error("Failed to read user aswer"); return; }
	if (correct_ans == "DA" && user_ans == "NE") { save_error("User failed to find answer, when jury have found it"); return; }
	if (correct_ans == "NE" && user_ans == "DA") { save_error("User found answer, but jury have not"); return; }
	if (user_ans != "NE" && user_ans != "DA") { save_error("Incorrect output. First line should be \"DA\" or \"NE\""); return; }
	long double user_radius, correct_radius;
	if (correct_ans == "DA") {
		while (correct_out >> correct_radius) {
			if (not(user_out >> user_radius)) { save_error("Failed to read one of the radiuses"); return; }
			long double absolute_error = abs(user_radius - correct_radius);
			if (abs(user_radius - correct_radius) <= 0.0001) continue;
			if (abs((user_radius - correct_radius) / correct_radius) <= 0.0001) continue;
			save_error("Incorrect radius");
			return;
		}
	}
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
