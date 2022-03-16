#include <bits/stdc++.h>
#pragma GCC optimize("-Ofast")
using namespace std;

const bool debug_mode = false;

const int64_t min_int64 = 0x8000000000000000;

#define ASSERT(code) { if (not(code)) return false; }
#define IF(code, verdict) { if (code) { save_result(verdict, 0); return; } }
#define IFNOT(code, verdict) { if (not(code)) { save_result(verdict, 0); return; } }

ofstream result_file;
int max_points = -1;

void save_result(string verdict, int points=0) {
	result_file << points << endl;
	result_file << verdict << endl;
}

template <typename T>
bool in_range(T x, T l, T r) {
	return (x >= l) and (x <= r);
}

template <typename T>
bool read_array(ifstream &input, int n, vector<T> &array) {
	array.resize(n);
	for (int i = 0; i < n; i++) ASSERT(input >> array[i]);
	return true;
}

bool read_obstacles(ifstream &input, int n, vector<tuple<int, int, char>> &array) {
	array.resize(n);
	for (int i = 0; i < n; i++) {
		int x, y;
		ASSERT(input >> x >> y);
		char orientation;
		ASSERT(input >> orientation);
		array[i] = {x, y, orientation};
	}
	return true;
}

template <typename T>
bool read_set(ifstream &input, int n, set<T> &elements) {
	for (int i = 0; i < n; i++) {
		T x;
		ASSERT(input >> x);
		elements.insert(x);
	}
	return true;
}

template <typename T>
bool has_distinct_elements(vector<T> &array) {
	set<T> elements;
	for (T a : array) elements.insert(a);
	return elements.size() == array.size();
}

int number_of_ones_in_binary_representation(uint64_t number) {
	int ones = 0;
	for (int i = 0; i < 64; i++) {
		ones += number % 2;
		number /= 2;
	}
	return ones;
}

template <typename any>
bool has_period(vector<any> &array, any T) {
	if (array.size() % T != 0) return false;
	for (int i = T; i < array.size(); i += T)
		for (int j = 0; j < T; j++)
			if (array[i + j] != array[i - T]) return false;
	return true;
}

template <typename T>
bool is_periodic(vector<T> &array) {
	for (int k = 2; k < array.size() / 2; k++)
		if (has_period(array, k)) return true;
	return false;
}

template <typename T>
set<T> arr2set(vector<T> &array) {
	set<T> result;
	for (T item : array) result.insert(item);
	return result;
}

bool is_correct_coloring(vector<int> colors, vector<tuple<int, int, char>> obstacles) {
	set<tuple<int, int, int>> all_visited;
	map<tuple<int, int, int>, int> obstacles_to_colors;
	for (int i = 0; i < colors.size(); i++)
		obstacles_to_colors[obstacles[i]] = colors[i] - 1;
	map<int, set<tuple<int, char>>> rows;
	map<int, set<tuple<int, char>>> cols;
	for (auto [x, y, type] : obstacles) {
		rows[y].insert({x, type});
		cols[x].insert({y, type});
	}
	for (auto [x_start, y_start, type_start] : obstacles) {
		for (int direction0 = 0; direction0 < 4; direction0++) {
			set<tuple<int, int, int>> visited;
			array<int, 4> counter = {0, 0, 0, 0};
			int direction = direction0;
			int x0 = x_start;
			int y0 = y_start;
			char type = type_start;
			while (visited.find({x0, y0, direction}) == visited.end()) {
				if (all_visited.find({x0, y0, direction}) != all_visited.end()) {
					counter[0] = counter[1] = counter[2] = counter[3] = 420;
					break;
				}
				if (debug_mode) cout << " - " << x0 << " " << y0 << " " << direction << endl;
				visited.insert({x0, y0, direction});
				all_visited.insert({x0, y0, direction});
				counter[obstacles_to_colors[{x0, y0, type}]]++;
				int target_found = false;
				int target_x, target_y, target_type;
				// directions:
				// 0 right
				// 1 up
				// 2 left
				// 3 down
				switch(direction) {
					case 0:
						for (auto [x, type] : rows[y0]) {
							int y = y0; if (x <= x0) continue;
							if (not target_found or (x < target_x)) {
								target_found = true;
								target_x = x; target_y = y; target_type = type;
							}
						}
						break;
					case 1:
						for (auto [y, type] : cols[x0]) {
							int x = x0; if (y <= y0) continue;
							if (not target_found or (y < target_y)) {
								target_found = true;
								target_x = x; target_y = y; target_type = type;
							}
						}
						break;
					case 2:
						for (auto [x, type] : rows[y0]) {
							int y = y0; if (x >= x0) continue;
							if (not target_found or (x > target_x)) {
								target_found = true;
								target_x = x; target_y = y; target_type = type;
							}
						}
						break;
					case 3:
						for (auto [y, type] : cols[x0]) {
							int x = x0; if (y >= y0) continue;
							if (not target_found or (y > target_y)) {
								target_found = true;
								target_x = x; target_y = y; target_type = type;
							}
						}
						break;
				}
				if (not target_found) {
					for (int i = 0; i < 4; i++) counter[i] = 0;
					break;
				}
				switch (direction) {
					case 0: // right
						direction = (target_type == '/') ? 1 : 3;
						break;
					case 1: // up
						direction = (target_type == '/') ? 0 : 2;
						break;
					case 2: // left
						direction = (target_type == '/') ? 3 : 1;
						break;
					case 3: // down
						direction = (target_type == '/') ? 2 : 0;
						break;
				}
				if (debug_mode) cout << "next direction: " << direction << endl;
				x0 = target_x;
				y0 = target_y;
				type = target_type;
			}
			if (x0 != x_start or y0 != y_start or type != type_start)
				continue;
			for (int i = 0; i < 4; i++) if (debug_mode) cout << " " << counter[i]; if (debug_mode) cout << endl;
			if (counter[0] == counter[1] && counter[0] == counter[2] && counter[0] == counter[3])
				continue;
			if (debug_mode) cout << "false" << endl;
			return false;
		}
	}
	return true;
}

void checker(ifstream &task_inp, ifstream &correct_out, ifstream &user_out) {
	int n;
	IFNOT(task_inp >> n, "o_O");
	vector<tuple<int, int, char>> obstacles;
	IFNOT(read_obstacles(task_inp, n, obstacles), "O_o");
	
	int jury_ans;
	IFNOT(correct_out >> jury_ans, "0_0 Failed to read jury answer.");
	jury_ans = (jury_ans != -1) ? true : false;

	vector<int> colors(n);
	IFNOT(user_out >> colors[0], "Failed to read user output.");
	IF(jury_ans and (colors[0] == -1), "Jury found the coloring and the user did not.");
	IF(not jury_ans and (colors[0] != -1), "The coloring does not satisfy the condition.");
	for (int i = 1; i < n; i++) IFNOT(user_out >> colors[i], "Failed to read user output.");

	IFNOT(is_correct_coloring(colors, obstacles), "The coloring does not satisfy the condition");
	save_result("OK", max_points);
}

int main(int argc, char *argv[]) {
	ifstream task_inp(argv[1]);
	ifstream correct_out(argv[2]);
	ifstream user_out(argv[3]);
	result_file.open("$result$.txt");
	max_points = stoi(argv[4]);
	checker(task_inp, correct_out, user_out);
}
