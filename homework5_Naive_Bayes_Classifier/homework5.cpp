#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<cmath>
using namespace std;

// number of rows in the data set
const int number_of_samples = 435;

// it is used for 'y','n' and '?'
// "yes", "no" and "q" will be used for counting
struct options {
	int yes = 0;
	int no = 0;
	int q = 0;
};

// this function parses the file
void read_file(string& file_name, vector<vector<char>>& data_arr) {
	ifstream data_file(file_name, ios::in);
	char symbol;

	// 'd' -> democrat
	// 'r' -> republican

	int row_idx = 0;
	int col_idx = 0;
	bool row_first_char_met = false;

	if (data_file.is_open()) {
		while (data_file.get(symbol)) {
			if (symbol == '\n') {
				++row_idx;
				col_idx = 0;
				row_first_char_met = false;
			}
			else if (symbol == ',') {
				continue;
			}
			else {
				if (row_first_char_met == false) {
					data_arr[row_idx][col_idx] = symbol;
					++col_idx;
					row_first_char_met = true;

					while (data_file.get(symbol)) {
						if (symbol == ',') break;
					}

					data_file.get(symbol); // get the char after the comma
				}

				data_arr[row_idx][col_idx] = symbol;
				++col_idx;
			}
		}

		data_file.close();
	}
	else {
		cout << "Error opening the file\n";
	}
}

// this function is used for counting data
vector<vector<options>> create_table(const vector<vector<char>>& data_arr) {
	vector<vector<options>> table(2, vector<options>(17)); // 17 is the number of columns we have

	int rows = data_arr.size();
	int cols = data_arr[0].size();

	// index 0 - democrats; index 1 - republicans

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (data_arr[i][0] == 'd') {
				if (j == 0) {
					table[0][j].yes += 1; // this way I count the democrats, their number is kept on the .yes property
				}
				else {
					if (data_arr[i][j] == 'y') {
						table[0][j].yes += 1; // he is a democrat and has voted yes for feature j
					}
					else if (data_arr[i][j] == 'n') {
						table[0][j].no += 1; // he is a democrat and has voted no for feature j
					}
					else if (data_arr[i][j] == '?') {
						table[0][j].q += 1; // he is a democrat and has not voted for feature j
					}
					else {
						cout << "Some error occurred!\n";
					}
				}
			}
			else if (data_arr[i][0] == 'r') {
				if (j == 0) {
					table[1][j].yes += 1; // this way I count the republicans, their number is kept on the .yes property
				}
				else {
					if (data_arr[i][j] == 'y') {
						table[1][j].yes += 1;
					}
					else if (data_arr[i][j] == 'n') {
						table[1][j].no += 1;
					}
					else if (data_arr[i][j] == '?') {
						table[1][j].q += 1;
					}
					else {
						cout << "Some error occurred\n";
					}
				}
			}
		}
	}
	return table;
}

// this function will modify the data set removing '?' in an appropriate way
// this function should be called after create_table
void modify_data_set(const vector<vector<options>>& table, vector<vector<char>>& data_arr) {
	int num_samples = data_arr.size();
	
	for (int row = 0; row < num_samples; ++row) {
		for (int col = 1; col < 17; ++col) {
			if (data_arr[row][0] == 'd') {
				if (data_arr[row][col] == '?') {
					data_arr[row][col] = (table[0][col].yes >= table[0][col].no) ? 'y' : 'n';
				}
			}
			else if (data_arr[row][0] == 'r') {
				if (data_arr[row][col] == '?') {
					data_arr[row][col] = (table[1][col].yes >= table[1][col].no) ? 'y' : 'n';
				}
			}
		}
	}
}

