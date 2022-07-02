#include<iostream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>
#include<fstream>
using namespace std;

const int num_lines = 286;
// index 0 -> "n"; index 1 -> "r"

void read_file(const string& file_name, vector<vector<string>>& data_arr) {
	ifstream data_file(file_name, ios::in);
	char symbol;
	string word;

	int row_idx = 0;

	if (data_file.is_open()) {
		while (data_file.get(symbol)) {
			if (symbol == '\n') {
				data_arr[row_idx].push_back(word);
				word.clear();
				++row_idx;
			}
			else if (symbol == ',') {
				data_arr[row_idx].push_back(word);
				word.clear();
			}
			else {
				word.push_back(symbol);
			}
		}
	}
	else {
		cout << "Error opening the file!/n";
	}
}

// the table is used for counting, which is needed in probability calculation
vector<vector<unordered_map<string, int>>> create_table(const vector<vector<string>>& data_arr) {
	vector<vector<unordered_map<string, int>>> table(2, vector<unordered_map<string, int>>(10));

	const int rows = data_arr.size();
	const int cols = data_arr[0].size();

	// index 0 -> "n" class
	// index 1 -> "r" class

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (data_arr[i][0][0] == 'n') { // class is 'n'
				if (j == 0) {
					if (table[0][0].find("n") == table[0][0].end()) { // no entry for 'n' class yet
						table[0][0].insert({ "n", 1 }); // the current is from class 'n', so start from 1
					}
					else { // an entry for 'n' exists
						table[0][0]["n"] += 1; // found one more
					}
				}
				else {
					if (table[0][j].find(data_arr[i][j]) == table[0][j].end()) {
						table[0][j].insert({ data_arr[i][j], 1 });
					}
					else {
						table[0][j][data_arr[i][j]] += 1;
					}
				}
			}
			else if (data_arr[i][0][0] == 'r') { // class is 'r'
				if (j == 0) {
					if (table[1][0].find("r") == table[1][0].end()) {
						table[1][0].insert({ "r", 1 });
					}
					else {
						table[1][0]["r"] += 1;
					}
				}
				else {
					if (table[1][j].find(data_arr[i][j]) == table[1][j].end()) {
						table[1][j].insert({ data_arr[i][j], 1 });
					}
					else {
						table[1][j][data_arr[i][j]] += 1;
					}
				}
			}
		}
	}

	return table;
}

// entropy of one attribute
double entropy_one_attr(const int first_class_num, const int second_class_num) {
	double total_number = first_class_num + second_class_num;

	double prob_first_class = first_class_num / total_number;
	double prob_second_class = second_class_num / total_number;

	double prob_first = 0;
	if (prob_first_class != 0) {
		prob_first = -prob_first_class * log2(prob_first_class);
	}

	double prob_second = 0;
	if (prob_second_class != 0) {
		prob_second = -prob_second_class * log2(prob_second_class);
	}

	double entropy = prob_first + prob_second;

	return entropy;
}

// entropy of two attributes
double entropy_two_attr(const vector<vector<unordered_map<string, int>>>& table, const int index) {
	double entropy_two = 0.0;

	for (auto it = table[0][index].begin(); it != table[0][index].end(); ++it) {
		for (auto it2 = table[1][index].begin(); it2 != table[1][index].end(); ++it2) {
			if (it->first == it2->first) { // if the string is the same then the same value is present in both unordered maps and the entropy is guaranteed to be != 0
				double prob = static_cast<double>(it->second + it2->second) / (table[0][0].at("n") + table[1][0].at("r"));
				double prob_times_entropy_one = prob * entropy_one_attr(it->second, it2->second);
				entropy_two += prob_times_entropy_one;
			}
		}
	}

	return entropy_two;
}

