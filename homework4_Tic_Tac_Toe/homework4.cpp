#include<iostream>
#include<algorithm>
using namespace std;

struct state {
	char board[3][3];
	short num_empty_fields;
};

// the player is asked whether he wants to start the game and the number 1 or 2 is returned by the function
int choose_turn() {
	int turn;

	do {
		cout << "1 or 2\n";
		cin >> turn;
	} while (turn != 1 && turn != 2); // makes sure that the player enters 1 or 2 as input

	return turn;
}

// the player is asked to choose a sign to play with and then the sign is returned
char choose_sign() {
	char sign;

	do {
		cout << "X or O\n";
		cin >> sign;
	} while (sign != 'X' && sign != 'O'); // makes sure that the player enters a correct sign

	return sign;
}

// initialize the board; the board matrix can initially contain garbage values, so intialize it to definite values
void initialize_board(state& st) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			st.board[i][j] = ' ';
		}
	}
	st.num_empty_fields = 9;
}

// we should validate player move because it can be invalid -> out of the board or on a field that is filled already
bool validate_player_move(const state& st, int row, int col) {
	if (row < 0 || row > 2 || col < 0 || col > 2) return false; // the move is out of the board
	if (st.board[row][col] != ' ') return false; // this field is already filled

	return true; // means the move is valid
}

pair<int, int> player_makes_move(state& st, char player_sign) {
	int row, col;

	do {
		cout << "Enter a position:\n";
		cin >> row >> col;
	} while (!validate_player_move(st, row, col));

	st.board[row][col] = player_sign; // the player makes a move
	st.num_empty_fields -= 1; // decrease the number of empty positions

	return { row, col }; // return the position that the player chose
}

void print_board(const state& st) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			cout << '|' << st.board[i][j] << '|';
		}
		cout << '\n';
	}
	cout << '\n';
}

pair <bool, char> terminal_test(const state& st, int row, int col) {
	if (row == -1 || col == -1) return { false, '-' }; // if it is called with incorrect data, then a check is not necessary (or has been done)

	if ((st.board[row][0] == st.board[row][1]) && (st.board[row][1] == st.board[row][2])) return { true, st.board[row][0] }; // row win
	if ((st.board[0][col] == st.board[1][col]) && (st.board[1][col] == st.board[2][col])) return { true, st.board[0][col] }; // col win

	// diagonal win
	if (row == col) {
		if ((st.board[0][0] == st.board[1][1]) && (st.board[1][1] == st.board[2][2])) return { true, st.board[0][0] };
	}
	if (row + col == 2) {
		if ((st.board[2][0] == st.board[1][1]) && (st.board[1][1] == st.board[0][2])) return { true, st.board[0][2] };
	}

	// means the state is a draw, because all the fields are filled
	if (st.num_empty_fields == 0) {
		return { true, '-' };
	}

	// there are empty fields and no one has won, so this is not a terminal state
	return { false, '-' };
}

// utility is called on a terminal state
int utility(int depth, char sign, char algorithm_sign) {
	if (sign == '-') { // means all the fields are filled
		return 0; // it is a draw
	}
	else if (sign == algorithm_sign) { // our algorithm wins
		return 50 - depth; // makes it optimal
	}
	else { // our algorithm loses
		return -(50 - depth);
	}
}

int min_value(state, int, int, int, char, char, int, int); // forward declaration

int max_value(state st, int row, int col, int depth, char algorithm_sign, char player_sign, int alpha, int beta) {
	pair<bool, char> term_test = terminal_test(st, row, col);
	if (term_test.first) return utility(depth, term_test.second, algorithm_sign);

	int v = INT_MIN; // worst value

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (st.board[i][j] == ' ') {
				st.board[i][j] = algorithm_sign; // algorithm is maximizer
				st.num_empty_fields -= 1;

				v = max(v, min_value(st, i, j, depth + 1, algorithm_sign, player_sign, alpha, beta));
				alpha = max(alpha, v);

				st.board[i][j] = ' ';
				st.num_empty_fields += 1;

				if (alpha >= beta) return v;
			}
		}
	}
	return v;
}

