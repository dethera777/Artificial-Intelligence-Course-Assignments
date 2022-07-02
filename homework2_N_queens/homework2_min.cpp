#include<iostream>
#include<algorithm>
#include<vector>
#include<ctime>
#include<chrono>
using namespace std;

// min initialization

class board {
private:
	int board_size;
	int to_add; // used to compute main diagonal indices
	vector<int> q;
	vector<int> r;
	vector<int> main_diag;
	vector<int> sec_diag;

public:
	board(int _board_size)
	{
		board_size = _board_size;
		to_add = board_size - 1;
		q.resize(board_size, 0);
		r.resize(board_size, 0);
		main_diag.resize(2 * board_size - 1, 0);
		sec_diag.resize(2 * board_size - 1, 0);
	};

private:
	// clears the arrays setting all element to 0
	void clear_arrays() {
		for (int i = 0; i < board_size; ++i) {
			q[i] = 0;
			r[i] = 0;
			main_diag[i] = 0;
			sec_diag[i] = 0;
		}
		for (int i = board_size; i < 2 * board_size - 1; ++i) {
			main_diag[i] = 0;
			sec_diag[i] = 0;
		}
	}
	// this initialization puts the queens on the board in such a way that the number of conflicts between the queens is minimized initially.
	void initialize() {
		for (int i = 0; i < board_size; ++i) { // iterate over columns
			int min_conf_row = min_conflict_position(i, false); // find the row with minimum number of conflicts in column i and put the queen there
			move_queen_and_update(min_conf_row, i, false); // move the queen and update the arrays r, main_diag and sec_diag
		}
	}

	pair<int, int> max_conflict_queen() { // pair.first == max number of conflicts, pair.second == column of the queen wth most conflicts
		vector<int>rand_queen_col;
		int max_val = -1;

		for (int i = 0; i < board_size; ++i) {
			int current = r[q[i]] + main_diag[i - q[i] + to_add] + sec_diag[i + q[i]] - 3; // we subtract 3 because we count that queen 3 times

			if (current > max_val) {
				rand_queen_col.clear();
				rand_queen_col.push_back(i); // we have found a queen with more conflicts than all other queens examined up to this point
				max_val = current;
			}
			else if (current == max_val) {
				rand_queen_col.push_back(i); // add the column, all queens with max number of conflicts up to this point are kept in rand_queen vector
			}

			// current < max_val -> do noting, because we have already found queens with more conflits, so skip the current queen
		}
		return { max_val, rand_queen_col[rand() % rand_queen_col.size()] }; // returns the maximum number of conflicts and the column of a random queen with maximum number of conflicts
	}

	int min_conflict_position(int max_conflict_queen_col, bool initialized) { // the input to this function is the second return value of the function above
		vector<int> rand_min_conf_pos;
		int min_val = INT_MAX;

		for (int i = 0; i < board_size; ++i) { // we iterate over rows here in the column max_conflict_queen_col
			int conflicts_and_queen = r[i] + main_diag[max_conflict_queen_col - i + to_add] + sec_diag[max_conflict_queen_col + i];

			// if i == q[max_conflict_queen_col] then we are at the row where the queen is at the moment.
			// In order to find the number of conflicts for this queen we need to subtract 3 because we count this queen
			// once in the row and once per each diagonal.
			// id i != q[max_conflict_queen_col] then we are not at the row where the queen is at the moment.
			// In order to find the number of conflicts for tis queen we need not to subtract anything, because at this position there is no queen and it is not counted in row array
			// and in the two diagonal arrays

			int current;

			if (initialized) {
				current = i != q[max_conflict_queen_col] ? current = conflicts_and_queen : current = conflicts_and_queen - 3;
			}
			else {
				current = conflicts_and_queen; // if the board is not initialized we look for a row where to put the queen in the current column and we are sure that in this column there is no queen placed, so we don't subtract anything.
			}

			if (current < min_val) {
				rand_min_conf_pos.clear();
				rand_min_conf_pos.push_back(i);
				min_val = current;
			}

			else if (current == min_val) {
				rand_min_conf_pos.push_back(i);
			}
		}

		return rand_min_conf_pos[rand() % rand_min_conf_pos.size()]; // return a random row with minimum number of conflicts where the queen can be placed
	}

	void move_queen_and_update(int min_conf_row, int max_conf_col, bool initialized) { // the first parameter is the result of the function above and the second one is the second return value of max_conflict_queen

		if (initialized) {
			r[q[max_conf_col]] -= 1; // remove the queen from the row
			main_diag[max_conf_col - q[max_conf_col] + to_add] -= 1; // remove it from the primary diagonal
			sec_diag[max_conf_col + q[max_conf_col]] -= 1; // remove it from the secondary diagonal
		}

		r[min_conf_row] += 1; // we add it to the new row
		main_diag[max_conf_col - min_conf_row + to_add] += 1; // add it to the main diagonal count
		sec_diag[max_conf_col + min_conf_row] += 1; // add it to the secondary diagonal count

		q[max_conf_col] = min_conf_row; // move the queen
	}
	void print_solution() {
		vector<pair<int, int>> coordinates(board_size); // make its size equal to board_size
		for (int i = 0; i < board_size; ++i) {
			coordinates[i].first = q[i]; // row
			coordinates[i].second = i; // col
		}

		sort(coordinates.begin(), coordinates.end(), [](pair<int, int> a, pair<int, int> b) {return a.first < b.first; });

		int k = 0;
		for (int i = 0; i < board_size; ++i) {
			for (int j = 0; j < board_size; ++j) {
				if (k < board_size && i == coordinates[k].first && j == coordinates[k].second) {
					cout << "* ";
					++k;
				}
				else cout << "- ";
			}
			cout << '\n';
		}
		cout << '\n';
	}

public:
	void solve() { // solves the problem

		auto start = chrono::high_resolution_clock::now();

		while (true) {

			initialize();
			int iter = 0;
			bool solution_found = false;

			while (iter < 7 * board_size) {
				pair<int, int> max_conf_queen = max_conflict_queen();
				if (max_conf_queen.first == 0) {
					solution_found = true;
					break;
				}

				int min_conf_row = min_conflict_position(max_conf_queen.second, true);

				move_queen_and_update(min_conf_row, max_conf_queen.second, true);

				++iter;
			}

			if (solution_found) {
				break;
			}
			else {
				clear_arrays();
				// cout << "Restarting...\n";
			}
		}

		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end - start);
		cout << "Execution time: " << time_span.count() << " seconds.\n";

		if (board_size <= 100) print_solution();
	}
};

int main() {

	int board_size_val;
	cout << "Enter number of queens:\n";
	cin >> board_size_val;

	if (board_size_val == 2 || board_size_val == 3) {
		cout << "These configurations are unsolvable\n";
		return 1;
	}
	else if (board_size_val < 1) {
		cout << "The board must have positive size\n";
		return 1;
	}

	board b(board_size_val);
	b.solve();

	return 0;
}