// returns the best attribute (with maximum information gain)
int best_attribute(vector<vector<unordered_map<string, int>>>& table, const vector<bool>& remaining) {
	const int rem_size = remaining.size();
	int best_idx = -1;
	double maximum_information_gain = -1000000;
	double current_information_gain = -1000000;

	const int num_from_n_class = (table[0][0].find("n") == table[0][0].end()) ? 0 : table[0][0]["n"];
	const int num_from_r_class = (table[1][0].find("r") == table[1][0].end()) ? 0 : table[1][0]["r"];

	const double entropy_of_one_attr = entropy_one_attr(num_from_n_class, num_from_r_class);

	for (int i = 1; i < rem_size; ++i) {
		if (remaining[i] == true) { // means that this attribute is not used in the current path, so we can use it
			current_information_gain = entropy_of_one_attr - entropy_two_attr(table, i);

			if (current_information_gain > maximum_information_gain) {
				maximum_information_gain = current_information_gain;
				best_idx = i;
			}
		}
	}

	return best_idx;
}

// checks if all rows in a data set have the same class
pair<bool, string> all_rows_same_class(vector<vector<unordered_map<string, int>>>& table) {
	int num_from_first_class = (table[0][0].find("n") == table[0][0].end()) ? 0 : table[0][0]["n"];
	int num_from_second_class = (table[1][0].find("r") == table[1][0].end()) ? 0 : table[1][0]["r"];

	if (num_from_first_class != 0 && num_from_second_class == 0) {
		return { true, "n" }; // it is a leaf node and all instances are from "n" class
	}
	else if (num_from_first_class == 0 && num_from_second_class != 0) {
		return { true, "r" }; // it is a leaf node and all instances are from "r" class
	}

	return { false, "-" }; // it is not a leaf node
}

// sorts the data set by an attribute
void sort_by_attribute(vector<vector<string>>& data_set, const int attr_idx) {
	sort(data_set.begin(), data_set.end(),
		[attr_idx](const vector<string>& lhs, const vector<string>& rhs) {
			return lhs[attr_idx] > rhs[attr_idx];
		});
}

// returns the number of diferent values for an attribute in the two classes combined
int number_of_diff_val_of_attr(const vector<vector<unordered_map<string, int>>>& table, const int attr_idx) {
	unordered_set<string> us;
	
	for (int i = 0; i < 2; ++i) {
		for (auto it = table[i][attr_idx].begin(); it != table[i][attr_idx].end(); ++it) {
			us.insert(it->first);
		}
	}

	return us.size(); // return the number of different values in this column
}

// returns the prevailing class in a data set
string prevailing_class(vector<vector<unordered_map<string, int>>>& table) {
	int from_class_n = (table[0][0].find("n") == table[0][0].end()) ? 0 : table[0][0]["n"];
	int from_class_r = (table[1][0].find("r") == table[1][0].end()) ? 0 : table[1][0]["r"];


	if (from_class_n >= from_class_r) return "n";
	else return "r";
}

struct node {
	int node_label; // index from 1 to 9 (10 and 11)
	string edge_label; // the name of the edge that led to this node
	vector<bool> remaining; // index 0 is not used; if true then this index (attribute) is not used in the path
	node* parent; // pointer to the parent
	vector<vector<string>>data_set; // the data set of that node
	int rows_in_data_set = 0; // num rows in the data set
	vector<vector<unordered_map<string, int>>> table; // the table with the counts for the node
	vector<node*> children; // keeps all of the children of the node
};

