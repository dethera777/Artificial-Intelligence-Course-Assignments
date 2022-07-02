#include<iostream>
#include<algorithm>
#include<vector>
#include<fstream>
#include<unordered_set>
#include<string>
using namespace std;

void read_file(const string& file_name, vector<pair<double, double>>& data_set) {
	ifstream data_file(file_name, ios::in);
	double number1;
	double number2;

	if (data_file.is_open()) {
		while (data_file >> number1 >> number2) {
			data_set.push_back({ number1, number2 });
		}
	}
	else {
		cout << "Error opening the file\n";
	}

	data_file.close();
}

inline double euclidean_dist(const pair<double, double>& p1, const pair<double, double>& p2) {
	double x_diff_sq = (p1.first - p2.first) * (p1.first - p2.first);
	double y_diff_sq = (p1.second - p2.second) * (p1.second - p2.second);

	double dist = sqrt(x_diff_sq + y_diff_sq);

	return dist;
}

// this function generates k centroids and each centroid is actually a point from the data set
vector<pair<double, double>> k_means_initialization(const vector<pair<double, double>>& data_set, const int k) {
	const int data_set_size = data_set.size();
	vector<bool> used_indices(data_set_size, false);
	vector<pair<double, double>> centroids;

	while (centroids.size() < k) {
		const int idx = rand() % data_set_size;
		if (used_indices[idx] == false) { 
			centroids.push_back(data_set[idx]);
			used_indices[idx] = true;
		}
	}

	return centroids;
}

// this function returns the distance between a point and the nearest centroid to it
double min_dist_point_centroid(const pair<double, double>& p, const vector<pair<double, double>>& chosen_centroids) {
	const int num_centroids = chosen_centroids.size();
	double min_dist = DBL_MAX;
	double curr_dist = DBL_MAX;

	for (int i = 0; i < num_centroids; ++i) {
		curr_dist = euclidean_dist(p, chosen_centroids[i]);
		if (curr_dist < min_dist) {
			min_dist = curr_dist;
		}
	}

	return min_dist;
}

// this function chooses the best next centroid
int choose_centroid(const vector<pair<double, double>>& data_set, const vector<pair<double, double>>& chosen_centroids) {
	const int data_set_size = data_set.size();
	double max_dist = DBL_MIN;
	double curr_dist = DBL_MIN;
	int best_idx = -1;

	for (int i = 0; i < data_set_size; ++i) {
		curr_dist = min_dist_point_centroid(data_set[i], chosen_centroids);
		if (curr_dist > max_dist) {
			max_dist = curr_dist;
			best_idx = i;
		}
	}

	return best_idx;
}

vector<pair<double, double>> k_means_plus_plus_initialization(const vector<pair<double, double>>& data_set, const int k) {
	const int data_set_size = data_set.size();
	vector<pair<double, double>> centroids;
	const int first_centroid_idx = rand() % data_set_size; // choose a random first centroid
	
	centroids.push_back(data_set[first_centroid_idx]);

	while (centroids.size() < k) {
		int next_centroid_idx = choose_centroid(data_set, centroids);
		centroids.push_back(data_set[next_centroid_idx]);
	}

	return centroids;
}

// this function determines to which centroid a sample belongs
int determine_cluster(const pair<double, double>& p, const vector<pair<double, double>>& centroids) {
	int idx = -1;
	double min_dist = DBL_MAX;
	double curr_dist = DBL_MAX;

	int k = centroids.size();

	for (int i = 0; i < k; ++i) {
		curr_dist = euclidean_dist(p, centroids[i]);
		if (curr_dist < min_dist) {
			min_dist = curr_dist;
			idx = i;
		}
	}

	return idx;
}

pair<double, double> center_of_cluster(const vector<pair<double, double>>& cluster) {
	const int cluster_size = cluster.size();
	pair<double, double> centroid = { 0.0, 0.0 };

	for (int i = 0; i < cluster_size; ++i) {
		centroid.first += cluster[i].first;
		centroid.second += cluster[i].second;
	}

	centroid.first = centroid.first / cluster_size;
	centroid.second = centroid.second / cluster_size;

	return centroid;
}

vector<pair<double, double>> new_centroids(const vector<vector<pair<double, double>>>& clusters) {
	int num_clusters = clusters.size();
	vector<pair<double, double>> updated_centroids(num_clusters);

	for (int i = 0; i < num_clusters; ++i) {
		updated_centroids[i] = center_of_cluster(clusters[i]); // I give it the i-th cluster and it gives me the updated centroid for that cluster
	}

	return updated_centroids;
}

bool same_centroids(const vector<pair<double, double>>& old_centroids, const vector<pair<double, double>>& new_centroids) {
	const int k = old_centroids.size();

	for (int i = 0; i < k; ++i) {
		if (old_centroids[i] != new_centroids[i]) return false;
	}

	return true;
}

void clear_clusters(vector<vector<pair<double, double>>>& clusters) {
	int num_clusters = clusters.size();

	for (int i = 0; i < num_clusters; ++i) {
		clusters[i].clear();
	}
}

