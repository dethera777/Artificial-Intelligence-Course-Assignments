#include<iostream>
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<stack>
#include<chrono>
using namespace std;

template<typename T>
void hash_combine(std::size_t& seed, const T& data)
{
	static std::hash<T> hasher;
	seed ^= hasher(data) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<>
struct hash<vector<vector<short>>>
{
	std::size_t operator()(const vector<vector<short>>& other) const
	{
		std::size_t seed = 0;
		for (const auto& row : other) {
			for (auto element : row) {
				hash_combine(seed, element);
			}
		}

		return seed;
	}
};

int n, I; // n is the size of the board and I is the empty index
unordered_map<vector<vector<short>>, pair<vector<vector<short>>, char>> parent;

struct state {
	vector<vector<short>> configuration;
	pair<short, short> empty_index;
	short manhattan_dist;
	short dist_from_start;
};

state initial_state;
state goal_state;

void create_initial_state() {
	int sq = sqrt(n + 1); // compute number of rows and number of columns
	initial_state.configuration.resize(sq, vector<short>(sq, -1)); // resize the initial state and fill it with invalid values

	for (int i = 0; i < sq; ++i) {
		for (int j = 0; j < sq; ++j) {
			cin >> initial_state.configuration[i][j];
			if (initial_state.configuration[i][j] == 0) {
				initial_state.empty_index.first = i;
				initial_state.empty_index.second = j;
			}
		}
	}
	initial_state.dist_from_start = 0;
}

void create_goal_state() {
	int sq = sqrt(n + 1);
	goal_state.configuration.resize(sq, vector<short>(sq, -1)); // resize the goal state and fill it with invalid values

	int position_of_zero = (I == -1) ? n : I;

	// scale the position_of_zero to two-dimensional coordinates
	goal_state.empty_index.first = position_of_zero / sq;
	goal_state.empty_index.second = position_of_zero % sq;

	int cnt = 1; // this way the goal_state is initialized

	for (int i = 0; i < sq; ++i) {
		for (int j = 0; j < sq; ++j) {
			if (goal_state.empty_index.first == i && goal_state.empty_index.second == j) goal_state.configuration[i][j] = 0;
			else {
				goal_state.configuration[i][j] = cnt;
				++cnt;
			}
		}
	}
}

short manhattan_initial(vector<vector<short>>& current) {
	int sq = sqrt(n + 1); // number of rows and number of columns
	int manhattan_dist = 0;

	// traverse the goal_state, for every element in the goal_state find this element in the current state
	for (int i = 0; i < sq; ++i) {
		for (int j = 0; j < sq; ++j) {

			int x_curr = -1;
			int y_curr = -1;

			if (goal_state.configuration[i][j] != 0) { // no need to look for 0 in the current state
				for (int k = 0; k < sq; ++k) {
					for (int t = 0; t < sq; ++t) {
						if (current[k][t] == goal_state.configuration[i][j]) {
							x_curr = k;
							y_curr = t;
							break;
						}
					}
				}
				manhattan_dist += (abs(x_curr - i) + abs(y_curr - j));
			}
		}
	}

	return manhattan_dist;
}

void set_manhattan_of_initial_state() {
	initial_state.manhattan_dist = manhattan_initial(initial_state.configuration); // computes manhattan distance of the initial_state
}

void set_manhattan_of_goal_state() {
	goal_state.manhattan_dist = 0; // manhattan of goal_state == 0
}

int x_moves[4] = { 0, 0, -1, 1 };
int y_moves[4] = { -1, 1, 0, 0 };

void manhattan_fast(const state& current, state& new_state) {
	int sq = sqrt(n + 1); // number of rows and number of columns
	int value_to_look_for = new_state.configuration[current.empty_index.first][current.empty_index.second];
	int x_idx_in_goal = -1;
	int y_idx_in_goal = -1;

	for (int i = 0; i < sq; ++i) {
		for (int j = 0; j < sq; ++j) {
			if (goal_state.configuration[i][j] == value_to_look_for) {
				x_idx_in_goal = i;
				y_idx_in_goal = j;
				break;
			}
		}
	}

	new_state.manhattan_dist = current.manhattan_dist + (abs(current.empty_index.first - x_idx_in_goal) + abs(current.empty_index.second - y_idx_in_goal))
		- (abs(new_state.empty_index.first - x_idx_in_goal) + abs(new_state.empty_index.second - y_idx_in_goal));
}

bool DLS_rec(const state& current, const int& f_limit, int& next_limit, const int& sq) {
	if (current.manhattan_dist == 0) return true;
	state new_state;

	for (int i = 0; i < 4; ++i) {
		if (current.empty_index.first + x_moves[i] >= 0 && current.empty_index.first + x_moves[i] < sq && current.empty_index.second + y_moves[i] >= 0 && current.empty_index.second + y_moves[i] < sq) {
			new_state = current;

			new_state.empty_index.first = new_state.empty_index.first + x_moves[i];
			new_state.empty_index.second = new_state.empty_index.second + y_moves[i];

			new_state.configuration[new_state.empty_index.first][new_state.empty_index.second] = 0;
			new_state.configuration[current.empty_index.first][current.empty_index.second] = current.configuration[new_state.empty_index.first][new_state.empty_index.second];

			// otherwise, there is a repetition in the current path in the tree
			if (parent.find(new_state.configuration) == parent.end()) {

				char action = 'e'; // means empty action
				if (x_moves[i] == 0 && y_moves[i] == -1) {
					action = 'r'; // means right
				}
				else if (x_moves[i] == 0 && y_moves[i] == 1) {
					action = 'l'; // means left
				}
				else if (x_moves[i] == -1 && y_moves[i] == 0) {
					action = 'd'; // means down
				}
				else if (x_moves[i] == 1 && y_moves[i] == 0) {
					action = 'u'; // means up
				}

				manhattan_fast(current, new_state);
				new_state.dist_from_start = current.dist_from_start + 1;
				int f_value_of_new_state = new_state.dist_from_start + new_state.manhattan_dist;

				if (f_value_of_new_state <= f_limit) {
					parent.insert({ new_state.configuration, {current.configuration, action} });
					if (DLS_rec(new_state, f_limit, next_limit, sq)) return true;
					parent.erase({ new_state.configuration });
				}
				else if (f_value_of_new_state < next_limit) {
					next_limit = f_value_of_new_state;
				}
			}
		}
	}

	return false;
}

bool DLS(int f_limit, int& next_limit) {
	next_limit = INT_MAX;

	if (f_limit == 0) return true;
	int sq = sqrt(n + 1);
	parent.insert({ initial_state.configuration, {initial_state.configuration, 'e'} }); // 'e' means no action

	bool res = DLS_rec(initial_state, f_limit, next_limit, sq);

	parent.erase(initial_state.configuration);

	return res;
}

void IDA_star() {
	int f_limit = initial_state.manhattan_dist; // the first limit is the manhattan of the initial state, which is already calculated (dist from start == 0), so f_limit == manhattan of initial state
	int next_limit = f_limit;

	do {
		f_limit = next_limit;
	} while (!DLS(f_limit, next_limit));
}

void print_action(char c) {
	switch (c)
	{
	case 'l': cout << "left\n"; break;
	case 'r': cout << "right\n"; break;
	case 'u': cout << "up\n"; break;
	case 'd': cout << "down\n"; break;
	}
}

void restore_optimal_path() {
	stack<char> st;
	int length = 0;

	while (goal_state.configuration != parent[goal_state.configuration].first) {
		st.push(parent[goal_state.configuration].second); // add the action executed in order to reach child from parent
		goal_state.configuration = parent[goal_state.configuration].first;
		++length;
	}

	--length;

	cout << "\nThe length of the shortest path is " << length << " moves" << '\n';
	cout << "Steps of the optimal solution:\n\n";

	while (!st.empty()) {
		print_action(st.top());
		st.pop();
	}
}

int main() {

	cout << "Enter board size and empty position:\n";
	cin >> n >> I;
	int sq = sqrt(n + 1);

	create_initial_state();
	create_goal_state();
	set_manhattan_of_initial_state();
	set_manhattan_of_goal_state();

	auto start = chrono::high_resolution_clock::now();

	IDA_star();
	restore_optimal_path();

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - start);
	cout << "\nSolution found in: " << time_span.count() << " seconds.\n";

	return 0;
}

// Some example test cases below

/*
15 0 -> should be 41
13 8 14 3 9 1 0 7 15 5 4 10 12 2 6 11

15 0 -> should be 49
6 14 10 5 15 8 7 1 3 4 2 0 12 9 11 13

15 0 -> should be 53
5 7 11 8 0 14 9 13 10 12 3 15 6 1 4 2

15 0 -> should be 56
14 13 4 11 15 8 6 9 0 7 3 1 2 10 12 5

15 0 -> should be 46
3 14 9 11 5 4 8 2 13 12 6 7 10 1 15 0

15 0 -> should be 56
4 7 14 13 10 3 9 12 11 5 6 15 1 2 8 0

15 0 -> should be 45
14 1 9 6 4 8 12 5 7 2 3 0 10 11 13 15

15 0 -> should be 52
13 14 6 12 4 5 1 0 9 3 10 2 15 11 8 7

15 0 -> should be 50
12 8 15 13 1 0 5 4 6 3 2 11 9 7 14 10

15 0 -> should be 49
8 11 4 6 7 3 10 9 2 12 15 13 0 1 5 14

15 0 -> should be 42
0 1 9 7 11 13 5 3 14 12 4 2 8 6 10 15
*/