bool naive_bayes(const vector<vector<options>>& table, const vector<char>& test_case, int a) {
	double democrat = 0.0;
	double republican = 0.0;

	// we have only two classes
	// we start from 1 because on index 1 is the first feature

	int number_of_democrats = table[0][0].yes;
	int number_of_republicans = table[1][0].yes;
	int total_number = number_of_democrats + number_of_republicans;
	int lambda = 1, k = 2; // lambda is always 1 and k is always 2 because the number of classes is constant
	int k_times_lambda = k * lambda;
	double a_times_lambda_democrats = a * lambda + number_of_democrats;
	double a_times_lambda_republicans = a * lambda + number_of_republicans;
	double total_number_plus_k_times_lambda = total_number + k_times_lambda;

	for (int feature = 0; feature < 17; ++feature) {
		if (feature == 0) { // this is used to compute the probability that is not conditional
			democrat += log((number_of_democrats + lambda) / total_number_plus_k_times_lambda);
			republican += log((number_of_republicans + lambda) / total_number_plus_k_times_lambda);
		}
		else {
			if (test_case[feature] == 'y') {
				democrat += log((table[0][feature].yes + lambda) / a_times_lambda_democrats);
				republican += log((table[1][feature].yes + lambda) / a_times_lambda_republicans);
			}
			else if (test_case[feature] == 'n') {
				democrat += log((table[0][feature].no + lambda) / a_times_lambda_democrats);
				republican += log((table[1][feature].no + lambda) / a_times_lambda_republicans);
			}
			else if (test_case[feature] == '?') {
				democrat += log((table[0][feature].q + lambda) / a_times_lambda_democrats);
				republican += log((table[1][feature].q + lambda) / a_times_lambda_republicans);
			}
		}
	}

	// these are the cases where the algorithm guesses right
	if (democrat > republican && test_case[0] == 'd') return true;
	if (republican > democrat && test_case[0] == 'r') return true;

	// the algorithm fails in guessing
	return false;
}

// performs all the tests in the validation set and computes the accuracy rate of the algorithm
double naive_bayes_all(const vector<vector<options>>& table, const vector<vector<char>>& validation_set, int a) {
	int total_number_of_tests = validation_set.size();
	int count_right_answers = 0;

	// execute the tests one by one and check if the algorithm returns correct result
	for (int i = 0; i < total_number_of_tests; ++i) {
		if (naive_bayes(table, validation_set[i], a)) ++count_right_answers;
	}

	double result = static_cast<double>(count_right_answers) / total_number_of_tests;

	return result;
}

// preparation: create the model
double naive_bayes_prep(const vector<vector<char>>& training_set, const vector<vector<char>>& validation_set, int a) {
	vector<vector<options>> table = create_table(training_set);
	return naive_bayes_all(table, validation_set, a); // the table is constructed, the model is built and now we are ready for the validation tests
}

double compute_average(const vector<double>& vv) {
	double total = 0.0;

	for (const double& elem : vv) {
		total += elem;
	}

	double res = total / vv.size();
	return res * 100;
}
void print_session_results(const vector<double>& results) {
	int n = results.size();

	for (int i = 0; i < n; ++i) {
		cout << "Result after session " << i + 1 << ':' << ' ' << results[i] * 100 << "% accuracy" << '\n';
	}
	cout << '\n';
}

void solve(vector<vector<char>>& data_set, int a) {
	random_shuffle(data_set.begin(), data_set.end()); // make a random permutation of the whole data set

	int num_samples = data_set.size();
	vector<double> results;

	// The examples in the data set are 435; the size of each of the first 5 validation sets will be 44 elements, and the size of the last 5 validation sets will be 43 elements

	int start_idx = 0;
	int end_idx = 43;

	// 10 training sessions and 10 validation sessions
	for (int session = 0; session < 10; ++session) {

		vector<vector<char>> training_set;
		vector<vector<char>> validation_set;

		for (int i = 0; i < num_samples; ++i) {
			if (i >= start_idx && i <= end_idx) { // if we are in the range of elements that should be in the validation set, add them to the validation set
				validation_set.push_back(data_set[i]);
			}
			else {
				training_set.push_back(data_set[i]); // the remaining elements are part of the training set
			}
		}
		// after this loop the training set and the validation set are constructed

		double session_result = naive_bayes_prep(training_set, validation_set, a); // get the result from this session
		results.push_back(session_result);

		if (session < 4) {
			start_idx = end_idx + 1;
			end_idx = start_idx + 43;
		}
		else {
			start_idx = end_idx + 1;
			end_idx = start_idx + 42;
		}
	}

	print_session_results(results);
	cout << "The average result after 10 training and validation sessions is: " << compute_average(results) << '%' << '\n';
}

void solution_with_original_values(vector<vector<char>>& data_arr) {
	solve(data_arr, 3); // 3 possible values here: 'y', 'n' and '?'
}

void solution_with_only_yes_no(vector<vector<char>>& data_arr) {
	vector<vector<options>> table = create_table(data_arr);
	modify_data_set(table, data_arr); // use the table to fill the missing values
	solve(data_arr, 2); // 2 possible values here: 'y' and 'n'
}

int main() {
	string file_name = "house-votes-84.data";
	vector<vector<char>> data_arr(number_of_samples, vector<char>(17));
	read_file(file_name, data_arr);
	solution_with_original_values(data_arr);
	// solution_with_only_yes_no(data_arr);
	return 0;
}