vector<vector<pair<double, double>>>& k_means_core(vector<vector<pair<double, double>>>& clusters, const vector<pair<double, double>>& centroids, const vector<pair<double, double>>& data_set, const int k, const int max_iter_k_means_core) {
	const int data_set_size = data_set.size();
	vector<pair<double, double>> curr_centroids = centroids;
	int iter_cnt = 0;

	while (true) {
		for (int i = 0; i < data_set_size; ++i) {
			const int cluster_idx = determine_cluster(data_set[i], curr_centroids);
			clusters[cluster_idx].push_back(data_set[i]);
		}
		++iter_cnt;

		vector<pair<double, double>> updated_centroids = new_centroids(clusters);

		if (same_centroids(curr_centroids, updated_centroids) || (iter_cnt == max_iter_k_means_core)) break;
		
		curr_centroids = updated_centroids;
		clear_clusters(clusters);
	}

	return clusters;
}

// this function computes the distance in a cluster
double within_cluster_distance(const vector<pair<double, double>>& cluster) {
	const int cluster_size = cluster.size();
	double dist_in_cluster = 0.0;

	for (int i = 0; i < cluster_size - 1; ++i) {
		for (int j = i + 1; j < cluster_size; ++j) {
			dist_in_cluster += euclidean_dist(cluster[i], cluster[j]);
		}
	}

	return dist_in_cluster;
}

double total_within_cluster_distance(const vector<vector<pair<double, double>>>& clusters) {
	double total_dist = 0.0;
	const int num_clusters = clusters.size();

	for (int i = 0; i < num_clusters; ++i) {
		total_dist += within_cluster_distance(clusters[i]);
	}

	return total_dist;
}

void write_to_file(const string& file_name, const vector<vector<pair<double, double>>>& best_solution) {
	ofstream output_file(file_name, ios::out);

	if (output_file.is_open()) {
		for (int row = 0; row < best_solution.size(); ++row) {
			for(int col = 0; col < best_solution[row].size(); ++col) {
				output_file << best_solution[row][col].first << ' ' << best_solution[row][col].second << ' ' << row << '\n';
			}
		}
	}
	else {
		cout << "Error opening the file\n";
	}

	output_file.close();
}

void k_means(const vector<pair<double, double>>& data_set, const int k, const int random_restart_max_iter,
	const int max_iter_k_means_core, const string& output_file_name, vector<pair<double, double>>(*init)(const vector<pair<double, double>>&, const int)) {

	vector<vector<pair<double, double>>> clusters(k); 
	vector<vector<pair<double, double>>> best_solution(k); 
	double min_solution_eval = DBL_MAX; 
	double curr_solution_eval = DBL_MAX;

	for (int iter = 0; iter < random_restart_max_iter; ++iter) {
		vector<pair<double, double>> initial_initialization = init(data_set, k);
		clusters = k_means_core(clusters, initial_initialization, data_set, k, max_iter_k_means_core);
		curr_solution_eval = total_within_cluster_distance(clusters);

		if (curr_solution_eval < min_solution_eval) {
			min_solution_eval = curr_solution_eval;
			best_solution = clusters;
		}
		clear_clusters(clusters);
	}

	write_to_file(output_file_name, best_solution);
}

void executor(const string& data_file_name, const string& algorithm_name) {
	string file_name_unbalance = "unbalance\\unbalance.txt";
	string file_name_normal = "normal\\normal.txt";

	string output_file_name_normal_k_means = "normal_output_K-Means.txt";
	string output_file_name_normal_k_means_plus_plus = "normal_output_K-Means++.txt";
	string output_file_name_unbalance_k_means = "unbalance_output_K-Means.txt";
	string output_file_name_unbalance_k_means_plus_plus = "unbalance_output_K-Means++.txt";

	vector<pair<double, double>> data_set;

	if (data_file_name == "U" && algorithm_name == "K") {
		read_file(file_name_unbalance, data_set);
		k_means(data_set, 8, 50, 50, output_file_name_unbalance_k_means, k_means_initialization); // -> writes data to a file
	}
	else if (data_file_name == "U" && algorithm_name == "K++") {
		read_file(file_name_unbalance, data_set);
		k_means(data_set, 8, 50, 50, output_file_name_unbalance_k_means_plus_plus, k_means_plus_plus_initialization); // -> writes data to a file
	}
	else if (data_file_name == "N" && algorithm_name == "K") {
		read_file(file_name_normal, data_set);
		k_means(data_set, 4, 90, 90, output_file_name_normal_k_means, k_means_initialization); // -> writes data to a file
	}
	else if (data_file_name == "N" && algorithm_name == "K++") {
		read_file(file_name_normal, data_set);
		k_means(data_set, 4, 90, 90, output_file_name_normal_k_means_plus_plus, k_means_plus_plus_initialization); // -> writes data to a file
	}
	else {
		cout << "Invalid input!!!\n";
	}
}

int main() {

	string data_file_name, algorithm_name;
	cout << "Enter U for unbalanced data and N for normal data:\n";
	getline(cin, data_file_name);
	cout << "Enter K for K-Means algorithm and K++ for K-Means++ algorithm:\n";
	getline(cin, algorithm_name);

	executor(data_file_name, algorithm_name);

	return 0;
}