int min_value(state st, int row, int col, int depth, char algorithm_sign, char player_sign, int alpha, int beta) {
	pair<bool, char> term_test = terminal_test(st, row, col);
	if (term_test.first) return utility(depth, term_test.second, algorithm_sign);

	int v = INT_MAX; // worst value

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (st.board[i][j] == ' ') {
				st.board[i][j] = player_sign; // player is minimizer
				st.num_empty_fields -= 1;

				v = min(v, max_value(st, i, j, depth + 1, algorithm_sign, player_sign, alpha, beta));
				beta = min(beta, v);

				st.board[i][j] = ' ';
				st.num_empty_fields += 1;

				if (alpha >= beta) return v;
			}
		}
	}
	return v;
}

// this function initializes recursion and finds the best possible move
pair<int, int> minimax_decision_best_move(state st, char algorithm_sign, char player_sign) {
	int best_value_move = INT_MIN; // worst value
	pair<int, int> best_move = { -1, -1 }; // make it invalid initially

	int alpha = INT_MIN;
	int beta = INT_MAX;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (st.board[i][j] == ' ') {
				st.board[i][j] = algorithm_sign; // algorithm is maximizer
				st.num_empty_fields -= 1;

				int curr_move_val = min_value(st, i, j, 1, algorithm_sign, player_sign, alpha, beta);

				if (curr_move_val > best_value_move) { // choose the best (maximize)
					best_value_move = curr_move_val;
					alpha = max(alpha, best_value_move);

					best_move.first = i;
					best_move.second = j;
				}

				st.board[i][j] = ' ';
				st.num_empty_fields += 1;
			}
		}
	}

	return best_move;
}

void game_over(char winner_sign, char algorithm_sign, char player_sign) {
	if (winner_sign == algorithm_sign) {
		cout << "Game over, algorithm wins.\n";
	}
	else if (winner_sign == player_sign) {
		cout << "Game over, player wins.\n";
	}
	else {
		cout << "Game over, it is a draw.\n";
	}
}

void play() {
	int player_turn = choose_turn();
	char player_sign = choose_sign();
	char algorithm_sign = '-';

	// set the sign of the algorithm here
	if (player_sign == 'X') algorithm_sign = 'O';
	else algorithm_sign = 'X';

	state board;
	initialize_board(board); // creates an empty board

	pair<int, int> coordinates_of_player_move = { -1,-1 };

	while (true) {
		if (player_turn == 1) {
			coordinates_of_player_move = player_makes_move(board, player_sign);
			print_board(board);

			pair<int, int> term_test = terminal_test(board, coordinates_of_player_move.first, coordinates_of_player_move.second);
			if (term_test.first) {
				game_over(term_test.second, algorithm_sign, player_sign);
				return;
			}
		}

		pair<int, int> alg_move = minimax_decision_best_move(board, algorithm_sign, player_sign);
		board.board[alg_move.first][alg_move.second] = algorithm_sign; // the algorithm marks the board
		board.num_empty_fields -= 1; // the algorithm has marked a field, so the empty fields decrease

		print_board(board);

		pair<int, int> term_test = terminal_test(board, alg_move.first, alg_move.second);
		if (term_test.first) {
			game_over(term_test.second, algorithm_sign, player_sign);
			return;
		}

		if (player_turn == 2) {
			coordinates_of_player_move = player_makes_move(board, player_sign);
			print_board(board);

			pair<int, int> term_test = terminal_test(board, coordinates_of_player_move.first, coordinates_of_player_move.second);
			if (term_test.first) {
				game_over(term_test.second, algorithm_sign, player_sign);
				return;
			}
		}
	}
}

int main() {
	play();

	return 0;
}