void create_tree_rec(node*& root, const int k) {
	pair<bool, string> all_records_same_class = all_rows_same_class(root->table);
	if (all_records_same_class.first) {
		if (all_records_same_class.second == "n") {
			root->node_label = 10; // 10 means a leaf of class "n"
		}
		else if (all_records_same_class.second == "r") {
			root->node_label = 11; // 11 means a leaf of class "r"
		}

		for (int i = 0; i < root->children.size(); ++i) {
			delete root->children[i];
			root->children[i] = nullptr;
		}

		return;
	}

	if (root->rows_in_data_set < k) {
		if (root->table[0][0]["n"] >= root->table[1][0]["r"]) {
			root->node_label = 10; // mark the node as leaf from class "n"
		}
		else if (root->table[0][0]["n"] < root->table[1][0]["r"]) {
			root->node_label = 11; // mark the node as leaf from class "r"
		}

		for (int i = 0; i < root->children.size(); ++i) {
			delete root->children[i];
			root->children[i] = nullptr;
		}

		return;
	}

	sort_by_attribute(root->data_set, root->node_label);
	int num_children = number_of_diff_val_of_attr(root->table, root->node_label);
	root->children.resize(num_children);

	int idx = 0;
	string value = root->data_set[idx][root->node_label];
	int data_set_size = root->data_set.size();

	for (int i = 0; i < num_children; ++i) {
		root->children[i] = new node;
		root->children[i]->parent = root;
		root->children[i]->edge_label = value;

		while ((idx < data_set_size) && (value == root->data_set[idx][root->node_label])) {
			root->children[i]->data_set.push_back(root->data_set[idx]);
			++idx;
		}

		root->children[i]->rows_in_data_set = root->children[i]->data_set.size();
		root->children[i]->table = create_table(root->children[i]->data_set);
		root->children[i]->remaining = root->remaining;
		root->children[i]->node_label = best_attribute(root->children[i]->table, root->children[i]->remaining);

		// all attributes in the path are already used
		if (root->children[i]->node_label == -1) {
			if (prevailing_class(root->children[i]->table) == "n") {
				root->children[i]->node_label = 10;
			}
			else {
				root->children[i]->node_label = 11;
			}
		}
		else { // there are attributes that are not used in the path
			root->children[i]->remaining[root->children[i]->node_label] = false;
			create_tree_rec(root->children[i], k);
		}

		if (idx < data_set_size) value = root->data_set[idx][root->node_label];
	}
}

void create_tree(node*& root, const vector<vector<string>>& data_arr, const vector<vector<unordered_map<string, int>>>& table, const int k) {
	root = new node;
	root->parent = nullptr; // root has no parent
	root->data_set = data_arr; // the data set of the root is the initial data set
	root->rows_in_data_set = root->data_set.size(); // this way I keep how many rows the data set contains
	root->table = table; // the table of the root is the initial table
	root->edge_label = "none"; // it is the root and no label leads to it
	root->remaining.resize(10, true); // unused attributes in the path
	root->node_label = best_attribute(root->table, root->remaining); // find best attribute
	root->remaining[root->node_label] = false; // this attribute is the root of the tree so cannot be used again

	sort_by_attribute(root->data_set, root->node_label); // sort the data set by the chosen attribute

	int num_children = number_of_diff_val_of_attr(root->table, root->node_label);
	root->children.resize(num_children);

	// create a child for each possibility
	int idx = 0;
	string value = root->data_set[idx][root->node_label];
	int data_set_size = root->data_set.size();

	for (int i = 0; i < num_children; ++i) {
		root->children[i] = new node;
		root->children[i]->parent = root; // set the parent of the node
		root->children[i]->edge_label = value; // keep the edge used for the transition

		// form the data set for each of the nodes
		while ((idx < data_set_size) && (value == root->data_set[idx][root->node_label])) {
			root->children[i]->data_set.push_back(root->data_set[idx]);
			++idx;
		}

		root->children[i]->rows_in_data_set = root->children[i]->data_set.size();
		root->children[i]->table = create_table(root->children[i]->data_set);
		root->children[i]->remaining = root->remaining;
		root->children[i]->node_label = best_attribute(root->children[i]->table, root->children[i]->remaining);
		root->children[i]->remaining[root->children[i]->node_label] = false;

		create_tree_rec(root->children[i], k);

		if (idx < data_set_size) value = root->data_set[idx][root->node_label];
	}
}

