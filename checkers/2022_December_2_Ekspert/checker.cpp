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
	int64_t x, y, product;
	if (not(task_inp >> x >> y)) {save_error("FAILED TO READ INPUT"); return;}
	product = x * y;
	int64_t A = x, B = y, C = 0, D = 1;
	int number_of_operations;
	if (not(user_out >> number_of_operations)) { save_error("Failed to read user aswer"); return; }
	if (number_of_operations > 100) { save_error("#operations > 100"); return; }
	for (int i = 0; i < number_of_operations; i++) {
		string R1, R2, R3;
		if (not(user_out >> R1)) { save_error("Failed to read user aswer"); return; }
		if (not(user_out >> R2)) { save_error("Failed to read user aswer"); return; }
		if (not(user_out >> R3)) { save_error("Failed to read user aswer"); return; }
		int64_t sum = 0;
		if (R1 == "A") sum += A; else if (R1 == "B") sum += B; else if (R1 == "C") sum += C; else sum += D;
		if (R2 == "A") sum += A; else if (R2 == "B") sum += B; else if (R2 == "C") sum += C; else sum += D;
		if (R3 == "A") A = sum; else if (R3 == "B") B = sum; else if (R3 == "C") C = sum; else D = sum;
	}
	string R;
	if (not(user_out >> R)) { save_error("Failed to read user aswer"); return; }
	int64_t user_result;
	if (R == "A") user_result = A; else if (R == "B") user_result = B; else if (R == "C") user_result = C; else user_result = D;
	if (user_result != product) { save_error("User result != x * y"); return; }
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
