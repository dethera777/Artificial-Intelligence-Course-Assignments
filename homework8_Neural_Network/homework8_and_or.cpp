#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

struct neuron {
	double value;
	vector<pair<int, double>> neighbours;
};

// the bias neuron will be on index 0 in the input layer
class neural_and_or {
	private:
		vector<neuron> input_layer;
		vector<neuron> output_layer;
		const double learning_rate = 0.9;
		const double bias = -1.0;
		const int max_epochs = 50000;

		const vector<pair<int, int>> inputs = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
		const vector<int> expected_result_and = { 0, 0, 0, 1 };
		const vector<int> expected_result_or = { 0, 1, 1, 1 };

		double g(double x) { // sigmoid function
			return 1.0 / (1.0 + exp(-x));
		}

		double g_prim(double x) { // the derivative of the sigmoid function
			return g(x) * (1.0 - g(x));
		}

		double get_random_edge_weight() {
			return (double)rand() / RAND_MAX * 2.0 - 1.0;
		}

		void create_network() {
			input_layer[0].value = bias; // bias is always at index 0

			for (int i = 0; i < input_layer.size(); ++i) {
				for (int j = 0; j < output_layer.size(); ++j) {
					double rand_edge_val = get_random_edge_weight();
					input_layer[i].neighbours.push_back({ j, rand_edge_val });
				}
			}
		}

		void train_neural_network(const vector<int>& expected_result) {
			int i = 0;

			while (i < max_epochs) {
				for (int k = 0; k < inputs.size(); ++k) {
					double in_output = 0.0;
					double error = 0.0;

					// set the values according to the example
					input_layer[1].value = inputs[k].first;
					input_layer[2].value = inputs[k].second;

					for (int t = 0; t < input_layer.size(); ++t) {
						in_output += (input_layer[t].value * input_layer[t].neighbours[0].second);
					}

					error = expected_result[k] - g(in_output);

					for (int t = 0; t < input_layer.size(); ++t) {
						input_layer[t].neighbours[0].second = input_layer[t].neighbours[0].second + learning_rate * error * g_prim(in_output) * input_layer[t].value;
					}
				}

				++i;
			}
		}

		void test_neural_network(const vector<int>& expected_result) {
			for (int i = 0; i < inputs.size(); ++i) {
				// set the initial values
				input_layer[1].value = inputs[i].first;
				input_layer[2].value = inputs[i].second;

				double in_output = 0.0;

				for (int t = 0; t < input_layer.size(); ++t) {
					in_output += (input_layer[t].value * input_layer[t].neighbours[0].second);
				}

				cout << "x = " << inputs[i].first << " y = " << inputs[i].second
					<< " Actual result = " << expected_result[i] << " Neural net result = " << g(in_output) << '\n';
			}
		}

	public:
		neural_and_or() { 
			input_layer.resize(3); // 2 neurons + 1 bias
			output_layer.resize(1); // 1 output neuron
			create_network();
		}

		void train_nn_test_nn_function_and() {
			train_neural_network(expected_result_and);
			cout << "Boolean function AND:\n";
			test_neural_network(expected_result_and);
		}

		void train_nn_test_nn_function_or() {
			train_neural_network(expected_result_or);
			cout << "Boolean function OR:\n";
			test_neural_network(expected_result_or);
		}
};

int main() {

	neural_and_or nn = neural_and_or();
	nn.train_nn_test_nn_function_or();
	// nn.train_nn_test_nn_function_and();

	return 0;
}