void delete_tree(node*& root) {
	if (root == nullptr) return;

	for (int i = 0; i < root->children.size(); ++i) {
		delete_tree(root->children[i]);
	}

	delete root;
	root = nullptr;
}

bool decision_tree_classify(node* root, const vector<string>& test_case, int curr_depth, const int max_depth, const bool flag) {
	if (root->node_label == 10) { // means the node is a leaf and the class is 10, which corresponds to "n" class
		return 'n' == test_case[0][0]; // if true -> correct guess
	}
	if (root->node_label == 11) { // means the node is a leaf and the class is 11, which corresponds to "r" class
		return 'r' == test_case[0][0];  // if true -> correct guess
	}
	// if maximum depth is reached and we want to execute post-pruning
	// if flag == true -> we use post-pruning, else -> use pre-pruning
	if (flag && (curr_depth == max_depth)) {
		string cl = prevailing_class(root->table);
		if (cl == "n") {
			return 'n' == test_case[0][0];
		}
		else {
			return 'r' == test_case[0][0];
		}
	}

	for (int i = 0; i < root->children.size(); ++i) {
		if (test_case[root->node_label] == root->children[i]->edge_label) { // if it coincides with the child, then go to this child
			return decision_tree_classify(root->children[i], test_case, curr_depth + 1, max_depth, flag);
		}
	}
}

double decision_tree_all(const vector<vector<string>>& training_set, const vector<vector<string>>& validation_set, const int k, const int max_depth, const bool flag) {
	int total_number_of_tests = validation_set.size();
	int count_right_answers = 0;

	vector<vector<unordered_map<string, int>>> table = create_table(training_set);
	node* root = nullptr;
	create_tree(root, training_set, table, k);

	for (int i = 0; i < total_number_of_tests; ++i) {
		if (decision_tree_classify(root, validation_set[i], 0, max_depth, flag)) ++count_right_answers;
	}

	double result = static_cast<double>(count_right_answers) / total_number_of_tests;
	
	delete_tree(root);

	return result;
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

void solve(vector<vector<string>>& data_set, const int k, const int max_depth, const bool flag) {
	random_shuffle(data_set.begin(), data_set.end());

	int num_samples = data_set.size();
	vector<double> results;

	int start_idx = 0;
	int end_idx = 28;

	// 10 training sessions and 10 validation sessions
	for (int session = 0; session < 10; ++session) {
		vector<vector<string>> training_set;
		vector<vector<string>> validation_set;

		for (int i = 0; i < num_samples; ++i) {
			if (i >= start_idx && i <= end_idx) {
				validation_set.push_back(data_set[i]);
			}
			else {
				training_set.push_back(data_set[i]);
			}
		}

		double session_result = decision_tree_all(training_set, validation_set, k, max_depth, flag);
		results.push_back(session_result);

		// the first 6 should have 29 elements and the other 4 sets should have 28 elements and the total is 286
		if (session < 5) {
			start_idx = end_idx + 1;
			end_idx = start_idx + 28; // move by 28
		}
		else {
			start_idx = end_idx + 1;
			end_idx = start_idx + 27; // move by 27
		}
	}
	
	print_session_results(results);
	cout << "The average result after 10 training and validation sessions is: " << compute_average(results) << '%' << '\n';
}

void solve_pre_pruning_with_k(vector<vector<string>>& data_set, const int k) {
	solve(data_set, k, 0, false);
}

void solve_post_pruning_with_max_depth(vector<vector<string>>& data_set, const int max_depth) {
	solve(data_set, 0, max_depth, true);
}

int main() {
	string file_name = "breast-cancer.data";
	vector<vector<string>> data_arr(num_lines);
	read_file(file_name, data_arr);
	solve_pre_pruning_with_k(data_arr, 4); // best result witk k == 4
	// solve_post_pruning_with_max_depth(data_arr, 3);

	return 0;
}